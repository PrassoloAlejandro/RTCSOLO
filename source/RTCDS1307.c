/*
 * RTC_DS1307.c
 *
 *  Created on: 14 ene. 2025
 *      Author: aguil
 */


/*
 * Copyright 2016-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    RTC_Test.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_common.h"
#include "fsl_iocon.h"
#include "fsl_swm.h"
#include "pin_mux.h"
#include "time.h"

#include "RTC_DS1307.h"


/*--------------------------------------
 * Conversion Functions
 *--------------------------------------*/
uint8_t DecToBCD(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

uint8_t BCDToDec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

/*--------------------------------------
 * I2C Write Function
 *--------------------------------------*/
status_t DS1307_WriteRegister(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    status_t status;

    status = I2C_MasterStart(I2C1_BASE, DS1307_I2C_ADDRESS, kI2C_Write);
    if (status != kStatus_Success) {
        printf("I2C Start Failed! Status: %d\n", status);
        return status;
    }
    I2C_ResetBus(I2C1_BASE);
    status = I2C_MasterWriteBlocking(I2C1_BASE, data, sizeof(data), kI2C_TransferDefaultFlag);
    if (status != kStatus_Success) {
        printf("I2C Write Failed! Status: %d\n", status);
        return status;
    }


    status = I2C_MasterStop(I2C1_BASE);
    if (status != kStatus_Success) {
        printf("I2C Stop Failed! Status: %d\n", status);
    }

    return status;
}

/*--------------------------------------
 * I2C Read Function
 *--------------------------------------*/
status_t DS1307_ReadRegisters(uint8_t reg, uint8_t *data, size_t length) {
    status_t status;

    if (kStatus_Success == I2C_MasterStart(I2C1_BASE, DS1307_I2C_ADDRESS, kI2C_Write)) {
        status = I2C_MasterWriteBlocking(I2C1_BASE, &reg, 1, kI2C_TransferDefaultFlag);

        if (kStatus_Success == I2C_MasterRepeatedStart(I2C1_BASE, DS1307_I2C_ADDRESS, kI2C_Read)) {
            status = I2C_MasterReadBlocking(I2C1_BASE, data, length, kI2C_TransferDefaultFlag);
            status = I2C_MasterStop(I2C1_BASE);
        }
    }

    return status;
}

/*--------------------------------------
 * Set RTC Time Function
 *--------------------------------------*/
status_t DS1307_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    uint8_t data[3];
    data[0] = DecToBCD(seconds); // Register 0x00
    data[1] = DecToBCD(minutes); // Register 0x01
    data[2] = DecToBCD(hours);   // Register 0x02

    for (uint8_t i = 0; i < 3; i++) {
        if (DS1307_WriteRegister(i, data[i]) != kStatus_Success) {
            return kStatus_Fail;
        }
    }

    return kStatus_Success;
}

/*--------------------------------------
 * Get RTC Time Function
 *--------------------------------------*/
status_t DS1307_GetTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    uint8_t data[3];

    if (DS1307_ReadRegisters(0x00, data, 3) != kStatus_Success) {
        return kStatus_Fail;
    }

    *seconds = BCDToDec(data[0] & 0x7F); // Mask CH bit
    *minutes = BCDToDec(data[1]);
    *hours = BCDToDec(data[2] & 0x3F);  // 24-hour format

    return kStatus_Success;
}

/*--------------------------------------
 * Set System Time Function
 *--------------------------------------*/
void setCurrentTime(void) {
    time_t rawTime;
    struct tm *utcTime;

    rawTime = time(NULL);
    if (rawTime == -1) {
        printf("Failed to get the current time.\n");
        return;
    }

    utcTime = gmtime(&rawTime);
    if (utcTime == NULL) {
        printf("Failed to convert the current time to UTC.\n");
        return;
    }

    utcTime->tm_hour -= 3; //Ajuste UTC-3
    if (utcTime->tm_hour < 0) {
        utcTime->tm_hour += 24;
        utcTime->tm_mday -= 1;
    }

    printf("UTC-3 Date: %02d/%02d/%02d Time: %02d:%02d:%02d\n",
               utcTime->tm_mday, utcTime->tm_mon + 1, utcTime->tm_year % 100,
               utcTime->tm_hour, utcTime->tm_min, utcTime->tm_sec);
    //printf("UTC-3 Time: %02d:%02d:%02d\n", utcTime->tm_hour, utcTime->tm_min, utcTime->tm_sec);

    if (DS1307_SetTime(utcTime->tm_hour, utcTime->tm_min, utcTime->tm_sec) == kStatus_Success &&
		DS1307_SetDate(utcTime->tm_mday, utcTime->tm_mon + 1, utcTime->tm_year % 100) == kStatus_Success) {
		printf("Date and Time set successfully!\n");
	} else {
		printf("Failed to set date and time!\n");
	}
}

/*--------------------------------------
 * Set RTC Date Function
 *--------------------------------------*/
status_t DS1307_SetDate(uint8_t day, uint8_t month, uint8_t year) {
    uint8_t data[3];
    data[0] = DecToBCD(day);   // Register 0x04
    data[1] = DecToBCD(month); // Register 0x05
    data[2] = DecToBCD(year);  // Register 0x06

    for (uint8_t i = 0; i < 3; i++) {
        if (DS1307_WriteRegister(0x04 + i, data[i]) != kStatus_Success) {
            return kStatus_Fail;
        }
    }

    return kStatus_Success;
}

/*--------------------------------------
 * Get RTC Date Function
 *--------------------------------------*/
status_t DS1307_GetDate(uint8_t *day, uint8_t *month, uint8_t *year) {
    uint8_t data[3];

    if (DS1307_ReadRegisters(0x04, data, 3) != kStatus_Success) {
        return kStatus_Fail;
    }

    *day = BCDToDec(data[0]);
    *month = BCDToDec(data[1]);
    *year = BCDToDec(data[2]);

    return kStatus_Success;
}

/*--------------------------------------
 * Get RTC Date and Time Function
 *--------------------------------------*/
rtc_datetime_t GetRTCDateTime(void) {
    rtc_datetime_t datetime = {0};

    if (DS1307_GetTime(&datetime.hours, &datetime.minutes, &datetime.seconds) == kStatus_Success &&
        DS1307_GetDate(&datetime.day, &datetime.month, &datetime.year) == kStatus_Success) {
        return datetime; // Retorna los datos correctamente leídos
    }

    // Si falla, regresa un valor predeterminado
    printf("Failed to read RTC date and time.\n");
    return datetime;
}


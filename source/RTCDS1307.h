/*
 * RTC_DS1307.h
 *
 *  Created on: 10 ene. 2025
 *      Author: aguil
 */

#ifndef CABECERA_RTC_DS1307_H_
#define CABECERA_RTC_DS1307_H_


#define DS1307_I2C_ADDRESS		0X68


typedef struct {
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} rtc_datetime_t;

/*--------------------------------------
 * Conversion Functions
 *--------------------------------------*/
/*
 * Esta funcion convierte de Decimal a BCD*
 *
 */

uint8_t DecToBCD(uint8_t val);

/*
 * Esta funcion convierte de BCD a Decimal*
 *
 */

uint8_t BCDToDec(uint8_t val);



/*--------------------------------------
 * I2C Write Function
 *--------------------------------------*/
/*
 * Hago la escritura por I2C en los registros del RTC
 *
 */
status_t DS1307_WriteRegister(uint8_t reg, uint8_t value);

/*--------------------------------------
 * I2C Read Function
 *--------------------------------------*/
/*
 * Hago la lectura  por I2C de los registros del RTC
 *
 */
status_t DS1307_ReadRegisters(uint8_t reg, uint8_t *data, size_t length);

/*--------------------------------------
 * Set RTC Time Function
 *--------------------------------------*/
/*
 * Seteo la hora en el RTC
 * Esto es necesario cuando el modulo deja de estar energizado o
 * se inicializa por primera vez.
 *
 */
status_t DS1307_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);

/*--------------------------------------
 * Set RTC Date Function
 *--------------------------------------*/
/*
 * Seteo la fecha en el RTC
 * Esto es necesario cuando el modulo deja de estar energizado o
 * se inicializa por primera vez.
 *
 */
status_t DS1307_SetDate(uint8_t day, uint8_t month, uint8_t year);


/*--------------------------------------
 * Get RTC Time Function
 *--------------------------------------*/
/*
 * Pido la hora al RTC
 *
 */
status_t DS1307_GetTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

/*--------------------------------------
 * Get RTC Date Function
 *--------------------------------------*/
/*
 * Pido la fecha al RTC
 *
 */
status_t DS1307_GetDate(uint8_t *day, uint8_t *month, uint8_t *year);

/*--------------------------------------
 * Set System Time Function
 *--------------------------------------*/
/*
 * Obtengo la fecha y hora del sistema.
 * Haciendo uso de DS1307_SetDate y DS1307_SetTime inicializo el RTC
 * con los datos obtenidos del sistema
 *
 */
void setCurrentTime(void);


rtc_datetime_t GetRTCDateTime(void);


#endif /* CABECERA_RTC_DS1307_H_ */

/*********************************************************************************
 * HEADER DO AHT10
 * 	BIBLIOTECA COM FUNÇÕES E MACROS UTILIZADAS PARA FACILITAR O USO DO SENSOR
 *  PLACA DE DESENVOLVIMENTO: STM32F103C8T6
 *  PROTOCOLO DE COMUNICAÇÃO: I2C
 * 	 	DATA: 06/07/2025
 *      	AUTOR: JANISON SOUSA
 *********************************************************************************/

#ifndef AH10_H_
#define AH10_H_

#include <stdint.h>

#define AHT_ADR   		(0x38)
#define AHT_ADR_WRITE   ((AHT_ADR << 1)|0)
#define AHT_ADR_READ    ((AHT_ADR << 1)|1)


typedef enum{
	AHT10_OK = 0,
	AHT10_ERR
}OPERATION_STATE;

typedef enum{
	IDLE = 0,
	BUSY
}DEVICE_STATE;

typedef enum{
	NOR = 0,
	CYC = 1,
	CMD_0 = 2,
	CMD_1 = 3
}DEVICE_MODE;

typedef enum{
	NOT_CALIBRATED = 0,
	CALIBRATED
}CALIBRATION_STATE;

//PROTOTIPOS
OPERATION_STATE aht_Write(uint8_t);
OPERATION_STATE aht_Read(uint8_t, uint8_t*, uint8_t); //Recebe o comando, um buffer de 6 bytes, e o tamanho do buffer
uint32_t getTemperature(void);
uint32_t getHumidity(void);

#endif /* AH10_H_ */

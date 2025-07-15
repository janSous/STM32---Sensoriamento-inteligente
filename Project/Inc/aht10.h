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
#include "i2c.h"


//DECLARAÇÃO DO ENDEREÇO I2C DO AHT10 E ESTABELECIMENTO DA COMUNICAÇÃO ENTRE SENSOR E PLACA
#define AHT_ADR   		(0x38)
#define AHT_ADR_WRITE   ((AHT_ADR << 1)|0)
#define AHT_ADR_READ    ((AHT_ADR << 1)|1)

//COMANDOS DO AHT10
#define AHT_CMD_SOFT_RESET		(0xBA) //Soft reset do sensor
#define AHT_CMD_INIT			(0xBE) //Inicia o sensor
#define AHT_CMD_TRIGGER_MEASURE	(0xAC) //Inicia a medição

const uint8_t BUFFER_SIZE = 6;

typedef enum{
	AHT10_OK = 0,
	AHT10_ERR
}OPERATION_STATE;

typedef enum{
	IDLE = 0,
	BUSY
}DEVICE_STATE;

typedef enum{
	NOR = 0, 	//Normal mode
	CYC = 1, 	//Cycle mode
	CMD_0 = 2,
	CMD_1 = 3
}DEVICE_MODE;

typedef enum{
	NOT_CALIBRATED = 0,
	CALIBRATED
}CALIBRATION_STATE;

//PROTOTIPOS
OPERATION_STATE aht_Sync(void);
OPERATION_STATE aht_Write(uint8_t);
OPERATION_STATE aht_Read(uint8_t,uint8_t*, uint8_t); //Recebe o comando, um buffer de 6 bytes, e o tamanho do buffer
float getTemperature(uint8_t*, float);
float getHumidity(uint8_t*, float);

#endif /* AH10_H_ */

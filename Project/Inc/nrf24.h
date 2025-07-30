
#ifndef NRF24_H_
#define NRF24_H_

#ifndef NRF24_H
#define NRF24_H
#include "stm32f1xx.h"
#include <stdint.h>

#define LED_RED    (1 << 0)   // Pino para vermelho (ex: PA0)
#define LED_GREEN  (1 << 1)   // Pino para verde (ex: PA1)
#define LED_BLUE   (1 << 2)   // Pino para azul (ex: PA2)
#define LED_OFF    0

// SPI commands
#define NRF24_CMD_W_REGISTER    0x20
#define NRF24_CMD_R_REGISTER    0x00
#define NRF24_CMD_FLUSH_TX      0xE1
#define NRF24_CMD_W_TX_PAYLOAD  0xA0
#define NRF24_CMD_NOP           0xFF

#define NRF24_CMD_R_RX_PAYLOAD  0x61

// Registers
#define CONFIG      0x00
#define EN_AA       0x01
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define TX_ADDR     0x10
#define EN_RXADDR   0x02
#define RX_PW_P0    0x11
#define RX_ADDR_P0  0x0A
#define FIFO_STATUS 0x17
#define SETUP_RETR 0x04

// Bit mnemonics
#define MASK_TX_DS  (1 << 5)
#define MASK_MAX_RT (1 << 4)

#define TX_DS   5
#define MAX_RT  4
#define RX_DR   6
#define FLUSH_TX  0xE1

void nRF24_Init(void);
uint8_t nRF24_Check(void);
void nRF24_TX_Mode(uint8_t *address, uint8_t channel);
void nRF24_RX_Mode(uint8_t *address, uint8_t channel);
void nRF24_WritePayload(uint8_t *pBuf, uint8_t length);
uint8_t nRF24_ReadPayload(uint8_t *pBuf, uint8_t length);
uint8_t nRF24_GetStatus(void);
void nRF24_ClearIRQFlags(void);
void nRF24_WriteReg(uint8_t reg, uint8_t value);
void nRF24_SetupRetransmissions(uint8_t retries, uint16_t delay_us);

// Low level
uint8_t nRF24_LL_RW(uint8_t data);
void nRF24_CE_H(void);
void nRF24_CE_L(void);
void nRF24_CSN_H(void);
void nRF24_CSN_L(void);
void nRF24_HW_Init(void);
uint8_t nRF24_ReadReg(uint8_t reg);
void nRF24_ReadBuf(uint8_t reg, uint8_t *pBuf, uint8_t len);
void delay_ms(uint32_t ms);

//Função customizada debug
void gpio_debug_init(void);

#endif

#endif /* NRF24_H_ */

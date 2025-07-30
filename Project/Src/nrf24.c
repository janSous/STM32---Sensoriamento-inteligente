/*
 * nrf24.c
 *
 *  Created on: Jul 25, 2025
 *      Author: jan
 */
#include "nrf24.h"
#include "aht10.h"

void nRF24_CE_H(void) { GPIOA->BSRR = GPIO_BSRR_BS3; }
void nRF24_CE_L(void) { GPIOA->BSRR = GPIO_BSRR_BR3; }
void nRF24_CSN_H(void){ GPIOA->BSRR = GPIO_BSRR_BS4; }
void nRF24_CSN_L(void){ GPIOA->BSRR = GPIO_BSRR_BR4; }

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 8000; i++) {
        __NOP();
    }
}

uint8_t nRF24_LL_RW(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));
    *((__IO uint8_t *)&SPI1->DR) = data;
    while (!(SPI1->SR & SPI_SR_RXNE));
    return SPI1->DR;
}

void nRF24_HW_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN;

    // PA3: saída push-pull (CE)
    GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
    GPIOA->CRL |= GPIO_CRL_MODE3_1;

    // PA4: saída push-pull (CSN)
    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
    GPIOA->CRL |= GPIO_CRL_MODE4_1;

    // PA5: SCK - AF_PP
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
    GPIOA->CRL |= (GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0 | GPIO_CRL_CNF5_1);

    // PA6: MISO - entrada pull-up
    GPIOA->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
    GPIOA->CRL |= GPIO_CRL_CNF6_1;
    GPIOA->ODR |= (1 << 6); // Pull-up

    // PA7: MOSI - AF_PP
    GPIOA->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOA->CRL |= (GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0 | GPIO_CRL_CNF7_1);

    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_1;
    SPI1->CR1 |= SPI_CR1_SPE;
}

uint8_t nRF24_ReadReg(uint8_t reg) {
    nRF24_CSN_L();
    nRF24_LL_RW(NRF24_CMD_R_REGISTER | reg);
    uint8_t result = nRF24_LL_RW(NRF24_CMD_NOP);
    nRF24_CSN_H();
    return result;
}

void nRF24_WriteReg(uint8_t reg, uint8_t value) {
    nRF24_CSN_L();
    nRF24_LL_RW(NRF24_CMD_W_REGISTER | reg);
    nRF24_LL_RW(value);
    nRF24_CSN_H();
}

void nRF24_WriteBuf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    nRF24_CSN_L();
    nRF24_LL_RW(NRF24_CMD_W_REGISTER | reg);
    for (uint8_t i = 0; i < len; i++)
        nRF24_LL_RW(pBuf[i]);
    nRF24_CSN_H();
}

void nRF24_Init(void) {
    nRF24_HW_Init();
    nRF24_CE_L();
    nRF24_CSN_H();
    nRF24_WriteReg(CONFIG, 0x00); // PWR_UP, PRIM_TX, CRC
    nRF24_WriteReg(EN_AA, 0x00);  // Sem ACK
    nRF24_WriteReg(RF_CH, 10);    // Canal 10 (2.41GHz)
    nRF24_WriteReg(RF_SETUP, 0x0E); // 2Mbps, 0dBm
    nRF24_WriteReg(STATUS, 0x70); // Limpa IRQs
    nRF24_WriteReg(FIFO_STATUS, 0x00);
}

uint8_t nRF24_Check(void) {
    uint8_t test[5] = {0xA5,0xA5,0xA5,0xA5,0xA5};
    uint8_t read[5] = {0};

    nRF24_WriteBuf(TX_ADDR, test, 5);
    nRF24_ReadBuf(TX_ADDR, read, 5);

    for (int i = 0; i < 5; i++)
        if (read[i] != 0xA5) return 0;

    return 1;
}

void gpio_debug_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

    // PA0, PA1 e PA2 como saída push-pull
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 |
                    GPIO_CRL_MODE1 | GPIO_CRL_CNF1 |
                    GPIO_CRL_MODE2 | GPIO_CRL_CNF2);
    GPIOA->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_MODE1_1 | GPIO_CRL_MODE2_1;

    // PB0 como entrada analógica
    GPIOB->CRL &= ~(0xF << (0 * 4));
}

void nRF24_TX_Mode(uint8_t *address, uint8_t channel) {
    nRF24_CE_L();
    nRF24_WriteReg(EN_AA, 0x01);
    nRF24_WriteReg(RF_CH, channel);
    nRF24_WriteReg(RF_SETUP, 0x0E);
    nRF24_WriteBuf(TX_ADDR, address, 5);
    nRF24_WriteReg(CONFIG, 0x0E);
    nRF24_CE_H();
    nRF24_WriteReg(SETUP_RETR, 0x1F);
    nRF24_SetupRetransmissions(5, 1500);
}


void nRF24_RX_Mode(uint8_t *address, uint8_t channel) {
    nRF24_CE_L();
    nRF24_WriteReg(EN_AA, 0x01);      // Habilita Auto-ACK para Pipe 0
    nRF24_WriteReg(EN_RXADDR, 0x01);  // Habilita recepção no Pipe 0
    nRF24_WriteReg(RF_CH, channel);
    nRF24_WriteReg(RF_SETUP, 0x0E);
    nRF24_WriteBuf(RX_ADDR_P0, address, 5);
    nRF24_WriteReg(RX_PW_P0, 5);      // Payload de 5 bytes
    nRF24_WriteReg(CONFIG, 0x0E);     // PWR_UP + PRIM_RX + CRC_EN
    nRF24_CE_H();
}

void nRF24_WritePayload(uint8_t *pBuf, uint8_t length) {
    nRF24_CSN_L();
    nRF24_LL_RW(NRF24_CMD_W_TX_PAYLOAD);
    for (uint8_t i = 0; i < length; i++)
        nRF24_LL_RW(pBuf[i]);
    nRF24_CSN_H();
}

uint8_t nRF24_ReadPayload(uint8_t *pBuf, uint8_t length) {
    uint8_t status;

    nRF24_CSN_L();
    status = nRF24_LL_RW(NRF24_CMD_R_RX_PAYLOAD); // Comando de leitura
    for (uint8_t i = 0; i < length; i++) {
        pBuf[i] = nRF24_LL_RW(NRF24_CMD_NOP); // Lê cada byte
    }
    nRF24_CSN_H();

    return status;
}

void nRF24_SetupRetransmissions(uint8_t retries, uint16_t delay_us) {
    uint8_t ard = 0;

    // Converte delay_us para valor ARD (4 bits)
    if (delay_us <= 250)       ard = 0;
    else if (delay_us <= 500)  ard = 1;
    else if (delay_us <= 750)  ard = 2;
    else if (delay_us <= 1000) ard = 3;
    else if (delay_us <= 1250) ard = 4;
    else if (delay_us <= 1500) ard = 5;
    else if (delay_us <= 1750) ard = 6;
    else if (delay_us <= 2000) ard = 7;
    else if (delay_us <= 2250) ard = 8;
    else if (delay_us <= 2500) ard = 9;
    else if (delay_us <= 2750) ard = 10;
    else if (delay_us <= 3000) ard = 11;
    else if (delay_us <= 3250) ard = 12;
    else if (delay_us <= 3500) ard = 13;
    else if (delay_us <= 3750) ard = 14;
    else                       ard = 15;

    uint8_t setup_retr = (ard << 4) | (retries & 0x0F);
    nRF24_WriteReg(SETUP_RETR, setup_retr);
}

uint8_t nRF24_GetStatus(void) {
    nRF24_CSN_L();
    uint8_t status = nRF24_LL_RW(NRF24_CMD_NOP);
    nRF24_CSN_H();
    return status;
}

void nRF24_ClearIRQFlags(void) {
    nRF24_WriteReg(STATUS, 0x70);
}

void nRF24_ReadBuf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    nRF24_CSN_L();
    nRF24_LL_RW(NRF24_CMD_R_REGISTER | reg);
    for (uint8_t i = 0; i < len; i++)
        pBuf[i] = nRF24_LL_RW(NRF24_CMD_NOP);
    nRF24_CSN_H();
}




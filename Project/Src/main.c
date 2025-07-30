#include "stm32f1xx.h"
#include "nrf24.h"
#include "i2c.h"
#include "adc.h"


volatile uint16_t ldr_raw = 0;

#define LED_RED    (1 << 0)   // Pino para vermelho (ex: PA0)
#define LED_GREEN  (1 << 1)   // Pino para verde (ex: PA1)
#define LED_BLUE   (1 << 2)   // Pino para azul (ex: PA2)
#define LED_OFF    0

// Comandos AHT10
#define AHT10_TRIG   0xAC
#define AHT10_RESET  0xBA


int main(void) {
    gpio_debug_init();
    adc_init();
    i2c_dma_init();
    aht10_Sync();
    nRF24_Init();

    if (!nRF24_Check()) {
        while (1) {
            GPIOA->ODR ^= (1 << 2); // Pisca PA2 (falha nRF)
            delay_ms(200);
        }
    }




    uint8_t tx_address[5] = {0x02, 0xDD, 0xCC, 0xBB, 0xAA};
    nRF24_TX_Mode(tx_address, 10); // Canal 10
   nRF24_RX_Mode(tx_address, 10);

    while (1) {
        // Lê dados do AHT10
        uint8_t cmd[3] = {AHT10_TRIG, 0x33, 0x00};
        aht10_Write(cmd, 3);
        delay_ms(80);
        aht10_Read(rx_buffer, 6);

        // Lê LDR (PB0)
        ldr_raw = adc_read();

        // Prepara payload
        uint8_t payload[32] ={};
        payload[0] = rx_buffer[1];
        payload[1] = rx_buffer[2];
        payload[2] = rx_buffer[3];
        payload[3] = rx_buffer[4];
        payload[4] = rx_buffer[5];
        payload[5] = (uint8_t) (ldr_raw >> 4);


        nRF24_WritePayload(payload, 32);
        nRF24_CE_H();
        delay_us(1);
        nRF24_CE_L();

        delay_ms(20);

        uint8_t status = nRF24_GetStatus();

        // Verificação para modo TX

        // Verifica flags
        if (status & (1 << TX_DS)) {
            // Transmissão com ACK (sucesso)
        	GPIOA->ODR &= ~(LED_RED | LED_GREEN | LED_BLUE);
        	GPIOA->ODR |= LED_GREEN;

            // Limpa a flag TX_DS
            nRF24_WriteReg(STATUS, (1 << TX_DS));
        }
        else if (status & (1 << MAX_RT)) {
        	GPIOA->ODR &= ~(LED_RED | LED_GREEN | LED_BLUE);
        	GPIOA->ODR = LED_RED;

            // Limpa flag MAX_RT
            nRF24_WriteReg(STATUS, (1 << MAX_RT));

            // Envia FLUSH_TX diretamente via SPI (sem função extra)
            nRF24_CSN_L();

            while (!(SPI1->SR & SPI_SR_TXE));
            SPI1->DR = 0xE1;

            while (!(SPI1->SR & SPI_SR_RXNE));
            volatile uint8_t dummy = SPI1->DR;

            nRF24_CSN_H();
        }
        else {
            // Nenhuma confirmação ainda – pisca o LED azul
        	GPIOA->ODR ^= LED_BLUE;
        }

        nRF24_ClearIRQFlags(); // Limpeza geral complementar
    }

}

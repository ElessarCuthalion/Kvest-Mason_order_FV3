/*
 * board.h
 *
 *  Created on: 12 сент. 2015 г.
 *      Author: Kreyl
 */

#pragma once

#include <inttypes.h>

// ==== General ====
#define BOARD_NAME      "QuestPlatform_v1"
#define WoodmanRoom     1
#define PianoRoom       2

#define QUEST_ROOM      WoodmanRoom

// Default Settings
#define APP_NAME        "Woodman"
// Sound
#define PlayDir         "0:\\"
// RGB LEDs
#define LED_CNT         30  // Number of WS2812 LEDs


// MCU type as defined in the ST header.
#define STM32F205xx

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ         12000000

#define SYS_TIM_CLK             (Clk.APB1FreqHz) // OS timer settings
#define I2C_REQUIRED            FALSE
#define ADC_REQUIRED            FALSE
#define SIMPLESENSORS_ENABLED   TRUE

#if 1 // ========================== GPIO =======================================

// Button
#define BUTTONS_CNT     1

// Inputs
#define Opto_in         GPIOB, 4, pudPullUp

#define Port1_in        GPIOC, 0, pudPullUp
#define Port2_in        GPIOC, 1, pudPullUp
#define Port3_in        GPIOC, 2, pudPullUp
#define Port4_in        GPIOC, 3, pudPullUp
#define Port5_in        GPIOB, 12, pudPullUp
#define PwPort1_in      GPIOC, 4, pudPullUp
#define PwPort2_in      GPIOC, 5, pudPullUp
#define PwPort3_in      GPIOC, 6, pudPullUp
#define PwPort4_in      GPIOC, 7, pudPullUp
#define PwPort5_in      GPIOB, 0, pudPullUp

#define LED_CH1_in      GPIOB, 6, pudPullUp
#define LED_CH2_in      GPIOB, 7, pudPullUp
#define LED_CH3_in      GPIOB, 8, pudPullUp
#define LED_CH4_in      GPIOB, 9, pudPullUp

#define USER_RX_in      GPIOA, 1, pudPullUp
#define USER_TX_in      GPIOA, 0, pudPullUp

// Outputs
#define Opto_out        GPIOB, 4, omPushPull

#define Port1_out       GPIOC, 0, omPushPull
#define Port2_out       GPIOC, 1, omPushPull
#define Port3_out       GPIOC, 2, omPushPull
#define Port4_out       GPIOC, 3, omPushPull
#define Port5_out       GPIOB, 12, omPushPull
#define PwPort1_out     GPIOC, 4, omPushPull
#define PwPort2_out     GPIOC, 5, omPushPull
#define PwPort3_out     GPIOC, 6, omPushPull
#define PwPort4_out     GPIOC, 7, omPushPull
#define PwPort5_out     GPIOB, 0, omPushPull

#define LED_CH1_out     GPIOB, 6, omPushPull
#define LED_CH2_out     GPIOB, 7, omPushPull
#define LED_CH3_out     GPIOB, 8, omPushPull
#define LED_CH4_out     GPIOB, 9, omPushPull
#define LED_PWM1        { GPIOB, 6, TIM4, 1, invNotInverted, omPushPull, 512 }
#define LED_PWM2        { GPIOB, 7, TIM4, 2, invNotInverted, omPushPull, 512 }
#define LED_PWM3        { GPIOB, 8, TIM4, 3, invNotInverted, omPushPull, 512 }
#define LED_PWM4        { GPIOB, 9, TIM4, 4, invNotInverted, omPushPull, 512 }

#define USER_RX_out     GPIOA, 1, omPushPull
#define USER_TX_out     GPIOA, 0, omPushPull


// Beeper

// Sensors

// External Power Input
#define ExternalPWR_Pin    GPIOA, 9, pudPullDown

// WS2812
#define LEDWS_PIN       GPIOB, 5, omPushPull, pudNone, AF6

// UART
#define UART_GPIO       GPIOA
#define UART_TX_PIN     2
#define UART_RX_PIN     3
#define UART_AF         AF7 // for USART2 @ GPIOA

#endif // GPIO

#if 1 // ========================= Timer =======================================
#endif // Timer

#if I2C_REQUIRED // ====================== I2C =================================
#define I2C1_ENABLED     TRUE
#define I2C_PIN       { GPIOA, 9, 10, I2C1_AF, I2C1_BAUDRATE, I2C1_DMA_TX, I2C1_DMA_RX }
#endif

#if 1 // =========================== SPI =======================================
#define VS_SPI          SPI2
#define VS_AF           AF5
#define LEDWS_SPI       SPI3
#endif

#if 1 // ========================== USART ======================================
#define UART            USART2
#define UART_TX_REG     UART->DR
#define UART_RX_REG     UART->DR
#endif

#if ADC_REQUIRED // ======================= Inner ADC ==========================
// Clock divider: clock is generated from the APB2
#define ADC_CLK_DIVIDER		adcDiv4

// ADC channels
#define BAT_CHNL 	        10

//#define ADC_VREFINT_CHNL    17  // All 4xx and F072 devices. Do not change.
#define ADC_CHANNELS        { BAT_CHNL }//{ BAT_CHNL, ADC_VREFINT_CHNL }
#define CallConst           450
#define ADC_CHANNEL_CNT     1   // Do not use countof(AdcChannels) as preprocessor does not know what is countof => cannot check
#define ADC_SAMPLE_TIME     ast239d5Cycles
#define ADC_SAMPLE_CNT      16   // How many times to measure every channel

#define ADC_MAX_SEQ_LEN     16  // 1...16; Const, see ref man
#define ADC_SEQ_LEN         (ADC_SAMPLE_CNT * ADC_CHANNEL_CNT)
#if (ADC_SEQ_LEN > ADC_MAX_SEQ_LEN) || (ADC_SEQ_LEN == 0)
#error "Wrong ADC channel count and sample count"
#endif
#endif

#if 1 // =========================== DMA =======================================
//STM32F205 Reference Manual s.179,180
#define STM32_DMA_REQUIRED  TRUE
// ==== Uart ====
// Remap is made automatically if required
#define UART_DMA_TX     STM32_DMA1_STREAM6
#define UART_DMA_RX     STM32_DMA1_STREAM5
#define UART_DMA_CHNL   4

#if I2C_REQUIRED // ==== I2C ====
#endif

// ==== SPI2 ==== ==== Sound VS1011 ====
#define VS_DMA          STM32_DMA1_STREAM4 // SPI2_TX
#define VS_DMA_CHNL     0   // Dummy
#define VS_DMA_MODE     STM32_DMA_CR_CHSEL(VS_DMA_CHNL) | \
                        DMA_PRIORITY_LOW | \
                        STM32_DMA_CR_MSIZE_BYTE | \
                        STM32_DMA_CR_PSIZE_BYTE | \
                        STM32_DMA_CR_DIR_M2P |    /* Direction is memory to peripheral */ \
                        STM32_DMA_CR_TCIE         /* Enable Transmission Complete IRQ */

// ==== SPI3 ====
#define LEDWS_DMA       STM32_DMA1_STREAM7
#define LEDWS_DMA_CHNL  0   // Dummy

#if ADC_REQUIRED
/* DMA request mapped on this DMA channel only if the corresponding remapping bit is cleared in the SYSCFG_CFGR1
 * register. For more details, please refer to Section10.1.1: SYSCFG configuration register 1 (SYSCFG_CFGR1) on
 * page173 */
#define ADC_DMA         STM32_DMA2_STREAM4
#define ADC_DMA_MODE    STM32_DMA_CR_CHSEL(0) |   /* DMA2 Stream4 Channel 0 */ \
                        DMA_PRIORITY_LOW | \
                        STM32_DMA_CR_MSIZE_HWORD | \
                        STM32_DMA_CR_PSIZE_HWORD | \
                        STM32_DMA_CR_MINC |       /* Memory pointer increase */ \
                        STM32_DMA_CR_DIR_P2M |    /* Direction is peripheral to memory */ \
                        STM32_DMA_CR_TCIE         /* Enable Transmission Complete IRQ */
#endif // ADC

#endif // DMA

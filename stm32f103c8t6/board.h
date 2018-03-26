/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for the STM32F103C-MINNI proto board.
 */

/*
 * Board identifier.
 */
#define BOARD_STM32F103C_ALARM
#define BOARD_NAME             "STM32F103C-ALARM"

/*
 * Board frequencies.
 */
#define STM32_LSECLK            32768
#define STM32_HSECLK            8000000

/*
 * MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
 */
#define STM32F103xB

/*
 * IO pins assignments.
 */
#define GPIOA_ADC_CH0           0
#define GPIOA_ADC_CH1           1
#define GPIOA_ADC_CH2           2
#define GPIOA_ADC_CH3           3
#define GPIOA_ADC_CH4           4
#define GPIOA_ADC_CH5           5
#define GPIOA_ADC_CH6           6
#define GPIOA_ADC_CH7           7
#define GPIOA_SYSLED            8
#define GPIOA_MB_TX             9
#define GPIOA_MB_RX             10
#define GPIOA_EXT_BTN           11
#define GPIOA_MB_TX_EN          12
#define GPIOA_SWDIO             13
#define GPIOA_SWCLK             14
#define GPIOA_PIN15             15


#define GPIOB_ADC_CH8           0
#define GPIOB_ADC_CH9           1
#define GPIOB_PWR_IN            2
#define GPIOB_PIN3              3
#define GPIOB_BUZZER            4
#define GPIOB_MB_TX_EN          5
#define GPIOB_SCL               6
#define GPIOB_SDA               7
#define GPIOB_PIN8              8
#define GPIOB_PIN9              9
#define GPIOB_TC35_TX           10
#define GPIOB_TC35_RX           11
#define GPIOB_RELAY1            12
#define GPIOB_RELAY2            13
#define GPIOB_RELAY3            14
#define GPIOB_RELAY4            15

#define GPIOC_OWLED             13

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *  0 	- PAL_MODE_INPUT_ANALOG
 *  2 	- PAL_MODE_UNCONNECTED, implemented as push pull output 2MHz.
 *  3 	- PAL_MODE_OUTPUT_PUSHPULL, 50MHz.
 *  4 	- PAL_MODE_RESET, implemented as input.
 *  7 	- PAL_MODE_OUTPUT_OPENDRAIN, 50MHz.
 *  8	- PAL_MODE_INPUT_PULLUP or PAL_MODE_INPUT_PULLDOWN depending on ODR.
 *  0xB	- PAL_MODE_STM32_ALTERNATE_PUSHPULL, 50MHz.
 *  0xF	- PAL_MODE_STM32_ALTERNATE_OPENDRAIN, 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 * Everything input with pull-up except:
 * PA0  - PAL_MODE_INPUT_ANALOG
 * PA1  - PAL_MODE_INPUT_ANALOG
 * PA2  - PAL_MODE_INPUT_ANALOG
 * PA3  - PAL_MODE_INPUT_ANALOG
 * PA4  - PAL_MODE_INPUT_ANALOG
 * PA5  - PAL_MODE_INPUT_ANALOG
 * PA6  - PAL_MODE_INPUT_ANALOG
 * PA7  - PAL_MODE_INPUT_ANALOG
 * PA8  - PAL_MODE_INPUT (TIM1 PWM)
 * PA9  - PAL_MODE_STM32_ALTERNATE_PUSHPULL (USART1 TX)
 * PA10 - PAL_MODE_RESET (USART1 RX)
 * PA11 - PAL_MODE_INPUT (EXT_BTN)
 * PA12 - PAL_MODE_OUTPUT_PUSHPULL
 * PA13 - PAL_MODE_RESET
 * PA14 - PAL_MODE_RESET
 * PA15 - PAL_MODE_RESET
 */
#define VAL_GPIOACRL            0x00000000      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x888344B4      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFFFFF

/*
 * Port B setup.
 * Everything input with pull-up except:
 * PB0  - PAL_MODE_INPUT_ANALOG
 * PB1  - PAL_MODE_INPUT_ANALOG
 * PB2  - PAL_MODE_INPUT
 * PB3  - PAL_MODE_RESET
 * PB4  - PAL_MODE_STM32_ALTERNATE_PUSHPULL (TIM3 PWM)
 * PB5  - PAL_MODE_OUTPUT_PUSHPULL
 * PB6  - PAL_MODE_STM32_ALTERNATE_OPENDRAIN (I2C1)
 * PB7  - PAL_MODE_STM32_ALTERNATE_OPENDRAIN (I2C1)
 * PB8  - PAL_MODE_INPUT
 * PB9  - PAL_MODE_OUTPUT_PUSHPULL
 * PB10 - PAL_MODE_STM32_ALTERNATE_PUSHPULL (USART3 TX)
 * PB11 - PAL_MODE_RESET (USART3 RX)
 * PB12 - PAL_MODE_OUTPUT_PUSHPULL
 * PB13 - PAL_MODE_OUTPUT_PUSHPULL
 * PB14 - PAL_MODE_OUTPUT_PUSHPULL
 * PB15 - PAL_MODE_OUTPUT_PUSHPULL
 */
#define VAL_GPIOBCRL            0xFF3B4400      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0x33334B34      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFFFFFF

/*
 * Port C setup.
 * Everything input with pull-up except:
 * PC13 - PAL_MODE_OUTPUT_PUSHPULL
 */
#define VAL_GPIOCCRL            0x88888888      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x88388888      /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFFF

/*
 * Port D setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIODCRL            0x88888888      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x88888888      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * USB bus activation macro, required by the USB driver.
 */
#define usb_lld_connect_bus(usbp) palSetPadMode(GPIOA, GPIOA_USB_DP, PAL_MODE_INPUT);

/*
 * USB bus de-activation macro, required by the USB driver.
 */
#define usb_lld_disconnect_bus(usbp) { \
 palSetPadMode(GPIOA, GPIOA_USB_DP, PAL_MODE_OUTPUT_PUSHPULL); \
 palClearPad(GPIOA, GPIOA_USB_DP); \
}

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */

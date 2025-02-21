/** @file   target.h
    @author M. P. Hayes, UCECE
    @date   12 February 2018
    @brief
*/
#ifndef TARGET_H
#define TARGET_H

#include "mat91lib.h"

/* This is for the carhat (chart) board configured as a racer!  */

/* System clocks  */
#define F_XTAL 12.00e6
#define MCU_PLL_MUL 16
#define MCU_PLL_DIV 1

#define MCU_USB_DIV 2
/* 192 MHz  */
#define F_PLL (F_XTAL / MCU_PLL_DIV * MCU_PLL_MUL)
/* 96 MHz  */
#define F_CPU (F_PLL / 2)

/* TWI  */
#define TWI_TIMEOUT_US_DEFAULT 10000

/* USB  */
#define USB_VBUS_PIO PA5_PIO
#define USB_CURRENT_MA 500

/* LEDs  */
#define LED_ERROR_PIO PA0_PIO
#define LED_STATUS_PIO PA1_PIO
#define LED_ACTIVE 0

/* General  */
#define APPENDAGE_PIO PA1_PIO
#define SERVO_PWM_PIO PA2_PIO

/* Button  */
#define BUTTON_PIO PA2_PIO
#define BUMP_DETECT PA27_PIO

/* H-bridges   */
#define MOTOR_LEFT_PWM_PIO PA25_PIO
#define MOTOR_RIGHT_PWM_PIO PB1_PIO

/* Radio  */
#define RADIO_CS_PIO PA8_PIO
#define RADIO_CE_PIO PA7_PIO
#define RADIO_IRQ_PIO PA6_PIO
#define RADIO_POWER_ENABLE_PIO PA9_PIO
#define RADIO_CONFIG_0 PA11_PIO
#define RADIO_CONFIG_1 PA10_PIO

/* LED tape  */
#define LEDTAPE_PIO PA15_PIO

/* H-Birdge Inputs*/
#define HBRIDGE_A1 = PA25_PIO  // HIGH = Forwards
#define HBRIDGE_B1 = PB14_PIO  // HIGH = Forwards
#define HBRIDGE_A2 = PB1_PIO   // HIGH = Backwards
#define HBRIDGE_B2 = PB0_PIO   // HIGH = Backwards

#define ACTUATOR1 PA14_PIO

#endif /* TARGET_H  */

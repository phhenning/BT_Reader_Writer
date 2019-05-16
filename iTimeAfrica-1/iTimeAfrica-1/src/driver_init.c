/**
 * \file
 *
 * \brief Driver initialization.
 *
 (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/*
 * Code generated by START.
 *
 * This file will be overwritten when reconfiguring your START project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <system.h>

void EXTERNAL_IRQ_0_initialization(void)
{

	// Set pin direction to input
	PD2_set_dir(PORT_DIR_IN);

	PD2_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_OFF);

	EXTERNAL_IRQ_0_init();
}

/* configure the pins and initialize the registers */
void SPI_0_initialization(void)
{

	// Set pin direction to input
	RFID_MISO_set_dir(PORT_DIR_IN);

	RFID_MISO_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_OFF);

	// Set pin direction to output
	RFID_MOsi_set_dir(PORT_DIR_OUT);

	RFID_MOsi_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	// Set pin direction to output
	RFID_SCK_set_dir(PORT_DIR_OUT);

	RFID_SCK_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	SPI_0_init();
}

void TIMER_0_initialization(void)
{

	TIMER_0_init();
}

/* configure pins and initialize registers */
void USART_0_initialization(void)
{

	// Set pin direction to input
	PD0_set_dir(PORT_DIR_IN);

	PD0_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_OFF);

	// Set pin direction to output
	PD1_set_dir(PORT_DIR_OUT);

	PD1_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	USART_0_init();
}

/**
 * \brief System initialization
 */
void system_init()
{
	mcu_init();

	/* PORT setting on ADC7 */

	// Disable pull-up.
	Vmeas_set_pull_mode(PORT_PULL_OFF);

	/* PORT setting on PB0 */

	// Set pin direction to output
	UART_Ard_2_Mod_set_dir(PORT_DIR_OUT);

	UART_Ard_2_Mod_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    true);

	/* PORT setting on PB1 */

	// Set pin direction to input
	UART_Mod_2_ard_set_dir(PORT_DIR_IN);

	UART_Mod_2_ard_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_OFF);

	/* PORT setting on PB2 */

	// Set pin direction to output
	RFID_CS_set_dir(PORT_DIR_OUT);

	RFID_CS_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	/* PORT setting on PC0 */

	// Set pin direction to output
	LED_A0_set_dir(PORT_DIR_OUT);

	LED_A0_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	/* PORT setting on PC1 */

	// Set pin direction to output
	LED_rstB_set_dir(PORT_DIR_OUT);

	LED_rstB_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    true);

	/* PORT setting on PC2 */

	// Set pin direction to input
	Sqr_set_dir(PORT_DIR_IN);

	Sqr_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_UP);

	/* PORT setting on PC3 */

	// Set pin direction to input
	config_set_dir(PORT_DIR_IN);

	config_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_UP);

	/* PORT setting on PC4 */

	/* PORT setting on PC5 */

	// Set pin direction to output
	scl_set_dir(PORT_DIR_OUT);

	scl_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	/* PORT setting on PC6 */

	// Set pin direction to input
	Reset_set_dir(PORT_DIR_IN);

	Reset_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_UP);

	/* PORT setting on PD3 */

	// Set pin direction to output
	LED_D3_set_dir(PORT_DIR_OUT);

	LED_D3_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	/* PORT setting on PD4 */

	// Set pin direction to output
	LCD_Clk_set_dir(PORT_DIR_OUT);

	LCD_Clk_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	/* PORT setting on PD5 */

	// Set pin direction to output
	LCD_Data_set_dir(PORT_DIR_OUT);

	LCD_Data_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	/* PORT setting on PD6 */

	// Set pin direction to output
	LCD_Data_Cmd_set_dir(PORT_DIR_OUT);

	LCD_Data_Cmd_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	/* PORT setting on PD7 */

	// Set pin direction to output
	LCD_En_set_dir(PORT_DIR_OUT);

	LCD_En_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    true);

	sysctrl_init();

	EXTERNAL_IRQ_0_initialization();

	SPI_0_initialization();

	TIMER_0_initialization();

	USART_0_initialization();
}
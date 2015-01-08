/*
 * By Christoffer Ramstad-Evensen
 *	
 * 1. Include EIC, TWIM, TWIS, USART, SPI and GPIO with ASF Wizard.
 * 2. Include conf_board.h and define CONF_BOARD_EIC and CONF_BOARD_USART0
 *  - This takes care of the multiplexing. See board_init().
 * 3. Goto Quick Starts at asf.atmel.com and lookup examples.
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */

#include <asf.h>
#include <conf_board.h>
#include "RF430CL330H.h"

// USART config
const sam_usart_opt_t usart_console_settings = {
	USART_SERIAL_BAUDRATE,
	USART_SERIAL_CHAR_LENGTH,
	USART_SERIAL_PARITY,
	USART_SERIAL_STOP_BIT,
	US_MR_CHMODE_NORMAL
};

// Interrupt (for testing)
volatile uint8_t b_toggle = 0;

// TWIM
#define INTERNAL_ADDRESS_LENGTH 2
#define DATA_BUF_TX_LENGTH 64

// Function for blinking LEDS without interrupts
static void test_blink_led_no_interrupt(void) {
	
	// Is button pressed?
	if (ioport_get_pin_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
		// Yes, so turn LED on.
		ioport_set_pin_level(LED_0_PIN, LED_0_ACTIVE);
	} else {
		// No, so turn LED off.
		ioport_set_pin_level(LED_0_PIN, !LED_0_ACTIVE);
	}
}

// Function for blinking LEDS with EIC
static void test_blink_led_send_usart_string_with_interrupt(void) {
	
	static const char* msg = "LED ON"; 
	
	// Is button pressed?
	if (b_toggle) {
		// Yes, so turn LED on.
		ioport_set_pin_level(LED_0_PIN, LED_0_ACTIVE);
		usart_write_line(USART_SERIAL, msg);
	} else {
		// No, so turn LED off.
		ioport_set_pin_level(LED_0_PIN, !LED_0_ACTIVE);
	}
}

// Interrupt service routine
static void eic_callback(void) {
	
	/* Check if EIC push button line interrupt line is pending. */
	if (eic_line_interrupt_is_pending(EIC, GPIO_PUSH_BUTTON_EIC_LINE)) {
		eic_line_clear_interrupt(EIC, GPIO_PUSH_BUTTON_EIC_LINE);
		// Toggle an integer
		b_toggle ^= 1;
	}
}

// Setupr for external interrupts
static void eic_setup(void) {
	
	eic_enable(EIC);
	
	struct eic_line_config eic_line_conf;
	eic_line_conf.eic_mode = EIC_MODE_EDGE_TRIGGERED;
	eic_line_conf.eic_edge = EIC_EDGE_FALLING_EDGE;
	eic_line_conf.eic_level = EIC_LEVEL_LOW_LEVEL;
	eic_line_conf.eic_filter = EIC_FILTER_DISABLED;
	eic_line_conf.eic_async = EIC_ASYNCH_MODE;
	
	eic_line_set_config(EIC, GPIO_PUSH_BUTTON_EIC_LINE, &eic_line_conf);
	eic_line_set_callback(EIC, GPIO_PUSH_BUTTON_EIC_LINE, eic_callback, GPIO_PUSH_BUTTON_EIC_IRQ, 1);
	eic_line_enable(EIC, GPIO_PUSH_BUTTON_EIC_LINE);
}

// Setup for USART
static void usart0_setup(void) {
	
	sysclk_enable_peripheral_clock(USART_SERIAL);
	usart_init_rs232(USART_SERIAL, &usart_console_settings, sysclk_get_main_hz());
	usart_enable_tx(USART_SERIAL);
	usart_enable_rx(USART_SERIAL);
}

// Function returning a TWI packet
twi_package_t twim_create_packet(uint16_t target_slave_address, uint16_t internal_address, \
	uint8_t internal_address_length, uint8_t* data_buf_tx, uint8_t data_buf_tx_length) {
	
	twi_package_t packet_tx;
	packet_tx.chip = target_slave_address; 
	packet_tx.addr[0] = (internal_address >> 16); // & 0xFF; 
	packet_tx.addr[1] = (internal_address >> 8); // & 0xFF;
	packet_tx.addr_length = internal_address_length; 
	packet_tx.buffer = (void *) data_buf_tx;
	packet_tx.length = data_buf_tx_length;
	
	return packet_tx;
}

// Setup for TWIM
void twim_setup(void) {
	
	struct twim_config twim_conf;
	twim_conf.twim_clk = sysclk_get_cpu_hz();
	twim_conf.speed = TWI_STD_MODE_SPEED; // Standard speed
	twim_conf.smbus = false;
	twim_conf.hsmode_speed = 0;
	twim_conf.data_setup_cycles = 0;
	twim_conf.hsmode_data_setup_cycles = 0;
	
	twim_set_config(TWIM0, &twim_conf);
	twim_set_callback(TWIM0, 0, twim_default_callback, 1);
}

// Might not need this one
status_code_t twim_send(Twim *twim, struct twim_package *package) {
	
	while (!STATUS_OK)
	{
		twi_master_write(&twim, &package);
	}
}

int main (void) {
	
	// Setup board
	board_init();
	// Setup system clock
	sysclk_init();
	// Set up EIC
	eic_setup();
	// Setup USART0
	usart0_setup();
	
	// Create packet for RF430
	uint8_t data_buf_tx[DATA_BUF_TX_LENGTH];
	twi_package_t packet_tx;
	packet_tx = twim_create_packet(RF430_I2C_SLAVE_ADDRESS, STATUS_REG, \
	INTERNAL_ADDRESS_LENGTH, data_buf_tx, DATA_BUF_TX_LENGTH);

	for (;;)
	{
		test_blink_led_send_usart_string_with_interrupt();	
	}	
}

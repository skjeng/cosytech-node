/**
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

const sam_usart_opt_t usart_console_settings = {
	USART_SERIAL_BAUDRATE,
	USART_SERIAL_CHAR_LENGTH,
	USART_SERIAL_PARITY,
	USART_SERIAL_STOP_BIT,
	US_MR_CHMODE_NORMAL
};

volatile uint8_t b_toggle = 0;

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

static void usart0_setup(void) {
	
	sysclk_enable_peripheral_clock(USART_SERIAL);
	usart_init_rs232(USART_SERIAL, &usart_console_settings, sysclk_get_main_hz());
	usart_enable_tx(USART_SERIAL);
	usart_enable_rx(USART_SERIAL);
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

	for (;;)
	{
		test_blink_led_send_usart_string_with_interrupt();	
	}	
}

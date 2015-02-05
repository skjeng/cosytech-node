/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>

void configure_port_pins(void);
void configure_i2c_master(void);
void configure_usart(void);

void configure_port_pins(void){
	
	//struct port_config pin_conf;
	struct port_config config_port_pin;
	
	//port_get_config_defaults(&pin_conf);
	port_get_config_defaults(&config_port_pin);
	
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	config_port_pin.input_pull = PORT_PIN_PULL_NONE;
	
	port_pin_set_config(PIN_PA02, &config_port_pin);
}


//Initial software module instances of USART
struct usart_module usart_instance;
//Sets USART to the EDBG virtual COM port of SAMD21 Xpro
void configure_usart(void){
	
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	
	config_usart.baudrate    = 9600;
	/*
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	
	
	while (usart_init(&usart_instance,
	EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {}
	*/
	usart_enable(&usart_instance);
}

struct i2c_master_module i2c_master_instance;
// Initialize and config I2C on SERCOM0 bus

void configure_i2c_master(void){
	/* Initialize config structure and software module. */
	struct i2c_master_config config_i2c_master;
	
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.buffer_timeout = 10000;
	
	//config_i2c_master.pinmux_pad0 = PINMUX_PA16C_SERCOM1_PAD0;
	config_i2c_master.pinmux_pad0 = PINMUX_PA08C_SERCOM0_PAD0;
	config_i2c_master.pinmux_pad1 = PINMUX_PA09C_SERCOM0_PAD1;
	//config_i2c_master.pinmux_pad1 = PINMUX_PA17C_SERCOM1_PAD1;

	/* Initialize and enable device with config. */
	i2c_master_init(&i2c_master_instance, SERCOM0, &config_i2c_master);
	//i2c_master_reset(&i2c_master_instance);
	i2c_master_enable(&i2c_master_instance);
}


int main (void)
{
	system_init();
	//delay_init();
	configure_port_pins();
	configure_i2c_master();
	port_pin_set_output_level(PIN_PA02, false);

	// Insert application code here, after the board has been initialized.
	
	while(1){
		delay_ms(500);
		port_pin_set_output_level(PIN_PA02, true);
		delay_ms(500);
		port_pin_set_output_level(PIN_PA02, false);
	}
}

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
#include <RF430CL330H/RF430CL330H.h>

#define _RESET PIN_PB11

void configure_port_pins(void);
void configure_usart(void);

void configure_port_pins(void){
	
	//struct port_config pin_conf;
	struct port_config config_port_pin;
	
	//port_get_config_defaults(&pin_conf);
	port_get_config_defaults(&config_port_pin);
	
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	config_port_pin.input_pull = PORT_PIN_PULL_NONE;
	
	port_pin_set_config(PIN_PA02, &config_port_pin);
	port_pin_set_config(_RESET, &config_port_pin);
}


//Initial software module instances of USART
struct usart_module usart_instance;
//Sets USART to the EDBG virtual COM port of SAMD21 Xpro
void configure_usart(void){
	
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	
	config_usart.baudrate    = 9600; //Need to find the SERCOM/pins connected to EDBG on the SAMW25
	/*
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING; //USART_RX_1_TX_0_XCK_1
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0; //PINMUX_PA22C_SERCOM3_PAD0
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1; //PINMUX_PA23C_SERCOM3_PAD1
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2; //PINMUX_UNUSED
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3; //PINMUX_UNUSED
	
	while (usart_init(&usart_instance,
	EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {}
	*/
	
	config_usart.mux_setting = USART_RX_1_TX_0_XCK_1; //USART_RX_1_TX_2_XCK_3
	config_usart.pinmux_pad0 = PINMUX_PA12C_SERCOM2_PAD0;
	config_usart.pinmux_pad1 = PINMUX_PA13C_SERCOM2_PAD1;
	config_usart.pinmux_pad2 = PINMUX_UNUSED;
	config_usart.pinmux_pad3 = PINMUX_UNUSED;
	
	while(usart_init(&usart_instance,SERCOM2, &config_usart) != STATUS_OK) {}
	
	usart_enable(&usart_instance);
}




int main (void)
{
	system_init();
	//delay_init();
	configure_port_pins();
	configure_usart();
	
	uint8_t string[] = "Testing usart\n";
	
	usart_write_buffer_wait(&usart_instance, string,sizeof(string));
	port_pin_set_output_level(_RESET, 1);
	port_pin_set_output_level(_RESET, 0);
	delay_ms(100);
	port_pin_set_output_level(_RESET, 1);
	delay_ms(1000);
	
	port_pin_set_output_level(PIN_PA02, !false);
	
	uint8_t ndef_data[] = COSY_DEFAULT_DATA;
	
	rf430_init();
	rf430_write_ndef(ndef_data);
	
	port_pin_set_output_level(PIN_PA02, false);

	// Insert application code here, after the board has been initialized.
	
	//example application with cosytech data. (This data should be fetched from web/bluetooth)
	

	
	while(1){
		delay_ms(500);
		port_pin_set_output_level(PIN_PA02, true);
		delay_ms(500);
		port_pin_set_output_level(PIN_PA02, false);
	}
}

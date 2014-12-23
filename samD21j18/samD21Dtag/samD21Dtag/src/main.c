/**
 * \file
 *
 * \brief Fill this in later for documentation
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <delay.h>
#include "RF430CL330H.h"

struct oled_instance {
	//! Pin identifier for first LED
	uint8_t led1_pin;
	//! Pin identifier for second LED
	uint8_t led2_pin;
	//! Pin identifier for third LED
	uint8_t led3_pin;
	//! Pin identifier for first button
	uint8_t button1_pin;
	//! Pin identifier for second button
	uint8_t button2_pin;
	//! Pin identifier for third button
	uint8_t button3_pin;
};

const struct oled_instance oled = {          
	.led1_pin = EXT3_PIN_7,          //PIN_PA12
	.led2_pin = EXT3_PIN_8,          //
	.led3_pin = EXT3_PIN_6,         
	.button1_pin = EXT3_PIN_9,      //PIN_PA28
	.button2_pin = EXT3_PIN_3,      //PIN_PA02
	.button3_pin = EXT3_PIN_4,      //PIN_PA03
};

//
volatile uint8_t menu_selection;

/* Global test variables */
bool pin_state1;
bool pin_state2;
bool pin_state3;
 
//Maybe include this in a header file?
void configure_usart(void);
void configure_port_pins(void);
void configure_i2c_master(void);
void configure_extint_channel(void);
void configure_extint_callbacks(void);
void extint_detection_callback(void);
void extint_detection_callback_line_2(void);
uint16_t rf430_i2c_read_register(uint16_t reg_addr);
void rf430_i2c_config(void);

//Initial software module instances of USART
struct usart_module usart_instance;
//Sets USART to the EDBG virtual COM port of SAMD21 Xpro

void configure_usart(void){
	
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	
	config_usart.baudrate    = 9600;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	
	while (usart_init(&usart_instance,
	EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {}
		
	usart_enable(&usart_instance);
}

// Initial software module instances of I2C
struct i2c_master_module i2c_master_instance;
// Initialize and config I2C on SERCOM2 bus

void configure_i2c_master(void){
	/* Initialize config structure and software module. */
	struct i2c_master_config config_i2c_master;
	
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.buffer_timeout = 10000;

	/* Initialize and enable device with config. */
	i2c_master_init(&i2c_master_instance, SERCOM2, &config_i2c_master);
	i2c_master_enable(&i2c_master_instance);
}
void configure_port_pins(void){
	
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	config_port_pin.input_pull = PORT_PIN_PULL_NONE;
	
	port_pin_set_config(PIN_PB00, &config_port_pin);
	
	// OLED pin setup
	port_pin_set_output_level(oled.led1_pin, 1);
	port_pin_set_output_level(oled.led2_pin, 1);
	port_pin_set_output_level(oled.led3_pin, 1);
	
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	config_port_pin.input_pull = PORT_PIN_PULL_NONE;
	
	port_pin_set_config(oled.led1_pin, &config_port_pin);
	port_pin_set_config(oled.led2_pin, &config_port_pin);
	port_pin_set_config(oled.led3_pin, &config_port_pin);
	
	config_port_pin.direction  = PORT_PIN_DIR_INPUT;
	config_port_pin.input_pull = PORT_PIN_PULL_UP;
	
	port_pin_set_config(oled.button1_pin, &config_port_pin);
	port_pin_set_config(oled.button2_pin, &config_port_pin);
	port_pin_set_config(oled.button3_pin, &config_port_pin);

}
void configure_extint_channel(void){
	struct extint_chan_conf config_extint_chan;
	
	config_extint_chan.gpio_pin			= PIN_PA02A_EIC_EXTINT2; //oled.button2_eic_pin;
	config_extint_chan.gpio_pin_mux		= MUX_PA02A_EIC_EXTINT2; 

	config_extint_chan.gpio_pin_pull      = EXTINT_PULL_UP;
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	
	extint_chan_set_config(2, &config_extint_chan); // PIN_PA02_EIC_LINE = EXINT(N) = 2
	
	config_extint_chan.gpio_pin			= PIN_PA28A_EIC_EXTINT8; //oled.button1_eic_pin;
	config_extint_chan.gpio_pin_mux		= MUX_PA28A_EIC_EXTINT8; 
	
	extint_chan_set_config(8, &config_extint_chan); // PIN_PA28_EIC_LINE = EXINT(N) = 8
	
	config_extint_chan.gpio_pin			= PIN_PA03A_EIC_EXTINT3; //oled.button1_eic_pin;
	config_extint_chan.gpio_pin_mux		= MUX_PA03A_EIC_EXTINT3;
	
	extint_chan_set_config(3, &config_extint_chan); // PIN_PA03_EIC_LINE = EXINT(N) = 3

}
void configure_extint_callbacks(void){
	extint_register_callback(extint_detection_callback_line_2, 2 ,EXTINT_CALLBACK_TYPE_DETECT); // PIN_PA02_EIC_LINE = EXINT(N) = 2
	extint_register_callback(extint_detection_callback, 8 ,EXTINT_CALLBACK_TYPE_DETECT); // PIN_PA28_EIC_LINE = EXINT(N) = 8
	//extint_register_callback(extint_detection_callback, 3 ,EXTINT_CALLBACK_TYPE_DETECT); // PIN_PA03_EIC_LINE = EXINT(N) = 3
	
	//enable callbacks
	extint_chan_enable_callback(2, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(8, EXTINT_CALLBACK_TYPE_DETECT);
	//extint_chan_enable_callback(3, EXTINT_CALLBACK_TYPE_DETECT);
}
void extint_detection_callback(void)
{
	uint8_t tmp = menu_selection;
	uint8_t string[] = "Menu selector has increased\n\r";
	uint8_t string2[] = "Menu selector has decreased\n\r";
	
	pin_state1 = port_pin_get_input_level(oled.button1_pin);
	port_pin_set_output_level(oled.led1_pin, pin_state1);
	
	pin_state2 = port_pin_get_input_level(oled.button2_pin);
	port_pin_set_output_level(oled.led2_pin, pin_state2);
	
	pin_state3 = port_pin_get_input_level(oled.button3_pin);
	port_pin_set_output_level(oled.led3_pin, pin_state3);
	
	if(!pin_state1){tmp++;
		usart_write_buffer_wait(&usart_instance, string, sizeof(string));
	}
	else if(!pin_state2){tmp--;
		usart_write_buffer_wait(&usart_instance, string2, sizeof(string2));}
	
	if((tmp > 3) || (tmp < 1)){
		tmp = menu_selection;
	}
	
	if (tmp != menu_selection){
		gfx_mono_generic_draw_filled_rect(0, menu_selection*8,GFX_MONO_LCD_WIDTH,8,GFX_PIXEL_XOR);
		menu_selection = tmp;
		gfx_mono_generic_draw_filled_rect(0, menu_selection*8,GFX_MONO_LCD_WIDTH,8,GFX_PIXEL_XOR);
	}
		
}
void extint_detection_callback_line_2(void){
	uint8_t string[] = "External interrupt on line 2 is run \n\r";
	uint8_t string2[] = "Menu selector has decreased\n\r";
	usart_write_buffer_wait(&usart_instance, string, sizeof(string));
	
	uint8_t tmp = menu_selection;
	pin_state2 = port_pin_get_input_level(oled.button2_pin);
	port_pin_set_output_level(oled.led2_pin, pin_state2);
	
	if(!pin_state2){
		tmp--;
		usart_write_buffer_wait(&usart_instance, string2, sizeof(string2));
	}
	
	if((tmp > 3) || (tmp < 1)){
		tmp = menu_selection;
	}
	
	if (tmp != menu_selection){
		gfx_mono_generic_draw_filled_rect(0, menu_selection*8,GFX_MONO_LCD_WIDTH,8,GFX_PIXEL_XOR);
		menu_selection = tmp;
		gfx_mono_generic_draw_filled_rect(0, menu_selection*8,GFX_MONO_LCD_WIDTH,8,GFX_PIXEL_XOR);
	}
}
//[OLED] code comment out if you don't have it connected
static void menu_draw_menu(uint8_t menu_select)
{
	gfx_mono_draw_string("CosyTech NFC tag", 0, 0, &sysfont);
	gfx_mono_draw_string("+Web Demo", 3, 8, &sysfont);
	gfx_mono_draw_string("+Ndef Demo", 3, 16, &sysfont);
	gfx_mono_draw_string("+Read NFC", 3, 24, &sysfont);
	gfx_mono_generic_draw_filled_rect(0,menu_select*8,GFX_MONO_LCD_WIDTH,8,GFX_PIXEL_XOR);
}

//Prototype from the Arduino library, see if it can't be done a better way with only ASF I2C calls
#define DATA_LENGTH 2
#define TIMEOUT 1000

uint16_t rf430_i2c_read_register(uint16_t reg_addr){
	
	uint16_t timeout = 0;
	static uint8_t read_buffer[DATA_LENGTH];
	uint8_t tx_addr[DATA_LENGTH] = {0,0};
	uint8_t rx_data[DATA_LENGTH] = {0,0};
	
	tx_addr[0] = reg_addr >> 8;      // MSB of address
	tx_addr[1] = reg_addr & 0xFF;
	
	struct i2c_master_packet packet = {
		.address     = RF430_I2C_SLAVE_ADDRESS,
		.data_length = DATA_LENGTH,
		.data        = tx_addr,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};
	
	while (i2c_master_write_packet_wait(&i2c_master_instance, &packet) != STATUS_OK) {
		/* Increment timeout counter and check if timed out. */
		if (timeout++ == TIMEOUT) {
			break;
		}
	}
	
	delay_ms(500);
	
	packet.data = read_buffer;
	
	while (i2c_master_read_packet_wait(&i2c_master_instance, &packet)  != STATUS_OK) {
		/* Increment timeout counter and check if timed out. */
		if (timeout++ == TIMEOUT) {
			break;
		}
	}
	
	rx_data[0] = packet.data[0];
	rx_data[1] = packet.data[1];
	
	return (rx_data[1] << 8 | rx_data[0]);
	
	
}
#define _RESET PIN_PA20
void rf430_i2c_config(void){
	uint16_t version;
	port_pin_set_output_level(_RESET, 1);
	port_pin_set_output_level(_RESET, 0);
	delay_ms(100);
	port_pin_set_output_level(_RESET, 1);
	delay_ms(1000);
	
	while(!(rf430_i2c_read_register(STATUS_REG) & READY));
	
	version = rf430_i2c_read_register(VERSION_REG);
	char version_buffer[50];
	sprintf(version_buffer, "Firmware version :0x%x\n", version);
	//uint8_t string[sizeof(version_buffer)] = version_buffer;
	usart_write_buffer_wait(&usart_instance, version_buffer, sizeof(version_buffer));
	
}


int main (void)
{
	menu_selection = 1;
	uint8_t test_string[] = "Hello are you Cosy?\n\r";
	
	system_init();
	// Insert application code here, after the board has been initialized.
	configure_usart();
	configure_i2c_master();
	configure_port_pins();
	configure_extint_channel();
	configure_extint_callbacks();
	
	gfx_mono_init();
	menu_draw_menu(menu_selection);


	system_interrupt_enable_global();
	while (1) {
		// Is button pressed?
		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			// Yes, so turn LED on.
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
		} else {
			// No, so turn LED off.
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		}
	}
}

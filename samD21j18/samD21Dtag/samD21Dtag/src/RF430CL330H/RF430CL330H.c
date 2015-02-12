/*
 * RF430CL330H.c
 *
 * Created: 09.02.2015 19:03:22
 *  Author: Sigvart
 */ 

#include <asf.h>
#include <stdio_serial.h>
#include <RF430CL330H/RF430CL330H.h>

//static struct usart_module cdc_uart_module;

/* static void configure_console(void)
{
	struct usart_config usart_conf;
	usart_conf.baudrate    = 9600;
	usart_conf.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING; //USART_RX_1_TX_0_XCK_1
	usart_conf.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0; //PINMUX_PA22C_SERCOM3_PAD0
	usart_conf.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1; //PINMUX_PA23C_SERCOM3_PAD1
	usart_conf.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2; //PINMUX_UNUSED
	usart_conf.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3; //PINMUX_UNUSED
	stdio_serial_init(&cdc_uart_module, EDBG_CDC_MODULE, &usart_conf);
	usart_enable(&cdc_uart_module);
}*/


void configure_i2c_master(void);

// Initial software module instances of I2C
struct i2c_master_module i2c_master_instance;
// Initialize and config I2C on SERCOM2 bus

void configure_i2c_master(void){
	/* Initialize config structure and software module. */
	struct i2c_master_config config_i2c_master;
	
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.buffer_timeout = 10000;
	
	config_i2c_master.pinmux_pad0 = PINMUX_PA16C_SERCOM1_PAD0;
	config_i2c_master.pinmux_pad1 = PINMUX_PA17C_SERCOM1_PAD1;

	/* Initialize and enable device with config. */
	i2c_master_init(&i2c_master_instance, SERCOM1, &config_i2c_master);
	//i2c_master_reset(&i2c_master_instance);
	i2c_master_enable(&i2c_master_instance);
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
	
	//usart_write_buffer_wait(&usart_instance, "Starting to write packet\n\r", sizeof("Starting to write packet\n\r"));
	
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
			//usart_write_buffer_wait(&usart_instance, "WRITE TIMEOUT\n\r", sizeof("TIMEOUT\n\r"));
			break;
		}
	}
	
	//delay_ms(500);
	
	packet.data = read_buffer;
	
	while (i2c_master_read_packet_wait(&i2c_master_instance, &packet)  != STATUS_OK) {
		/* Increment timeout counter and check if timed out. */
		if (timeout++ == TIMEOUT) {
			//usart_write_buffer_wait(&usart_instance, "READ TIMEOUT\n\r", sizeof("TIMEOUT\n\r"));
			break;
		}
	}
	
	rx_data[0] = packet.data[0];
	rx_data[1] = packet.data[1];
	
	return (rx_data[1] << 8 | rx_data[0]);	
}

void rf430_i2c_write_register(uint16_t reg_addr, uint16_t val){
	uint16_t timeout = 0;
	uint8_t tx_addr_data[3] = {0};
	
	tx_addr_data[0] = reg_addr >> 8;
	tx_addr_data[1] = reg_addr & 0xFF;
	tx_addr_data[3] = val >> 8;
	tx_addr_data[2] = val & 0xFF;
	
	struct i2c_master_packet packet = {
		.address     = RF430_I2C_SLAVE_ADDRESS,
		.data_length = 4,
		.data        = tx_addr_data,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};
	
	while (i2c_master_write_packet_wait(&i2c_master_instance, &packet) != STATUS_OK) {
		/* Increment timeout counter and check if timed out. */
		if (timeout++ == TIMEOUT) {
			//usart_write_buffer_wait(&usart_instance, "WRITE TIMEOUT\n\r", sizeof("TIMEOUT\n\r"));
			break;
		}
	}
	
	//usart_write_buffer_wait(&usart_instance, "WRITE SUCCESS\n\r", sizeof("WRITE SUCCESS\n\r"));
}

void rf430_i2c_write_continous(uint16_t reg_addr, uint8_t* write_data, const uint16_t data_length){
	uint16_t timeout = 0;
	uint8_t tx_addr_data[2 + data_length];
	
	tx_addr_data[0] = reg_addr >> 8;      // MSB of address
	tx_addr_data[1] = reg_addr & 0xFF;
	
	for(int i = 2; i<data_length+2; i++){
		tx_addr_data[i] = write_data [i-2];
	};
	
	struct i2c_master_packet packet = {
		.address     = RF430_I2C_SLAVE_ADDRESS,
		.data_length = DATA_LENGTH+data_length,
		.data        = tx_addr_data,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};
	
	while (i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &packet) != STATUS_OK) {
		/* Increment timeout counter and check if timed out. */
		if (timeout++ == TIMEOUT) {
			//usart_write_buffer_wait(&usart_instance, "WRITE TIMEOUT\n\r", sizeof("TIMEOUT\n\r"));
			break;
		}
	}
	i2c_master_send_stop(&i2c_master_instance);
	//usart_write_buffer_wait(&usart_instance, "CONTINOUS DATA WRITE SUCCESS\n\r", sizeof("DATA WRITE SUCCESS\n\r"));
}

void rf430_i2c_config(void){
	uint16_t version = 0;
	//usart_write_buffer_wait(&usart_instance, "Starting config\n\r", sizeof("Starting config\n\r"));
	
	while(!(rf430_i2c_read_register(STATUS_REG) & READY));
	//usart_write_buffer_wait(&usart_instance, "STATUS: READY\n\r", sizeof("STATUS: READY\n\r"));
	
	version = rf430_i2c_read_register(VERSION_REG);
	char version_buffer[50] = {0};
	sprintf(version_buffer, "Firmware version :0x%x \n\r", version);
	//usart_write_buffer_wait(&usart_instance, version_buffer, sizeof(version_buffer));
	
	if (version == 0x0101 || version == 0x0201)
	{ // the issue exists in these two versions
		rf430_i2c_write_register(TEST_MODE_REG, TEST_MODE_KEY);
		rf430_i2c_write_register(CONTROL_REG, 0x0080);
		if (version == 0x0101)
		{ // Ver C
			rf430_i2c_write_register(0x2a98, 0x0650);
			//usart_write_buffer_wait(&usart_instance, "Version C Registred\n\r", sizeof("Version C Registred\n\r"));
		}
		else
		{ // Ver D
			rf430_i2c_write_register(0x2a6e, 0x0650);
		}
		rf430_i2c_write_register(0x2814, 0);
		rf430_i2c_write_register(TEST_MODE_REG, 0);
	}
	
	//Enable interrupts for End of Read and End of Write
	rf430_i2c_write_register(INT_ENABLE_REG, EOW_INT_ENABLE + EOR_INT_ENABLE);

	//Configure INTO pin for active low and enable RF
	rf430_i2c_write_register(CONTROL_REG, (INT_ENABLE + INTO_DRIVE + RF_ENABLE) );
	
}
void rf430_i2c_write_ndef(uint8_t *NDEF_Application_Data, uint16_t length){
	
	//configure_console();
	
	//printf("Found %x \r\n", NDEF_Application_Data);
	rf430_i2c_write_register(INT_ENABLE_REG, rf430_i2c_read_register(CONTROL_REG) | ~EOR_INT_ENABLE);
	
	rf430_i2c_write_continous(0, NDEF_Application_Data, length);

	//Enable interrupts for End of Read and End of Write
	rf430_i2c_write_register(INT_ENABLE_REG, EOW_INT_ENABLE + EOR_INT_ENABLE);

	//Configure INTO pin for active low and enable RF
	rf430_i2c_write_register(CONTROL_REG, (INT_ENABLE + INTO_DRIVE + RF_ENABLE) );
}
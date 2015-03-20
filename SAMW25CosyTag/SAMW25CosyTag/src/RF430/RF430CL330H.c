/*
 * RF430CL330H.c
 *
 * Created: 05.02.2015 23:36:40
 *  Author: Sigvart
 */ 


#include <asf.h>
#include "config/conf_twi.h"
#include "RF430/RF430CL330H.h"

#define _RESET PIN_PB11
void configure_reset_pin(void);

void configure_reset_pin(void){
	
	//struct port_config pin_conf;
	struct port_config config_port_pin;
	
	//port_get_config_defaults(&pin_conf);
	port_get_config_defaults(&config_port_pin);
	
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	config_port_pin.input_pull = PORT_PIN_PULL_NONE;
	
	port_pin_set_config(_RESET, &config_port_pin);
	
	port_pin_set_output_level(_RESET, 1);
	port_pin_set_output_level(_RESET, 0);
	delay_ms(100);
	port_pin_set_output_level(_RESET, 1);
}

void rf430_configure_i2c_master(void);
struct i2c_master_module i2c_master_instance;
// Initialize and config I2C on SERCOM0 bus

void rf430_configure_i2c_master(void){
	/* Initialize config structure and software module. */
	struct i2c_master_config config_i2c_master;
	
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.buffer_timeout = 10000;
	
	config_i2c_master.pinmux_pad0 = SAMW25_SDA;
	config_i2c_master.pinmux_pad1 = SAMW25_SCL;

	/* Initialize and enable device with config. */
	i2c_master_init(&i2c_master_instance, SERCOM1, &config_i2c_master);
	i2c_master_enable(&i2c_master_instance);
}



uint16_t rf430_i2c_read_register(uint16_t reg_addr){
	
	#define DATA_LENGTH 2
	#define TIMEOUT 1000
	
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
		if (timeout++ == TIMEOUT){
			break;
		}
	}
	
	packet.data = read_buffer;
	
	while (i2c_master_read_packet_wait(&i2c_master_instance, &packet)  != STATUS_OK) {
		if (timeout++ == TIMEOUT) {
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
	tx_addr_data[2] = val & 0xFF;
	tx_addr_data[3] = val >> 8;
	
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
			break;
		}
	}
};

void rf430_i2c_write_continous(uint16_t reg_addr, uint8_t* write_data, uint16_t data_length){
	uint16_t timeout = 0;
	uint8_t tx_addr_data[2 + data_length];
	
	//Register address(think maybe this bit shifting stuff can be avoided need to be tested)
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
			break;
		}
	}
	
	i2c_master_send_stop(&i2c_master_instance); //not sure if needed
}

void rf430_init(void){
	printf("RF430 initialization begining \r\n");
	uint16_t version = 0;
	
	configure_reset_pin();
	printf("Reset initialization done \r\n");
	rf430_configure_i2c_master();
	printf("Configure i2c initialization done \r\n");
	
	//while(!(rf430_i2c_read_register(STATUS_REG) & READY));
	version = rf430_i2c_read_register(VERSION_REG);
	printf("Read register done \r\n");
	
	//Bug fix errat r430cl330h datasheet
	if (version == 0x0101 || version == 0x0201){
		rf430_i2c_write_register(0x2a98, 0x0650);
		rf430_i2c_write_register(TEST_MODE_REG, TEST_MODE_KEY);
		rf430_i2c_write_register(CONTROL_REG, 0x0080);
		
		if (version == 0x0101)
		{ // Ver C
			rf430_i2c_write_register(0x2a98, 0x0650);
		}
		else
		{ // Ver D
			rf430_i2c_write_register(0x2a6e, 0x0650);
		}
		
		rf430_i2c_write_register(0x2814, 0);
		rf430_i2c_write_register(TEST_MODE_REG, 0);
		
	}
	/*
	//Enable interrupts for End of Read and End of Write
	rf430_i2c_write_register(INT_ENABLE_REG, EOW_INT_ENABLE + EOR_INT_ENABLE);
	*/
	//Configure INTO pin for active low and enable RF
	rf430_i2c_write_register(CONTROL_REG, (INT_ENABLE + INTO_DRIVE + RF_ENABLE));
	
}

void rf430_write_ndef(uint8_t *NDEF_application_data, uint16_t length){
	
	//rf430_i2c_write_register(INT_ENABLE_REG, rf430_i2c_read_register(CONTROL_REG) | ~EOR_INT_ENABLE);
	
	rf430_i2c_write_continous(0, NDEF_application_data, length);
	
	//Enable interrupts for End of Read and End of Write
	//rf430_i2c_write_register(INT_ENABLE_REG, EOW_INT_ENABLE + EOR_INT_ENABLE);

	//Configure INTO pin for active low and enable RF
	//rf430_i2c_write_register(CONTROL_REG, (INT_ENABLE + INTO_DRIVE + RF_ENABLE));
}


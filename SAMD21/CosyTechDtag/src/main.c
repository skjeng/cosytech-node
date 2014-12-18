/**

 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <asf.h>
#include "RF430CL330H.h"

//! [address]
#define SLAVE_ADDRESS RF430_I2C_ADDR
//! [address]

/* Init i2c packet. */
//! [packet]
struct i2c_master_packet packet = {
	.address     = SLAVE_ADDRESS,
	.data_length = DATA_LENGTH,
	.data        = write_buffer,
	.ten_bit_address = false,
	.high_speed      = false,
	.hs_master_code  = 0x0,
};
//! [packet]
//! [init]

char TxAddr[2] = {0};
char TxData[2] = {0};
char RxData[2] = {0};
char cmd[0];

uint16_t flags = 0;
uint16_t version;

packet.address = addr

void Write_Register(uint16_t reg_addr, uint16_t data){
	
	TxAddr[0] = reg_addr >> 8;
	TxAddr[1] = reg_addr & 0xff;
	
	TxData[0] = data >> 8;
	TxData[1] = data & 0xff;
	
	packet.address = (TxAddr[1] << 8 | TxAddr[0]);
	/* Write buffer to slave until success. */
	//! [write_packet]
	while (i2c_master_write_packet_wait(&i2c_master_instance, &packet) != STATUS_OK) {
		/* Increment timeout counter and check if timed out. */
		if (timeout++ == TIMEOUT) {
			break;
		}
	}
	//! [write_packet]
}

uint16_t Read_Register(uint16_t reg_addr){
	uint16_t data;

	TxAddr[0] = reg_addr >> 8;
	TxAddr[1] = reg_addr & 0xff;
	
	i2c.write((SLAVE_ADDRESS<<1), TxAddr, 2);
	rxled=1;
	wait(0.5);
	rxled=0;
	i2c.read((SLAVE_ADDRESS<<1), RxData, 2);
	
	//pc.printf("ReadData[%x]:0x%x\n",reg_addr,RxData[1]<<8| RxData[0]);
	i2c.stop();
	data = RxData[1] << 8 | RxData[0];
	return data;
}


void init_rf430(void){
	_reset = 1;
	_reset = 0;
	wait(0.1f);
	_reset = 1;
	
	while(!(Read_Register(STATUS_REG)& READY));
	pc.printf("Fireware Version:");
	version = Read_Register(VERSION_REG);
	pc.printf("0x%x\n",version);
	
	if (version == 0x0101 || version == 0x0201){
		// the issue exists in these two versions
		//Write_Register(0xFFE0, 0x004E);
		cmd[0] = 0x004E;
		i2c.write(0xFFE0<<1,cmd,1);
		//Write_Register(0xFFFE, 0x0080,1);
		cmd[0] = 0x0080;
		i2c.write(0xFFFE<<1,cmd,1);
		if (version == 0x0101) { // Ver C
			//Write_Register(0x2a98, 0x0650);
			cmd[0] = 0x0650;
			i2c.write(0x2a98<<1,cmd,1);
		}
		else{ // Ver D
			//Write_Register(0x2a6e, 0x0650);
			cmd[0] = 0x0650;
			i2c.write(0x2a6e<<1, cmd ,1);
		}
		//Write_Register(0x2814, 0);
		cmd[0] = 0;
		i2c.write(0x2814<<1, cmd ,1);
		//Write_Register(0xFFE0, 0);
		i2c.write(0xFFE0<<1, cmd ,1);
	}

}

//! [packet_data]
#define DATA_LENGTH 10
static uint8_t write_buffer[DATA_LENGTH] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
};

static uint8_t read_buffer[DATA_LENGTH];
//! [packet_data]



/* Number of times to try to send packet if failed. */
//! [timeout]
#define TIMEOUT 1000
//! [timeout]

/* Init software module. */
//! [dev_inst]
struct i2c_master_module i2c_master_instance;
//! [dev_inst]

void configure_i2c_master(void);

//! [initialize_i2c]
void configure_i2c_master(void)
{
	/* Initialize config structure and software module. */
	//! [init_conf]
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	//! [init_conf]

	/* Change buffer timeout to something longer. */
	//! [conf_change]
	config_i2c_master.buffer_timeout = 10000;
	//! [conf_change]

	/* Initialize and enable device with config. */
	//! [init_module]
	i2c_master_init(&i2c_master_instance, SERCOM2, &config_i2c_master);
	//! [init_module]

	//! [enable_module]
	i2c_master_enable(&i2c_master_instance);
	//! [enable_module]
}
//! [initialize_i2c]

int main(void)
{
	system_init();

	//! [init]
	/* Configure device and enable. */
	//! [config]
	configure_i2c_master();
	//! [config]

	/* Timeout counter. */
	//! [timeout_counter]
	uint16_t timeout = 0;
	//! [timeout_counter]

	//! [main]


	/* Read from slave until success. */
	//! [read_packet]
	packet.data = read_buffer;
	while (i2c_master_read_packet_wait(&i2c_master_instance, &packet) !=
			STATUS_OK) {
		/* Increment timeout counter and check if timed out. */
		if (timeout++ == TIMEOUT) {
			break;
		}
	}
	//! [read_packet]

	//! [main]

	while (true) {
		/* Infinite loop */
	}

}

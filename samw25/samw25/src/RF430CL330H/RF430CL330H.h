/*
 * RF430CL330H.h
 *
 * Copyright (c) 2014 CosyTech. All rights reserved.
 *  Author: Sigvart
 *  RF430 User Address Map
 *  -----------------------------------------
 *  Address     | Size  | Description       |
 *  -----------------------------------------
 *  0xFFFE      | 2B    | Control Register  |
 *  0xFFFC      | 2B    | Status Register   |
 *  0xFFFA      | 2B    | Interrupt Enable  |
 *  0xFFF8      | 2B    | Interrupt Flags   |
 *  0xFFF6      | 2B    | CRC Result        |
 *  0xFFF4      | 2B    | CRC Length        |
 *  0xFFF2      | 2B    | CRC Start Address |
 *  0xFFF0      | 2B    | Comm WD Ctrl Reg  |
 *  -----------------------------------------
 *  0x0000 -    | 2kB   | NDEF App Memory   |
 *    0x07FF    |       |                   |
 *
 *
 *
 *					[Slave]					[Host/master]
 *                   RF430						SAMD21
 *             _________________           _________________
 *            |              SDA|<-------=>|PA08            |
 *            |                 |   I2C    |                |
 *            |              SCL|<=------->|PA09            |
 *            |                 |          |                |
 *      GND<--|E(2-0)       /RST|<---------|PA10            |
 *            |             INTO|--------->|PA11            |
 *            |                 |          |                |
 *            |                 |          |                |
 *            |                 |          |                |
 *            |_________________|          |________________|
 *
 */ 


#ifndef RF430CL330H_H_
#define RF430CL330H_H_

#define RF430_I2C_ADDRESS                   (0x50 >> 1)
#define RF430_I2C_SLAVE_ADDRESS				(0x28)
#define RF430_I2C_READBIT                   (0x01)
#define RF430_I2C_BUSY                      (0x00)
#define RF430_I2C_READY                     (0x01)
#define RF430_I2C_READYTIMEOUT              (20)

#define CONTROL_REG         0xFFFE
#define STATUS_REG          0xFFFC
#define INT_ENABLE_REG      0xFFFA
#define INT_FLAG_REG        0xFFF8
#define CRC_RESULT_REG      0xFFF6
#define CRC_LENGTH_REG      0xFFF4
#define CRC_START_ADDR_REG  0xFFF2
#define COMM_WD_CTRL_REG    0xFFF0
#define VERSION_REG         0xFFEE //Firmware version in ROM
#define TEST_FUNCTION_REG   0xFFE2
#define TEST_MODE_REG       0xFFE0
#define TEST_MODE_KEY       0x004E

#define BIT(_bit_)          (1 << (_bit_))
#define BIT0                0x0001
#define BIT1                0x0002
#define BIT2                0x0004
#define BIT3                0x0008
#define BIT4                0x0010
#define BIT5                0x0020
#define BIT6                0x0040
#define BIT7                0x0080
#define BIT8                0x0100
#define BIT9                0x0200
#define BIT10               0x0400
#define BIT11               0x0800
#define BIT12               0x1000
#define BIT13               0x2000
#define BIT14               0x4000
#define BIT15               0x8000

//define the different virtual register bits
//CONTROL_REG bits
#define SW_RESET        BIT0
#define RF_ENABLE       BIT1
#define INT_ENABLE      BIT2
#define INTO_HIGH       BIT3
#define INTO_DRIVE      BIT4
#define BIP8_ENABLE     BIT5
#define STANDBY_ENABLE  BIT6
#define TEST430_ENABLE  BIT7

//STATUS_REG bits
#define READY           BIT0
#define CRC_ACTIVE      BIT1
#define RF_BUSY         BIT2

//INT_ENABLE_REG bits
#define EOR_INT_ENABLE              BIT1
#define EOW_INT_ENABLE              BIT2
#define CRC_INT_ENABLE              BIT3
#define BIP8_ERROR_INT_ENABLE       BIT4
#define NDEF_ERROR_INT_ENABLE       BIT5
#define GENERIC_ERROR_INT_ENABLE    BIT7

//INT_FLAG_REG bits
#define EOR_INT_FLAG            BIT1
#define EOW_INT_FLAG            BIT2
#define CRC_INT_FLAG            BIT3
#define BIP8_ERROR_INT_FLAG     BIT4
#define NDEF_ERROR_INT_FLAG     BIT5
#define GENERIC_ERROR_INT_FLAG  BIT7

//COMM_WD_CTRL_REG bits
#define WD_ENABLE               BIT0
#define TIMEOUT_PERIOD_2_SEC    0
#define TIMEOUT_PERIOD_32_SEC   BIT1
#define TIMEOUT_PERIOD_8_5_MIN  BIT2
#define TIMEOUT_PERIOD_MASK     BIT1 + BIT2 + BIT3

#define COSY_DEFAULT_DATA                                                              \
{                                                                                       \
	/*NDEF Tag Application Name*/                                                           \
	0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01,                                               \
	\
	/*Capability Container ID*/                                                             \
	0xE1, 0x03,                                                                             \
	0x00, 0x0F, /* CCLEN */                                                                 \
	0x20,       /* Mapping version 2.0 */                                                   \
	0x00, 0xF9, /* MLe (249 bytes); Maximum R-APDU data size */                             \
	0x00, 0xF6, /* MLc (246 bytes); Maximum C-APDU data size */                             \
	0x04,       /* Tag, File Control TLV (4 = NDEF file) */                                 \
	0x06,       /* Length, File Control TLV (6 = 6 bytes of data for this tag) */           \
	0xE1, 0x04, /* File Identifier */                                                       \
	0x0B, 0xDF, /* Max NDEF size (3037 bytes of useable memory) */                          \
	0x00,       /* NDEF file read access condition, read access without any security */     \
	0x00,       /* NDEF file write access condition; write access without any security */   \
	\
	/* NDEF File ID */                                                                      \
	0xE1, 0x04,                                                                             \
	\
	/* NDEF File for Hello World  (48 bytes total length) */                                \
	0x00, 0x14, /* NLEN; NDEF length (2 byte long message) */                               \
	0xD1, 0x01, 0x10,                                                                       \
	0x54, /* T = text */                                                                    \
	0x02,                                                                                   \
	0x65, 0x6E, /* 'e', 'n', */                                                             \
	\
	/* 'Hello, world!' NDEF data; Empty NDEF message, length should match NLEN*/            \
	/* 'Sigvart Hovland!' NDEF data; Empty NDEF message, length should match NLEN 0x74, 0x20, 0x48, 0x6f, 0x76, 0x6c, 0x61, 0x64, 0x6e, */ \
	0x53, 0x69, 0x67, 0x76, 0x61, 0x72, 0x74, 0x21, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20          \
}

//Maybe make a callable class if you want a cleaner interface
const struct R430_I2C{
	
};

uint16_t rf430_i2c_read_register(uint16_t reg_addr);
void rf430_i2c_write_register(uint16_t reg_addr, uint16_t val); //Maybe hide this interface from main, create more sensible functions?
void rf430_i2c_write_continous(uint16_t reg_addr, uint8_t* write_data, uint16_t data_length);
<<<<<<< HEAD
void rf430_write_ndef(uint8_t *NDEF_Application_Data, uint16_t length);
>>>>>>> parent of 3159396... API change for reciving NDEF_Application_data
void rf430_init(void);

#endif /* RF430CL330H_H_ */
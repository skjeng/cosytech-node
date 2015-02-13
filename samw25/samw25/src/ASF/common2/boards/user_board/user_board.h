/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup group_common_boards
 * \defgroup user_board_group User board
 *
 * @{
 */

void system_board_init(void);

/** Name string macro */
#define BOARD_NAME                "USER_BOARD"

/** @} */

#ifdef __cplusplus
}
#endif

#define EXT3_PIN_3                PIN_PA04
#define EXT3_PIN_4                PIN_PA05
#define EXT3_PIN_5                PIN_PB22
#define EXT3_PIN_6                PIN_PB23
#define EXT3_PIN_7                PIN_PA22
#define EXT3_PIN_8                PIN_PA23
#define EXT3_PIN_9                PIN_PA06
//#define EXT3_PIN_10               PIN_PA27 NC
#define EXT3_PIN_11               PIN_PA08
#define EXT3_PIN_12               PIN_PA09
#define EXT3_PIN_13               PIN_PA01
#define EXT3_PIN_14               PIN_PA00
#define EXT3_PIN_15               PIN_PA07
#define EXT3_PIN_16               PIN_PA18
#define EXT3_PIN_17               PIN_PA16
#define EXT3_PIN_18               PIN_PA19

#define EXT3_SPI_MODULE              SERCOM5
#define EXT3_SPI_SERCOM_MUX_SETTING  SPI_SIGNAL_MUX_SETTING_E
#define EXT3_SPI_SERCOM_PINMUX_PAD0  PINMUX_PA16D_SERCOM3_PAD0 // PINMUX_PB16C_SERCOM5_PAD0
#define EXT3_SPI_SERCOM_PINMUX_PAD1  PINMUX_PB17C_SERCOM5_PAD1 //
#define EXT3_SPI_SERCOM_PINMUX_PAD2  PINMUX_PB22D_SERCOM5_PAD2
#define EXT3_SPI_SERCOM_PINMUX_PAD3  PINMUX_PB23D_SERCOM5_PAD3 //PINMUX_PA19C_SERCOM1_PAD3
#define EXT3_SPI_SERCOM_DMAC_ID_TX   SERCOM5_DMAC_ID_TX
#define EXT3_SPI_SERCOM_DMAC_ID_RX   SERCOM5_DMAC_ID_RX

#endif // USER_BOARD_H

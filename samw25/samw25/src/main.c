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
#include "winc\driver/include/m2m_wifi.h"
#include "winc\driver/source/nmasic.h"
#include <wifi.h>

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
	
	config_usart.baudrate    = 115200; //Need to find the SERCOM/pins connected to EDBG on the SAMW25
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


/** Index of scan list to request scan result. */
static uint8_t scan_request_index = 0;
/** Number of APs found. */
static uint8_t num_founded_ap = 0;
/** UART module for debug. */
static struct usart_module cdc_uart_module;
static void configure_console(void)
{
	struct usart_config usart_conf;

	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = USART_RX_1_TX_0_XCK_1;
	usart_conf.pinmux_pad0 = PINMUX_PA22C_SERCOM3_PAD0;
	usart_conf.pinmux_pad1 = PINMUX_PA23C_SERCOM3_PAD1;
	usart_conf.pinmux_pad2 = PINMUX_UNUSED;
	usart_conf.pinmux_pad3 = PINMUX_UNUSED;
	usart_conf.baudrate    = 115200;

	stdio_serial_init(&cdc_uart_module, SERCOM3, &usart_conf);
	usart_enable(&cdc_uart_module);
}


static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
	case M2M_WIFI_RESP_SCAN_DONE:
	{
		tstrM2mScanDone *pstrInfo = (tstrM2mScanDone *)pvMsg;
		scan_request_index = 0;
		if (pstrInfo->u8NumofCh >= 1) {
			m2m_wifi_req_scan_result(scan_request_index);
			scan_request_index++;
		} else {
			m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
		}

		break;
	}

	case M2M_WIFI_RESP_SCAN_RESULT:
	{
		tstrM2mWifiscanResult *pstrScanResult = (tstrM2mWifiscanResult *)pvMsg;
		uint16_t demo_ssid_len;
		uint16_t scan_ssid_len = strlen((const char *)pstrScanResult->au8SSID);

		/* display founded AP. */
		printf("[%d] SSID:%s\r\n", scan_request_index, pstrScanResult->au8SSID);

		num_founded_ap = m2m_wifi_get_num_ap_found();
		if (scan_ssid_len) {
			/* check same SSID. */
			demo_ssid_len = strlen((const char *)MAIN_WLAN_SSID);
			if
			(
				(demo_ssid_len == scan_ssid_len) &&
				(!memcmp(pstrScanResult->au8SSID, (uint8_t *)MAIN_WLAN_SSID, demo_ssid_len))
			) {
				/* A scan result matches an entry in the preferred AP List.
				 * Initiate a connection request.
				 */
				printf("Found %s \r\n", MAIN_WLAN_SSID);
				m2m_wifi_connect((char *)MAIN_WLAN_SSID,
						sizeof(MAIN_WLAN_SSID),
						MAIN_WLAN_AUTH,
						(void *)MAIN_WLAN_PSK,
						M2M_WIFI_CH_ALL);
				break;
			}
		}

		if (scan_request_index < num_founded_ap) {
			m2m_wifi_req_scan_result(scan_request_index);
			scan_request_index++;
		} else {
			printf("can not find AP %s\r\n", MAIN_WLAN_SSID);
			m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
		}

		break;
	}

	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
		if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
			m2m_wifi_request_dhcp_client();
		} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
			printf("Wi-Fi disconnected\r\n");

			/* Request scan. */
			m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
		}

		break;
	}

	case M2M_WIFI_REQ_DHCP_CONF:
	{
		uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
		printf("Wi-Fi connected\r\n");
		printf("Wi-Fi IP is %u.%u.%u.%u\r\n",
				pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
		break;
	}

	default:
	{
		break;
	}
	}
}



int main (void)
{
	
	// broken, locks up at some point
	tstrWifiInitParam param;
	int8_t ret;
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

	/* Initialize the BSP. */
	nm_bsp_init();

	/* Initialize Wi-Fi parameters structure. */
	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));

	/* Initialize Wi-Fi driver with data and status callbacks. */
	param.pfAppWifiCb = wifi_cb;
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}

	/* Request scan. */
	m2m_wifi_request_scan(M2M_WIFI_CH_ALL);

	

	
	while(1){
		/* Handle pending events from network controller. */
		while (m2m_wifi_handle_events(NULL) != M2M_SUCCESS) {
		}
		delay_ms(500);
		port_pin_set_output_level(PIN_PA02, true);
		delay_ms(500);
		port_pin_set_output_level(PIN_PA02, false);
	}
}

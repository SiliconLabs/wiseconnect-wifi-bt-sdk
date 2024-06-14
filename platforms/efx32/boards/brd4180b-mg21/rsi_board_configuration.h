#ifndef RSI_BOARD_CONFIG_H
#define RSI_BOARD_CONFIG_H

typedef struct {
    unsigned char port;
    unsigned char pin;
} rsi_pin_t;

#define CONCAT(a, b)      a ## b
#define CONCAT3(a, b, c)  a ## b ## c
#define CONCAT_EXPAND(a, b)      CONCAT(a,b)
#define CONCAT3_EXPAND(a, b, c)  CONCAT3(a,b,c)

#define PIN(port_id, pin_id)  (rsi_pin_t){.port=gpioPort##port_id, .pin=pin_id}

//! For 9117 EVK swap below pin configuration
#ifdef EXP_BOARD
#define SLEEP_CONFIRM_PIN   PIN(B, 1)
#ifndef LOGGING_STATS
#define WAKE_INDICATOR_PIN  PIN(B, 0)
#else   /* LOGGING_STATS */
#define LOGGING_WAKE_INDICATOR_PIN  PIN(B, 0)
#define LOGGING_STATS_PORT    gpioPortB
#define LOGGING_STATS_PIN     00
#endif  /* LOGGING_STATS */
#else   /* EXP_BOARD */
#define SLEEP_CONFIRM_PIN   PIN(B, 0)
#ifndef LOGGING_STATS // 9116
#define WAKE_INDICATOR_PIN  PIN(B, 1)
#else   /* LOGGING_STATS // 9116 */
#define LOGGING_WAKE_INDICATOR_PIN  PIN(B, 1)
#define LOGGING_STATS_PORT    gpioPortB
#define LOGGING_STATS_PIN     01
#endif  /* LOGGING_STATS // 9116 */
#endif  /* EXP_BOARD */

#define RESET_PIN           PIN(D, 2)
#define INTERRUPT_PIN       PIN(D, 3)

#define SPI_CLOCK_PIN PIN(C, 2)
#define SPI_MOSI_PIN  PIN(C, 0)
#define SPI_MISO_PIN  PIN(C, 1)
#define SPI_CS_PIN    PIN(C, 3)




#define RX_LDMA_CHANNEL   0
#define TX_LDMA_CHANNEL   1

#define BRD4180B_CLI_ENABLED                    0            //Enable this to use CLI using UARTDRV on brd4180b board
#define BRD4180B_DEBUG_PRINTS_PERIPHERAL        USART1
#define BRD4180B_DEBUG_PRINTS_PERIPHERAL_NO     1
#define BRD4180B_DEBUG_PRINTS_TX_PIN            PIN(A, 0)
#define BRD4180B_CLI_DEBUG_PRINTS_ENABLED       0            //Enable this for debug prints on USART1
#define BRD4180B_CLI_USE_VCOM                   0            //Enable this to use uart cli over vcom, Disable to use USART0 breakout pins for cli
#define DEBUG_PRINTS_CLOCK            CONCAT_EXPAND(cmuClock_USART, BRD4180B_DEBUG_PRINTS_PERIPHERAL_NO)

#define COM_PORT_PERIPHERAL        USART0
#define COM_PORT_PERIPHERAL_NO     0
#define COM_PORT_CLOCK            CONCAT_EXPAND(cmuClock_USART, COM_PORT_PERIPHERAL_NO)
#define COM_PORT_NVIC             CONCAT3_EXPAND( USART, COM_PORT_PERIPHERAL_NO, _RX_IRQn)
#define COM_PORT_TX_PIN           PIN(A, 5)
#define COM_PORT_RX_PIN           PIN(A, 6)
//#define COM_PORT_CTS_PIN          PIN(A, 4)
//#define COM_PORT_RTS_PIN          PIN(C, 1)

#define COM_PORT_BAUDRATE              115200
#define COM_PORT_PARITY                usartNoParity
#define COM_PORT_STOP_BITS             usartStopbits1
#define COM_PORT_FLOW_CONTROL_TYPE     usartHwFlowControlNone


#define SL_BOARD_ENABLE_VCOM        1
#define SL_BOARD_ENABLE_VCOM_PIN    PIN(D, 4)

/*#define RSI_SPI_INTERFACE
#define RSI_SPI_HIGH_SPEED_ENABLE*/

// Declaration of MCU type
#define RSI_ARM_CM4F

#endif // RSI_BOARD_CONFIG_H

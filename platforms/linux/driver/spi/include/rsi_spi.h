/*******************************************************************************
* @file  rsi_spi.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/**
 * @file rsi_spi.h
 * @author 
 * @version 1.0
 *
 *
 * @section DESCRIPTION
 *
 * This file contians the function prototypes of related to spi interface
 * 
 */
#include "rsi_nic.h"
#include "rsi_linux_common.h"


#define  SPI_INITIALIZATION 1
#define  SPI_INTERNAL_READ  2
#define  SPI_INTERNAL_WRITE 3
#define  AHB_MASTER_READ    4
#define  AHB_MASTER_WRITE   5
#define  AHB_SLAVE_READ     6
#define  AHB_SLAVE_WRITE    7
#define  DUMMY_WRITE      8
#define  SPI_ULP_INIT       9
#define  SPI_READ_PKT_LEN_REG 0x20

#define RSI_BUSY 1
#define RSI_FAIL -1
#define RSI_SUCCESS 0

#define SPI_STATUS_SUCCESS      0x58
#define SPI_STATUS_BUSY         0x54
#define SPI_STATUS_FAILURE      0x52
#define SPI_STATUS_ZERO         0x0

#define SSP_START_TOKEN_8   0x55
#define SSP_START_TOKEN_16  0x0055
#define SSP_START_TOKEN_32  0x00000055

#define MAX_SPI_PKT_SIZE        1600
#define RSI_SPI_BUFSIZ		6000

#define CMD_INITIALIZATION      0x15
#define CMD_READ_WRITE          0x40
#define CMD_WRITE               0x20
#define AHB_BUS_ACCESS          0x10
#define AHB_SLAVE               0x08
#define TRANSFER_LEN_16_BITS    0x04


#define SPI_MODE_REG_ADDR   0x20200008
#define RSI_RS9116_INIT_CMD 0x005c4a12
#define RSI_SPI_ACK_INTR_REG 0x41050008

#define BIT_GRAN_8      1
#define BIT_GRAN_16     2
#define BIT_GRAN_32     4

#define SPI_INTR_GPIO 17

#define ULP_RESET_REG			0x161
#define WATCH_DOG_TIMER_1		0x16c
#define WATCH_DOG_TIMER_2		0x16d
#define WATCH_DOG_DELAY_TIMER_1		0x16e
#define WATCH_DOG_DELAY_TIMER_2		0x16f
#define WATCH_DOG_TIMER_ENABLE		0x170

#define RESTART_WDT			BIT(11)
#define BYPASS_ULP_ON_WDT		BIT(1)

/* Interrupt Bit Related Macros */
#define PKT_BUFF_AVAILABLE		1
#define FW_ASSERT_IND			2


/* common registers in SPI function1 */
#define TA_SOFT_RESET_REG		0x0004
#define TA_TH0_PC_REG			0x0400
#define TA_HOLD_THREAD_REG		0x0844
#define TA_RELEASE_THREAD_REG		0x0848
#define TA_SPI_WAKE_REQUEST		0x1

#define TA_SOFT_RST_CLR			0
#define TA_SOFT_RST_SET			BIT(0)
#define TA_PC_ZERO			0
#define TA_HOLD_THREAD_VALUE		0xF
#define TA_RELEASE_THREAD_VALUE		0xF
#define TA_BASE_ADDR			0x2200
#define MISC_CFG_BASE_ADDR		0x4105



#define SWBL_REGOUT     0x4105003c
#define FW_WDT_DISABLE_REQ    0x69


#define RF_SPI_PROG_REG_BASE_ADDR 0x40080000

#define GSPI_CTRL_REG0      (RF_SPI_PROG_REG_BASE_ADDR)
#define GSPI_CTRL_REG1      (RF_SPI_PROG_REG_BASE_ADDR + 0x2)
#define GSPI_DATA_REG0      (RF_SPI_PROG_REG_BASE_ADDR + 0x4)
#define GSPI_DATA_REG1      (RF_SPI_PROG_REG_BASE_ADDR + 0x6)
#define GSPI_DATA_REG2      (RF_SPI_PROG_REG_BASE_ADDR + 0x8)

#define GSPI_DMA_MODE     BIT(13)

#define GSPI_2_ULP      BIT(12)
#define GSPI_TRIG     BIT(7)
#define GSPI_READ     BIT(6)
#define GSPI_RF_SPI_ACTIVE    BIT(8)

enum spi_interrupt_type {
//	BUFFER_FULL		= 0x0,
	BUFFER_AVAILABLE	= 0x2,
	FIRMWARE_ASSERT_IND	= 0x3,
	MSDU_PACKET_PENDING	= 0x4,
	UNKNOWN_INT		= 0XE
};
#define FSM_CARD_NOT_READY    0

/* Buffer status register related info */
#define PKT_BUFF_SEMI_FULL    0
#define PKT_BUFF_FULL     1
#define PKT_MGMT_BUFF_FULL    2
#define MSDU_PKT_PENDING    3


#define RSI_GET_SPI_INTERRUPT_TYPE(_I, TYPE)      \
{					   \
	TYPE =                             \
	(_I & (1 << PKT_BUFF_AVAILABLE)) ? \
	BUFFER_AVAILABLE :		   \
	(_I & (1 << MSDU_PKT_PENDING)) ?   \
	MSDU_PACKET_PENDING :              \
	(_I & (1 << FW_ASSERT_IND)) ?      \
	FIRMWARE_ASSERT_IND : UNKNOWN_INT; \
}
struct cmd_fmt_s {
	unsigned char buf[4];
};


struct receive_info {
	bool buffer_full;
	bool semi_buffer_full;
	bool mgmt_buffer_full;
	u32 mgmt_buf_full_counter;
	u32 buf_semi_full_counter;
	u8 watch_bufferfull_count;
	u32 spi_intr_status_zero;
	u32 spi_int_counter;
	u32 total_spi_msdu_pending_intr;
	u32 total_spi_unknown_intr;
	u32 buf_full_counter;
	u32 buf_available_counter;
};
struct rsi_spi_rx_q {
	u8 num_rx_pkts;
	struct sk_buff_head head;
};
struct rsi_91x_spidev {
	struct spi_device *spi_dev;
	//struct task_struct *sdio_irq_task;
	struct receive_info rx_info;
	//u32 next_read_delay;
	//u32 sdio_high_speed_enable;
	//u8 sdio_clock_speed;
	//u32 cardcapability;
	//u8 prev_desc[16];
	u32 tx_blk_size;
	u32 rx_blk_size;
	u8 *data_rcv_pkt;
	u8 write_fail;
	u8 buff_status_updated;
	struct rsi_spi_rx_q rx_q;
	struct rsi_thread rx_thread;
};

int rsi_spi_disconnect(struct spi_device *spi_dev);
int rsi_ssp_receive_start_token(PRSI_ADAPTER Adapter, u16 gran);
int rsi_dummy_write(PRSI_ADAPTER Adapter);
int rsi_spi_write(PRSI_ADAPTER Adapter, const u8 *buf, size_t len);
int rsi_spi_read(PRSI_ADAPTER Adapter, u8 *buf, size_t len);
int rsi_spi_read_write(PRSI_ADAPTER Adapter,  u8 *txbuf, u8 *rxbuf,
		u32 rx_tx_length);
int rsi_spi_read_write_reg_multiple(PRSI_ADAPTER Adapter, u8 fnum, u32 addr,
		u8 *data,u32 count);
int rsi_spi_master_reg_write(PRSI_ADAPTER Adapter, unsigned long addr,
		unsigned long data, u16 size);
int rsi_spi_master_reg_read(PRSI_ADAPTER Adapter, u32 addr, u32 *read_buf,
		u16 size);
int rsi_spi_write_register_multiple(PRSI_ADAPTER Adapter, u32 addr, u8 *data,
		u16 count);
int rsi_spi_read_register_multiple(PRSI_ADAPTER Adapter, u32 addr, u8 *data,
		u16 count);
int rsi_spi_write_register(PRSI_ADAPTER Adapter, u32 addr, u8 *data);
int rsi_spi_read_register(PRSI_ADAPTER Adapter, u32 addr, u8 *data);
int rsi_spi_host_intf_write_pkt(PRSI_ADAPTER Adapter, u8 *pkt, u32 len);
int rsi_spi_host_intf_read_pkt(PRSI_ADAPTER Adapter, u8 *pkt, u32 len);
int rsi_spi_write_multiple(PRSI_ADAPTER Adapter, u8 fnum, u32 addr, u8 *pkt,
		u32 len);
int rsi_write_data(PRSI_ADAPTER Adapter, u8 *data, u32 length, u32 address);
int rsi_spi_load_data_master_write(PRSI_ADAPTER Adapter, u32 base_address,
		u32 instructions_sz, u16 block_size, u8 *ta_firmware);
int rsi_spi_ta_reset_ops(PRSI_ADAPTER Adapter);
void rsi_spi_interrupt_handler(PRSI_ADAPTER Adapter);

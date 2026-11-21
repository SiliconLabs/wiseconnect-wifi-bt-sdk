/*******************************************************************************
* @file  rsi_sdio.h
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
 * @file rsi_sdio_intf.h
 * @author 
 * @version 1.0
 *
 *
 * @section DESCRIPTION
 *
 * This file contians the function prototypes of related to sdio interface
 * 
 */
#ifndef __rsi_SDIO_INTF__
#define __rsi_SDIO_INTF__

#include "rsi_nic.h"
#include "rsi_linux_common.h"

#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18))&&(LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,23)))
#include <linux/sdio/ctsystem.h>
#include <linux/sdio/sdio_busdriver.h>
#include <linux/sdio/_sdio_defs.h>
#include <linux/sdio/sdio_lib.h>
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/sdio_ids.h>
#endif
#include <linux/sched.h>

/* Interrupt Status Values */
typedef enum {
  SDIO_BUFFER_FULL      = 0X1,
  SDIO_BUFFER_FREE      = 0X2,
  FIRMWARE_STATUS       = 0X3,
  SDIO_DATA_PENDING     = 0X4,
  SDIO_UNKNOWN_INTERRUPT= 0XB
} INTERRUPT_TYPE;

#define SD_DATA_PENDING      (1 << 3)
#define SD_FIRMWARE_STATUS   (1 << 2)
#define SD_PKT_BUFF_EMPTY    (1 << 1)
#define SD_PKT_BUFF_FULL     (1 << 0)

#define FSM_CARD_NOT_READY    0
#define RSI_GET_INTERRUPT_TYPE(_I)              \
                (_I & (SD_PKT_BUFF_FULL)) ?        \
                SDIO_BUFFER_FULL :\
                        (_I & ( SD_PKT_BUFF_EMPTY)) ?       \
                        SDIO_BUFFER_FREE :\
                                (_I & ( SD_FIRMWARE_STATUS)) ?     \
                                FIRMWARE_STATUS:\
                                        (_I & ( SD_DATA_PENDING)) ? \
                                        SDIO_DATA_PENDING: SDIO_UNKNOWN_INTERRUPT



#define SDIO_SET_CMD52_ARG(arg,rw,func,raw,address,writedata) \
    (arg) = (((rw) & 1) << 31)           | \
            (((func) & 0x7) << 28)       | \
            (((raw) & 1) << 27)          | \
            (1 << 26)                    | \
            (((address) & 0x1FFFF) << 9) | \
            (1 << 8)                     | \
            ((writedata) & 0xFF)

#define SDIO_SET_CMD52_READ_ARG(arg,func,address) \
    SDIO_SET_CMD52_ARG(arg,0,(func),0,address,0x00)
#define SDIO_SET_CMD52_WRITE_ARG(arg,func,address,value) \
    SDIO_SET_CMD52_ARG(arg,1,(func),0,address,value)
#define SDIO_MASTER_ACCESS_MSBYTE               0x000FA
#define SDIO_MASTER_ACCESS_LSBYTE               0x000FB
#define FLASH_SIZE_ADDR               0x04000016

#define WATCH_DOG_TIMER_1		0x16c
#define WATCH_DOG_TIMER_2		0x16d
#define WATCH_DOG_DELAY_TIMER_1		0x16e
#define WATCH_DOG_DELAY_TIMER_2		0x16f
#define WATCH_DOG_TIMER_ENABLE		0x170

#define RESTART_WDT			BIT(11)
#define BYPASS_ULP_ON_WDT		BIT(1)

#ifdef USE_USB_INTF
#define RF_SPI_PROG_REG_BASE_ADDR	0x40080000
#else
#define RF_SPI_PROG_REG_BASE_ADDR	0
#endif

#define GSPI_CTRL_REG0			(RF_SPI_PROG_REG_BASE_ADDR)
#define GSPI_CTRL_REG1			(RF_SPI_PROG_REG_BASE_ADDR + 0x2)
#define GSPI_DATA_REG0			(RF_SPI_PROG_REG_BASE_ADDR + 0x4)	
#define GSPI_DATA_REG1			(RF_SPI_PROG_REG_BASE_ADDR + 0x6)
#define GSPI_DATA_REG2			(RF_SPI_PROG_REG_BASE_ADDR + 0x8)

#define GSPI_DMA_MODE                   BIT(13)

#define GSPI_2_ULP			BIT(12)
#define GSPI_TRIG			BIT(7)	
#define GSPI_READ			BIT(6)
#define GSPI_RF_SPI_ACTIVE		BIT(8)
/*CONTENT OF THIS REG IS WRITTEN TO ADDRESS IN SD_CSA_PTR*/

#define SD_REQUEST_MASTER                 0x10000

#define rsi_SDIO_BLOCK_SIZE       256
#define SDIO_CLOCK		  (50 * 10000000)

#define rsi_likely(a)            likely(a)

#define RSI_RCV_BUFFER_LEN 1600
#define FRAME_DESC_SZ 16

#define RSI_DESC_QUEUE_NUM_MASK      0x7
#define RSI_DESC_AGGR_ENAB_MASK      0x80



/* FOR SD CARD ONLY */
#define SDIO_RX_NUM_BLOCKS_REG     0x000F1
#define SDIO_FW_STATUS_REG         0x000F2
#define SDIO_NXT_RD_DELAY2         0x000F5     /* Next read delay 2 */
#define SDIO_FUN1_INT_REG          0x000F9     /* Function interrupt register*/
#define SDIO_READ_START_LVL        0x000FC
#define SDIO_READ_FIFO_CTL         0x000FD
#define SDIO_WRITE_FIFO_CTL        0x000FE
#define SDIO_WAKEUP_REG            0x000FF

#define WLAN_TX_D_Q 5
/* common registers in SDIO function1 */
#define SDIO_FUN1_INTR_CLR_REG     0x0008
#define TA_SOFT_RESET_REG          0x0004
#define TA_TH0_PC_REG              0x0400
#define TA_HOLD_THREAD_REG         0x0844	
#define TA_RELEASE_THREAD_REG      0x0848
#define TA_POLL_BREAK_STATUS_REG   0x085C

/* WLAN registers in SDIO function 1 */
#define SDIO_RF_CNTRL_REG       0x0000000C     /* Lower MAC control register-1 */
#define SDIO_LMAC_CNTRL_REG     0x00000024    /* Lower MAC control register */
#define SDIO_LMAC_LOAD_REG      0x00000020    /* Lower MAC load register */
#define SDIO_TCP_CHK_SUM        0x0000006C     /* TCP check sum enable register */

#define RF_SELECT               0x0000000c  /* RF Select Register */

/* SDIO STANDARD CARD COMMON CNT REG(CCCR) */

/*IN THESE REGISTERS EACH BIT(0-7) REFERS TO A FUNCTION */

#define CCCR_REVISION       0x00
#define SD_SPEC_REVISION    0x01
#define SD_IO_ENABLE        0x02
#define SD_IO_READY         0x03
#define SD_INT_ENABLE       0x04
#define SD_INT_PENDING      0x05
#define SD_IO_ABART         0x06
#define SD_BUS_IF_CNT       0x07
#define SD_CARD_CAPABILITY  0x08

/*PTR to CARD'S COMMON CARD INFO STRUCT(CIS):0x09-0x0B*/
#define SD_CIS_PTR          0x09 
#define SD_BUS_SUSPEND      0x0C
#define SD_FUNCTION_SELEC   0x0D
#define SD_EXEC_FLAGS       0x0E
#define SD_READY_FLAGS      0x0F
#define SD_FN0_BLK_SZ       0x10 /*FUNCTION0 BLK SIZE:0x10-0x11 */
#define SD_RESERVED         0x12 /*0x12-0xFF:reserved for future */
#define SDIO_REG_HIGH_SPEED 0x13


/* SDIO_FUN1_FIRM_LD_CTRL_REG register bits */

#define TA_SOFT_RST_CLR      0
#define TA_SOFT_RST_SET      BIT(0)
#define TA_PC_ZERO           0
#define TA_HOLD_THREAD_VALUE        0xF
#define TA_RELEASE_THREAD_VALUE     0xF
#define TA_DM_LOAD_CLR       BIT(21)
#define TA_DM_LOAD_SET       BIT(20)

/* Function prototypes */
struct rsi_osd_host_intf_operations *rsi_get_osd_host_intf_operations(void);
struct rsi_osi_host_intf_operations *rsi_get_osi_host_intf_operations(void);
struct rsi_os_intf_operations *rsi_get_os_intf_operations(void);
struct rsi_os_intf_operations *rsi_get_os_intf_operations_from_origin(void);

int read_register(PRSI_ADAPTER adapter, uint32 Addr,uint8 fun_num,uint8 *data);
int write_register(PRSI_ADAPTER adapter,uint8 reg_dmn,uint32 Addr,uint8 *data);
int host_intf_read_pkt(PRSI_ADAPTER adapter,uint8 *pkt,uint32 Len);
int host_intf_write_pkt(PRSI_ADAPTER adapter,uint8 *pkt,uint32 Len, uint8 queueno);
void send_ulp_sleep_ack_to_ta(PRSI_ADAPTER adapter);
void send_sleep_status_to_coex(uint8 qnum, uint8 ack_status);
int read_register_multiple(PRSI_ADAPTER adapter, 
                                     uint32 Addr,
                                     uint32 Count,
                                     uint8 *data );
int write_register_multiple(PRSI_ADAPTER adapter,
                                      uint32 Addr,
                                      uint8 *data,
                                      uint32 Count);
int ack_interrupt(PRSI_ADAPTER adapter,uint8 INT_BIT);
int register_driver(void);
void unregister_driver(void);
int remove(void);
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
struct net_device * rsi_getcontext(PSDFUNCTION pfunction);
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
void* rsi_getcontext(struct sdio_func *pfunction);
#endif

#if KERNEL_VERSION_BTWN_2_6_(18, 22)
VOID rsi_sdio_claim_host(PSDFUNCTION pFunction);
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
VOID rsi_sdio_claim_host(struct sdio_func *pfunction);
#endif

#if  KERNEL_VERSION_BTWN_2_6_(18, 22)
BOOLEAN __devinit rsi_sdio_probe(PSDFUNCTION pfunction, PSDDEVICE   pDevice);
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
int   rsi_sdio_probe(struct sdio_func *pfunction, const struct sdio_device_id *id);
#endif

#if KERNEL_VERSION_BTWN_2_6_(18, 22)
VOID rsi_sdio_release_host(PSDFUNCTION pFunction);
#el if KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
VOID rsi_sdio_release_host(struct sdio_func *pfunction);
#endif
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
VOID rsi_sdio_interrupt_handler(PVOID pContext);
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
VOID rsi_sdio_interrupt_handler(struct sdio_func *function);
#endif

#if KERNEL_VERSION_BTWN_2_6_(18, 22)
VOID rsi_setcontext(PSDFUNCTION pFunction, void *adapter);
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
VOID rsi_setcontext(struct sdio_func *pfunction, void *adapter);
#endif

int rsi_setupcard(PRSI_ADAPTER adapter);
int init_host_interface(PRSI_ADAPTER adapter);
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
int32 rsi_sdio_release_irq(PSDFUNCTION pFunction);
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
int32 rsi_sdio_release_irq(struct sdio_func *pfunction);
#endif

int32 deregister_sdio_irq(PRSI_ADAPTER adapter);

int rsi_setclock(PRSI_ADAPTER adapter, uint32 Freq);

int rsi_abort_handler(PRSI_ADAPTER Adapter );
int rsi_init_sdio_slave_regs(PRSI_ADAPTER adapter);
int disable_sdio_interrupt(PRSI_ADAPTER adapter);
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
VOID __devexit rsi_sdio_disconnect ( PSDFUNCTION pfunction, PSDDEVICE   pDevice);
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
VOID rsi_sdio_disconnect ( struct sdio_func *pfunction);
#endif

#endif

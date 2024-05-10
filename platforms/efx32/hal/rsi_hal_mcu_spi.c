/*******************************************************************************
* @file  rsi_hal_mcu_spi.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
 * Includes
 */
#include "rsi_driver.h"
#include "em_gpio.h"
#include "em_ldma.h"
#include "rsi_board_configuration.h"
#if BRD4180B_CLI_ENABLED
#include "sl_spidrv_instances.h"
#endif

#define LDMA_MAX_TRANSFER_LENGTH		4096
#define LDMA_DESCRIPTOR_ARRAY_LENGTH	(LDMA_MAX_TRANSFER_LENGTH / 2048)
/**
 * Global Variables
 */
#if SPI_EXTENDED_TX_LEN_2K
uint8_t dummy[2500];
#else
uint8_t dummy[1600];
#endif

// LDMA descriptor and transfer configuration structures for USART TX channel
LDMA_Descriptor_t ldmaTXDescriptor[LDMA_DESCRIPTOR_ARRAY_LENGTH];
LDMA_TransferCfg_t ldmaTXConfig;

// LDMA descriptor and transfer configuration structures for USART RX channel
LDMA_Descriptor_t ldmaRXDescriptor[LDMA_DESCRIPTOR_ARRAY_LENGTH];
LDMA_TransferCfg_t ldmaRXConfig;
volatile uint8_t rx_done;

#if BRD4180B_CLI_ENABLED
// use SPI handle for EXP header
#define SPI_HANDLE                  sl_spidrv_exp_handle

// Flag to signal that transfer is complete
static volatile bool transfer_complete = false;

// Callback fired when data is transmitted
void transfer_callback(SPIDRV_HandleData_t *handle,
                       Ecode_t transfer_status,
                       int items_transferred)
{
  (void)&handle;
  (void)items_transferred;

  // Post semaphore to signal to application
  // task that transfer is successful
  if (transfer_status == ECODE_EMDRV_SPIDRV_OK) {
      transfer_complete = true;
  }
}

void spidrv_init(void)
{
  sl_spidrv_init_instances();
}
#endif

/*==================================================================*/
/**
 * @fn         int16_t cs_enable()
 * @param[out] None
 * @return     0, 0=success
 * @section description
 * This API is used to enable the spi chip select pin in SPI interface.
 */
void cs_enable(void)
{
  // enables the SPI chip select pin on SPI interface
}

/*==================================================================*/
/**
 * @fn         int16_t cs_disable()
 * @param[out] None
 * @return     0, 0=success
 * @section description
 * This API is used to disable the spi chip select pin in SPI interface.
 */
void cs_disable(void)
{
  // disables the SPI chip select pin on SPI interface
}

/*==================================================================*/
/**
 * @fn         int16_t rsi_spi_transfer(uint8_t *ptrBuf,uint16_t bufLen,uint8_t *valBuf,uint8_t mode)
 * @param[in]  uint8_t *tx_buff, pointer to the buffer with the data to be transfered
 * @param[in]  uint8_t *rx_buff, pointer to the buffer to store the data received
 * @param[in]  uint16_t transfer_length, Number of bytes to send and receive
 * @param[in]  uint8_t mode, To indicate mode 8 BIT/32 BIT mode transfers.
 * @param[out] None
 * @return     0, 0=success
 * @section description
 * This API is used to transfer/receive data to the Wi-Fi module through the SPI interface.
 */
int16_t rsi_spi_transfer(uint8_t *tx_buff, uint8_t *rx_buff, uint16_t transfer_length, uint8_t mode)
{

  UNUSED_PARAMETER(mode); //This statement is added only to resolve compilation warnings, value is unchanged
  int i = 0;
  if (tx_buff == NULL) {
    tx_buff = (uint8_t*)&dummy;
  } else if (rx_buff == NULL) {
    rx_buff = (uint8_t*)&dummy;
  }

#if !BRD4180B_CLI_ENABLED
  //Configure LDMA Tx and Rx descriptors
  if (transfer_length <= 2048) {
	// Configure Tx descriptor. Source is tx_buff, destination is USART2_TXDATA, length is transfer_length
	ldmaTXDescriptor[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(tx_buff, &(USART2->TXDATA), transfer_length);
	//Configure Rx descriptor, Source is USART2_RXDATA, destination is rx_buff, length is transfer_length
	ldmaRXDescriptor[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(USART2->RXDATA), rx_buff, transfer_length);
  }
  else {
	//Transfer length is more than 2048 bytes. Initialize multiple LDMA Tx and Rx descriptors.
    for(i = 0; i < (LDMA_DESCRIPTOR_ARRAY_LENGTH - 1); i++) {
	  ldmaTXDescriptor[i] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE((tx_buff + (2048 * i)), &(USART2->TXDATA), 2048, 1);
	  ldmaRXDescriptor[i] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&(USART2->RXDATA), (rx_buff + (2048 * i)), 2048, 1);
    }
    ldmaTXDescriptor[i] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE((tx_buff + (2048 * i)), &(USART2->TXDATA), \
	          															    (transfer_length - (2048 * i)));
    ldmaRXDescriptor[i] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(USART2->RXDATA), (rx_buff + (2048 * i)), \
	  																	    (transfer_length - (2048 * i)));																			
  }
  // Transfer a byte on free space in the USART buffer
  ldmaTXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART2_TXBL);

  // Transfer a byte on receive data valid
  ldmaRXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART2_RXDATAV);

  // Set the receive state to not done
  rx_done = 0;

  // Start both channels
  LDMA_StartTransfer(RX_LDMA_CHANNEL, &ldmaRXConfig, (LDMA_Descriptor_t*)&ldmaRXDescriptor);
  LDMA_StartTransfer(TX_LDMA_CHANNEL, &ldmaTXConfig, (LDMA_Descriptor_t*)&ldmaTXDescriptor);

  // Wait in EM1 until all data is received
  while (!rx_done);
  
  return 0;
#else
  Ecode_t ecode;
  transfer_complete = false;
  ecode = SPIDRV_MTransfer(SPI_HANDLE, tx_buff, rx_buff, transfer_length, transfer_callback);
  EFM_ASSERT(ecode == ECODE_OK);

  // wait for transfer to complete
  while (!transfer_complete) ;

  return 0;
#endif
}

/**************************************************************************//**
 * @brief LDMA IRQHandler
 *****************************************************************************/
//Adding guard to fix multiple definition error on enabling uart cli
#if !BRD4180B_CLI_ENABLED
void LDMA_IRQHandler()
{
  uint32_t flags = LDMA_IntGet();

  // Clear the transmit channel's done flag if set
  if (flags & (1 << TX_LDMA_CHANNEL))
    LDMA_IntClear(1 << TX_LDMA_CHANNEL);

  /*
   * Clear the receive channel's done flag if set and change receive
   * state to done.
   */
  if (flags & (1 << RX_LDMA_CHANNEL)) {
    LDMA_IntClear(1 << RX_LDMA_CHANNEL);
    rx_done = true;
  }

  // Stop in case there was an error
  if (flags & LDMA_IF_ERROR)
    __BKPT(0);
}
#endif

/*******************************************************************************
* @file  rsi_91x_spi_ops.c
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
 * @file rsi_91x_spi_ops.c
 * @author 
 * @version 1.0
 *
 *
 * @section DESCRIPTION
 *
 * The file contains Generic HAL changes for SPI.
 */


#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include "rsi_linux.h"
//#include "rsi_spi.h"
void rsi_spi_ack_intr(PRSI_ADAPTER Adapter, u8 int_bit);

/**
 * rsi_ssp_receive_start_token() - Gets the start token from device, which is
 *				   used to find granularity of intake data
 *				   while issuing read command.
 * @gran - granularity of the intake data.
 * returns success on reception of start token.
 */
int rsi_ssp_receive_start_token(PRSI_ADAPTER Adapter, u16 gran)
{
	u32 count = 0;
	u8 tx = 7, rx = 0;

	while (1) {
		if (count++ > 260) {
			Adapter->token_flag = 1;
			RSI_DEBUG(RSI_ZONE_MGMT_DUMP,
			"%s: ERROR start token timeout token not received not\
			sending any more commands\n", __func__);
			return RSI_FAIL;
		}
		rsi_spi_read_write(Adapter, &tx, &rx, 1);
		if (gran == BIT_GRAN_8 && rx == SSP_START_TOKEN_8)
			break;
	}
	return  RSI_SUCCESS;
}

#define MAXCMDS 10
#define MAXPKTLEN (4 * MAXCMDS)
int rsi_dummy_write(PRSI_ADAPTER Adapter)
{
	u8  buffer[MAXPKTLEN] = {0};
	u8 tx_buf[4] = {0x62, 0x3f, 0x62, 0x3f};
	u8 Cntr = 0, i = 0;
	int status = RSI_FAIL;

	memset(buffer, 0, sizeof(buffer));
	for (i = 0; i < MAXPKTLEN; i += 4)
		memcpy((buffer + i), tx_buf, 4);

	while (1) {
		if (++Cntr == 255) {
			RSI_DEBUG(RSI_ZONE_ERROR, "%s: Dead in Dummy Write\n",
					__func__);
			break;
		}
		memset(Adapter->dummy_buf, 0, MAXPKTLEN);
		rsi_spi_read_write(Adapter, (u8 *)&buffer, Adapter->dummy_buf,
				MAXPKTLEN);
		if (Adapter->dummy_buf[MAXPKTLEN - 1] == SPI_STATUS_BUSY)
			continue;
		else if (Adapter->dummy_buf[MAXPKTLEN - 1] ==
				SPI_STATUS_SUCCESS)
			rsi_spi_read_write(Adapter, (u8 *)&buffer,
					Adapter->dummy_buf, 2);
		status =  RSI_SUCCESS;
		break;
	}
	return status;
}

int rsi_spi_write(PRSI_ADAPTER Adapter, const u8 *buf, size_t len)
{
	struct spi_device *spi = Adapter->spi;
	struct spi_transfer t = {
		.tx_buf         = buf,
		.len            = len,
	};
	struct spi_message m;
	int result;

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);

	result = spi_sync(spi, &m);
	return result;
}

int rsi_spi_read(PRSI_ADAPTER Adapter, u8 *buf, size_t len)
{
	struct spi_device *spi = Adapter->spi;
	struct spi_transfer t = {
		.rx_buf         = buf,
		.len            = len,
	};
	struct spi_message      m;
	int status;

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	status = spi_sync(spi, &m);
	return status;
}

int rsi_spi_read_write(PRSI_ADAPTER Adapter, u8 *txbuf, u8 *rxbuf,
		u32 rx_tx_length)
{
	struct spi_device *spi = Adapter->spi;
	int                     status;
	struct spi_message      message;
	struct spi_transfer     t;

	if (rx_tx_length > RSI_SPI_BUFSIZ)
		return -EINVAL;
	spi_message_init(&message);
	memset((void *)&t, 0, sizeof(t));
	t.tx_buf = txbuf;
	t.rx_buf = rxbuf;
	t.len = rx_tx_length;
	spi_message_add_tail(&t, &message);

	status = spi_sync(spi, &message);
	if (status == 0)
		status = message.status;
	else
		RSI_DEBUG(RSI_ZONE_ERROR, "%s: spi_sync failed: %d\n", __func__,
				status);

	return status;
}

int rsi_spi_read_write_reg_multiple(PRSI_ADAPTER Adapter, u8 fnum, u32 addr,
	       u8 *data, u32 count)
{
	struct cmd_fmt_s tx_cmd;
	int    status = RSI_FAIL;
	u16 busy_retry = 0;
	int  tkn_status = RSI_SUCCESS;
	u8  junk[16] = {0};
	u32  ii = 0;

	down(&Adapter->spi_dev_sem);
	memset((void *)&tx_cmd, 0, sizeof(struct cmd_fmt_s));
	memset((void *)Adapter->dummy_buf, 0, MAX_SPI_PKT_SIZE);

	switch (fnum) {
	case SPI_INTERNAL_READ:
		RSI_DEBUG(RSI_ZONE_INFO, "SPI_INTERNAL_READ Len %d %x\n",
				count, addr);
		/* Prepare the cmd */
		tx_cmd.buf[0] = CMD_READ_WRITE;
		tx_cmd.buf[0] |= (u8)(count & 0x0003);

		/* Only LSB 5 bits of the address are valid */
		tx_cmd.buf[1] |= (u8)(addr & 0x3F);
		rsi_spi_read_write(Adapter,
				(u8 *)&tx_cmd,
				Adapter->dummy_buf,
				2);

		/* Check the status */
		switch (Adapter->dummy_buf[1]) {
		case SPI_STATUS_SUCCESS:
		case SPI_STATUS_ZERO:
			if (Adapter->dummy_buf[1] == SPI_STATUS_ZERO)
				RSI_DEBUG(RSI_ZONE_ERROR, "dummy_buf value is ZERO\n");

			RSI_DEBUG(RSI_ZONE_INFO,
					"dummy_buf indicates success %d\n",
					Adapter->dummy_buf[1]);
			tkn_status = rsi_ssp_receive_start_token(Adapter,
					BIT_GRAN_8);
			if (tkn_status == RSI_BUSY) {
				RSI_DEBUG(RSI_ZONE_INFO, "Start token Busy \
						while SPI_INTERNAL_READ\n");
				status = RSI_BUSY;
				break;
			} else if (tkn_status == RSI_FAIL) {
				RSI_DEBUG(RSI_ZONE_ERROR, "Start token ERROR while \
						SPI_INTERNAL_READ\n");
				status = RSI_FAIL;
				break;
			}
			RSI_DEBUG(RSI_ZONE_INFO, "Recvd token success\n");
			status = rsi_spi_read_write(Adapter, junk, data, 2);
			status = RSI_SUCCESS;
			break;

		case SPI_STATUS_BUSY:
			RSI_DEBUG(RSI_ZONE_INFO,
					"dummy_buf value SPI_STATUS_BUSY\n");
			status = RSI_BUSY;
			break;

		case SPI_STATUS_FAILURE:
			RSI_DEBUG(RSI_ZONE_ERROR,
					"dummy_buf value SPI_STATUS_FAILURE\n");
			status = RSI_FAIL;
			break;

		default:
			RSI_DEBUG(RSI_ZONE_ERROR, "dummy_buf value default = %d\n",
					Adapter->dummy_buf[1]);
			dump_stack();
			rsi_spi_ack_intr(Adapter, (1 << PKT_BUFF_AVAILABLE));
			status = RSI_FAIL;
		}
		break;

	case AHB_MASTER_READ:
		RSI_DEBUG(RSI_ZONE_MGMT_DUMP, "ssp_bus_req: AHB_MASTER_READ Len \
				%d\n", count);

		/* Prepare the cmd */
		tx_cmd.buf[0] = CMD_READ_WRITE;
		tx_cmd.buf[0] |= AHB_BUS_ACCESS;
		tx_cmd.buf[0] |= TRANSFER_LEN_16_BITS;

		*(u16 *)&tx_cmd.buf[2] = count;

		/* Configure the TX FIFO to the corresponding granularity */
		rsi_spi_read_write(Adapter, (u8 *)&tx_cmd, Adapter->dummy_buf,
				2);
		/* Check the status */
		if (Adapter->dummy_buf[1] == SPI_STATUS_SUCCESS) {
			RSI_DEBUG(RSI_ZONE_MGMT_DUMP, "[ssp_bus_request_synch] \
					AHB_MASTER_READ status SUCCESS\n");
			rsi_spi_read_write(Adapter, (u8 *)&tx_cmd.buf[2],
					&Adapter->dummy_buf[2], 2);
			rsi_spi_read_write(Adapter, (u8 *)&addr,
					&Adapter->dummy_buf[4], 4);
			/* Get the Start token first */
			tkn_status = rsi_ssp_receive_start_token(Adapter,
					BIT_GRAN_8);
			if (tkn_status == RSI_BUSY) {
				RSI_DEBUG(RSI_ZONE_ERROR, "Start token BUSY while \
						AHB_MASTER_READ\n");
				status = RSI_BUSY;
				break;
			} else if (tkn_status == RSI_FAIL) {
				RSI_DEBUG(RSI_ZONE_ERROR, "ERROR while \
						AHB_MASTER_READ\n");
				status = RSI_FAIL;
				break;
			} /* End if <condition> */
			*(u32 *)data = 0;
			rsi_spi_read(Adapter, data, 4);
			status = RSI_SUCCESS;
			break;
		} else if (Adapter->dummy_buf[1] == SPI_STATUS_BUSY) {
			RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request_synch] \
					AHB_MASTER_READ status BUSY\n");
			status = RSI_BUSY;
			break;
		} else if (Adapter->dummy_buf[1] == SPI_STATUS_FAILURE) {
			RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request_synch] \
					AHB_MASTER_READ status FAILURE\n");
			status = RSI_FAIL;
			break;
		}
		RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request_synch] AHB_MASTER_READ \
				status Unknown:  %02x\n",
				Adapter->dummy_buf[1]);
		status = RSI_FAIL;
		break;
	case AHB_SLAVE_READ:
		/* Prepare the cmd */
		RSI_DEBUG(RSI_ZONE_INFO, "AHB_SLAVE_READ len %d %d\n", count,
				addr);
		tx_cmd.buf[0] = CMD_READ_WRITE;
		tx_cmd.buf[0] |= AHB_BUS_ACCESS;
		tx_cmd.buf[0] |= AHB_SLAVE;
		tx_cmd.buf[0] |= TRANSFER_LEN_16_BITS;
		tx_cmd.buf[1] = (u8)addr;

		*(u16 *)&tx_cmd.buf[2] = count;

		rsi_spi_read_write(Adapter, (u8 *)&tx_cmd,
				&Adapter->dummy_buf[0], 2);

		switch (Adapter->dummy_buf[1]) {
		case SPI_STATUS_SUCCESS:
			rsi_spi_read_write(Adapter, (u8 *)&tx_cmd.buf[2],
					&Adapter->dummy_buf[2], 2);
			tkn_status = rsi_ssp_receive_start_token(Adapter,
					BIT_GRAN_8);
			if (tkn_status == RSI_BUSY) {
				RSI_DEBUG(RSI_ZONE_ERROR, "Start token BUSY while \
						AHB_SLAVE_READ\n");
				status = RSI_BUSY;
				break;
			} else if (tkn_status == RSI_FAIL) {
				RSI_DEBUG(RSI_ZONE_ERROR, "ERROR while \
						AHB_SLAVE_READ\n");
				status = RSI_FAIL;
				break;
			}
			status = rsi_spi_read(Adapter, (u8 *)data, count);
			if (!status)
				status = RSI_SUCCESS;
			else
				status = RSI_FAIL;
			break;
		case SPI_STATUS_BUSY:
			status = RSI_BUSY;
			break;

		case SPI_STATUS_FAILURE:
			status = RSI_FAIL;
			break;

		default:
			status = RSI_FAIL;
		} /* End of switch */
		break; /*END_AHB_SLAVE_READ*/
	case SPI_INITIALIZATION:
		RSI_DEBUG(RSI_ZONE_MGMT_DUMP, "[ssp_bus_request] \
				SPI_INITIALIZATION\n");
		/* Prepare the cmd */
		tx_cmd.buf[0] = (RSI_RS9116_INIT_CMD & 0xFF);
		tx_cmd.buf[1] = ((RSI_RS9116_INIT_CMD >> 8) & 0xFF);
		tx_cmd.buf[2] = ((RSI_RS9116_INIT_CMD >> 16) & 0xFF);
		tx_cmd.buf[3] = ((RSI_RS9116_INIT_CMD >> 24) & 0xFF);
		status = rsi_spi_read_write(Adapter,
				(u8 *)&tx_cmd,
				Adapter->dummy_buf,
				4);
		switch (Adapter->dummy_buf[3]) {
		case SPI_STATUS_SUCCESS:
			RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request] \
					SPI_INITIALIZATION SUCCESS\n");
			status = RSI_SUCCESS;
			break;

		case SPI_STATUS_FAILURE:
			RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request] \
					SPI_INITIALIZATION FAILURE\n");
			status = RSI_FAIL;
			break;

		default:
			status = RSI_FAIL;
			break;
		} /* End switch */
		break;

	case SPI_ULP_INIT:
		RSI_DEBUG(RSI_ZONE_MGMT_DUMP,
				"[ssp_bus_request] SPI_ULP_INIT\n");

		/* Prepare the cmd */
		tx_cmd.buf[0] = (RSI_RS9116_INIT_CMD & 0xFF);
		tx_cmd.buf[1] = ((RSI_RS9116_INIT_CMD >> 8) & 0xFF);
		tx_cmd.buf[2] = ((RSI_RS9116_INIT_CMD >> 16) & 0xFF);
		tx_cmd.buf[3] = ((RSI_RS9116_INIT_CMD >> 24) & 0xFF);

		while (1) {
			status = rsi_spi_read_write(Adapter,
					(u8 *)&tx_cmd,
					Adapter->dummy_buf,
					2);

			if (Adapter->dummy_buf[1] == SPI_STATUS_FAILURE) {
				RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request] \
						SPI_INITIALIZATION FAILUR\n");
				status = RSI_FAIL;
				break;
			} else if (Adapter->dummy_buf[1] == SPI_STATUS_ZERO) {
				status = rsi_spi_read_write(Adapter,
						(u8 *)&tx_cmd.buf[2],
						Adapter->dummy_buf,
						2);
				if (Adapter->dummy_buf[1] ==
						SPI_STATUS_SUCCESS) {
					status = RSI_SUCCESS;
					break;
				}
			}
		}
		break;
	case SPI_INTERNAL_WRITE:
#define MAX_RETRY 2
		RSI_DEBUG(RSI_ZONE_MGMT_DUMP, "[ssp_bus_request]SPI_INTERNAL_WRITE \
				Len %d\n", count);
		/* Prepare the cmd */
		tx_cmd.buf[0] = CMD_READ_WRITE;
		tx_cmd.buf[0] |= CMD_WRITE;
		tx_cmd.buf[0] |= (u8)(count & 0x0003);

		/* Only LSB 5 bits of the address are valid */
		tx_cmd.buf[1] |= (u8)(addr & 0x3F);

		/* Configure the TX FIFO to the corresponding granularity */
		do {
			rsi_spi_read_write(Adapter,
					(u8 *)&tx_cmd,
					Adapter->dummy_buf,
					2);
			/* Check the status */
			if (Adapter->dummy_buf[1] == SPI_STATUS_SUCCESS) {
				RSI_DEBUG(RSI_ZONE_MGMT_DUMP,
					"[ssp_bus_request_synch] \
					SPI_INTERNAL_WRITE status SUCCESS\n");
				rsi_spi_read_write(Adapter,
						data,
						Adapter->dummy_buf,
						count);
				status = RSI_SUCCESS;
				break;
			} else if (Adapter->dummy_buf[1] == SPI_STATUS_BUSY) {
				RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request_synch] \
						SPI_INTERNAL_WRITE status \
						BUSY\n");
				if (ii++ <= MAX_RETRY) {
					rsi_dummy_write(Adapter);
					continue;
					status = RSI_SUCCESS;
				} else {
					status = RSI_BUSY;
				}
				break;
			} else if (Adapter->dummy_buf[1] ==
					SPI_STATUS_FAILURE) {
				RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request_synch] \
						SPI_INTERNAL_WRITE status \
						FAILURE\n");
				status = RSI_FAIL;
				break;
			}
			RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request_synch] \
					SPI_INTERNAL_WRITE status \
					Unknown:  %02x\n",
					Adapter->dummy_buf[1]);
			status = RSI_FAIL;
			break;
		} while (1);
		break;

	case AHB_MASTER_WRITE:
		RSI_DEBUG(RSI_ZONE_MGMT_DUMP, "[ssp_bus_request] AHB_MASTER_WRITE \
				Len %d\n", count);
		/* Prepare the cmd */
		tx_cmd.buf[0] = CMD_READ_WRITE;
		tx_cmd.buf[0] |= CMD_WRITE;
		tx_cmd.buf[0] |= AHB_BUS_ACCESS;
		tx_cmd.buf[0] |= TRANSFER_LEN_16_BITS;

		*(u16 *)&tx_cmd.buf[2] = count;
		do {
			rsi_spi_read_write(Adapter, (char *)&tx_cmd,
					Adapter->dummy_buf, 2);

			if (Adapter->dummy_buf[1] == SPI_STATUS_SUCCESS) {
				rsi_spi_read_write(Adapter,
						(char *)&tx_cmd.buf[2],
						&Adapter->dummy_buf[2],
						2);
				rsi_spi_read_write(Adapter,
						(char *)&(addr),
						&Adapter->dummy_buf[4],
						4);
				break;
			} else if ((Adapter->dummy_buf[1] == SPI_STATUS_BUSY) &&
					busy_retry < 5) {
				continue;
			} else {
				break;
			}
		} while (1);

		if (Adapter->dummy_buf[1] == SPI_STATUS_SUCCESS) {
			RSI_DEBUG(RSI_ZONE_MGMT_DUMP, "[ssp_bus_request_synch] \
					AHB_MASTER_WRITE status SUCCESS\n");
			rsi_spi_write(Adapter, data, count);
			status = RSI_SUCCESS;
			break;
		} else if (Adapter->dummy_buf[1] == SPI_STATUS_BUSY) {
			RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request_synch] \
					AHB_MASTER_WRITE status BUSY\n");
			status = RSI_BUSY;
			break;
		} else if (Adapter->dummy_buf[1] == SPI_STATUS_FAILURE) {
			RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request_synch] \
					AHB_MASTER_WRITE status FAILURE\n");
			status = RSI_FAIL;
			break;
		}
		RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request_synch] AHB_MASTR_WRITE \
				status Unknown:%02x\n", Adapter->dummy_buf[1]);
		status = RSI_FAIL;
		break;
	case AHB_SLAVE_WRITE:
		status = rsi_write_data(Adapter, data, count, addr);
		break;
	case DUMMY_WRITE:
		status = rsi_dummy_write(Adapter);
		break;
	default:
		RSI_DEBUG(RSI_ZONE_ERROR, "[ssp_bus_request_synch] ERROR, cmd-type = \
				Unknown: %d\n", fnum);
		status = RSI_FAIL;
		break;
	} /* End switch */
	up(&Adapter->spi_dev_sem);
	return status;
}

int rsi_spi_master_reg_write(PRSI_ADAPTER Adapter, unsigned long addr,
		unsigned long data, u16 size)
{
	return rsi_spi_read_write_reg_multiple(Adapter, AHB_MASTER_WRITE, addr,
			(u8 *)&data, size); //sending addr of pointer.
}

int rsi_spi_master_reg_read(PRSI_ADAPTER Adapter, u32 addr, u32 *read_buf,
		u16 size)
{
	return rsi_spi_read_write_reg_multiple(Adapter, AHB_MASTER_READ, addr,
			(u8 *)read_buf, size);
}

int rsi_spi_write_register_multiple(PRSI_ADAPTER Adapter, u32 addr, u8 *data,
		u16 count)
{
	return rsi_spi_read_write_reg_multiple(Adapter, AHB_SLAVE_WRITE, addr,
			data, count);
}

int rsi_spi_read_register_multiple(PRSI_ADAPTER Adapter, u32 addr, u8 *data,
		u16 count)
{
	return rsi_spi_read_write_reg_multiple(Adapter, AHB_SLAVE_READ, addr,
			data, count);
}

#define MAX_RETRY 2
int rsi_write_data(PRSI_ADAPTER Adapter, u8 *data, u32 length, u32 address)
{
	struct cmd_fmt_s tx_cmd;
	int    status = RSI_FAIL;
	u32 ii = 0;
	u8 Cntr = 0;

	RSI_DEBUG(RSI_ZONE_INFO, "[ssp_bus_request]AHB_SLAVE_WRITE Len%u\n",
			length);
	memset((void *)&tx_cmd, 0, sizeof(struct cmd_fmt_s));
	/* Prepare the cmd */
	tx_cmd.buf[0] = CMD_READ_WRITE;
	tx_cmd.buf[0] |= CMD_WRITE;
	tx_cmd.buf[0] |= AHB_BUS_ACCESS;
	tx_cmd.buf[0] |= AHB_SLAVE;
	tx_cmd.buf[0] |= TRANSFER_LEN_16_BITS;
	tx_cmd.buf[1] = (address);

	*(u16 *)&tx_cmd.buf[2] = (length);

	do  {
		rsi_spi_read_write(Adapter,
				(u8 *)&tx_cmd,
				Adapter->dummy_buf,
				2);
		switch (Adapter->dummy_buf[1]) {
		case SPI_STATUS_SUCCESS:
			do {
				rsi_spi_read_write(Adapter,
						(u8 *)&tx_cmd.buf[2],
						Adapter->dummy_buf,
						2);
				RSI_DEBUG(RSI_ZONE_INFO, "Address of data = \
						%p and queuno is %d\n", data,
						address);

				status = rsi_spi_write(Adapter, data,  length);
				if (++Cntr == 255)  {
					RSI_DEBUG(RSI_ZONE_ERROR, "%s: Dead in Slave \
							Write\n", __func__);
					break;
				}
			} while (status);
			status = RSI_SUCCESS;
			break;
		case SPI_STATUS_BUSY:
			if (ii++ == MAX_RETRY) {
				rsi_dummy_write(Adapter);
				continue;
				status = RSI_SUCCESS;
			} else  {
				status = RSI_BUSY;
				//rsi_hex_dump(RSI_ZONE_INFO, "SPI BUSY", data, 16);
			}
			break;
		case SPI_STATUS_FAILURE:
			status = RSI_FAIL;
			break;
		default:
			status = RSI_FAIL;
			return -1;
		} /* End switch */
		if (status == RSI_SUCCESS)
			break;
	} while (1);
	return status;
}

int rsi_spi_write_register(PRSI_ADAPTER Adapter, u32 addr, u8 *data)
{
	return rsi_spi_read_write_reg_multiple(Adapter, SPI_INTERNAL_WRITE,
			addr, data, 2);
}

int rsi_spi_read_register(PRSI_ADAPTER Adapter, u32 addr, u8 *data)
{
	return rsi_spi_read_write_reg_multiple(Adapter, SPI_INTERNAL_READ,
			addr, data, 2);
}

int rsi_spi_write_multiple(PRSI_ADAPTER Adapter, u8 fnum, u32 addr, u8 *pkt,
		u32 len)
{
	int status = 0;
	u32 frag;
	u32 ii = 0;

	switch (fnum) {
	case SPI_INITIALIZATION:
		while (1) {
			status = rsi_spi_read_write_reg_multiple(Adapter, fnum,
					addr, pkt, len);
			if (status == RSI_BUSY || status == RSI_FAIL)
				continue;
			break;
		}
		if (status)
			RSI_DEBUG(RSI_ZONE_ERROR, "%s: error during SPI_INIT cmd\n",
					__func__);
		break;
	case SPI_INTERNAL_READ:
		while (ii < 150) {
			status = rsi_spi_read_write_reg_multiple(Adapter, fnum,
					addr, pkt, len);
			if (status == RSI_BUSY || status == RSI_FAIL) {
				ii++;
				continue;
			}
			break;
		}
		if (status)
			RSI_DEBUG(RSI_ZONE_ERROR, "%s: error during SPI_INTERNAL_READ \
					cmd\n", __func__);
		break;
	case AHB_MASTER_WRITE:
		RSI_DEBUG(RSI_ZONE_MGMT_DUMP, "AHB_MASTER_WRITE called\n");
		while (ii < 4) {
			status = rsi_spi_read_write_reg_multiple(Adapter, fnum,
					addr, pkt, len);
			if (status == RSI_BUSY || status == RSI_FAIL) {
				ii++;
				continue;
			}
			break;
		}
		if (status)
			RSI_DEBUG(RSI_ZONE_ERROR, "%s: ERROR during AHB_MASTER_WRITE \
					cmd\n", __func__);
		break;
	case AHB_MASTER_READ:
		RSI_DEBUG(RSI_ZONE_MGMT_DUMP, "AHB_MASTER_READ: from \
				write_multiple\n");
		while (ii < 4) {
			status = rsi_spi_read_write_reg_multiple(Adapter, fnum,
					addr, pkt, len);
			if (status == RSI_BUSY || status == RSI_FAIL) {
				ii++;
				continue;
			}
			break;
		}
		if (status)
			RSI_DEBUG(RSI_ZONE_ERROR, "%s: ERROR during AHB_MASTER_READ \
					cmd\n", __func__);
		break;
	case AHB_SLAVE_READ:
		while (ii < 150) {
			status = rsi_spi_read_write_reg_multiple(Adapter, fnum,
					addr, pkt, len);
			if (status == RSI_BUSY || status == RSI_FAIL) {
				ii++;
				continue;
			}
			break;
		}
		if (status)
			RSI_DEBUG(RSI_ZONE_ERROR, "%s: ERROR during AHB_SLAVE_READ \
					cmd\n", __func__);
		break;
	case AHB_SLAVE_WRITE:
		frag = len % 4;
		if (frag)
			len += (4 - frag);
		while (ii < 150) {
			/* because of the flow control at command boundary,
			 * length in first command should be 16 (desc) + n*64
			 * and should be less than 256 (FIFO size). So 208
			 * bytes are used.
			 */

			/* send complete frame (desc+payload) in single cmd */
			if (len <= 256) {
				status = rsi_spi_read_write_reg_multiple(Adapter,
						fnum, addr, (void *)pkt, len);
			} else {
				status = rsi_spi_read_write_reg_multiple(Adapter,
						fnum, addr, (void *)pkt, 208);
			}
			if ((status == RSI_BUSY) || (status == RSI_FAIL)) {
				ii++;
				continue;
			}
			break;
		}
		if (status) {
			RSI_DEBUG(RSI_ZONE_ERROR, "%s: ERROR during AHB_SLAVE_WRITE \
					for tx descriptor\n", __func__);
			break; /* breaking switch case, so that we can retry */
		}
		while (ii < 1500) {
			if (len > 256) {
				status = rsi_spi_read_write_reg_multiple(Adapter,
						fnum, addr,
						(void *)(((u8 *)pkt) + 208),
						(len - 208));
				if (status == RSI_BUSY ||
						status == RSI_FAIL) {
					ii++;
					continue;
				}
			}
			break;
		}
		if (status) {
			RSI_DEBUG(RSI_ZONE_ERROR, "%s: ERROR during AHB_SLAVE_WRITE \
					for tx frame body.\n", __func__);
			RSI_DEBUG(RSI_ZONE_ERROR, "%s: Frame descriptor sent to \
					device and frame body failed.\n",
					__func__);
			RSI_DEBUG(RSI_ZONE_ERROR, "This can happen for the \
					following\n");
			RSI_DEBUG(RSI_ZONE_ERROR, "1) When enough clocks are not \
				provided for device to take the descriptor \
					in.\n");
			RSI_DEBUG(RSI_ZONE_ERROR, "When device is in sleep it runs on \
					host clock to take the descriptor in \
					and thus it requires more clocks\n");
			RSI_DEBUG(RSI_ZONE_ERROR, "Please retry C1, C2 commands more \
					times.\n");
			RSI_DEBUG(RSI_ZONE_ERROR, "2) If frame formats and protocol \
				between driver and firmware is not followed \
					properly\n");
			RSI_DEBUG(RSI_ZONE_ERROR, "3) Firmware stuck up and not able \
				to take the descriptor in.\n");
			RSI_DEBUG(RSI_ZONE_ERROR, "4) Clock glitch occurred, in this \
			case unknown response token will be observed.\n");
			RSI_DEBUG(RSI_ZONE_ERROR, "Clean clock has to be provided by \
					hardware integration team\n");
			RSI_DEBUG(RSI_ZONE_ERROR, "5) Power line failure, 3.3V should \
				be proper. Contact hardware integration team.\
					\n");
		}
		break;
	case DUMMY_WRITE:
		while (ii < 150) {
			status = rsi_spi_read_write_reg_multiple(Adapter, fnum,
					addr, NULL, len);
			if (status == RSI_BUSY || status == RSI_FAIL) {
				ii++;
				continue;
			}
			break;
		}
		break;
	default:
		RSI_DEBUG(RSI_ZONE_ERROR, "%s: wrong command received\n", __func__);
		break;
	} /* End <switch statement> */
	return status;
}

int rsi_spi_host_intf_write_pkt(PRSI_ADAPTER Adapter, u8 *pkt, u32 len)
{
	int status;
	u32 queueno = (pkt[1] >> 4) & 0xf;

	/*FIXME: Filling the queue no as address as in NLink */
	queueno = ((pkt[1] >> 4) & 0xf);
	status = rsi_spi_write_multiple(Adapter, AHB_SLAVE_WRITE,
			queueno, (u8 *)pkt, len);
	if (status)
		RSI_DEBUG(RSI_ZONE_ERROR, "%s:card write failed: %d\n", __func__,
				status);
	return status;
}

int rsi_spi_host_intf_read_pkt(PRSI_ADAPTER Adapter, u8 *pkt, u32 len)
{
	int status;

	/* Don't worry about address from which to read */
	status = rsi_spi_write_multiple(Adapter, AHB_SLAVE_READ, 0,
			(u8 *)pkt, len);
	if (status)
		RSI_DEBUG(RSI_ZONE_ERROR,
			"%s: failed to read frame from the card: %d\n",
			__func__, status);
	return status;
}

int rsi_spi_load_data_master_write(PRSI_ADAPTER Adapter, u32 base_address,
		u32 instructions_sz, u16 block_size, u8 *ta_firmware)
{
	u32 num_blocks;
	u32 cur_indx, ii;
	u8  temp_buf[256];

	num_blocks = (instructions_sz + block_size - 1) / block_size;
	RSI_DEBUG(RSI_ZONE_INFO, "num_blocks: %d\n", num_blocks);

	for (cur_indx = 0, ii = 0; ii < num_blocks; ii++,
			cur_indx += block_size) {
		memset(temp_buf, 0, block_size);
		memcpy(temp_buf, ta_firmware + cur_indx, block_size);

		if (rsi_spi_write_multiple(Adapter,
					AHB_MASTER_WRITE,
					(base_address + cur_indx),
					(u8 *)(temp_buf),
					block_size) != RSI_SUCCESS) {
			RSI_DEBUG(RSI_ZONE_ERROR, "%s: Unable to load  blk\n",
					__func__);
			return RSI_FAIL;
		}
	}
	return RSI_SUCCESS;
}

#define TA_BASE_ADDR1         0x22000000
int rsi_spi_ta_reset_ops(PRSI_ADAPTER Adapter)
{
	u8 *data;

	data = kzalloc(sizeof(u32), GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	RSI_DEBUG(RSI_ZONE_INIT, "%s: Bringing TA Out of Reset\n", __func__);
	put_unaligned_le32(TA_HOLD_THREAD_VALUE, data);
	/* Bringing TA out of reset */
	if (rsi_spi_read_write_reg_multiple(Adapter,
				AHB_MASTER_WRITE,
				TA_BASE_ADDR1 + TA_HOLD_THREAD_REG,
				(u8 *)data,
				4) != RSI_SUCCESS) {
		RSI_DEBUG(RSI_ZONE_ERROR, "%s: Unable to hold TA threads\n", __func__);
		goto fail;
	}

	/* Assuming TA will go to hold by this time
	 * If you find that TA is not in hold by this time
	 * in any chip or any project, then wait till TA goes to
	 * hold by polling poll_status register.
	 */
	put_unaligned_le32(TA_PC_ZERO, data);
	/* Bringing TA out of reset */
	if (rsi_spi_read_write_reg_multiple(Adapter,
				AHB_MASTER_WRITE,
				TA_BASE_ADDR1 + TA_TH0_PC_REG,
				(u8 *)data,
				4) != RSI_SUCCESS) {
		RSI_DEBUG(RSI_ZONE_ERROR, "%s: Unable to Reset TA PC value\n",
				__func__);
		goto fail;
	}
	put_unaligned_le32(TA_RELEASE_THREAD_VALUE, data);
	/* Bringing TA out of reset */
	if (rsi_spi_read_write_reg_multiple(Adapter,
				AHB_MASTER_WRITE,
				TA_BASE_ADDR1 + TA_RELEASE_THREAD_REG,
				(u8 *)data,
				4) != RSI_SUCCESS) {
		RSI_DEBUG(RSI_ZONE_ERROR, "%s: Unable to release TA threads\n",
				__func__);
		goto fail;
	}
	return RSI_SUCCESS;
fail:
	kfree(data);
	return -EINVAL;
}

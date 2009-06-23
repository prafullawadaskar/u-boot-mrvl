/*
 * original file from linux: drivers/usb/host/ehci-orion.c
 *
 * Tzachi Perelstein <tzachi@marvell.com>
 *
 * Updated-by: Prafulla Wadaskar <prafulla@marvell.com>
 * Purpose: to reuse usb_phy_v1_setup function
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <common.h>
#include <asm/arch/kirkwood.h>

#define rdl(off)	readl(KW_USB20_BASE + (off))
#define wrl(off, val)	writel((val), KW_USB20_BASE + (off))

#define USB_CMD			0x140
#define USB_MODE		0x1a8
#define USB_CAUSE		0x310
#define USB_MASK		0x314
#define USB_WINDOW_CTRL(i)	(0x320 + ((i) << 4))
#define USB_WINDOW_BASE(i)	(0x324 + ((i) << 4))
#define USB_IPG			0x360
#define USB_PHY_PWR_CTRL	0x400
#define USB_PHY_TX_CTRL		0x420
#define USB_PHY_RX_CTRL		0x430
#define USB_PHY_IVREF_CTRL	0x440
#define USB_PHY_TST_GRP_CTRL	0x450

/*
 * Implement Orion USB controller specification guidelines
 */
void orion_usb_phy_v1_setup(void)
{
	/* The below GLs are according to the Orion Errata document */
	/*
	 * Clear interrupt cause and mask
	 */
	wrl(USB_CAUSE, 0);
	wrl(USB_MASK, 0);

	/*
	 * Reset controller
	 */
	wrl(USB_CMD, rdl(USB_CMD) | 0x2);
	while (rdl(USB_CMD) & 0x2);

	/*
	 * GL# USB-10: Set IPG for non start of frame packets
	 * Bits[14:8]=0xc
	 */
	wrl(USB_IPG, (rdl(USB_IPG) & ~0x7f00) | 0xc00);

	/*
	 * GL# USB-9: USB 2.0 Power Control
	 * BG_VSEL[7:6]=0x1
	 */
	wrl(USB_PHY_PWR_CTRL, (rdl(USB_PHY_PWR_CTRL) & ~0xc0)| 0x40);

	/*
	 * GL# USB-1: USB PHY Tx Control - force calibration to '8'
	 * TXDATA_BLOCK_EN[21]=0x1, EXT_RCAL_EN[13]=0x1, IMP_CAL[6:3]=0x8
	 */
	wrl(USB_PHY_TX_CTRL, (rdl(USB_PHY_TX_CTRL) & ~0x78) | 0x202040);

	/*
	 * GL# USB-3 GL# USB-9: USB PHY Rx Control
	 * RXDATA_BLOCK_LENGHT[31:30]=0x3, EDGE_DET_SEL[27:26]=0,
	 * CDR_FASTLOCK_EN[21]=0, DISCON_THRESHOLD[9:8]=0, SQ_THRESH[7:4]=0x1
	 */
	wrl(USB_PHY_RX_CTRL, (rdl(USB_PHY_RX_CTRL) & ~0xc2003f0) | 0xc0000010);

	/*
	 * GL# USB-3 GL# USB-9: USB PHY IVREF Control
	 * PLLVDD12[1:0]=0x2, RXVDD[5:4]=0x3, Reserved[19]=0
	 */
	wrl(USB_PHY_IVREF_CTRL, (rdl(USB_PHY_IVREF_CTRL) & ~0x80003 ) | 0x32);

	/*
	 * GL# USB-3 GL# USB-9: USB PHY Test Group Control
	 * REG_FIFO_SQ_RST[15]=0
	 */
	wrl(USB_PHY_TST_GRP_CTRL, rdl(USB_PHY_TST_GRP_CTRL) & ~0x8000);

	/*
	 * Stop and reset controller
	 */
	wrl(USB_CMD, rdl(USB_CMD) & ~0x1);
	wrl(USB_CMD, rdl(USB_CMD) | 0x2);
	while (rdl(USB_CMD) & 0x2);

	/*
	 * GL# USB-5 Streaming disable REG_USB_MODE[4]=1
	 * TBD: This need to be done after each reset!
	 * GL# USB-4 Setup USB Host mode
	 */
	wrl(USB_MODE, 0x13);
}


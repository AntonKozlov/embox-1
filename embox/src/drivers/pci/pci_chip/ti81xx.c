/**
 * @file 
 * @brief 
 *
 * @author  Anton Kozlov 
 * @date    24.02.2014
 */

#include <stdint.h>
#include <hal/reg.h>

#include <embox/unit.h>
EMBOX_UNIT_INIT(ti81xx_pci_init);

#define TI81_CM 0x48140000
#define TI81_PCI_CFG (TI81_CM + 0x640)
#define TI81_PCI_CFG_DEVTYPE                  0x00000003
#define TI81_PCI_CFG_DEVTYPE_ROOTC            0x00000002
#define TI81_PCI_CFG_CFGPLL                   0xffff0000
#define TI81_PCI_CFG_CFGPLL_MUL25X            0x01c90000
#define TI81_PCI_CFG_STSPLL                   0x00000f00
#define TI81_PCI_CFG_STSPLL_LCK               0x00000100

#define TI81_PRCM 0x48180000
#define TI81_RM_DEF_RSTCTRL (TI81_PRCM + 0xb10)
#define TI81_RM_DEF_RSTCTRL_PCI (1 << 7)

#define TI81_RM_DEF_RSTST (TI81_PRCM + 0xb14)
#define TI81_RM_DEF_RSTST_PCI (1 << 7)

#define TI81_CM_DEF_CLKSTCTRL (TI81_PRCM + 0x510)
#define TI81_CM_DEF_CLKSTCTRL_CLKTRCTRL       0x00000003
#define TI81_CM_DEF_CLKSTCTRL_CLKTRCTRL_SLEEP 0x00000001
#define TI81_CM_DEF_CLKSTCTRL_CLKTRCTRL_WKUP  0x00000002
#define TI81_CM_DEF_CLKSTCTRL_CLK_GCLK        0x00000100
#define TI81_CM_DEF_CLKSTCTRL_CLK_GCLK_GATED  0x00000000
#define TI81_CM_DEF_CLKSTCTRL_CLK_GCLK_ACTIV  0x00000100

#define TI81_CM_DEF_PCI_CLKCTRL (TI81_PRCM + 0x578)
#define TI81_CM_DEF_PCI_CLKCTRL_MODLMODE      0x00000003
#define TI81_CM_DEF_PCI_CLKCTRL_MODLMODE_DIS  0x00000000
#define TI81_CM_DEF_PCI_CLKCTRL_MODLMODE_EN   0x00000002
#define TI81_CM_DEF_PCI_CLKCTRL_IDLE          0x00030000
#define TI81_CM_DEF_PCI_CLKCTRL_IDLE_FUNC     0x00000000
#define TI81_CM_DEF_PCI_CLKCTRL_STANBY        0x00040000

#define TI81_PCI_REGION0 0x51000000
#define TI81_PCI_CMD_STATUS_OFF		      (TI81_PCI_REGION0 + 0x4)
#define TI81_PCI_CMD_STATUS_LTSSM_EN 	      0x00000001

#define TI81_PCI_REGION1 0x20000000

#include <kernel/printk.h>

static void ti81xx_pci_clk_enable(void) {
	unsigned long reg;

	/* do domain transition wakeup */
	REG_STORE(TI81_CM_DEF_CLKSTCTRL, TI81_CM_DEF_CLKSTCTRL_CLKTRCTRL_WKUP);
	/* do clock enable */
	REG_STORE(TI81_CM_DEF_PCI_CLKCTRL, TI81_CM_DEF_PCI_CLKCTRL_MODLMODE_EN);
	/* bring out of reset */
	REG_ANDIN(TI81_RM_DEF_RSTCTRL, ~TI81_RM_DEF_RSTCTRL_PCI);

	/* wait for reset complete */
	while (!(REG_LOAD(TI81_RM_DEF_RSTST) & TI81_RM_DEF_RSTST_PCI)) {

	}

	/* clear complete reset flag */
	REG_STORE(TI81_RM_DEF_RSTST, TI81_RM_DEF_RSTST_PCI);

	/* wait for 'idle' state of peripherial */
	while (((reg = REG_LOAD(TI81_CM_DEF_PCI_CLKCTRL)) & TI81_CM_DEF_PCI_CLKCTRL_IDLE) 
		       == TI81_CM_DEF_PCI_CLKCTRL_IDLE) {
	}
}

static int ti81xx_pci_init(void) {
	
	REG_STORE(TI81_PCI_CFG, TI81_PCI_CFG_DEVTYPE_ROOTC);

	ti81xx_pci_clk_enable();


	REG_ORIN(TI81_PCI_CFG, TI81_PCI_CFG_CFGPLL_MUL25X);

	while (!(REG_LOAD(TI81_PCI_CFG) & TI81_PCI_CFG_STSPLL_LCK)) {

	}
	REG_STORE(TI81_CM_DEF_CLKSTCTRL, TI81_CM_DEF_CLKSTCTRL_CLKTRCTRL_WKUP);

	REG_ANDIN(TI81_PCI_CMD_STATUS_OFF, ~TI81_PCI_CMD_STATUS_LTSSM_EN);

	/*printk("%s: cmd_status=%08lx\n", __func__, REG_LOAD(TI81_PCI_CMD_STATUS_OFF));*/

	return 0;
}

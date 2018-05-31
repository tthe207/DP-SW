#ifndef _SWD_H_
#define	_SWD_H_
#include "sys.h"

#define SWD_PORT	GPIOB
#define SWD_CLK		GPIO_Pin_13
#define SWD_DIO		GPIO_Pin_15

#define SWD_DIO_IN()	{SWD_PORT->CRH&=0x0FFFFFFF;SWD_PORT->CRH|=0x80000000;}
#define SWD_DIO_OUT()	{SWD_PORT->CRH&=0x0FFFFFFF;SWD_PORT->CRH|=0x30000000;}

#define Set_SWDIO	PBout(15)
#define Set_SWDCLK	PBout(13)
#define Read_SWDIO	PBin(15)


// Command Status Response Codes
#define HOST_COMMAND_OK         0x55
#define HOST_INVALID_COMMAND    0x80
#define HOST_COMMAND_FAILED     0x81
#define HOST_AP_TIMEOUT         0x82
#define HOST_WIRE_ERROR         0x83
#define HOST_ACK_FAULT          0x84
#define HOST_DP_NOT_CONNECTED   0x85

// Property SRST values
#define SRST_ASSERTED           0x1
#define SRST_DEASSERTED         0x0

#define DAP_RETRY_COUNT         255

//-----------------------------------------------------------------------------
// ARM Debug Interface Constants
//-----------------------------------------------------------------------------

/* DP底层包头指令 */
#define SW_IDCODE_RD            0xA5
#define SW_ABORT_WR             0x81
#define SW_CTRLSTAT_RD          0x8D
#define SW_CTRLSTAT_WR          0xA9
#define SW_RESEND_RD            0x95
#define SW_SELECT_WR            0xB1
#define SW_RDBUFF_RD            0xBD

/* DP包头标志 */
#define SW_REQ_PARK_START       0x81
#define SW_REQ_PARITY           0x20
#define SW_REQ_A32              0x18
#define SW_REQ_RnW              0x04
#define SW_REQ_APnDP            0x02

/* DP包应答 */
#define SW_ACK_OK               0x1
#define SW_ACK_WAIT             0x2
#define SW_ACK_FAULT            0x4
#define SW_ACK_PARITY_ERR       0x8

/* DP上层包头指令 */
#define DAP_IDCODE_RD           0x02
#define DAP_ABORT_WR            0x00
#define DAP_CTRLSTAT_RD         0x06
#define DAP_CTRLSTAT_WR         0x04
#define DAP_SELECT_WR           0x08
#define DAP_RDBUFF_RD           0x0E

/* DP上层包头标志 */
#define DAP_CMD_PACKED          0x80
#define DAP_CMD_A32             0x0C
#define DAP_CMD_RnW             0x02
#define DAP_CMD_APnDP           0x01
#define DAP_CMD_MASK            0x0F


/* Cortex-M3 调试寄存器 */
#define DDFSR   0xE000ED30      // 调试 fault 状态寄存器
#define DHCSR   0xE000EDF0      // 调试停机控制及状态寄存器
#define DCRSR   0xE000EDF4      // 调试内核寄存器选择者寄存器
#define DCRDR   0xE000EDF8      // 调试内核寄存器数据寄存器
#define DEMCR   0xE000EDFC      // 调试异常及监视器控制寄存器
#define AIRCR   0xE000ED0C      // 应用程序中断及复位控制寄存器

#define DBGMCU_IDCODE	0xE0042000	//设备ID代码地址
#define LOW_DENSITY_DEV	0x412
#define MEDIUM_DENSITY_DEV	0x410
#define HIGH_DENSITY_DEV	0x414
#define XL_DENSITY_DEV		0x430
#define CONNECT_DEV			0x418

/* AP区块地址 */
#define CHIPAP_BANK_0  0x00000000      // BANK 0 => CSW, TAR, Reserved, DRW
#define CHIPAP_BANK_1  0x00000010      // BANK 1 => BD0, BD1, BD2, BD3
#define CHIPAP_BANK_F  0x000000F0      // BANK F => Reserved, CFG, BASE, ID

/* AP上层包头指令 */
#define CHIPAP_ID_WR        0x0D	//写IDR标识寄存器
#define CHIPAP_ID_RD        0x0F	//读IDR标识寄存器
#define CHIPAP_CSW_WR		0x01	//写控制与状态寄存器
#define CHIPAP_CSW_RD		0x03	//读控制与状态寄存器
#define CHIPAP_TAR_WR		0x05	//写传输地址寄存器
#define CHIPAP_TAR_RD		0x07
#define CHIPAP_DRW_WR  		0x0D	//写数据寄存器
#define CHIPAP_DRW_RD  		0x0F	//读数据寄存器

#define CHIPAP_BD0_RD		0x03	//分组寄存器0 等同于TAR
#define CHIPAP_BD1_RD		0x07	//分组寄存器1 等同于TAR + 4
#define CHIPAP_BD2_RD		0x0B	//分组寄存器2 等同于TAR + 8
#define CHIPAP_BD3_RD		0x0F	//分组寄存器3 等同于TAR + 12

#define CHIPAP_BD0_WR		0x01	//分组寄存器0 等同于TAR
#define CHIPAP_BD1_WR		0x05	//分组寄存器1 等同于TAR + 4
#define CHIPAP_BD2_WR		0x09	//分组寄存器2 等同于TAR + 8
#define CHIPAP_BD3_WR		0x0D	//分组寄存器3 等同于TAR + 12

#define FLASH_KEYR_ADD		FLASH_R_BASE + 0x04	//解锁键
#define FLASH_SR_ADD		FLASH_R_BASE + 0x0C	//闪存状态寄存器
#define FLASH_CR_ADD		FLASH_R_BASE + 0x10	//闪存控制寄存器
#define FLASH_AR_ADD		FLASH_R_BASE + 0x14	//闪存地址寄存器
#define FLASH_WRPR_ADD		FLASH_R_BASE + 0x20	//写保护寄存器

//用户根据自己的需要设置
#define STM32_FLASH_SIZE 	512 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 	1              	//使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////


//FLASH解锁键值
#define SWD_FLASH_KEY1               0X45670123
#define SWD_FLASH_KEY2               0XCDEF89AB

#define DOWNLOAD_ADDR		0x08000000
#define DOWNLOAD_DEB		0x08000700

u8 	 SWD_Connect(void);
void SWD_Init(void);
u32  SWD_GetWord(void);
void SWD_Debug(void);

void printf_bin(void);
void download_bin(void);
#endif


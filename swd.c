#include "swd.h"
#include "delay.h"
#include "usart.h"
#include "malloc.h"
#include "runled.h"
u32 swd_word;
u8 ack_error;

const u8 even_parity[] =
{
    0x00, 0x10, 0x10, 0x00,
    0x10, 0x00, 0x00, 0x10,
    0x10, 0x00, 0x00, 0x10,
    0x00, 0x10, 0x10, 0x00
};
/***********************************************************************************************
*函数名 : SWD_Init
*函数功能描述 : 初始化SWD的时钟与数据口
*函数参数 : 无
*函数返回值 : 无
*作者 : TTHE
*函数创建日期 : 2018/5/11 星期五
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
void SWD_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 

	GPIO_InitStructure.GPIO_Pin = SWD_CLK|SWD_DIO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SWD_PORT, &GPIO_InitStructure);
}

/***********************************************************************************************
*函数名 : SWD_GenOneClk
*函数功能描述 : 模拟一个时钟周期
*函数参数 : 无
*函数返回值 : 无
*作者 : TTHE
*函数创建日期 : 2018/5/11 星期五
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
void SWD_GenOneClk(void)
{
	Set_SWDCLK = 1;
	delay_us(1);
	Set_SWDCLK = 0;
	delay_us(1);
}

/***********************************************************************************************
*函数名 : SWD_Reset
*函数功能描述 : 根据SWD协议 发出至少50个空闲时钟周期
*函数参数 : 无
*函数返回值 : 无
*作者 : TTHE
*函数创建日期 : 2018/5/11 星期五
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
void SWD_Reset(void)
{
	u8 i;
	Set_SWDIO = 1;
	SWD_DIO_OUT();

	for(i = 0; i < 64; i ++)
	{
		SWD_GenOneClk();
	}
}

/***********************************************************************************************
*函数名 : SWD_SendByte
*函数功能描述 : SWD接口发送一个字节
*函数参数 : data - 发送数据
*函数返回值 : 无
*作者 : TTHE
*函数创建日期 : 2018/5/11 星期五
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
void SWD_SendByte(u8 data)
{
	u8 i;
	SWD_DIO_OUT();
	for(i = 0; i < 8; i ++)
	{
		if(data&0x01)
		{
			Set_SWDIO = 1;
		}
		else
		{
			Set_SWDIO = 0;
		}
		data = data >> 1;
		SWD_GenOneClk();
	}
}

/***********************************************************************************************
*函数名 : SWD_ReadByte
*函数功能描述 : SWD接口读一个字节
*函数参数 : 无
*函数返回值 : 读到字节值
*作者 : TTHE
*函数创建日期 : 2018/5/11 星期五
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
u8 SWD_ReadByte(void)
{
	u8 res,i;
	SWD_DIO_IN();
	res = 0;
	for(i = 0; i < 8; i++)
	{
		res |= Read_SWDIO << i;
		SWD_GenOneClk();
	}
	return res;
}

/***********************************************************************************************
*函数名 : SWD_CalcParity
*函数功能描述 : 计算 swd_word 值的奇偶补充位
*函数参数 : 无
*函数返回值 : 若swd_word中1的个数为奇数 返回1 否则返回0
*作者 : TTHE
*函数创建日期 : 2018/5/11 星期五
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
u8 SWD_CalcParity(void)
{
	u16 parity;

	parity = 0xffff & (swd_word ^ (swd_word >> 16));
	parity = 0xff & (parity ^ (parity >> 8));
	parity = 0x0f & (parity ^ (parity >> 4));
	parity = 0x03 & (parity ^ (parity >> 2));
	
	return 0x01 & (parity ^ (parity >> 1));
}

/***********************************************************************************************
*函数名 : SWD_Packet
*函数功能描述 : SWD接口的一次完整包传输
*函数参数 : request - 包头
*函数返回值 : 返回主机应答
*作者 : TTHE
*函数创建日期 : 2018/5/11 星期五
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
u8 SWD_Packet(u8 request, u8 retry)
{
	u8 ack = 0,limit,i;
	u8 paritybit;
	if(retry == 0)
	{
		retry = DAP_RETRY_COUNT;
	}
	limit = retry;

	do
	{
		Set_SWDIO = 0;
		SWD_DIO_OUT();
		SWD_GenOneClk();

		SWD_SendByte(request);

		SWD_DIO_IN();
		SWD_GenOneClk();

		for(i = 0; i < 3; i++)
		{
			ack |= Read_SWDIO << i;
			SWD_GenOneClk();
		}

		if((ack == SW_ACK_WAIT) && --retry)
		{
			for(i = retry; i < limit; i++)
				delay_us(20);
		}
		else
		{
			break;
		}
	}while(1);

	if(ack == SW_ACK_OK)
	{
		if(request & SW_REQ_RnW)
		{
			swd_word = 0;
			swd_word = SWD_ReadByte();
			swd_word |= SWD_ReadByte() << 8;
			swd_word |= SWD_ReadByte() << 16;
			swd_word |= SWD_ReadByte() << 24;
			paritybit = Read_SWDIO;
			SWD_GenOneClk();
			if(paritybit ^ SWD_CalcParity())
			{
				ack = SW_ACK_PARITY_ERR;
			}
		}
		else
		{
			SWD_DIO_OUT();
			SWD_GenOneClk();
			SWD_SendByte(swd_word & 0xff);
			SWD_SendByte((swd_word >> 8) & 0xff);
			SWD_SendByte((swd_word >> 16) & 0xff);
			SWD_SendByte((swd_word >> 24) & 0xff);
		
			Set_SWDIO = 0x01 & SWD_CalcParity();
			SWD_GenOneClk();
		}
	}
	Set_SWDIO = 0;
	SWD_DIO_OUT();
	SWD_GenOneClk();

	if(ack != SW_ACK_OK)
	{
		ack_error = ack;
	}
	return ack;
}

/***********************************************************************************************
*函数名 : SWD_Response
*函数功能描述 : 转义应答符号
*函数参数 : SW_Ack - 实际应答
*函数返回值 : 转义后应答
*作者 : TTHE
*函数创建日期 : 2018/5/11 星期五
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
u8 SWD_Response(u8 SW_Ack)
{
    switch (SW_Ack)
    {
	    case SW_ACK_OK:     return HOST_COMMAND_OK;
	    case SW_ACK_WAIT:   return HOST_AP_TIMEOUT;
	    case SW_ACK_FAULT:  return HOST_ACK_FAULT;
	    default:            return HOST_WIRE_ERROR;
    }
}

/***********************************************************************************************
*函数名 : SWD_LineReset
*函数功能描述 : 符合协议规范的一次 线复位操作
*函数参数 : 无
*函数返回值 : 复位应答
*作者 : TTHE
*函数创建日期 : 2018/5/11 星期五
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
u8 SWD_LineReset(void)
{
	u8 ack;
	SWD_Reset();
	SWD_SendByte(0);
	ack = SWD_Packet(SW_IDCODE_RD,1);

	SWD_SendByte(0);
	return SWD_Response(ack);
}

u32 SWD_GetWord(void)
{
	return swd_word;
}

/***********************************************************************************************
*函数名 : SWD_SwitchJTAG2SW
*函数功能描述 : 将DP接口切换为SW-DP
*函数参数 : 无
*函数返回值 : 无
*作者 : TTHE
*函数创建日期 : 2018/5/12 星期六
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
void SWD_SwitchJTAG2SW(void)
{
	SWD_Reset();
	/* 切换DP接口为 SW-DP */
	SWD_SendByte(0x9E);	
	SWD_SendByte(0xE7);
}

/***********************************************************************************************
*函数名 : SWD_Connect
*函数功能描述 : 连接主机
*函数参数 : 无
*函数返回值 : 应答
*作者 : TTHE
*函数创建日期 : 2018/5/11 星期五
*函数修改日期 : 尚未修改
*修改人 : 尚未修改
*修改原因 :  尚未修改
***********************************************************************************************/
u8 SWD_Connect(void)
{
	u8 res;
	SWD_SwitchJTAG2SW();
	res = SWD_LineReset();
	return res;
}

u8 SWD_Request(u8 DAP_Addr)
{
    u8 req;

    // Convert the DAP address into a SWD packet request value
    req = DAP_Addr & DAP_CMD_MASK;      // mask off the bank select bits
    req = req | even_parity[req];       // compute and add parity bit
    req = req << 1;                     // move address/parity bits
    req = req | SW_REQ_PARK_START;      // add start and park bits
    return req;
}


u32 SWD_DAP_Read(u8 DAP_Addr)
{
    u8 req;
	u32 read_data;
	
    // Reset global error accumulator
    ack_error = SW_ACK_OK;
    // Format the packet request header
    req = SWD_Request(DAP_Addr);

    // Shift the first packet and if DP access, send the results
    SWD_Packet(req, 0);
    if (!(req & SW_REQ_APnDP))
    {
        read_data = swd_word;
    }


    // For AP access, get and send results of the last read
    if (req & SW_REQ_APnDP)
    {
        SWD_Packet(SW_RDBUFF_RD, 0);
        read_data = swd_word;
    }
	
    // Finish with idle cycles
    SWD_SendByte(0);
	return read_data;
}

void SWD_DAP_Write(u8 DAP_Addr, u32 write_data)
{
    u8 req;
    // Reset global error accumulator
    ack_error = SW_ACK_OK;

    // Format the packet request header
    req = SWD_Request(DAP_Addr);

    // Perform the requested number of writes
    swd_word = write_data;

    SWD_Packet(req, 0);

    // For AP access, check results of last write (use default retry count
    // because previous write may need time to complete)
    if (req & SW_REQ_APnDP)
    {
        SWD_Packet(SW_RDBUFF_RD, 0);
    }
	
    // Finish with idle cycles
    SWD_SendByte(0);
}

void SWD_SelectAPBank(u32 banknum)
{
    SWD_DAP_Write(DAP_SELECT_WR, banknum);
}

void SWD_SelectAddrInAPBank0(u32 addr)
{
	SWD_SelectAPBank(CHIPAP_BANK_0);
    SWD_DAP_Write(CHIPAP_TAR_WR, addr);
}

void SWD_WriteDRWInAPBank(u32 drwvalue)
{
    SWD_DAP_Write(CHIPAP_DRW_WR, drwvalue);
}

u32 SWD_ReadDRWInAPBank(void)
{
	return SWD_DAP_Read(CHIPAP_DRW_RD);
}

void SWD_SetValueSize(u8 size)
{
	u32 rw_data = 0;
	
	SWD_SelectAPBank(CHIPAP_BANK_0);
    rw_data = SWD_DAP_Read(CHIPAP_CSW_RD);
	rw_data &= ~(0x3f);  
	rw_data |= size|0x10;
    SWD_DAP_Write(CHIPAP_CSW_WR, rw_data);
}

void SWD_WriteValueToAddr(u32 addr,u32 dvalue)
{
	SWD_SelectAddrInAPBank0(addr);
	SWD_WriteDRWInAPBank(dvalue);
}

u32 SWD_ReadValueInAddr(u32 addr)
{
	u32 res = 0;
	SWD_SelectAddrInAPBank0(addr);
	res = SWD_ReadDRWInAPBank();
	
	return res;
}

u32 SWD_ReadPID(void)
{
	/* 选择F BANK 读取ID */
	SWD_SelectAPBank(CHIPAP_BANK_F);
    return SWD_DAP_Read(CHIPAP_ID_RD);
}

void SWD_UnlockFlash(void)
{
	SWD_WriteValueToAddr(FLASH_KEYR_ADD,SWD_FLASH_KEY1);
	SWD_WriteValueToAddr(FLASH_KEYR_ADD,SWD_FLASH_KEY2);
	printf("flash unlock ack is %#x\r\n",SWD_Response(ack_error));
}
void SWD_lockFlash(void)
{
	u32 rw_data;
	rw_data = SWD_ReadValueInAddr(FLASH_CR_ADD);
	rw_data |= 1<<7;//上锁
	SWD_WriteValueToAddr(FLASH_CR_ADD,rw_data);
}
//得到FLASH状态
u8 SWD_GetFlashStatus(void)
{	
	u32 rw_data;
	rw_data =SWD_ReadValueInAddr(FLASH_SR_ADD);

	if(rw_data&(1<<0))return 1;		    //忙
	else if(rw_data&(1<<2))return 2;	//编程错误
	else if(rw_data&(1<<4))return 3;	//写保护错误
	return 0;							//操作完成
}

u8 SWD_WaitFlashDone(u16 time)
{
	u8 res;
	do
	{
		res=SWD_GetFlashStatus();
		if(res!=1)break;//非忙,无需等待了,直接退出.
		delay_us(1);
		time--;
	 }while(time);
	 if(time==0)res=0xff;//TIMEOUT
	 return res;
}

u8 SWD_FlashErasePage(u32 paddr)
{
	u8 res=0;
	u32 rw_data;
	res=SWD_WaitFlashDone(0X5FFF);//等待上次操作结束,>20ms    
	if(res==0)
	{ 
		
		rw_data = SWD_ReadValueInAddr(FLASH_CR_ADD);
		rw_data |= 1<<1;//页擦除
		SWD_WriteValueToAddr(FLASH_CR_ADD, rw_data);
		SWD_WriteValueToAddr(FLASH_AR_ADD, paddr);//设置页地址
		rw_data = SWD_ReadValueInAddr(FLASH_CR_ADD);
		rw_data |= 1<<6;//开始擦除		
		SWD_WriteValueToAddr(FLASH_CR_ADD, rw_data);
		
		res=SWD_WaitFlashDone(0X5FFF);//等待操作结束,>20ms  
		
		if(res!=1)//非忙
		{
			rw_data = SWD_ReadValueInAddr(FLASH_CR_ADD);
			rw_data &=~(1<<1);//清除页擦除标志.	
			SWD_WriteValueToAddr(FLASH_CR_ADD, rw_data);
		}
	}
	return res;
}

u8 SWD_FlashWriteHalfWord(u32 faddr, u32 dat)
{
	u8 res;	   	    
	u32 rw_data;
	res=SWD_WaitFlashDone(0XFF);	 
	if(res==0)//OK
	{
		rw_data = SWD_ReadValueInAddr(FLASH_CR_ADD);
		rw_data |=1<<0;		//编程使能
		SWD_WriteValueToAddr(FLASH_CR_ADD, rw_data);
		
		SWD_SetValueSize(1);
		SWD_WriteValueToAddr(faddr,dat);
		SWD_SetValueSize(2);
		
		res=SWD_WaitFlashDone(0XFF);//等待操作完成
		if(res!=1)//操作成功
		{
			rw_data = SWD_ReadValueInAddr(FLASH_CR_ADD);
			rw_data &=~(1<<0);	//清除PG位.
			SWD_WriteValueToAddr(FLASH_CR_ADD, rw_data);
		}
	} 
	return res;
} 

u8 SWD_FlashWriteWord(u32 faddr, u32 dat)
{
	SWD_FlashWriteHalfWord(faddr,dat);
	return SWD_FlashWriteHalfWord(faddr + 2,dat);
}


void SWD_Debug(void)
{
    u32 rw_data = 0;
	u8 i = 0;	
	
	/* 系统及调试上电请求 */
    rw_data = 0x50000000;
    SWD_DAP_Write(DAP_CTRLSTAT_WR, rw_data);

	/* 清楚错误标志 */
	rw_data = 0x1E;
    SWD_DAP_Write(DAP_ABORT_WR, rw_data);


	/* 设置MEMAP之间数据传输32bit格式 且地址自增 */
	SWD_SetValueSize(2);

	
	rw_data = SWD_ReadValueInAddr(FLASH_CR_ADD);
	printf("CR REG IS %#x\r\n",rw_data);

//	SWD_UnlockFlash();
	
	rw_data = SWD_ReadValueInAddr(FLASH_CR_ADD);
	printf("CR REG IS %#x\r\n",rw_data);
	printf("ACK %#x\r\n",SWD_Response(ack_error));

	
//	SWD_FlashWriteWord(DOWNLOAD_ADDR,0x12345578);


	/*
	rw_data = SWD_ReadValueInAddr(FLASH_CR_ADD);
	rw_data |=1<<0; 	//编程使能
	SWD_WriteValueToAddr(FLASH_CR_ADD, rw_data);
	
	rw_data = SWD_ReadValueInAddr(FLASH_CR_ADD);
	printf("CR REG IS %#x\r\n",rw_data);

	rw_data = SWD_ReadValueInAddr(DOWNLOAD_DEB);
	printf("VALUE IS %#x\r\n",rw_data);
	SWD_SelectAddrInAPBank0(DOWNLOAD_DEB + 6);
	printf("ACK %#x\r\n",SWD_Response(ack_error));
//	SWD_WriteDRWInAPBank(0x01);
//	SWD_WriteDRWInAPBank(0x02);
//	SWD_WriteDRWInAPBank(0x04);
//	SWD_WriteDRWInAPBank(0x05);
	printf("ACK %#x\r\n",SWD_Response(ack_error));
	*/
#if 0
	rw_data = CHIPAP_BANK_0;
    SWD_DAP_Move(0, DAP_SELECT_WR, &rw_data);
	rw_data = 0x08000710;
    res = SWD_DAP_Move(0, CHIPAP_TAR_WR, &rw_data);
	printf("1. %#x\r\n",res);
	rw_data = CHIPAP_BANK_1;
    SWD_DAP_Move(0, DAP_SELECT_WR, &rw_data);
	rw_data = 0x12000000;
 //   res = SWD_DAP_Move(0, CHIPAP_BD0_WR, &rw_data);
	rw_data = 0;
    res = SWD_DAP_Move(0, CHIPAP_BD0_RD, &rw_data);
	printf("1. %#x\r\n",res);
	printf("WORD IS %#x\r\n",rw_data);
	rw_data = 0x00345678;
//    SWD_DAP_Move(0, CHIPAP_BD0_WR, &rw_data);
    res = SWD_DAP_Move(0, CHIPAP_BD1_RD, &rw_data);
	printf("1. %#x\r\n",res);
	printf("WORD IS %#x\r\n",rw_data);
#endif
}

void printf_bin(void)
{
	u8 i = 0;
	u32 rw_data;
	

	SWD_SelectAddrInAPBank0(DOWNLOAD_ADDR);

	for(i = 0; i < 10; i++)
	{
    	rw_data = SWD_DAP_Read(CHIPAP_DRW_RD);
		printf("WORD IS %#010x\r\n",rw_data);
	}
	SWD_SelectAddrInAPBank0(DOWNLOAD_ADDR + 1024);

	for(i = 0; i < 10; i++)
	{
    	rw_data = SWD_DAP_Read(CHIPAP_DRW_RD);
		printf("WORD IS %#010x\r\n",rw_data);
	}
	SWD_SelectAddrInAPBank0(DOWNLOAD_ADDR + 2048);

	for(i = 0; i < 10; i++)
	{
    	rw_data = SWD_DAP_Read(CHIPAP_DRW_RD);
		printf("WORD IS %#010x\r\n",rw_data);
	}	
	SWD_SelectAddrInAPBank0(DOWNLOAD_ADDR + 3072);

	for(i = 0; i < 10; i++)
	{
    	rw_data = SWD_DAP_Read(CHIPAP_DRW_RD);
		printf("WORD IS %#010x\r\n",rw_data);
	}
}

void download_bin(void)
{
	u16 j,sectorbyte;
	u32 data_word = 0,size,i,sector;
	u32 addr,rw_data;
	/* 使能停机模式的调试 */
	rw_data = 0xA05F0001;
	SWD_WriteValueToAddr(DHCSR, rw_data);
	
	rw_data = 0xA05F0003;
	SWD_WriteValueToAddr(DHCSR, rw_data);
	
	rw_data = SWD_ReadValueInAddr(DBGMCU_IDCODE) & 0xFFF;
	switch(rw_data)
	{
		case LOW_DENSITY_DEV:
			/* 小容量设备 */
			sectorbyte = 1024;
			printf("low-density devices\r\n");
			break;
		case MEDIUM_DENSITY_DEV:
			/* 中容量设备 */
			sectorbyte = 1024;
			printf("medium-density devices\r\n");
			break;
		case HIGH_DENSITY_DEV:
			/* 大容量设备 */
			sectorbyte = 2048;
			printf("high-density devices\r\n");
			break;
		default:break;
	}
	
	SWD_UnlockFlash();

	sector = sizeof(binary) / sectorbyte;
	size = sizeof(binary) % sectorbyte;
	for(j = 0; j < sector; j++)
	{
		SWD_FlashErasePage(DOWNLOAD_ADDR + j * sectorbyte);
		for(i = 0; i < sectorbyte/4; i++)
		{
			addr = j * sectorbyte + i * 4;
			data_word = binary[addr];
			data_word |= binary[addr + 1]<<8;
			data_word |= binary[addr + 2]<<16;
			data_word |= binary[addr + 3]<<24;
			SWD_FlashWriteWord(DOWNLOAD_ADDR + addr,data_word);
		}
	}
	
	SWD_FlashErasePage(DOWNLOAD_ADDR + j * sectorbyte);
	for(i = 0; i < size/4; i++)
	{
		addr = j * sectorbyte + i * 4;
		data_word = binary[addr];
		data_word |= binary[addr + 1]<<8;
		data_word |= binary[addr + 2]<<16;
		data_word |= binary[addr + 3]<<24;
		
		SWD_FlashWriteWord(DOWNLOAD_ADDR + addr,data_word);
	}
	delay_ms(10);
	SWD_lockFlash();
	
	rw_data = 0xA05F0001;
	SWD_WriteValueToAddr(DHCSR, rw_data);
	
	rw_data = 0x05FA0001;
	SWD_WriteValueToAddr(AIRCR, rw_data);
	printf("OK\r\n");
}

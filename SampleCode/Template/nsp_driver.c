#include "nsp_driver.h"

//Use this definition if the input mode is QUASI
//#define QUASI_MODE //�������ģʽ����QUASI����ʹ�ô˶���
//===========================================================
//Systerm intial, please modify according to your MCU platform

#define NSP_TIMER		(TIMER0)

#define SCLK_PORT		(PC)
#define SCLK_PIN_MASK	(BIT0)
#define SCLK_SET_LOW	(PC0=0)
#define SCLK_SET_HIGH	(PC0=1)

#define SDA_PORT		(PC)
#define SDA_PIN_MASK	(BIT1)
#define SDA_SET_LOW		(PC1=0)
#define SDA_SET_HIGH	(PC1=1)


/*#define SCLK_PORT		BP0R
#define SCLK_PORT_BIT	BIT0
#define SCLK_PORT_D		BP0D
#define SCLK_PORT_M		BP0M

#define SDA_PORT		BP0R
#define SDA_PORT_BIT	BIT1
#define SDA_PORT_D		BP0D
#define SDA_PORT_M		BP0M*/

UINT8 	u8HOST_FLAG=0; 
UINT8 	u8RX_ERROR_COUNT=0;
UINT16  u16CMD_TX_BYTE	= 0;
UINT16  u16CMD_RX_BYTE	= 0;
UINT8  	CMD_TX_DATA[8]={0};		
UINT8  	CMD_RX_DATA[13]={0};			

//===========================================================
//Systerm intial, please modify according to your MCU platform
void GPIO1_SCLK_OUTPUT(void) 
{
//�]�m����X�Ҧ�  Set to output mode
    GPIO_SetMode(SCLK_PORT, SCLK_PIN_MASK, GPIO_MODE_OUTPUT);
}

//Systerm intial, please modify according to your MCU platform
void GPIO1_SCLK_OUTPUT0(void) 
{
//�]�m����X�C�q�� Set to output low level
    SCLK_SET_LOW;
}

//Systerm intial, please modify according to your MCU platform
void GPIO1_SCLK_OUTPUT1(void) 
{
    //�]�m����X���q�� Set to output High level
    SCLK_SET_HIGH;
}
//----------------------------------
//Systerm intial, please modify according to your MCU platform
void GPIO2_SDA_OUTPUT(void) 
{
//�]�m����X�Ҧ�  Set to output mode
    GPIO_SetMode(SDA_PORT, SDA_PIN_MASK, GPIO_MODE_OUTPUT);
}
//Systerm intial, please modify according to your MCU platform
void GPIO2_SDA_OUTPUT0(void) 
{
//�]�m����X�C�q�� Set to output low level
    SDA_SET_LOW;
}
//Systerm intial, please modify according to your MCU platform
void GPIO2_SDA_OUTPUT1(void) 
{
//�]�m����X���q�� Set to output High level
    SDA_SET_HIGH;
}
//----------------------------------
//Systerm intial, please modify according to your MCU platform
void GPIO2_SDA_INPUT(void) 
{
//�]�m����J�Ҧ�  Set to input mode
    GPIO_SetMode(SDA_PORT, SDA_PIN_MASK, GPIO_MODE_INPUT); 
}
//Systerm intial, please modify according to your MCU platform
UINT8 GPIO2_SDA_INPUT_READ() 
{
	UINT8 u8Res = 0;
//Ū����J�ƭ�     Read input values
    if ((GPIO_GET_IN_DATA(SDA_PORT)&SDA_PIN_MASK)!=0) {
        u8Res = 1;
    }
	return (u8Res);
}
//----------------------------------
//Systerm intial, please modify according to your MCU platform, during which you can perform other operations on host
void HOST_PIN_DURATION(void)
{
//�]�m����ɶ� 100us;  Delay time 100 us
    // TIMER_Delay(NSP_TIMER, 100);
	CLK_SysTickDelay(100);
}

//Systerm intial, please modify according to your MCU platform, during which you can perform other operations on host
void HOST_PIN_DURATION_SHORT(void)
{
//�]�m����ɶ� 10us;  Delay time 10 us
//NSP ISP CLOCK Typical = 48KHZ
//NSP ISP CLOCK Max = 58KHZ 
//DELAY 10.42 us        CLK=48KHZ
//DELAY 10 us           CLK=50KHZ
//DELAY 9.1 us          CLK=55KHZ
//DELAY 8.62 us         CLK=58KHZ

	// TIMER_Delay(NSP_TIMER, 10);
	CLK_SysTickDelay(10);
}

//Systerm intial,0 please modify according to your MCU platform, during which you can perform other operations on host
void HOST_CMD_INTERVAL_SHORT(void)
{
//�]�m����ɶ� 60us;  Delay time 60 us
    // TIMER_Delay(NSP_TIMER, 60);
	CLK_SysTickDelay(60);
}

//Systerm intial, please modify according to your MCU platform, during which you can perform other operations on host
void HOST_CMD_INTERVAL(void)
{
//�]�m����ɶ� 300us;  Delay time 300 us
    // TIMER_Delay(NSP_TIMER, 300);
	CLK_SysTickDelay(300);
}

//Systerm intial, please modify according to your MCU platform, during which you can perform other operations on host
void HOST_Delay500uS(void)
{
//�]�m����ɶ� 500us;  Delay time 500 us
    // TIMER_Delay(NSP_TIMER, 500);
	CLK_SysTickDelay(500);
}
//===========================================================
//protocol
void HOST_BUS_Init(void)
{
	GPIO1_SCLK_OUTPUT();
	GPIO2_SDA_OUTPUT();
	GPIO1_SCLK_OUTPUT1();
	GPIO2_SDA_OUTPUT1();	
}

void CMD_RX_DATA_INIT(void)
{
	CMD_RX_DATA[0] = 0;
	CMD_RX_DATA[1] = 0;
	CMD_RX_DATA[2] = 0;
	CMD_RX_DATA[3] = 0;
	CMD_RX_DATA[4] = 0;
	CMD_RX_DATA[5] = 0;
	CMD_RX_DATA[6] = 0;
	CMD_RX_DATA[7] = 0;
	CMD_RX_DATA[8] = 0;
	CMD_RX_DATA[9] = 0;
	CMD_RX_DATA[10] = 0;
	CMD_RX_DATA[11] = 0;
	CMD_RX_DATA[12] = 0;
}

void HOST_Init(UINT8* SP_VOL)
{
	*SP_VOL = 0x80;	
	HOST_BUS_Init();
}

void I2C_LIKE_START(void)
{
	//SCLK:1,SDA:1->0
	GPIO1_SCLK_OUTPUT1();
	HOST_PIN_DURATION();
	
	GPIO2_SDA_OUTPUT1();
	HOST_PIN_DURATION();
	GPIO2_SDA_OUTPUT0();
	HOST_PIN_DURATION();
}

void I2C_LIKE_STOP(void)
{
	//SCLK:1,SDA:0->1
	GPIO1_SCLK_OUTPUT0();
	HOST_PIN_DURATION();
	GPIO2_SDA_OUTPUT();		//SDA:input->output
	GPIO2_SDA_OUTPUT0();
	HOST_PIN_DURATION();
	
	GPIO1_SCLK_OUTPUT1();
	HOST_PIN_DURATION();
	GPIO2_SDA_OUTPUT1();
	HOST_PIN_DURATION();	
}

void I2C_LIKE_ISP_STOP(void)
{
	//SCLK:1,SDA:0->1
	GPIO1_SCLK_OUTPUT0();
	HOST_PIN_DURATION();
	GPIO2_SDA_OUTPUT();		//SDA:input->output
	GPIO2_SDA_OUTPUT0();
	HOST_PIN_DURATION();
	
	GPIO1_SCLK_OUTPUT1();
	HOST_PIN_DURATION();
	GPIO2_SDA_OUTPUT1();
	//HOST_PIN_DURATION();	
}

void I2C_LIKE_START_SHORT(void)
{
	//SCLK:1,SDA:1->0
	GPIO1_SCLK_OUTPUT1();
	HOST_PIN_DURATION_SHORT();
	
	GPIO2_SDA_OUTPUT1();
	HOST_PIN_DURATION_SHORT();
	GPIO2_SDA_OUTPUT0();
	HOST_PIN_DURATION_SHORT();
}

void I2C_LIKE_STOP_SHORT(void)
{
	//SCLK:1,SDA:0->1
	GPIO1_SCLK_OUTPUT0();
	HOST_PIN_DURATION_SHORT();
	GPIO2_SDA_OUTPUT();		//SDA:input->output
	GPIO2_SDA_OUTPUT0();
	HOST_PIN_DURATION_SHORT();
	
	GPIO1_SCLK_OUTPUT1();
	HOST_PIN_DURATION_SHORT();
	GPIO2_SDA_OUTPUT1();
	HOST_PIN_DURATION_SHORT();	
}

void I2C_LIKE_ISP_STOP_SHORT(void)
{
	//SCLK:1,SDA:0->1
	GPIO1_SCLK_OUTPUT0();
	HOST_PIN_DURATION_SHORT();	
	GPIO2_SDA_OUTPUT();		//SDA:input->output
	GPIO2_SDA_OUTPUT0();
	HOST_PIN_DURATION_SHORT();
	
	GPIO1_SCLK_OUTPUT1();
	HOST_PIN_DURATION_SHORT();
	GPIO2_SDA_OUTPUT1();
	//HOST_PIN_DURATION();		
}
//------------------------------------------------
UINT8 I2C_LIKE_TX_RX_PROCESS()
{	
	UINT8 i = 0;
	UINT8 j = 0;
	UINT8 n = 0;
	UINT8 u8BitMap[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	
	if ((CMD_TX_DATA[0] == CMD_IO_TYPE) || (CMD_TX_DATA[0] == CMD_GET_INOUT))
	{
		n = 1;
	}
	
	CMD_RX_DATA_INIT();
	u8HOST_FLAG |= FLAG_TX_DOING;
	
	I2C_LIKE_START();
	
	//TX process
	for(i = 0; i < u16CMD_TX_BYTE; i++)
	{
		for(j = 0; j < 8; j++)
		{
			GPIO1_SCLK_OUTPUT0();
			//if( ((CMD_TX_DATA[i] << j) & 0x80) > 0 )
			if ((CMD_TX_DATA[i] & u8BitMap[j]) !=0 )
			{
				GPIO2_SDA_OUTPUT1();
			}
			else
			{
				GPIO2_SDA_OUTPUT0();
			}
			HOST_PIN_DURATION();
			
			GPIO1_SCLK_OUTPUT1();
			HOST_PIN_DURATION();
		}
	}
	
	u8HOST_FLAG &= ~FLAG_TX_DOING;
	u8HOST_FLAG |= FLAG_RX_DOING;
#ifdef QUASI_MODE
    	GPIO2_SDA_OUTPUT1();
#endif
	GPIO2_SDA_INPUT();
	
	//RX process
	for(i = 0; i < u16CMD_RX_BYTE; i++)
	{
		for(j = 0; j < 8; j++)
		{
			GPIO1_SCLK_OUTPUT0();
			HOST_PIN_DURATION();
			
			GPIO1_SCLK_OUTPUT1();
			HOST_PIN_DURATION();
			
			CMD_RX_DATA[i]=CMD_RX_DATA[i]<<1;
			if ( GPIO2_SDA_INPUT_READ() != 0 )
			{
				CMD_RX_DATA[i] = CMD_RX_DATA[i] | 0x1;
			}
		}		
	}			
	
	I2C_LIKE_STOP();
	u8HOST_FLAG &= ~FLAG_RX_DOING;
	
	if (u16CMD_RX_BYTE == 1)
	{
		if (CMD_RX_DATA[0] == RIGHT_RTN)
		{
			u8RX_ERROR_COUNT = 0;
			return 1;
		}
		if (CMD_RX_DATA[0] == ERROR_RTN)
		{
			u8RX_ERROR_COUNT ++;
			return 0;
		}	
		if (CMD_RX_DATA[0] == UNSUPPORTED_RTN)
		{
			u8RX_ERROR_COUNT = 0;
			return 0xFF;
		}
	}
	else if (u16CMD_RX_BYTE == 2)
	{
		if (((UINT8)(CMD_RX_DATA[0] ^ CMD_RX_DATA[1]) & 0xFF)== 0xFF) 
		{
			u8RX_ERROR_COUNT = 0;
			if ((n == 1) && (CMD_RX_DATA[0] == UNSUPPORTED_RTN))
			{
				return 0xFF;	
			}
			else
			{
				return 1;
			}
		}
		else 
		{
			u8RX_ERROR_COUNT ++;
			return 0;
		}
	}
	else
	{
		j = 0;
		for(i = 0; i <= (u16CMD_RX_BYTE-2); i++)
		{
			j = j + CMD_RX_DATA[i];
		}
		j = j ^ CMD_RX_DATA[u16CMD_RX_BYTE-1];
		
		if (j == 0xFF)
		{
			u8RX_ERROR_COUNT = 0;
			return 1;
		}
		else
		{
			u8RX_ERROR_COUNT ++;
			return 0;
		}
	}
	return 0;
}
//------------------------------------------------
UINT8 I2C_LIKE_TX_RX_ISP_PROCESS_NO_BUFFER()
{	
	UINT8 i = 0;
	UINT8 j = 0;
	UINT8 n = 0;
	UINT8 u8BitMap[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	
	CMD_RX_DATA_INIT();
	u8HOST_FLAG |= FLAG_TX_DOING;
	
	I2C_LIKE_START();
	
	//TX process
	for(i = 0; i < u16CMD_TX_BYTE; i++)
	{
		for(j = 0; j < 8; j++)
		{
			GPIO1_SCLK_OUTPUT0();
			if ((CMD_TX_DATA[i] & u8BitMap[j]) !=0 )
			{
				GPIO2_SDA_OUTPUT1();
			}
			else
			{
				GPIO2_SDA_OUTPUT0();
			}
			HOST_PIN_DURATION();
			
			GPIO1_SCLK_OUTPUT1();
			HOST_PIN_DURATION();
		}
	}
	
	u8HOST_FLAG &= ~FLAG_TX_DOING;
	u8HOST_FLAG |= FLAG_RX_DOING;
#ifdef QUASI_MODE
    	GPIO2_SDA_OUTPUT1();
#endif
	GPIO2_SDA_INPUT();
	
	//RX process
	for(i = 0; i < u16CMD_RX_BYTE; i++)
	{
		for(j = 0; j < 8; j++)
		{
			GPIO1_SCLK_OUTPUT0();
			HOST_PIN_DURATION();
			
			GPIO1_SCLK_OUTPUT1();
			HOST_PIN_DURATION();
			
			CMD_RX_DATA[i]=CMD_RX_DATA[i]<<1;
			if ( GPIO2_SDA_INPUT_READ() != 0 )
			{
				CMD_RX_DATA[i] = CMD_RX_DATA[i] | 0x1;
			}
		}		
	}			
	
	I2C_LIKE_ISP_STOP();
	u8HOST_FLAG &= ~FLAG_RX_DOING;
	
	if (u16CMD_RX_BYTE == 1)
	{
		if (CMD_RX_DATA[0] == RIGHT_RTN)
		{
			u8RX_ERROR_COUNT = 0;
			return 1;
		}
		if (CMD_RX_DATA[0] == ERROR_RTN)
		{
			u8RX_ERROR_COUNT ++;
			return 0;
		}	
		if (CMD_RX_DATA[0] == UNSUPPORTED_RTN)
		{
			u8RX_ERROR_COUNT = 0;
			return 0xFF;
		}
	}
	return 0;
}
//------------------------------------------------
UINT8 I2C_LIKE_TX_RX_BUFFER_PROCESS(UINT8 *DATA_BUFFER)
{	
	UINT16 i = 0;
	UINT8 j = 0;
	UINT8 n = 0;
	UINT8 m = 0;
	UINT8 CMD_CHECKSUM = 0;
	//UINT8 CMD_CHECKSUM_CHECK = 0;
	UINT8 u8BitMap[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	
	if ((CMD_TX_DATA[0] == CMD_MULTI_PLAY) || (CMD_TX_DATA[0] == CMD_MULTI_PLAY_2B))
	{
		n = 1;
	}
	/*else if (CMD_TX_DATA[0] == )
	{
		n = 2;
	}*/
	
	CMD_RX_DATA_INIT();
	u8HOST_FLAG |= FLAG_TX_DOING;
	
	I2C_LIKE_START();
	
	//TX process
	for(i = 0; i < u16CMD_TX_BYTE; i++)
	{
		if (n == 1)
		{
			if (i <= 1) 
			{
				m = i;
				CMD_CHECKSUM = ((CMD_CHECKSUM + CMD_TX_DATA[m]) & 0xFF);
			}
			else if ((i >= 2) && (i < (u16CMD_TX_BYTE - 1)))
			{
				m = 2;
				CMD_TX_DATA[m] = *DATA_BUFFER;
				CMD_CHECKSUM = ((CMD_CHECKSUM + CMD_TX_DATA[m]) & 0xFF);
				DATA_BUFFER++;
			}
			else if (i == (u16CMD_TX_BYTE - 1))
			{
				CMD_CHECKSUM  = CMD_CHECKSUM  ^ 0xFF;
				m = 3;
				CMD_TX_DATA[m] = CMD_CHECKSUM;
			}
		}
		
		for(j = 0; j < 8; j++)
		{
			GPIO1_SCLK_OUTPUT0();
			if ((CMD_TX_DATA[m] & u8BitMap[j]) !=0 )
			{
				GPIO2_SDA_OUTPUT1();
			}
			else
			{
				GPIO2_SDA_OUTPUT0();
			}
			HOST_PIN_DURATION();
			
			GPIO1_SCLK_OUTPUT1();
			HOST_PIN_DURATION();
		}
	}
	
	u8HOST_FLAG &= ~FLAG_TX_DOING;
	u8HOST_FLAG |= FLAG_RX_DOING;
#ifdef QUASI_MODE
    	GPIO2_SDA_OUTPUT1();
#endif
	GPIO2_SDA_INPUT();
	
	//RX process
	for(i = 0; i < u16CMD_RX_BYTE; i++)
	{
		for(j = 0; j < 8; j++)
		{
			GPIO1_SCLK_OUTPUT0();
			HOST_PIN_DURATION();
			
			GPIO1_SCLK_OUTPUT1();
			HOST_PIN_DURATION();
			if ( n == 1)
			{
				CMD_RX_DATA[i]=CMD_RX_DATA[i]<<1;
				if ( GPIO2_SDA_INPUT_READ() != 0 )
				{
					CMD_RX_DATA[i] = CMD_RX_DATA[i] | 0x1;
				}
			}
		}		
	}			
	
	I2C_LIKE_STOP();
	u8HOST_FLAG &= ~FLAG_RX_DOING;
	
	if (u16CMD_RX_BYTE == 1)
	{
		if (CMD_RX_DATA[0] == RIGHT_RTN)
		{
			u8RX_ERROR_COUNT = 0;
			return 1;
		}
		if (CMD_RX_DATA[0] == ERROR_RTN)
		{
			u8RX_ERROR_COUNT ++;
			return 0;
		}	
		if (CMD_RX_DATA[0] == UNSUPPORTED_RTN)
		{
			u8RX_ERROR_COUNT = 0;
			return 0xFF;
		}
	}
	
	return 0;
}
//------------------------------------------------
UINT8 I2C_LIKE_TX_RX_ISP_PROCESS(UINT8 *ISP_BUFFER)
{	
	UINT16 i = 0;
	UINT8 j = 0;
	UINT8 n = 0;
	UINT8 m = 0;
	UINT8 ISP_CHECKSUM = 0;
	UINT8 ISP_CHECKSUM_CHECK = 0;
	UINT8 u8BitMap[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	
	if (CMD_TX_DATA[0] == CMD_ISP_WRITE_PAGE)
	{
		n = 1;
	}
	else if (CMD_TX_DATA[0] == CMD_ISP_WRITE_PARTIAL)
	{
		n = 3;
	}
	else if ((CMD_TX_DATA[0] == CMD_ISP_READ_PAGE)||(CMD_TX_DATA[0] == CMD_ISP_READ_PARTIAL))
	{
		n = 2;
	}
	
	CMD_RX_DATA_INIT();
	u8HOST_FLAG |= FLAG_TX_DOING;
	
	I2C_LIKE_START_SHORT();
	
	//TX process
	for(i = 0; i < u16CMD_TX_BYTE; i++)
	{
		if (n == 1)
		{
			if (i <= 3) 
			{
				m = i;
				ISP_CHECKSUM = ((ISP_CHECKSUM + CMD_TX_DATA[m]) & 0xFF);
			}
			else if ((i >= 4) && (i < (u16CMD_TX_BYTE - 1)))
			{
				m = 4;
				CMD_TX_DATA[m] = *ISP_BUFFER;
				ISP_CHECKSUM = ((ISP_CHECKSUM + CMD_TX_DATA[m]) & 0xFF);
				ISP_BUFFER++;
			}
			else if (i == (u16CMD_TX_BYTE - 1))
			{
				ISP_CHECKSUM = ISP_CHECKSUM ^ 0xFF;
				m = 4;
				CMD_TX_DATA[m] = ISP_CHECKSUM;
			}
		}
		else if (n == 3)
		{
			if (i <= 5) 
			{
				m = i;
				ISP_CHECKSUM = ((ISP_CHECKSUM + CMD_TX_DATA[m]) & 0xFF);
			}
			else if ((i >= 6) && (i < (u16CMD_TX_BYTE - 1)))
			{
				m = 6;
				CMD_TX_DATA[m] = *ISP_BUFFER;
				ISP_CHECKSUM = ((ISP_CHECKSUM + CMD_TX_DATA[m]) & 0xFF);
				ISP_BUFFER++;
			}
			else if (i == (u16CMD_TX_BYTE - 1))
			{
				ISP_CHECKSUM = ISP_CHECKSUM ^ 0xFF;
				m = 6;
				CMD_TX_DATA[m] = ISP_CHECKSUM;
			}
		}
		else if (n == 2)
		{ 
			m = i;
		}
		
		for(j = 0; j < 8; j++)
		{
			GPIO1_SCLK_OUTPUT0();
			if ((CMD_TX_DATA[m] & u8BitMap[j]) !=0 )
			{
				GPIO2_SDA_OUTPUT1();
			}
			else
			{
				GPIO2_SDA_OUTPUT0();
			}
			HOST_PIN_DURATION_SHORT();
			
			GPIO1_SCLK_OUTPUT1();
			HOST_PIN_DURATION_SHORT();
		}
	}
	
	u8HOST_FLAG &= ~FLAG_TX_DOING;
	u8HOST_FLAG |= FLAG_RX_DOING;
#ifdef QUASI_MODE
    	GPIO2_SDA_OUTPUT1();
#endif
	GPIO2_SDA_INPUT();
	
	//RX process
	for(i = 0; i < u16CMD_RX_BYTE; i++)
	{
		if ( n == 2) 
		{
			*ISP_BUFFER = 0;
		}
		for(j = 0; j < 8; j++)
		{
			GPIO1_SCLK_OUTPUT0();
			HOST_PIN_DURATION_SHORT();
			HOST_PIN_DURATION_SHORT();
			
			GPIO1_SCLK_OUTPUT1();
			HOST_PIN_DURATION_SHORT();
			HOST_PIN_DURATION_SHORT();
			
			if (( n == 1) || ( n == 3))
			{
				CMD_RX_DATA[i]=CMD_RX_DATA[i]<<1;
				if ( GPIO2_SDA_INPUT_READ() != 0 )
				{
					CMD_RX_DATA[i] = CMD_RX_DATA[i] | 0x1;
				}
			}
			else if ( n == 2) 
			{
				if (i < (u16CMD_RX_BYTE-1))
				{
					*ISP_BUFFER = *ISP_BUFFER<<1 ;
					if ( GPIO2_SDA_INPUT_READ() != 0 )
					{
						*ISP_BUFFER = *ISP_BUFFER | 0x1;
					}
				}
				else if ( i == (u16CMD_RX_BYTE-1))
				{
					ISP_CHECKSUM = ISP_CHECKSUM<<1 ;
					if ( GPIO2_SDA_INPUT_READ() != 0 )
					{
						ISP_CHECKSUM = ISP_CHECKSUM | 0x1;
					}
				}
				
			}
			
		}
		if ( n == 2) 
		{
			if (i < (u16CMD_RX_BYTE-1))
			{
				ISP_CHECKSUM_CHECK = *ISP_BUFFER + ISP_CHECKSUM_CHECK;
				ISP_BUFFER++; 
			}
		}
			
	}			
	
	if ((n == 1) || (n == 3))
	{
		I2C_LIKE_ISP_STOP_SHORT();
	}
	else
	{
		I2C_LIKE_STOP_SHORT();
	}
		
	u8HOST_FLAG &= ~FLAG_RX_DOING;
	
	if (u16CMD_RX_BYTE == 1)
	{
		if (CMD_RX_DATA[0] == RIGHT_RTN)
		{
			u8RX_ERROR_COUNT = 0;
			return 1;
		}
		if (CMD_RX_DATA[0] == ERROR_RTN)
		{
			u8RX_ERROR_COUNT ++;
			return 0;
		}	
		if (CMD_RX_DATA[0] == UNSUPPORTED_RTN)
		{
			u8RX_ERROR_COUNT = 0;
			return 0xFF;
		}
	}
	if (n == 2)
	{
		if ((ISP_CHECKSUM ^ ISP_CHECKSUM_CHECK) == 0xFF)
		{
			u8RX_ERROR_COUNT = 0;
			return 1;
		}
		else
		{
			u8RX_ERROR_COUNT ++;
			return 0;
		}
	}
	
	return 0;
}
//----------------------------------
UINT8 WAIT_ISP_PROCESS_DONE()
{
#ifdef QUASI_MODE
    	GPIO2_SDA_OUTPUT1();
#endif
	GPIO2_SDA_INPUT();
	
	HOST_CMD_INTERVAL();
	while ( GPIO2_SDA_INPUT_READ() == 0 )
	{
	}
		
	GPIO2_SDA_OUTPUT();		//SDA:input->output
	GPIO2_SDA_OUTPUT1();
	HOST_CMD_INTERVAL();
}
//----------------------------------
void N_SLAVE_RESET(void)
{
	if(u8RX_ERROR_COUNT >=5) 
	{
		u8RX_ERROR_COUNT = 0;
		N_RESET();	
	}
}
//===========================================================
// function command
UINT8 N_READ_ID(UINT32* PID)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_READ_ID;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_READ_ID_TX_BYTE;
	u16CMD_RX_BYTE = CMD_READ_ID_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		 *PID = (CMD_RX_DATA[0]<<24) | (CMD_RX_DATA[1]<<16) | (CMD_RX_DATA[2]<<8) | CMD_RX_DATA[3];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_READ_STATUS(UINT8* COMMAND_STATUS)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_READ_STATUS;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_READ_STATUS_TX_BYTE;
	u16CMD_RX_BYTE = CMD_READ_STATUS_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		*COMMAND_STATUS = CMD_RX_DATA[0];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_DO_LVD(void)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_DO_LVD;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_DO_LVD_TX_BYTE;
	u16CMD_RX_BYTE = CMD_DO_LVD_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_GET_LVD(UINT8* LVD_STATUS)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_GET_LVD;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_GET_LVD_TX_BYTE;
	u16CMD_RX_BYTE = CMD_GET_LVD_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		*LVD_STATUS = CMD_RX_DATA[0];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_CHECKSUM_RIGHT(UINT8* CHECKSUM_BIT)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_CHECKSUM_RIGHT;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_CHECKSUM_RIGHT_TX_BYTE;
	u16CMD_RX_BYTE = CMD_CHECKSUM_RIGHT_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		*CHECKSUM_BIT = CMD_RX_DATA[0];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_GET_CHECKSUM(UINT16* CHECKSUM_BYTES)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_GET_CHECKSUM;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_GET_CHECKSUM_TX_BYTE;
	u16CMD_RX_BYTE = CMD_GET_CHECKSUM_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		*CHECKSUM_BYTES = (CMD_RX_DATA[0]<<8) | CMD_RX_DATA[1];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_DO_CHECKSUM(void)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_DO_CHECKSUM;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_DO_CHECKSUM_TX_BYTE;
	u16CMD_RX_BYTE = CMD_DO_CHECKSUM_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_GET_FW_VERSION(UINT32* PFW_VERSION)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_GET_FW_VERSION;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_GET_FW_VERSION_TX_BYTE;
	u16CMD_RX_BYTE = CMD_GET_FW_VERSION_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		 *PFW_VERSION = (CMD_RX_DATA[0]<<16) | (CMD_RX_DATA[1]<<8) | CMD_RX_DATA[2];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_PLAY_REPEAT(UINT8 REPEAT_COUNT)
{
	UINT8 RTN = 0;
	
	CMD_TX_DATA[0] = CMD_REPEAT;
	CMD_TX_DATA[1] = REPEAT_COUNT;
	CMD_TX_DATA[2] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]) & 0xFF) ^ 0xFF;
	u16CMD_TX_BYTE = CMD_REPEAT_TX_BYTE;
	u16CMD_RX_BYTE = CMD_REPEAT_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_STOP_REPEAT()
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_STOP_REPEAT;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_STOP_REPEAT_TX_BYTE;
	u16CMD_RX_BYTE = CMD_STOP_REPEAT_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_PLAY(UINT16 PlayListIndex)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_PLAY;
	CMD_TX_DATA[1] = PlayListIndex >>8;
	CMD_TX_DATA[2] = PlayListIndex & 0xFF;
	CMD_TX_DATA[3] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]+ CMD_TX_DATA[2])& 0xFF)^ 0xFF;
	u16CMD_TX_BYTE = CMD_PLAY_TX_BYTE;
	u16CMD_RX_BYTE = CMD_PLAY_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_Delay500uS();
	HOST_Delay500uS();
	HOST_Delay500uS();
	return RTN;
}
//----------------------------------
UINT8 N_PLAY_CHANNEL(UINT8 ChannelMsk, UINT16* PlayIndexArry)
{
	UINT8 RTN = 0, u8Count = 0, u8i, u8Checksum = 0;
	CMD_TX_DATA[0] = CMD_PLAY_CHANNEL;
	CMD_TX_DATA[1] = ChannelMsk;
	u8Checksum += CMD_TX_DATA[0];
	u8Checksum += CMD_TX_DATA[1];
	if( ChannelMsk&BIT0 )
	{
		CMD_TX_DATA[2] = PlayIndexArry[0] >>8;
		u8Checksum += CMD_TX_DATA[2];
		CMD_TX_DATA[3] = PlayIndexArry[0] & 0xFF;	
		u8Checksum += CMD_TX_DATA[3];
		u8Count = 1;
	}
	if( ChannelMsk&BIT1 )
	{
		CMD_TX_DATA[2+u8Count*2] = PlayIndexArry[u8Count] >>8;
		u8Checksum += CMD_TX_DATA[2+u8Count*2];
		CMD_TX_DATA[2+u8Count*2+1] = PlayIndexArry[u8Count] & 0xFF;	
		u8Checksum += CMD_TX_DATA[2+u8Count*2+1];
		u8Count += 1;
	}	
	CMD_TX_DATA[2+u8Count*2] = u8Checksum ^ 0xff;

	u16CMD_TX_BYTE = CMD_PLAY_CHANNEL_TX_BYTE + u8Count*2;
	u16CMD_RX_BYTE = CMD_PLAY_CHANNEL_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_Delay500uS();
	HOST_Delay500uS();
	HOST_Delay500uS();
	
	return RTN;	
}
//----------------------------------
UINT8 N_PLAY_SLEEP(UINT16 PlayListIndex)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_PLAY_SLEEP;
	CMD_TX_DATA[1] = PlayListIndex >>8;
	CMD_TX_DATA[2] = PlayListIndex & 0xFF;
	CMD_TX_DATA[3] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]+ CMD_TX_DATA[2])& 0xFF)^ 0xFF;
	u16CMD_TX_BYTE = CMD_PLAY_SLEEP_TX_BYTE;
	u16CMD_RX_BYTE = CMD_PLAY_SLEEP_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_Delay500uS();
	HOST_Delay500uS();
	HOST_Delay500uS();
	return RTN;
}
//----------------------------------
UINT8 N_IO_CONFIG(UINT8 IO_FLAG)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_IO_CONFIG;
	CMD_TX_DATA[1] = IO_FLAG;
	CMD_TX_DATA[2] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]) & 0xFF) ^ 0xFF;
	u16CMD_TX_BYTE = CMD_IO_CONFIG_TX_BYTE;
	u16CMD_RX_BYTE = CMD_IO_CONFIG_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_IO_TYPE(UINT8* READ_IO_FLAG)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_IO_TYPE;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_IO_TYPE_TX_BYTE;
	u16CMD_RX_BYTE = CMD_IO_TYPE_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		*READ_IO_FLAG = CMD_RX_DATA[0];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_SET_OUT(UINT8 IO_VALUE)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_SET_OUT;
	CMD_TX_DATA[1] = IO_VALUE;
	CMD_TX_DATA[2] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]) & 0xFF) ^ 0xFF;
	u16CMD_TX_BYTE = CMD_SET_OUT_TX_BYTE;
	u16CMD_RX_BYTE = CMD_SET_OUT_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_GET_INOUT(UINT8* READ_IO_VALUE)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_GET_INOUT;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_GET_INOUT_TX_BYTE;
	u16CMD_RX_BYTE = CMD_GET_INOUT_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		*READ_IO_VALUE = CMD_RX_DATA[0];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_BZPIN_EN()
{
	UINT8 RTN = 0;
	u8HOST_FLAG |=FLAG_SPBZOUT_EN;
	CMD_TX_DATA[0] = CMD_BZPIN_EN;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_BZPIN_EN_TX_BYTE;
	u16CMD_RX_BYTE = CMD_BZPIN_EN_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_BZPIN_DIS()
{
	UINT8 RTN = 0;
	u8HOST_FLAG &= (0xFF^FLAG_SPBZOUT_EN);
	CMD_TX_DATA[0] = CMD_BZPIN_DIS;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_BZPIN_DIS_TX_BYTE;
	u16CMD_RX_BYTE = CMD_BZPIN_DIS_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_SET_VOL(UINT8 SP_VOL)
{
	UINT8 RTN = 0;
	
	CMD_TX_DATA[0] = CMD_SET_VOL_NSP;
	CMD_TX_DATA[1] = SP_VOL;
	CMD_TX_DATA[2] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]) & 0xFF) ^ 0xFF;
	u16CMD_TX_BYTE = CMD_SET_VOL_TX_BYTE;
	u16CMD_RX_BYTE = CMD_SET_VOL_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_GET_VOL(UINT8* SP_VOL)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_GET_VOL;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_GET_VOL_TX_BYTE;
	u16CMD_RX_BYTE = CMD_GET_VOL_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		*SP_VOL = CMD_RX_DATA[0];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
//After the host sends STOP cmd,
//it will send READ_STATUS cmd every interval (HOST_CMD_INTERVAL), 
//and the loop will execute until the READ_STATUS cmd gets the correct result, 
//so as to ensure that the NSP STOP operation has been completed.
UINT8 N_STOP()
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	CMD_TX_DATA[0] = CMD_STOP;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_STOP_TX_BYTE;
	u16CMD_RX_BYTE = CMD_STOP_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	while ((N_READ_STATUS(&u8NSP_STATUS1) != 1) || (u8NSP_STATUS1 != CMD_VALID))
	{
		HOST_Delay500uS();
		HOST_Delay500uS();
	}
	return RTN;
}
//----------------------------------
UINT8 N_STOP_CHANNEL(UINT8 ChannelMsk)
{
	UINT8 RTN = 0;

	CMD_TX_DATA[0] = CMD_STOP_CHANNEL;
	CMD_TX_DATA[1] = ChannelMsk;
	CMD_TX_DATA[2] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]) & 0xFF) ^ 0xFF;
	u16CMD_TX_BYTE = CMD_STOP_CHANNEL_TX_BYTE;
	u16CMD_RX_BYTE = CMD_STOP_CHANNEL_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	HOST_Delay500uS();
	HOST_Delay500uS();
	HOST_Delay500uS();
	
	return RTN;
}
//----------------------------------
//After the host sends RESET cmd,
//it will send READ_STATUS cmd every interval (HOST_CMD_INTERVAL), 
//and the loop will execute until the READ_STATUS cmd gets the correct result, 
//so as to ensure that the NSP stop operation has been completed.
UINT8 N_RESET()
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	CMD_TX_DATA[0] = CMD_RESET;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_RESET_TX_BYTE;
	u16CMD_RX_BYTE = CMD_RESET_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	while ((N_READ_STATUS(&u8NSP_STATUS1) != 1) || (u8NSP_STATUS1 != CMD_VALID))
	{
		HOST_Delay500uS();
		HOST_Delay500uS();
	}
	return RTN;
}
//----------------------------------
UINT8 N_PWR_DOWN()
{
	UINT8 RTN = 0;
	UINT8 n = 0;
	
	CMD_TX_DATA[0] = CMD_PWR_DOWN;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_PWR_DOWN_TX_BYTE;
	u16CMD_RX_BYTE = CMD_PWR_DOWN_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	for (n= 0; n <= 40; n++)
	{
		HOST_Delay500uS();
	}
	return RTN;
}
//----------------------------------
UINT8 N_ISP_CHECKSUM(UINT16* ISP_CHECKSUM_BYTES)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_ISP_CHECKSUM;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_ISP_CHECKSUM_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_CHECKSUM_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		*ISP_CHECKSUM_BYTES = (CMD_RX_DATA[0]<<8) | CMD_RX_DATA[1];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_ISP_WRITE_START(UINT32 PROC_ID,UINT32 CHIP_PDID)
{
	UINT8 RTN = 0;
	UINT8 i = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	CMD_TX_DATA[0] = CMD_ISP_WRITE_START;
	CMD_TX_DATA[1] = (PROC_ID & 0xff000000) >> 24;
	CMD_TX_DATA[2] = (PROC_ID & 0x00ff0000) >> 16;
	CMD_TX_DATA[3] = (CHIP_PDID & 0xff000000) >> 24;
	CMD_TX_DATA[4] = (CHIP_PDID & 0xff0000) >> 16;
	CMD_TX_DATA[5] = (CHIP_PDID & 0xff00) >> 8;
	CMD_TX_DATA[6] = (CHIP_PDID & 0xff) ;
	CMD_TX_DATA[7] = 0;
	for(i = 0; i <(CMD_ISP_WRITE_START_TX_BYTE-1); i++)
		{
			CMD_TX_DATA[7] = ((CMD_TX_DATA[7] + CMD_TX_DATA[i]) & 0xFF);
		}
	CMD_TX_DATA[7] = (CMD_TX_DATA[7] ^ 0xff);
	u16CMD_TX_BYTE = CMD_ISP_WRITE_START_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_WRITE_START_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_ISP_PROCESS_NO_BUFFER();
	if (RTN == 1)
	{
		WAIT_ISP_PROCESS_DONE();
	}
	return RTN;
}
//----------------------------------
UINT8 N_ISP_WRITE_END(void)
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	CMD_TX_DATA[0] = CMD_ISP_WRITE_END;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_ISP_WRITE_END_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_WRITE_END_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_ISP_PROCESS_NO_BUFFER();
	if (RTN == 1)
	{
		WAIT_ISP_PROCESS_DONE();
	}
	return RTN;
}
//----------------------------------
UINT8 N_ISP_WRITE_PAGE(UINT32 ISP_ADDR,PUINT8 ISP_BUFFER)
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	
	CMD_TX_DATA[0] = CMD_ISP_WRITE_PAGE;
	CMD_TX_DATA[1] = (ISP_ADDR & 0xFF0000)>>16;
	CMD_TX_DATA[2] = (ISP_ADDR & 0xFF00)>>8;
	CMD_TX_DATA[3] = (ISP_ADDR & 0xFF);
	u16CMD_TX_BYTE = CMD_ISP_WRITE_PAGE_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_WRITE_PAGE_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_ISP_PROCESS(ISP_BUFFER);
	if (RTN == 1)
	{
		WAIT_ISP_PROCESS_DONE();
	}
	return RTN;
}
//----------------------------------
UINT8 N_ISP_READ_PAGE(UINT32 ISP_ADDR,PUINT8 ISP_BUFFER)
{
	UINT8 RTN = 0;
	
	CMD_TX_DATA[0] = CMD_ISP_READ_PAGE;
	CMD_TX_DATA[1] = (ISP_ADDR & 0xFF0000)>>16;
	CMD_TX_DATA[2] = (ISP_ADDR & 0xFF00)>>8;
	CMD_TX_DATA[3] = (ISP_ADDR & 0xFF);
	CMD_TX_DATA[4] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]+ CMD_TX_DATA[2]+ CMD_TX_DATA[3])& 0xFF)^ 0xFF;
	u16CMD_TX_BYTE = CMD_ISP_READ_PAGE_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_READ_PAGE_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_ISP_PROCESS(ISP_BUFFER);
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_ISP_READ_PARTIAL(UINT32 ISP_ADDR,UINT16 ISPSize,PUINT8 ISP_BUFFER)
{
	UINT8 RTN = 0;
	UINT8 i = 0;
	
	CMD_TX_DATA[0] = CMD_ISP_READ_PARTIAL;
	CMD_TX_DATA[1] = (ISP_ADDR & 0xFF0000)>>16;
	CMD_TX_DATA[2] = (ISP_ADDR & 0xFF00)>>8;
	CMD_TX_DATA[3] = (ISP_ADDR & 0xFF);
	CMD_TX_DATA[4] = (ISPSize & 0xFF00)>>8;
	CMD_TX_DATA[5] = (ISPSize & 0xFF);
	CMD_TX_DATA[6] = 0;
	for(i = 0; i <(CMD_ISP_READ_PARTIAL_TX_BYTE-1); i++)
		{
			CMD_TX_DATA[6] = CMD_TX_DATA[6] + CMD_TX_DATA[i];
		}
	CMD_TX_DATA[6] = (CMD_TX_DATA[6] ^ 0xff);
	u16CMD_TX_BYTE = CMD_ISP_READ_PARTIAL_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_READ_PARTIAL_RX_BYTE + ISPSize;
	
	RTN = I2C_LIKE_TX_RX_ISP_PROCESS(ISP_BUFFER);
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_ISP_READ_RES_INDEX(UINT16 ResourceIndex)
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	CMD_TX_DATA[0] = CMD_ISP_READ_RES_INDEX;
	CMD_TX_DATA[1] = ResourceIndex >>8;
	CMD_TX_DATA[2] = ResourceIndex & 0xFF;
	CMD_TX_DATA[3] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]+ CMD_TX_DATA[2])& 0xFF)^ 0xFF;
	u16CMD_TX_BYTE = CMD_ISP_READ_RES_INDEX_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_READ_RES_INDEX_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_ISP_PROCESS_NO_BUFFER();
	if (RTN == 1)
	{
		WAIT_ISP_PROCESS_DONE();
	}
	return RTN;
}
//----------------------------------
UINT8 N_ISP_GET_RES_INFO(UINT32* SpaceSize,UINT32* ResourceStartAddr,UINT16* FisrtPageDataByte,UINT16* PageCount,UINT16* LastPageDataByte)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_ISP_GET_RES_INFO;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_ISP_GET_RES_INFO_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_GET_RES_INFO_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		 *SpaceSize             = (CMD_RX_DATA[0]<<16) | (CMD_RX_DATA[1]<<8) | CMD_RX_DATA[2];
		 *ResourceStartAddr     = (CMD_RX_DATA[3]<<16) | (CMD_RX_DATA[4]<<8) | CMD_RX_DATA[5];
		 *FisrtPageDataByte     = (CMD_RX_DATA[6]<<8) | CMD_RX_DATA[7];
		 *PageCount              = (CMD_RX_DATA[8]<<8) | CMD_RX_DATA[9];
		 *LastPageDataByte      = (CMD_RX_DATA[10]<<8) | CMD_RX_DATA[11];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_ISP_GET_USER_SPACE_INFO(UINT32* SpaceSize,UINT32* ResourceStartAddr,UINT16* FisrtPageDataByte,UINT16* PageCount,UINT16* LastPageDataByte)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_ISP_GET_USER_SPACE_INFO;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_ISP_GET_USER_SPACE_INFO_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_GET_USER_SPACE_INFO_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		 *SpaceSize             = (CMD_RX_DATA[0]<<16) | (CMD_RX_DATA[1]<<8) | CMD_RX_DATA[2];
		 *ResourceStartAddr     = (CMD_RX_DATA[3]<<16) | (CMD_RX_DATA[4]<<8) | CMD_RX_DATA[5];
		 *FisrtPageDataByte     = (CMD_RX_DATA[6]<<8) | CMD_RX_DATA[7];
		 *PageCount              = (CMD_RX_DATA[8]<<8) | CMD_RX_DATA[9];
		 *LastPageDataByte      = (CMD_RX_DATA[10]<<8) | CMD_RX_DATA[11];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_ISP_WRITE_PARTIAL_START(void)
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	CMD_TX_DATA[0] = CMD_ISP_WRITE_PARTIAL_START;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_ISP_WRITE_PARTIAL_START_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_WRITE_PARTIAL_START_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_ISP_PROCESS_NO_BUFFER();
	if (RTN == 1)
	{
		WAIT_ISP_PROCESS_DONE();
	}
	return RTN;
}
//----------------------------------
UINT8 N_ISP_WRITE_PARTIAL_BAK(UINT32 ISP_ADDR)
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;	
	
	CMD_TX_DATA[0] = CMD_ISP_WRITE_PARTIAL_BAK;
	CMD_TX_DATA[1] = (ISP_ADDR & 0xFF0000)>>16;
	CMD_TX_DATA[2] = (ISP_ADDR & 0xFF00)>>8;
	CMD_TX_DATA[3] = (ISP_ADDR & 0xFF);
	CMD_TX_DATA[4] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]+ CMD_TX_DATA[2]+ CMD_TX_DATA[3])& 0xFF)^ 0xFF;
	u16CMD_TX_BYTE = CMD_ISP_WRITE_PARTIAL_BAK_TX_BYTE;
	u16CMD_RX_BYTE = CMD_ISP_WRITE_PARTIAL_BAK_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_ISP_PROCESS_NO_BUFFER();
	if (RTN == 1)
	{
		WAIT_ISP_PROCESS_DONE();
	}
	return RTN;
}
//----------------------------------
UINT8 N_ISP_WRITE_PARTIAL(UINT32 ISP_ADDR,UINT16 ISPSize,PUINT8 ISP_BUFFER)
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	CMD_TX_DATA[0] = CMD_ISP_WRITE_PARTIAL;
	CMD_TX_DATA[1] = (ISP_ADDR & 0xFF0000)>>16;
	CMD_TX_DATA[2] = (ISP_ADDR & 0xFF00)>>8;
	CMD_TX_DATA[3] = (ISP_ADDR & 0xFF);
	CMD_TX_DATA[4] = (ISPSize & 0xFF00)>>8;
	CMD_TX_DATA[5] = (ISPSize & 0xFF);
	u16CMD_TX_BYTE = CMD_ISP_WRITE_PARTIAL_TX_BYTE + ISPSize ;
	u16CMD_RX_BYTE = CMD_ISP_WRITE_PARTIAL_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_ISP_PROCESS(ISP_BUFFER);
	if (RTN == 1)
	{
		WAIT_ISP_PROCESS_DONE();
	}
	return RTN;
}
//----------------------------------
UINT8 N_MULTI_PLAY(UINT8 PlayListNum,PUINT8 DATA_BUFFER)
{
	UINT8 RTN = 0;
	
	CMD_TX_DATA[0] = CMD_MULTI_PLAY;
	CMD_TX_DATA[1] = PlayListNum;
	u16CMD_TX_BYTE = CMD_MULTI_PLAY_TX_BYTE+PlayListNum-1;
	u16CMD_RX_BYTE = CMD_MULTI_PLAY_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_BUFFER_PROCESS(DATA_BUFFER);
	HOST_Delay500uS();
	HOST_Delay500uS();
	HOST_Delay500uS();
	return RTN;
}
//----------------------------------
UINT8 N_MULTI_PLAY_2B(UINT8 PlayListNum,PUINT16 DATA_BUFFER)
{
	UINT8 RTN = 0;
	UINT8 i = 0;
	UINT8 MultiPlayBuffer2B[64]={0};
	PUINT8 DATA2_BUFFER = &MultiPlayBuffer2B[0];
	
	CMD_TX_DATA[0] = CMD_MULTI_PLAY_2B;
	CMD_TX_DATA[1] = PlayListNum;
	u16CMD_TX_BYTE = CMD_MULTI_PLAY_2B_TX_BYTE + PlayListNum * 2;
	u16CMD_RX_BYTE = CMD_MULTI_PLAY_2B_RX_BYTE;
	
	for (i = 0; i < PlayListNum; i++)
	{
		*DATA2_BUFFER = (*DATA_BUFFER  & 0xFF00)>>8;
		DATA2_BUFFER++;
		*DATA2_BUFFER = (*DATA_BUFFER  & 0xFF);
		DATA2_BUFFER++;
		DATA_BUFFER++;
	}
	
	RTN = I2C_LIKE_TX_RX_BUFFER_PROCESS(&MultiPlayBuffer2B[0]);
	HOST_Delay500uS();
	HOST_Delay500uS();
	HOST_Delay500uS();
	return RTN;
}
//----------------------------------
UINT8 N_AUTO_SLEEP_EN()
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_AUTO_SLEEP_EN;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_AUTO_SLEEP_EN_TX_BYTE;
	u16CMD_RX_BYTE = CMD_AUTO_SLEEP_EN_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_AUTO_SLEEP_DIS()
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_AUTO_SLEEP_DIS;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_AUTO_SLEEP_DIS_TX_BYTE;
	u16CMD_RX_BYTE = CMD_AUTO_SLEEP_DIS_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL_SHORT(); 
	return RTN;
}
//----------------------------------
//After the host sends N_PAUSE cmd,
//it will send READ_STATUS cmd every 500us, 
//and the loop will execute until the READ_STATUS cmd gets the correct result, 
//so as to ensure that the NSP STOP operation has been completed.
UINT8 N_PAUSE()
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	CMD_TX_DATA[0] = CMD_PAUSE;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_PAUSE_TX_BYTE;
	u16CMD_RX_BYTE = CMD_PAUSE_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	while ((N_READ_STATUS(&u8NSP_STATUS1) != 1) || (u8NSP_STATUS1 != CMD_VALID))
	{
		HOST_Delay500uS();
		HOST_Delay500uS();
	}
	return RTN;
}

UINT8 N_PAUSE_CHANNEL(UINT8 ChannelMsk)
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	CMD_TX_DATA[0] = CMD_PAUSE_CHANNEL;
	CMD_TX_DATA[1] = ChannelMsk;
	CMD_TX_DATA[2] = ( CMD_TX_DATA[0] + CMD_TX_DATA[1] ) ^ 0xFF;
	u16CMD_TX_BYTE = CMD_PAUSE_CHANNEL_TX_BYTE;
	u16CMD_RX_BYTE = CMD_PAUSE_CHANNEL_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	while ((N_READ_STATUS(&u8NSP_STATUS1) != 1))
	{
		HOST_Delay500uS();
		HOST_Delay500uS();
	}
	return RTN;	
}
//----------------------------------
//After the host sends N_RESUME cmd,
//it will send READ_STATUS cmd every 500us, 
//and the loop will execute until the READ_STATUS cmd gets the correct result, 
//so as to ensure that the NSP STOP operation has been completed.
UINT8 N_RESUME()
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	CMD_TX_DATA[0] = CMD_RESUME;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_RESUME_TX_BYTE;
	u16CMD_RX_BYTE = CMD_RESUME_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	while (N_READ_STATUS(&u8NSP_STATUS1) != 1)
	{
		HOST_Delay500uS();
		HOST_Delay500uS();
	}
	return RTN;
}

UINT8 N_RESUME_CHANNEL(UINT8 ChannelMsk)
{
	UINT8 RTN = 0;
	UINT8 u8NSP_STATUS1 = 0;
	
	CMD_TX_DATA[0] = CMD_RESUME_CHANNEL;
	CMD_TX_DATA[1] = ChannelMsk;
	CMD_TX_DATA[2] = ( CMD_TX_DATA[0] + CMD_TX_DATA[1] ) ^ 0xFF;
	u16CMD_TX_BYTE = CMD_RESUME_CHANNEL_TX_BYTE;
	u16CMD_RX_BYTE = CMD_RESUME_CHANNEL_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	while (N_READ_STATUS(&u8NSP_STATUS1) != 1)
	{
		HOST_Delay500uS();
		HOST_Delay500uS();
	}
	return RTN;	
}
//----------------------------------
UINT8 N_CHECK_INDEX_RCOUNT(UINT16 PlayListIndex)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_CHECK_INDEX_RCOUNT;
	CMD_TX_DATA[1] = PlayListIndex >>8;
	CMD_TX_DATA[2] = PlayListIndex & 0xFF;
	CMD_TX_DATA[3] = ((CMD_TX_DATA[0] + CMD_TX_DATA[1]+ CMD_TX_DATA[2])& 0xFF)^ 0xFF;
	u16CMD_TX_BYTE = CMD_CHECK_INDEX_RCOUNT_TX_BYTE;
	u16CMD_RX_BYTE = CMD_CHECK_INDEX_RCOUNT_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	HOST_CMD_INTERVAL();
	return RTN;
}
//----------------------------------
UINT8 N_GET_INDEX_RCOUNT(UINT8* RESOURCE_COUNT)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_GET_INDEX_RCOUNT;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_GET_INDEX_RCOUNT_TX_BYTE;
	u16CMD_RX_BYTE = CMD_GET_INDEX_RCOUNT_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		*RESOURCE_COUNT = CMD_RX_DATA[0];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
UINT8 N_GET_MAX_INDEX(UINT16* MAX_INDEX)
{
	UINT8 RTN = 0;
	CMD_TX_DATA[0] = CMD_GET_MAX_INDEX;
	CMD_TX_DATA[1] = CMD_TX_DATA[0] ^ 0xFF;
	u16CMD_TX_BYTE = CMD_GET_MAX_INDEX_TX_BYTE;
	u16CMD_RX_BYTE = CMD_GET_MAX_INDEX_RX_BYTE;
	
	RTN = I2C_LIKE_TX_RX_PROCESS();
	if (RTN == 1)
	{
		*MAX_INDEX = (CMD_RX_DATA[0]<<8) | CMD_RX_DATA[1];
	}
	HOST_CMD_INTERVAL_SHORT();
	return RTN;
}
//----------------------------------
void N_WAKUP(void)
{
	GPIO1_SCLK_OUTPUT0();
	HOST_CMD_INTERVAL();
	HOST_CMD_INTERVAL();
	GPIO1_SCLK_OUTPUT1();
	HOST_Delay500uS();
	HOST_Delay500uS();
	HOST_Delay500uS();
}



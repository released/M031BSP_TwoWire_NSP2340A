/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#include "misc_config.h"

#include "nsp_driver.h"
#include "nsp_PlaySample.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_REVERSE1              			    (flag_PROJ_CTL.bit1)
#define FLAG_PROJ_REVERSE2                 				(flag_PROJ_CTL.bit2)
#define FLAG_PROJ_REVERSE3                              (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)


#define FLAG_NSP_SleepWakeUpSample                      (flag_PROJ_CTL.bit8)        //Q
#define FLAG_NSP_Example1                               (flag_PROJ_CTL.bit9)        //W
#define FLAG_NSP_Example2                               (flag_PROJ_CTL.bit10)       //E
#define FLAG_NSP_idx1                                   (flag_PROJ_CTL.bit11)       //1
#define FLAG_NSP_idx2                                   (flag_PROJ_CTL.bit12)       //2
#define FLAG_NSP_idx3                                   (flag_PROJ_CTL.bit13)
#define FLAG_NSP_idx4                                   (flag_PROJ_CTL.bit14)
#define FLAG_NSP_idx5                                   (flag_PROJ_CTL.bit15)
#define FLAG_NSP_idx6                                   (flag_PROJ_CTL.bit16)
#define FLAG_NSP_idx7                                   (flag_PROJ_CTL.bit17)
#define FLAG_NSP_idx8                                   (flag_PROJ_CTL.bit18)
#define FLAG_NSP_idx9                                   (flag_PROJ_CTL.bit19)       //9
#define FLAG_NSP_idx10                                  (flag_PROJ_CTL.bit20)       //0

#define FLAG_NSP_STOP                                   (flag_PROJ_CTL.bit21)       //S
#define FLAG_NSP_VOL_INC                                (flag_PROJ_CTL.bit22)       //A,a
#define FLAG_NSP_VOL_DEC                                (flag_PROJ_CTL.bit23)       //D,d

/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned int counter_systick = 0;
volatile uint32_t counter_tick = 0;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

unsigned int get_systick(void)
{
	return (counter_systick);
}

void set_systick(unsigned int t)
{
	counter_systick = t;
}

void systick_counter(void)
{
	counter_systick++;
}

void SysTick_Handler(void)
{

    systick_counter();

    if (get_systick() >= 0xFFFFFFFF)
    {
        set_systick(0);      
    }

    // if ((get_systick() % 1000) == 0)
    // {
       
    // }

    #if defined (ENABLE_TICK_EVENT)
    TickCheckTickEvent();
    #endif    
}

void SysTick_delay(unsigned int delay)
{  
    
    unsigned int tickstart = get_systick(); 
    unsigned int wait = delay; 

    while((get_systick() - tickstart) < wait) 
    { 
    } 

}

void SysTick_enable(unsigned int ticks_per_second)
{
    set_systick(0);
    if (SysTick_Config(SystemCoreClock / ticks_per_second))
    {
        /* Setup SysTick Timer for 1 second interrupts  */
        dbg_printf("Set system tick error!!\n");
        while (1);
    }

    #if defined (ENABLE_TICK_EVENT)
    TickInitTickEvent();
    #endif
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000)
    {
        set_tick(0);
    }
}

void delay_ms(uint32_t ms)
{
	TIMER_Delay(TIMER0, 1000*ms);
}


void nsp_vol_decrease(void)
{
    uint8_t volume = 0;
    N_GET_VOL(&volume);
    printf("volume 0x%2X\r\n"  , volume);
    if (volume-- <= 1)
    {
        volume = 1;
    }
    N_SET_VOL(volume);
}

void nsp_vol_increase(void)
{
    uint8_t volume = 0;
    N_GET_VOL(&volume);
    printf("volume 0x%2X\r\n"  , volume);
    if (volume++ >= 0x80)
    {
        volume = 0x80;
    }
    N_SET_VOL(volume);
}

void NSP_idx_set6_10(uint8_t idx) // index 6 ~ 10
{
    N_WAKUP();
    N_AUTO_SLEEP_DIS();		

	N_SET_VOL(0x40);	// max : 0x80 (128)
    N_PLAY(idx);  

    #if 1	// force to stop playing , if press 'S'
    do
    {
        HOST_Delay500uS();
        
        if(FLAG_NSP_STOP)
        {
            printf("trigger stop playing\r\n");
            FLAG_NSP_STOP = 0;	
            N_STOP(); 
            printf("stop playing\r\n");
            break;
        }

        if (FLAG_NSP_VOL_INC)
        {
            FLAG_NSP_VOL_INC = 0;
            nsp_vol_increase();
        }
        if (FLAG_NSP_VOL_DEC)
        {
            FLAG_NSP_VOL_DEC = 0;
            nsp_vol_decrease();
        }

    }while(!TwoWire_AskStatus());
    #else
    TwoWire_WaitPlayEND();
    #endif
        
    printf("TwoWire_WaitPlayEND (%d)\r\n",idx);

}

void NSP_idx_set2_5(uint8_t idx) // index 2 ~ 5
{
    N_WAKUP();
    N_AUTO_SLEEP_DIS();		

//		N_SET_VOL(0x20);	// max : 0x80 (128)
    N_PLAY(idx);  
    TwoWire_WaitPlayEND();
    printf("TwoWire_WaitPlayEND (%d)\r\n",idx);
}

void NSP_idx1(void)
{
    N_WAKUP();
    N_AUTO_SLEEP_DIS();		

    N_SET_VOL(0x40);	// max : 0x80 (128)
    N_PLAY(1);  
    TwoWire_WaitPlayEND();
    printf("TwoWire_WaitPlayEND (1) \r\n");    
}

void NSP_example2(void)
{    
    N_WAKUP();
    N_AUTO_SLEEP_DIS();		

	N_SET_VOL(0x10);	// max : 0x80 (128)
    N_PLAY(6);  
    TwoWire_WaitPlayEND();
    printf("TwoWire_WaitPlayEND (E) \r\n");

    // entry power down , volume data will lost
    N_AUTO_SLEEP_EN();		
    N_PWR_DOWN();
}


void NSP_example1(void)
{
    N_WAKUP();
    N_AUTO_SLEEP_DIS();		

    N_SET_VOL(0x40);	// max : 0x80 (128)
    N_PLAY(2);  
    TwoWire_WaitPlayEND();
    printf("TwoWire_WaitPlayEND (W) \r\n");

    // entry power down , volume data will lost
    N_AUTO_SLEEP_EN();		
    N_PWR_DOWN();
}

void NSP_process(void)
{
    if (FLAG_NSP_SleepWakeUpSample)
    {
        FLAG_NSP_SleepWakeUpSample = 0;
        TwoWire_SleepWakeUpSample();
    }
    if (FLAG_NSP_Example1)
    {
        FLAG_NSP_Example1 = 0;
        NSP_example1();
    }
    if (FLAG_NSP_Example2)
    {
        FLAG_NSP_Example2 = 0;
        NSP_example2();
    }
    if (FLAG_NSP_idx1)
    {
        FLAG_NSP_idx1 = 0;
        NSP_idx1();
    }
    if (FLAG_NSP_idx2)
    {
        FLAG_NSP_idx2 = 0;
        NSP_idx_set2_5(2);
    }
    if (FLAG_NSP_idx3)
    {
        FLAG_NSP_idx3 = 0;
        NSP_idx_set2_5(3);
    }
    if (FLAG_NSP_idx4)
    {
        FLAG_NSP_idx4 = 0;
        NSP_idx_set2_5(4);
    }
    if (FLAG_NSP_idx5)
    {
        FLAG_NSP_idx5 = 0;
        NSP_idx_set2_5(5);
    }
    if (FLAG_NSP_idx6)
    {
        FLAG_NSP_idx6 = 0;
        NSP_idx_set6_10(6);
    }
    if (FLAG_NSP_idx7)
    {
        FLAG_NSP_idx7 = 0;
        NSP_idx_set6_10(7);
    }
    if (FLAG_NSP_idx8)
    {
        FLAG_NSP_idx8 = 0;
        NSP_idx_set6_10(8);
    }
    if (FLAG_NSP_idx9)
    {
        FLAG_NSP_idx9 = 0;
        NSP_idx_set6_10(9);
    }
    if (FLAG_NSP_idx10)
    {
        FLAG_NSP_idx10 = 0;
        NSP_idx_set6_10(10);
    }

    if (FLAG_NSP_VOL_INC)
    {
        FLAG_NSP_VOL_INC = 0;
        nsp_vol_increase();
    }
    if (FLAG_NSP_VOL_DEC)
    {
        FLAG_NSP_VOL_DEC = 0;
        nsp_vol_decrease();
    }

}

//
// check_reset_source
//
uint8_t check_reset_source(void)
{
    uint32_t src = SYS_GetResetSrc();

    SYS->RSTSTS |= 0x1FF;
    printf("Reset Source <0x%08X>\r\n", src);

    #if 1   //DEBUG , list reset source
    if (src & BIT0)
    {
        printf("0)POR Reset Flag\r\n");       
    }
    if (src & BIT1)
    {
        printf("1)NRESET Pin Reset Flag\r\n");       
    }
    if (src & BIT2)
    {
        printf("2)WDT Reset Flag\r\n");       
    }
    if (src & BIT3)
    {
        printf("3)LVR Reset Flag\r\n");       
    }
    if (src & BIT4)
    {
        printf("4)BOD Reset Flag\r\n");       
    }
    if (src & BIT5)
    {
        printf("5)System Reset Flag \r\n");       
    }
    if (src & BIT6)
    {
        printf("6)Reserved.\r\n");       
    }
    if (src & BIT7)
    {
        printf("7)CPU Reset Flag\r\n");       
    }
    if (src & BIT8)
    {
        printf("8)CPU Lockup Reset Flag\r\n");       
    }
    #endif

    
    if (src & SYS_RSTSTS_PORF_Msk) {
        SYS_ClearResetSrc(SYS_RSTSTS_PORF_Msk);
        
        printf("power on from POR\r\n");
        return FALSE;
    } 
    else if (src & SYS_RSTSTS_CPURF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_CPURF_Msk);

        printf("power on from CPU reset\r\n");
        return FALSE;         
    }    
    else if (src & SYS_RSTSTS_PINRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_PINRF_Msk);
        
        printf("power on from nRESET pin\r\n");
        return FALSE;
    }
    
    printf("power on from unhandle reset source\r\n");
    return FALSE;
}

void TMR1_IRQHandler(void)
{
	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
		tick_counter();

		if ((get_tick() % 1000) == 0)
		{
			FLAG_PROJ_TIMER_PERIOD_1000MS = 1;
		}

		if ((get_tick() % 50) == 0)
		{

		}	
    }
}

void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    TIMER_Start(TIMER1);
}

void loop(void)
{
	// static uint32_t LOG1 = 0;
	// static uint32_t LOG2 = 0;

    if ((get_systick() % 1000) == 0)
    {
        // dbg_printf("%s(systick) : %4d\r\n",__FUNCTION__,LOG2++);    
    }

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)//(is_flag_set(flag_timer_period_1000ms))
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0;//set_flag(flag_timer_period_1000ms ,DISABLE);

        // dbg_printf("%s(timer) : %4d\r\n",__FUNCTION__,LOG1++);
        PB14 ^= 1;        
    }

    NSP_process();
    
}

void UARTx_Process(void)
{	
	uint8_t res = 0;
	res = UART_READ(UART0);

	if (res > 0x7F)
	{
		dbg_printf("invalid command\r\n");
	}
	else
	{
		dbg_printf("press : %c\r\n" , res);
		switch(res)
		{
			case '1':
                FLAG_NSP_idx1 = 1;
				break;
			case '2':
                FLAG_NSP_idx2 = 1;
				break;
			case '3':
                FLAG_NSP_idx3 = 1;
				break;
			case '4':
                FLAG_NSP_idx4 = 1;
				break;
			case '5':
                FLAG_NSP_idx5 = 1;
				break;
			case '6':
                FLAG_NSP_idx6 = 1;
				break;
			case '7':
                FLAG_NSP_idx7 = 1;
				break;
			case '8':
                FLAG_NSP_idx8 = 1;
				break;
			case '9':
                FLAG_NSP_idx9 = 1;
				break;
			case '0':
                FLAG_NSP_idx10 = 1;
				break;

			case 'Q':
			case 'q':				
				FLAG_NSP_SleepWakeUpSample = 1;
				break;

			case 'W':
			case 'w':				
				FLAG_NSP_Example1 = 1;
				break;

			case 'E':
			case 'e':				
				FLAG_NSP_Example2 = 1;
				break;

			case 'S':
			case 's':				
				FLAG_NSP_STOP = 1;			
				break;

			case 'A':
			case 'a':
                FLAG_NSP_VOL_INC = 1;
				break;
			case 'D':
			case 'd':
                FLAG_NSP_VOL_DEC = 1;
				break;	

			case 'X':
			case 'x':
			case 'Z':
			case 'z':
                SYS_UnlockReg();
				// NVIC_SystemReset();	// Reset I/O and peripherals , only check BS(FMC_ISPCTL[1])
                // SYS_ResetCPU();     // Not reset I/O and peripherals
                SYS_ResetChip();    // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)	
				break;
		}
	}
}

void UART02_IRQHandler(void)
{
    if(UART_GET_INT_FLAG(UART0, UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk))     /* UART receive data available flag */
    {
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {
			UARTx_Process();
        }
    }

    if(UART0->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART_ClearIntFlag(UART0, (UART_INTSTS_RLSINT_Msk| UART_INTSTS_BUFERRINT_Msk));
    }	
}

void UART0_Init(void)
{
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
    UART_EnableInt(UART0, UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk);
    NVIC_EnableIRQ(UART02_IRQn);
	
	#if (_debug_log_UART_ == 1)	//debug
	dbg_printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	dbg_printf("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());
	dbg_printf("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	dbg_printf("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	dbg_printf("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	dbg_printf("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	
	#endif	

    #if 0
    dbg_printf("FLAG_PROJ_TIMER_PERIOD_1000MS : 0x%2X\r\n",FLAG_PROJ_TIMER_PERIOD_1000MS);
    dbg_printf("FLAG_PROJ_REVERSE1 : 0x%2X\r\n",FLAG_PROJ_REVERSE1);
    dbg_printf("FLAG_PROJ_REVERSE2 : 0x%2X\r\n",FLAG_PROJ_REVERSE2);
    dbg_printf("FLAG_PROJ_REVERSE3 : 0x%2X\r\n",FLAG_PROJ_REVERSE3);
    dbg_printf("FLAG_PROJ_REVERSE4 : 0x%2X\r\n",FLAG_PROJ_REVERSE4);
    dbg_printf("FLAG_PROJ_REVERSE5 : 0x%2X\r\n",FLAG_PROJ_REVERSE5);
    dbg_printf("FLAG_PROJ_REVERSE6 : 0x%2X\r\n",FLAG_PROJ_REVERSE6);
    dbg_printf("FLAG_PROJ_REVERSE7 : 0x%2X\r\n",FLAG_PROJ_REVERSE7);
    #endif

}

void GPIO_Init (void)
{
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB14MFP_Msk)) | (SYS_GPB_MFPH_PB14MFP_GPIO);
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB15MFP_Msk)) | (SYS_GPB_MFPH_PB15MFP_GPIO);

    GPIO_SetMode(PB, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT15, GPIO_MODE_OUTPUT);	

}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);
    
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);	

//    CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);	

    /* Select HCLK clock source as HIRC and HCLK source divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    CLK_EnableModuleClock(TMR0_MODULE);
  	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);

    CLK_EnableModuleClock(TMR1_MODULE);
  	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);

    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

   /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

/*
 * This is a template project for M031 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

int main()
{
    UINT32	nsp_ID = 0;

	SYS_Init();

	GPIO_Init();
	UART0_Init();
	TIMER1_Init();
    check_reset_source();

    SysTick_enable(1000);
    #if defined (ENABLE_TICK_EVENT)
    TickSetTickEvent(1000, TickCallback_processA);  // 1000 ms
    TickSetTickEvent(5000, TickCallback_processB);  // 5000 ms
    #endif

    HOST_BUS_Init();	// BP01 : SDA  , BP00 : SCL
    
    N_RESET();
	N_WAKUP();
	N_AUTO_SLEEP_DIS();		

	N_SET_VOL(0x40);

    N_READ_ID(&nsp_ID); // need to set ID in playlist tool , default : 0x00
    printf("nsp_ID = 0x%2X\r\n" , nsp_ID);

    /* Got no where to go, just loop forever */
    while(1)
    {
        loop();

    }
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/

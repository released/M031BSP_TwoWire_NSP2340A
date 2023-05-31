#ifndef _NSP_PLAY_SAMPLE_H_
#define _NSP_PLAY_SAMPLE_H_

/* exact-width unsigned integer types */
typedef unsigned          char UINT8;
typedef unsigned short     int UINT16;
typedef unsigned           int UINT32;
typedef unsigned char *		PUINT8;
typedef unsigned short int  *	PUINT16;

/*----- Waiting for playback to finish -----*/
void TwoWire_WaitPlayEND(void);
/*----- Waiting for playback to finish -----*/
UINT8 TwoWire_WaitExecutionEND(void);
/*----- Ask for status -----*/
UINT8 TwoWire_AskStatus(void);

/*----- Index Play Sample -----*/
void TwoWire_IndexPlaySample(void);
/*----- MultiPlay Sample -----*/
void TwoWire_MultiPlaySample(void);
/*----- Sleep Wake Up Sample -----*/
void TwoWire_SleepWakeUpSample(void);
/*----- Low Power Detection -----*/
void TwoWire_LowPowerDetectionSample(void);
/*----- NSP IO Control Sample -----*/
void TwoWire_NSP_IO_CtrlSample(void);
/*----- CheckSum Sample -----*/
void TwoWire_CheckSumSample(void);
/*----- Busy Pin Set Sample -----*/
void TwoWire_BusyPinSetSample(void);
/*----- Volume Control Sample -----*/
void TwoWire_VolumeCtrlSample(void);
/*----- Read ID & FW Version Sample -----*/
void TwoWire_ReadIdAndFwVerSample(void);
/*----- Mixed Sample 1 -----*/
void TwoWire_MixedSample1(void);
/*----- Mixed Sample 2 -----*/
void TwoWire_MixedSample2(void);
/*----- Mixed Sample 3 -----*/
void TwoWire_MixedSample3(void);
/*----- NSP ISP Update all resource Sample -----*/
void TwoWire_ISPUpdateAllResourceSample(void);
/*----- NSP ISP Update one resource Sample -----*/
void TwoWire_ISPUpdateOneResourceSample(void);
/*----- Continue Play Sample -----*/
void TwoWire_ContinuePlaySample(void);

/*----- NSP ISP Update user data Sample -----*/
UINT8 TwoWire_UserDataWrite(UINT32 UserWrite_ADDR,PUINT8 ISP_BUFFER,UINT16 WriteSize);
/*----- NSP ISP read user data Sample -----*/
UINT8 TwoWire_UserDataRead(UINT32 UserRead_ADDR,PUINT8 ISP_BUFFER,UINT16 ReadSize);
/*----- NSP ISP Update user space Sample -----*/
void TwoWire_ISPUserSpaceWriteAndRead(void);

#endif





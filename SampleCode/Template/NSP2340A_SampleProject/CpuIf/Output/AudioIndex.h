#ifndef _AUDIOINDEX_H_
#define _AUDIOINDEX_H_

typedef enum Audio_ID_Table
{
	AudID_NONE=0xFFFF,
	_1=0x0000,	//NUO
	_2=0x0001,	//NUO
	_3=0x0002,	//NUO
	_4=0x0003,	//NUO
	_5=0x0004,	//NUO
	RESC3E6521B1647BAA9=0x0005,	//NUO
	RES28401FDD29072DD8=0x0006,	//NUO
	ALONE_10SEC=0x0007,	//NUO
	SAD_10SEC=0x0008,	//NUO
} eAudTableID;
#define INT_AUD_ID_TAB_COUNT (9)
#define INT_AUD_TAB() \
	const eAudTableID g_IntAudTab [] = \
	{ \
	    _1 \
	   ,_2 \
	   ,_3 \
	   ,_4 \
	   ,_5 \
	   ,RESC3E6521B1647BAA9 \
	   ,RES28401FDD29072DD8 \
	   ,ALONE_10SEC \
	   ,SAD_10SEC \
	};
#define EXT_AUD_ID_TAB_COUNT (0)
#define EXT_AUD_TAB() \
	const eAudTableID g_ExtAudTab [] = \
	{ \
		AudID_NONE \
	};
#define INT_EQU_ID_TAB_COUNT (0)
#define INT_EQU_TAB() \
	const eEquTableID g_IntEquTab [] = \
	{ \
		EquID_NONE \
	};
#define EXT_EQU_ID_TAB_COUNT (0)
#define EXT_EQU_TAB() \
	const eEquTableID g_ExtEquTab [] = \
	{ \
		EquID_NONE \
	};

#endif


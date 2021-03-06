
#define	DEFAULT_HOLD_TIME_MS		(1000)
#define	DEFAULT_ZIP_LENGTH_MB		(1)
#define	DEFAULT_ARM_REGULATOR_ID	(1)
#define	DEFAULT_THREAD_MAX			(16)

/* PMIC */
#define	VOLTAGE_STEP_UV				(12500)	/* 12.5 mV */

/*
 *	=============================================================================
 * 	|	ASV Group	|	ASV0	|	ASV1	|	ASV2	|	ASV3	|	ASV4	|
 *	-----------------------------------------------------------------------------
 * 	|	ARM_IDS		|	<= 10mA	|	<= 15mA	|	<= 20mA	|	<= 50mA	|	> 50mA	|
 *	-----------------------------------------------------------------------------
 * 	|	ARM_RO		|	<= 110	|	<= 130	|	<= 140	|	<= 180 	|	180 >	|
 *	=============================================================================
 * 	|  0: 1400 MHZ	|	1,350 mV|	1,300 mV|	1,250 mV|	1,200 mV|	1,200 mV|
 *	-----------------------------------------------------------------------------
 * 	|  1: 1300 MHZ	|	1,300 mV|	1,250 mV|	1,200 mV|	1,150 mV|	1,150 mV|
 *	-----------------------------------------------------------------------------
 * 	|  2: 1200 MHZ	|	1,250 mV|	1,200 mV|	1,150 mV|	1,100 mV|	1,100 mV|
 *	-----------------------------------------------------------------------------
 * 	|  3: 1100 MHZ	|	1,200 mV|	1,150 mV|	1,100 mV|	1,050 mV|	1,050 mV|
 *	-----------------------------------------------------------------------------
 * 	|  4: 1000 MHZ	|	1,175 mV|	1,125 mV|	1,075 mV|	1,025 mV|	1,050 mV|
 *	-----------------------------------------------------------------------------
 * 	|  5: 900 MHZ	|	1,150 mV|	1,100 mV|	1,050 mV|	1,000 mV|	1,050 mV|
 *	-----------------------------------------------------------------------------
 * 	|  6: 800 MHZ	|	1,125 mV|	1,075 mV|	1,025 mV|	1,000 mV|	1,050 mV|
 *	-----------------------------------------------------------------------------
 * 	|  7: 700 MHZ	|	1,100 mV|	1,050 mV|	1,000 mV|	1,000 mV|	1,050 mV|
 *	-----------------------------------------------------------------------------
 * 	|  8: 6500 MHZ	|	1,075 mV|	1,025 mV|	1,000 mV|	1,000 mV|	1,050 mV|
 *	-----------------------------------------------------------------------------
 * 	|  9: 500 MHZ	|	1,075 mV|	1,025 mV|	1,000 mV|	1,000 mV|	1,050 mV|
 *	-----------------------------------------------------------------------------
 * 	| 10: 400 MHZ	|	1,075 mV|	1,025 mV|	1,000 mV|	1,000 mV|	1,050 mV|
 *	=============================================================================
 */
#define	FREQ_ARRAY_SIZE		(11)
#define	UV(v)				(v*1000)

struct asv_tb_info {
	int ids;
	int ro;
	long Mhz[FREQ_ARRAY_SIZE];
	long uV [FREQ_ARRAY_SIZE];
};

#define	ASB_FREQ_MHZ {	\
	[ 0] = 1400,	\
	[ 1] = 1300,	\
	[ 2] = 1200,	\
	[ 3] = 1100,	\
	[ 4] = 1000,	\
	[ 5] =  900,	\
	[ 6] =  800,	\
	[ 7] =  700,	\
	[ 8] =  600,	\
	[ 9] =  500,	\
	[10] =  400,	\
	}

struct asv_tb_info asv_tables[] = {
	[0] = {	.ids = 10, .ro = 110,
			.Mhz = ASB_FREQ_MHZ,
			.uV  = { UV(1350), UV(1300), UV(1250), UV(1200), UV(1175), UV(1150),
					 UV(1125), UV(1100), UV(1075), UV(1075), UV(1075), },
	},
	[1] = {	.ids = 15, .ro = 130,
			.Mhz = ASB_FREQ_MHZ,
			.uV  = { UV(1300), UV(1250), UV(1200), UV(1150), UV(1125), UV(1100),
					 UV(1075), UV(1050), UV(1025), UV(1025), UV(1025), },
	},
	[2] = {	.ids = 20, .ro = 140,
			.Mhz = ASB_FREQ_MHZ,
			.uV  = { UV(1250), UV(1200), UV(1150), UV(1100), UV(1075), UV(1050),
					 UV(1025), UV(1000), UV(1000), UV(1000), UV(1000), },
	},
	[3] = {	.ids = 50, .ro = 180,
			.Mhz = ASB_FREQ_MHZ,
			.uV  = { UV(1200), UV(1150), UV(1100), UV(1050), UV(1025), UV(1000),
					 UV(1000), UV(1000), UV(1000), UV(1000), UV(1000), },
	},
	[4] = {	.ids = 50, .ro = 180,
			.Mhz = ASB_FREQ_MHZ,
			.uV  = { UV(1200), UV(1150), UV(1100), UV(1050), UV(1050), UV(1050),
					 UV(1050), UV(1050), UV(1050), UV(1050), UV(1050), },
	},
};
#define	ASV_ARRAY_SIZE	(int)(sizeof(asv_tables)/sizeof(asv_tables[0]))



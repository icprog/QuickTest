#ifndef CONSTANT_H
#define CONSTANT_H

#define ARMCMDPORT 2001
#define UDP_MAX_LEN 1600
#define SAMPLE_RATE_80 80
#define conFREQ_NOM    50 //sine
#define conNO_DEFINE   255
#define conWave_SIN   0

#define conProtocal_92    0
#define conProtocal_FT3   1

#define conCurrent   1
#define conVoltage   0

#define Fault_AE  0
#define Fault_BE  1
#define Fault_CE  2
#define Fault_AB  3
#define Fault_BC  4
#define Fault_CA  5
#define Fault_ABE  6
#define Fault_BCE  7
#define Fault_CAE  8
#define Fault_ABC  9
#define Fault_NORMAL 10
#define Fault_ANY 11

#define MaxOutGoose  6   //最多 6 组 GOOSE

typedef union MyFloat_tag{
    float f1;
    char  by1[4];
    }MyFloat;

//2014-10-19,   从小徐新的 TestModule_message.h 移植过来
/* 	Commad head	*/

#define COMMAND_HEAD				"8c908c90"
#define MESSAGE_COMMAND_HEAD		"8c918c91"

    /* SMV	*/

#define SMV_SOURCE_MAC				"ffffffffffff"
#define SMV_DESTINATION_MAC			"ffffffffffff"
#define SMV_TPID					"8100"
#define SMV_TCI						"8000"
#define SMV_ETHERTYPE				"88ba"
#define SMV_APPID					"4000"
#define SMV_LENGTH					"00da"
#define SMV_RESERVED_ONE			"0000"
#define SMV_RESERVED_TWO			"0000"

#define SMV_SAVPDU_TAG				"60"
#define SMV_SAVPDU_LENGTH			"81cf"

    /* NOASDU*/
#define SMV_NOASDU_TAG				"80"
#define SMV_NOASDU_LENGTH			"01"
#define SMV_NOASDU_VALUE			"01"

    /* SEQASDU */
#define SMV_SEQASDU_TAG				"a2"
#define SMV_SEQASDU_LENGTH			"81c9"
    /* ASDU	*/
#define SMV_ASDU_TAG				"30"
#define SMV_ASDU_LENGTH				"81c6"

    /*	SVID */
#define SMV_SVID_TAG				"80"
#define SMV_SVID_LENGTH				"04"
#define SMV_SVID_VALUE				"73766964"


  /*  SMPCNT	*/
#define SMV_SMPCNT_TAG				"82"
#define SMV_SMPCNT_LENGTH			"02"
#define SMV_SMPCNT_VALUE			"0000"



    /*	 CONFREV	*/
#define SMV_CONFREV_TAG				"83"
#define SMV_CONFREV_LENGTH			"04"
#define SMV_CONFREV_VALUE			"00000001"


    /*	SMPSYNCH	*/
#define SMV_SMPSYNCH_TAG			"85"
#define SMV_SMPSYNCH_LENGTH			"01"
#define SMV_SMPSYNCH_VALUE			"01"

    /*	 SEQDATA	*/

#define SMV_SEQDATA_TAG				"87"
#define SMV_SEQDATA_LENGTH			"81b0"


#define SMV_SEQDATA_CHANNEL_COUNT 	22

    /*	SEQDATA_VALUE	*/

#define SMV_SEQDATA_VALUE			"00000000"
#define SMV_SEQDATA_QUALITY			"00000000"



    /* GOOSE */

#define GSE_SOURCE_MAC				"010CCD020000"
#define GSE_DESTINATION_MAC			"ffffffffffff"

#define GSE_TPID					"8100"
#define GSE_TCI						"8000"
#define GSE_ETHERTYPE				"88b8"
#define GSE_APPID					"2000"

#define GSE_LENGTH					"0000"
#define GSE_RESERVED_ONE			"0000"
#define GSE_RESERVED_TWO			"0000"
#define GSE_PDU_TAG					"61"
#define GSE_PDU_LENGTH				"00"

#define GSE_GOCBREF_TAG				"80"
#define GSE_GOCBREF_LENGTH			"07"
#define	GSE_GOCBREF_VALUE			"676f6362526566"

#define GSE_TIIMEALLOWEDTOLIVE_TAG		"81"
#define	GSE_TIIMEALLOWEDTOLIVE_LENGTH	"02"
#define	GSE_TIIMEALLOWEDTOLIVE_VALUE	"0fa0"

#define GSE_DATASET_TAG					"82"
#define GSE_DATASET_LENGTH				"07"
#define GSE_DATASET_VALUE				"44617461736574"

#define GSE_GOID_TAG				"83"
#define GSE_GOID_LENGTH				"04"
#define GSE_GOID_VALUE				"676f4944"

#define GSE_UTCTIME_TAG				"84"
#define GSE_UTCTIME_LENGTH			"08"
#define GSE_UTCTIME_VALUE			"0000000000000050"
// ST 变为 4 位
#define GSE_STNUM_TAG				"85"
#define GSE_STNUM_LENGTH			"04"
#define GSE_STNUM_VALUE				"00000001"

#define GSE_SQNUM_TAG				"86"
#define GSE_SQNUM_LENGTH			"04"
#define GSE_SQNUM_VALUE				"00000000"

#define GSE_TEST_TAG				"87"
#define GSE_TEST_LENGTH				"01"
#define GSE_TEST_VALUE				"00"

#define GSE_CONFREV_TAG				"88"
#define GSE_CONFREV_LENGTH			"01"
#define GSE_CONFREV_VALUE			"01"

#define GSE_NDSCOM_TAG				"89"
#define GSE_NDSCOM_LENGTH			"01"
#define GSE_NDSCOM_VALUE			"00"

#define GSE_NUMDASETENTRIES_TAG		"8a"
#define GSE_NUMDASETENTRIES_LENGTH	"02"
#define GSE_NUMDASETENTRIES_VALUE	"0000"

#define GSE_DATA_TAG				"ab"
#define GSE_DATA_LENGTH				"00"


#endif // CONSTANT_H

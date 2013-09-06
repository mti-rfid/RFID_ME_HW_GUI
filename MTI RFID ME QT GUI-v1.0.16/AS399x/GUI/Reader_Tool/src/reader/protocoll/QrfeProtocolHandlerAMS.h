/*
 *****************************************************************************
 * Copyright @ 2009 by austriamicrosystems AG                                *
 * All rights are reserved.                                                  *
 *                                                                           *
 * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
 * THE SOFTWARE.                                                             *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       * 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
 *****************************************************************************
 */

/*
 * QrfeProtocollHandlerRFE.h
 *
 *  Created on: 15.01.2009
 *      Author: stefan.detter
 */

#ifndef QRFEPROTOCOLLHANDLERAMS_H_
#define QRFEPROTOCOLLHANDLERAMS_H_

#include "QrfeProtocolHandler.h"

#include "../epc/EPC_Defines.h"

#include <QMap>

#include <QrfeTrace.h>

#define POLY 0x1021//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
#if 0
enum REPORT_ID{
	OUT_FIRM_HARDW_ID 	= 0x10,
	IN_FIRM_HARDW_ID 	= 0x11,
	OUT_DEVICE_INFO 	= 0x12,
	IN_DEVICE_INFO 		= 0x13,
	OUT_CPU_RESET  		= 0x16,
	#if 0
	OUT_ANTENNA_POWER 	= 0x18,
	IN_ANTENNA_POWER 	= 0x19,
	#endif
	OUT_WRITE_REG 		= 0x1A,
	IN_WRITE_REG 		= 0x1B,
	OUT_READ_REG 		= 0x1C,
	IN_READ_REG 		= 0x1D,
	OUT_INVENTORY  		= 0x31,
	IN_INVENTORY 		= 0x32,
	OUT_SELCET_TAG  	= 0x33,
	IN_SELCET_TAG  		= 0x34,
	OUT_WRITE_TO_TAG 	= 0x35,
	IN_WRITE_TO_TAG 	= 0x36,
	OUT_READ_FROM_TAG 	= 0x37,
	IN_READ_FROM_TAG  	= 0x38,
	OUT_LOCK_UNLOCK 	= 0x3B,
	IN_LOCK_UNLOCK  	= 0x3C,
	OUT_KILL_TAG 		= 0x3D,
	IN_KILL_TAG 		= 0x3E,
	OUT_INVENTORY_6B_ID	= 0x3F,
	IN_INVENTORY_6B_ID  = 0x40,
	OUT_CHANGE_FREQ 	= 0x41,
	IN_CHANGE_FREQ 		= 0x42,
	OUT_INVENTORY_RSSI 	= 0x43,
	IN_INVENTORY_RSSI 	= 0x44,
	OUT_NXP_COMMAND 	= 0x45,
	IN_NXP_COMMAND 		= 0x46,
	OUT_REGS_COMPLETE   = 0x57,
	IN_REGS_COMPLETE    = 0x58,
	OUT_GEN2_CONFIG     = 0x59,
	IN_GEN2_CONFIG      = 0x5A,
	//Add by yingwei tseng for region, 2009/12/03 
	OUT_SET_REGION      = 0xA8,
	IN_SET_REGION       = 0xA9,
	OUT_GET_REGION      = 0xAA,
	IN_GET_REGION       = 0xAB,
	//End by yingwei tseng for region, 2009/12/03 
	//Add by yingwei tseng for antenna power, 2010/01/04
	#if 0
	OUT_CTRL_ANTENNA_PORT= 0xC0,
	IN_CTRL_ANTENNA_PORT = 0xC1,
	OUT_SET_ANTENNA_POWER = 0xE4,
	IN_SET_ANTENNA_POWER  = 0xE5	
	#endif
	RFID_AntennaPortSetPowerState      = 0x18,
	RFID_AntennaPortSetPowerStateResp  = 0x19,
	RFID_AntennaPortSetPowerLevel      = 0xC0,
	RFID_AntennaPortSetPowerLevel_Resp = 0xC1,
	RFID_AntennaPortSetState           = 0xE4,
	RFID_AntennaPortSetState_Resp      = 0xE5	
	//End by yingwei tseng for antenna power, 2010/01/04
};
#endif

enum REPORT_ID{
	RFID_RadioSetRegion                  = 0xA8,
	RFID_RadioSetRegion_Resp             = 0xA9,
	RFID_RadioGetRegion                  = 0xAA,
	RFID_RadioGetRegion_Resp             = 0xAB,

	RFID_MacGetModuleID 	             = 0x10,
	RFID_MacGetModuleID_Resp 	         = 0x11,
	RFID_MacBypassWriteRegister 	     = 0x1A,
	RFID_MacBypassWriteRegister_Resp     = 0x1B,
	RFID_MacBypassReadRegister 		     = 0x1C,
	RFID_MacBypassReadRegister_Resp      = 0x1D,
	RFID_MacWriteOemData                 = 0xA4,
	RFID_MacWriteOemData_Resp            = 0xA5,
	RFID_MacReadOemData                  = 0xA6,
	RFID_MacReadOemData_Resp             = 0xA7,
	
	RFID_AntennaPortSetPowerState        = 0x18,
	RFID_AntennaPortSetPowerState_Resp   = 0x19,	
	RFID_AntennaPortSetFrequency         = 0x41,
	RFID_AntennaPortSetFrequency_Resp    = 0x42,	
	RFID_AntennaPortSetPowerLevel        = 0xC0,
	RFID_AntennaPortSetPowerLevel_Resp   = 0xC1,
	//Add by yingwei tseng for get power level, 2010/08/02
	RFID_AntennaPortGetPowerLevel        = 0xC2,
	RFID_AntennaPortGetPowerLevel_Resp   = 0xC3,
	//End by yingwei tseng for get power level, 2010/08/02
	RFID_AntennaPortSetOperation         = 0xE4,
	RFID_AntennaPortSetOperation_Resp    = 0xE5,
	//Add by yingwei tseng for modulation tseting, 2010/05/10
	RFID_AntennaPortTransmitPattern      = 0xE6,
	RFID_AntennaPortTransmitPattern_Resp = 0xE7,
	//End by yingwei tseng for modulation tseting, 2010/05/10
	//Add by yingwei tseng for pulse modulation tseting, 2010/10/12
    RFID_AntennaPortTransmitPulse        = 0xEA,
	RFID_AntennaPortTransmitPulse_Resp   = 0xEB,
	//End by yingwei tseng for pulse modulation tseting, 2010/10/12

	RFID_18K6CTagInventory      	     = 0x31,
	RFID_18K6CTagInventory_Resp  	     = 0x32,
	RFID_18K6CTagSelect      	         = 0x33,
	RFID_18K6CTagSelect_Resp     	     = 0x34,
	RFID_18K6CTagWrite 	                 = 0x35,
	RFID_18K6CTagWrite_Resp        	     = 0x36,	
	RFID_18K6CTagRead 	                 = 0x37,
	RFID_18K6CTagRead_Resp               = 0x38,	
	RFID_18K6CTagLock                    = 0x3B,
	RFID_18K6CTagLock_Resp       	     = 0x3C,
	RFID_18K6CTagKill            	     = 0x3D,
	RFID_18K6CTagKill_Resp       	     = 0x3E,
	RFID_18K6CTagInventoryRSSI 	         = 0x43,
	RFID_18K6CTagInventoryRSSI_Resp      = 0x44,
	RFID_18K6CTagNXPCommand              = 0x45,
	RFID_18K6CTagNXPCommand_Resp 	     = 0x46,
	RFID_18K6CSetQueryParameter          = 0x59,
	RFID_18K6CSetQueryParameter_Resp     = 0x5A,		
	//Add by yingwei tseng for memory bank block write, 2011/03/14 
	RFID_18K6CTagBlockWrite              = 0x70,
	RFID_18K6CTagBlockWrite_Resp         = 0x71,
	//End by yingwei tseng for memory bank block write, 2011/03/14 
	//Add by yingwei tseng for NXP alarm message, 2010/03/15
	RFID_18K6CTagNxpEasAlarmCommand         = 0x72,    
	RFID_18K6CTagNxpEasAlarmCommand_Resp    = 0x73,
	//End by yingwei tseng for NXP alarm message, 2010/03/15
	
	RFID_18K6BTagInventory    	         = 0x3F,
	RFID_18K6BTagInventory_Resp          = 0x40,
	RFID_18K6BTagWrite                   = 0x47,
	RFID_18K6BTagWrite_Resp              = 0x48,
	RFID_18K6BTagRead                    = 0x49,
	//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//RFID_18K6BTagRead_Resp               = 0x50,
	RFID_18K6BTagRead_Resp               = 0x4A,
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

	//RFID_EngSetExternalPA                = 0xE0,    //Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

	OUT_DEVICE_INFO 	= 0x12,
	IN_DEVICE_INFO 		= 0x13,
	//OUT_CPU_RESET  		= 0x16,    //Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 	
	OUT_REGS_COMPLETE   = 0x57,
	IN_REGS_COMPLETE    = 0x58
};
//End by yingwei tseng for REPORT_ID table, 2010/05/10

enum INVENTORY_TYPE{
	START_INVENTORY_ROUND = 0x01,
	NEXT_TAG_INFO = 0x02,
	GET_ALL_TAGS_INFO = 0x03,    //Add by yingwei tseng for get all tags information except first tag, 2010/04/02
};

enum ANTENNA_POWER{
	POWER_OFF = 0x00,
	POWER_ON = 0xFF,
};

typedef enum
{
//Add by yingwei tseng for fixed mem bank length, 2010/09/14
    RFID_STATUS_OK                      = 0x00,
    RFID_ERROR_CMD_INVALID_DATA_LENGTH  = 0x0E,
    RFID_ERROR_INVALID_PARAMETER        = 0x0F,
    RFID_ERROR_SYS_CHANNEL_TIMEOUT      = 0x10,
    RFID_ERROR_SYS_MODULE_FAILURE       = 0xFF,
    RFID_ERROR_18K6C_REQRN              = 0x01,
    RFID_ERROR_18K6C_ACCESS             = 0x02,
    RFID_ERROR_18K6C_KILL               = 0x03,
    RFID_ERROR_18K6C_NOREPLY            = 0x04,
    RFID_ERROR_18K6C_LOCK               = 0x05,
    RFID_ERROR_18K6C_BLOCKWRITE         = 0x06,
    RFID_ERROR_18K6C_BLOCKERASE         = 0x07,
    RFID_ERROR_18K6C_READ               = 0x08,
    RFID_ERROR_18K6C_SELECT             = 0x09,
    RFID_ERROR_18K6B_INVALID_CRC        = 0x11,
    RFID_ERROR_18K6B_RFICREG_FIFO       = 0x12,
    RFID_ERROR_18K6B_NO_RESPONSE        = 0x13,
    RFID_ERROR_18K6B_NO_ACKNOWLEDGE     = 0x14,
    RFID_ERROR_18K6B_PREAMBLE           = 0x15,
    RFID_ERROR_6CTAG_OTHER_ERROR        = 0x80,
    RFID_ERROR_6CTAG_MEMORY_OVERRUN     = 0x83,
    RFID_ERROR_6CTAG_MEMORY_LOCKED      = 0x84,
    RFID_ERROR_6CTAG_INSUFFICIENT_POWER = 0x8B,
    RFID_ERROR_6CTAG_NONSPECIFIC_ERROR  = 0x8F
//End by yingwei tseng for fixed mem bank length, 2010/09/14
//Del by yingwei tseng for fixed mem bank length, 2010/09/14
#if 0
//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

    RFID_STATUS_OK                      = 0x00,
    RFID_ERROR_INVALID_PARAMETER        = 0x0F,
    RFID_ERROR_MODULE_FAILURE           = 0xFF,
    RFID_ERROR_GEN2_REQRN               = 0x01,
    RFID_ERROR_GEN2_ACCESS              = 0x02,
    RFID_ERROR_GEN2_KILL                = 0x03,
    RFID_ERROR_GEN2_NOREPLY             = 0x04,
    RFID_ERROR_GEN2_LOCK                = 0x05,
    RFID_ERROR_GEN2_BLOCKWRITE          = 0x06,
    RFID_ERROR_GEN2_BLOCKERASE          = 0x07,
    RFID_ERROR_GEN2_READ                = 0x08,
    RFID_ERROR_GEN2_SELECT              = 0x09,
    RFID_ERROR_GEN2_CHANNEL_TIMEOUT     = 0x10,
    RFID_ERROR_6CTAG_OTHER_ERROR        = 0x80,
    RFID_ERROR_6CTAG_MEMORY_OVERRUN     = 0x83,
    RFID_ERROR_6CTAG_MEMORY_LOCKED      = 0x84,
    RFID_ERROR_6CTAG_INSUFFICIENT_POWER = 0x8B,
    RFID_ERROR_6CTAG_NONSPECIFIC_ERROR  = 0x8F,
    RFID_ERROR_6BTAG_INVALID_CRC        = 0x01,
    RFID_ERROR_6BTAG_NO_RESPONSE        = 0x03,
    RFID_ERROR_6BTAG_NO_ACKNOWLEDGE     = 0x04
//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
#endif
//End by yingwei tseng for fixed mem bank length, 2010/09/14
//Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
#if 0
	AMS_RET_GEN2_NO_ERROR = 0x00,
	/* Errors specified by gen2 standard, transmitted via reply */
	AMS_RET_GEN2_OTHER_ERROR = 0x80,			/*!< @brief	Gen2 Error: Other error */
	AMS_RET_GEN2_MEM_OVERRUN = 0x83,			/*!< @brief	Gen2 Error: Memory overrun -> address does not exist */
	AMS_RET_GEN2_MEM_LOCKED = 0x84,				/*!< @brief Gen2 Error: Memory of the tag is locked */
	AMS_RET_GEN2_INSUFFICIENT_POWER = 0x8B,		/*!< @brief Gen2 Error: The tag has too less power */
	AMS_RET_GEN2_NON_SPECIFIC = 0x8F,			/*!< @brief Gen2 Error: Non specific error */
	AMS_RET_ERR_REQRN     =1,
	AMS_RET_ERR_ACCESS    =2,
	AMS_RET_ERR_KILL      =3,
	AMS_RET_ERR_NOREPLY   =4,
	AMS_RET_ERR_LOCK      =5,
	AMS_RET_ERR_BLOCKWRITE=6,
	AMS_RET_ERR_BLOCKERASE=7,
	AMS_RET_ERR_READ      =8,
	AMS_RET_ERR_SELECT    =9,
	AMS_RET_ERR_CHANNEL_TIMEOUT=10,
#endif	
//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
} eAMS_RET_VALUE;


class QrfeProtocolHandlerAMS : public QrfeProtocolHandler
, QrfeTraceModule
{
	Q_OBJECT

public:
	QrfeProtocolHandlerAMS(QIODevice* dev, DeviceType deviceType, QObject* parent = 0);
	virtual ~QrfeProtocolHandlerAMS();

	eAMS_RET_VALUE lastReturnCode();

public slots:
	Result getHardwareRevision ( QString &hardwareRevision );
	Result getSoftwareRevision ( QString &softwareRevision );

	//Result reboot ( );    //Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//Mod by yingwei tseng for antenna power, 2010/01/04
	#if 0
	Result setAntennaPower ( bool on );
	#endif
	Result setAntennaPortSetPowerState( bool on);
	//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	Result AntennaPortSetPowerLevel (int pLevel);
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//End by yingwei tseng for antenna power, 2010/01/04
	Result AntennaPortGetPowerLevel(int &pLevel);    //Add by yingwei tseng for get power level, 2010/08/02

	Result getRegs ( QByteArray& value );
	Result getParam ( ushort address, QByteArray& value );
	Result setParam ( ushort address, QByteArray value );
	Result setGen2Config (bool set_lf, int *lf_khz, bool set_cd, int *coding, bool set_sess, int *session, bool set_pilot, bool *longPilot, bool set_q, int *qbegin );
	Result setGetSensitivity( signed char *sensitivity, bool set);

	Result doGen2SingleInventory ( QStringList &epc );
	Result doGen2SingleInventoryRSSI ( QStringList &epc, QMap<QString, QByteArray> &rssi );
	Result doISO6BSingleInventory ( QStringList &epc );

	Result selectTag( QString epc );

    //Mod by yingwei tseng for modify read/write tag format, 2010/10/7 
	//Result readFromTag ( uchar mem_bank, ushort address, uchar count, QByteArray &data );
	Result readFromTag ( uchar mem_bank, ushort address, QByteArray password, uchar count, QByteArray &data );
	//End by yingwei tseng for modify read/write tag format, 2010/10/7 
	//Mod by yingwei tseng for memory bank block write, 2011/03/14 
	//Result writeToTag ( uchar mem_bank, ushort address, QByteArray data, QByteArray password, int *writtenBytes = NULL);
	Result writeToTag ( uchar mem_bank, ushort address, QByteArray data, QByteArray password, int writeMode, int *writtenBytes = NULL);
	//End by yingwei tseng for memory bank block write, 2011/03/14 

	//Add by yingwei tseng for frequency, 2010/01/04
	#if 0
	Result setFrequency( ulong frequencyKHz, uchar mode, uchar rssi, uchar profile, QByteArray& result );
	#endif
	Result setFrequency(ulong frequencyKHz, uchar rssi);
	//End by yingwei tseng for frequency, 2010/01/04
	Result getFrequencies ( ulong *start, ulong *stop, uchar *num_freqs, ushort *idleTime, ushort* listenTime, ushort* allocationTime, signed char *rssi, uchar *activeProfile);

	Result setAllocationParams( ushort listeningTime_ms, ushort allocationTime_ms, ushort idleTime_ms  );
	Result continuousSend( ulong frequencyKHz, ushort timeout_ms );


	Result lockTag ( LOCK_MODE mode, LOCK_MEMORY_SPACE memory, QByteArray password );
	Result killTag ( QByteArray password );

	//Mod by yingwei tseng for new NXP function, 2010/04/01
	#if 0
	Result nxpCommand (QByteArray command, QByteArray password );
	#endif
	Result nxpCommand (QByteArray command, QByteArray password , QByteArray config);
	Result nxpEasAlarmCommand (QByteArray& value);    //Add by yingwei tseng for NXP alarm message, 2010/03/15
	//End by yingwei tseng for new NXP function, 2010/04/01
	//Add by yingwei tseng for region, 2009/12/03 
	Result  setRegion ( QByteArray value );
	Result  getRegion(QByteArray &region);
	//End by yingwei tseng for region, 2009/12/03 
	//Mod by yingwei tseng for modulation tseting, 2010/05/10
	Result setAntennaPortSetState(bool modulationMode, bool operationMode);
	//Result setAntennaPortSetState (bool modulationMode);    //Add by yingwei tseng for antenna power, 2010/01/04
	//End by yingwei tseng for modulation tseting, 2010/05/10
	//mod by yo chen, 2010/11/10
	Result setAntennaPortTransmitPattern (ushort loopNumber);    //Add by yingwei tseng for modulation tseting, 2010/05/10
	Result setAntennaPortTransmitPulse(ushort loopSecond);      //Add by yingwei tseng for pulse modulation tseting, 2010/10/12
	//Add by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12
	Result getMacFirmwareOemCfg (QByteArray address, QByteArray& value);
	Result setMacFirmwareOemCfg(QByteArray address, QByteArray value);
	//End by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12

    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
    //void setDeviceFlag(int flag);    //Del by yingwei tseng for auto-select multi rate in UART mode, 2010/09/08
	unsigned short crc16(unsigned char *buf, unsigned short bit_length);
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	void setInventoryAction();    //Add by yingwei tseng for USB/UART offline problem, 2010/11/11

private:
	Result send2Reader(QByteArray msg);

public slots:
	virtual void deviceWasRemoved();

protected:
	virtual int computeData 	( const QByteArray& data );
	virtual int computeMessage ( const QByteArray& msg );

private:
	// The flag for the commands
	bool	m_readerIDFLAG;
	bool	m_hardsoftwareRevisionFLAG;

	//Mod by yingwei tseng for antenna power, 2010/01/04
	#if 0
	bool	m_antennaPowerFLAG;
	#endif
	bool    m_AntennaPortSetPowerState;
	#if 0
	bool    m_antennaPowerCtrlPortFLAG;    
	#endif
	bool    m_AntennaPortSetPowerLevel;
	//End by yingwei tseng for antenna power, 2010/01/04
	bool    m_AntennaPortGetPowerLevel;    //Add by yingwei tseng for get power level, 2010/08/02

	bool	m_maxOutputPowerFLAG;
	bool	m_minOutputPowerFLAG;
	bool	m_currentOutputPowerFLAG;

	bool	m_gen2ConfigFLAG;
	bool	m_getParamFLAG;
	bool	m_setParamFLAG;

	bool	m_singleInventoryFLAG;

	bool 	m_selectTagFLAG;
	bool 	m_readFromTagFLAG;
	bool 	m_writeTagIDFLAG;
	bool 	m_setFrequencyFLAG;
	bool 	m_lockTagFLAG;
	bool 	m_killTagFLAG;
	bool 	m_nxpCommandFLAG;
	bool    m_nxpEasAlarmCommandFLAG;    //Add by yingwei tseng for NXP alarm message, 2010/03/15
	//Add by yingwei tseng for region, 2009/12/03 
	bool    m_setRegionFLAG;    
	bool    m_getRegionFLAG;
	QByteArray m_getRegion;
	//End by yingwei tseng for region, 2009/12/03 	
	bool    m_AntennaPortSetState;    //Add by yingwei tseng for antenna power, 2010/01/04
	bool    m_AntennaPortTransmitPatternFLAG;    //Add by yingwei tseng for modulation tseting, 2010/05/10
	bool    m_AntennaPortTransmitPulseFLAG;      //Add by yingwei tseng for pulse modulation tseting, 2010/10/12
    //Add by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12
    bool    m_getMacFirmwareOemCfgFLAG;
    bool    m_setMacFirmwareOemCfgFLAG;
	//End by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12

	// Buffer for the result of a command
	QString m_hardsoftwareRevision;

	short	m_maxOutputPower;
	short	m_minOutputPower;
	short	m_currentOutputPower;

	uchar 		m_setParamError;
	QByteArray 	m_getParamData;
	QByteArray 	m_gen2ConfigData;

	QStringList					m_singleInventoryResult;
	QMap<QString, QByteArray>	m_singleInventoryRSSI;
	bool 						m_singleInventoryEpcsToRead;

	uchar 		m_selectTagError;
	uchar	 	m_readFromTagError;
	QByteArray 	m_readFromTagResult;
	uchar 		m_writeToTagError;
	uchar 		m_writtenWords;
	uchar 		m_lockTagError;
	uchar 		m_killTagError;
	uchar 		m_nxpCommandError;
	uchar       m_nxpEasAlarmCommandError;    //Add by yingwei tseng for NXP alarm message, 2010/03/15
	//Add by yingwei tseng for region, 2009/12/03 
	uchar       m_setRegionError;
	uchar       m_getRegionError;	
	//End by yingwei tseng for region, 2009/12/03 
	uchar       m_setPowerError;    //Add by yingwei tseng for antenna power, 2010/01/04
	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	uchar       m_AntennaPortSetPowerStateError;
	uchar       m_setGen2ConfigError;
	uchar       m_doGen2SingleInventoryError;
	uchar       m_getRevisionError;
	uchar       m_getParamError;
	uchar       m_AntennaPortTransmitPattern;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	uchar       m_AntennaPortTransmitPulse;    //Add by yingwei tseng for pulse modulation tseting, 2010/10/12
	//Add by yingwei tseng for get power level, 2010/08/02
	uchar       m_setPowerLevelError;    
	uchar       m_getPowerLevelError;    
	//End by yingwei tseng for get power level, 2010/08/02
	
	QByteArray 	m_setFrequencyResult;
	QByteArray 	m_AntennaPortTransmitPatternResult;    //Add by yingwei tseng for modulation tseting, 2010/05/10

    //Add by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12
    QByteArray  m_getMacFirmwareOemCfgData;
    QByteArray  m_setMacFirmwareOemCfgData;
	uchar       m_setMacFirmwareOemCfgError;
	//End by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12

	QByteArray  m_nxpEasAlarmCommandData;    //Add by yingwei tseng for NXP alarm message, 2010/03/15
	int m_deviceFlag;    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	int m_getPowerLevel;    //Add by yingwei tseng for get power level, 2010/08/02

	eAMS_RET_VALUE m_lastReturnCode;

};

#endif /* QRFEPROTOCOLLHANDLERAMS_H_ */

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

#ifndef QRFEPAMSREADER_H_
#define QRFEPAMSREADER_H_

#include "../QrfeReaderInterface.h"
#include "../protocoll/QrfeProtocolHandlerAMS.h"
#include "../QrfeGen2ReaderInterface.h"
#include <QMutex>
#include <QTimer>

#include <QrfeTrace.h>

#define NOT_RESPONDING_WAIT_TIME_MS			200

class CyclicInventoryRunner;

class QrfeAmsReader : public QrfeReaderInterface, public QrfeGen2ReaderInterface
	, QrfeTraceModule
{
	Q_OBJECT

public:
	QrfeAmsReader(QrfeProtocolHandlerAMS* ph, QObject* parent = 0);
	virtual ~QrfeAmsReader();

public:
	virtual Result initDevice( );

	virtual Result getReaderID ( QString &readerID );
	virtual Result getHardwareRevision ( QString &hardwareRevision );
	virtual Result getSoftwareRevision ( QString &softwareRevision );
	virtual ENUM_PRODUCT getDevType( );		

	virtual Result getMaxAttenuation ( ushort &maxAttenuation );
	virtual Result getCurrentAttenuation ( ushort &currentAttenuation );

	//virtual Result reboot ( );    //Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	virtual Result setHeartBeat ( bool on, ushort interval = 0 );
	//Add by yingwei tseng for antenna power, 2010/01/04
	#if 0
	virtual Result setAntennaPower ( bool on );
	#endif
	virtual Result setAntennaPortSetPowerState (bool on);
	//Mod by yingwei tseng for get power level, 2010/08/02
	//virtual Result setAntennaPortSetPowerLevel (int pLevel);
	virtual Result AntennaPortSetPowerLevel (int pLevel);
	//End by yingwei tseng for get power level, 2010/08/02
	virtual Result AntennaPortGetPowerLevel(int &pLevel);    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

	//Mod by yingwei tseng for modulation tseting, 2010/05/10
	Result setAntennaPortSetState(bool modulationMode, bool operationMode);
	//Result setAntennaPortSetState(bool modulationMode);
	//End by yingwei tseng for modulation tseting, 2010/05/10
	//mod by yo chen for the type of loopNumber from ulong to ushort, 2010/11/10
	Result setAntennaPortTransmitPattern (ushort loopNumber);    //Add by yingwei tseng for modulation tseting, 2010/05/10
	Result setAntennaPortTransmitPulse(ushort loopSecond);     //Add by yingwei tseng for pulse modulation tseting, 2010/10/12
	//Ebd by yingwei tseng for antenna power, 2010/01/04
	int getCurrentTime();//Add by yingwei tseng for total times, 2010/02/03
	void readerSetDefaultPowerLevel();    //Add by yingwei tseng for get region from OEM Cfg, 2010/12/09
	virtual Result setAttenuation ( ushort value );

	virtual Result getRegs (QByteArray &value );
	virtual Result getParam ( ushort address, QByteArray &value );
	virtual Result setParam ( ushort address, QByteArray value );
	virtual Result setGen2Config (bool set_lf, int *lf_khz, bool set_cd, int *coding, bool set_sess, int *session, bool setPil, bool *longPilot, bool set_q, int *qbegin );
	virtual Result setGetSensitivity ( signed char *sensitivity, bool set);

	virtual Result setTagType(TagType type);

	virtual Result doSingleInventory ( QStringList &epc );

	virtual Result startCyclicInventory ( );
	virtual Result stopCyclicInventory ( );

	//Add by yingwei tseng for region, 2009/12/03 
	virtual Result setRegion ( QByteArray value );    
	virtual Result getRegion(QByteArray &region);	
	//End by yingwei tseng for region, 2009/12/03 
	//Add by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12	
    virtual Result getMacFirmwareOemCfg ( QByteArray address, QByteArray &value );    
    virtual Result setMacFirmwareOemCfg ( QByteArray address, QByteArray value );
    //End by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12	
	virtual Gen2Result readFromTag ( QString tagId, uchar mem_bank, ushort address, QByteArray passwd, uchar count, QByteArray &data );
    //Mod by yingwei tseng for memory bank block write, 2011/03/14 
	//virtual Gen2Result writeToTag ( QString tagId, uchar mem_bank, ushort address, QByteArray passwd, QByteArray data, int *writtenBytes=NULL );	
    virtual Gen2Result writeToTag ( QString tagId, uchar mem_bank, ushort address, QByteArray passwd, QByteArray data, int writeMode, int *writtenBytes=NULL );
	//End by yingwei tseng for memory bank block write, 2011/03/14 

	//Mod by yingwei tseng for check EPC, 2010/07/06
#if 0
	virtual Gen2Result writeTagId ( QString tagId_Before, QString tagId_After, QByteArray passwd );
#endif
	virtual Gen2Result writeTagId ( QString tagId_Before, QString tagId_After, QByteArray passwd, int *writtenBytes = 0 );
	//End by yingwei tseng for check EPC, 2010/07/06

	//Add by yingwei tseng for check EPC, 2010/07/19
	virtual Gen2Result selectTagId ( QString tagId_After );	
	//End by yingwei tseng for check EPC, 2010/07/19
	virtual Gen2Result lockTag ( QString tagId, LOCK_MODE mode, LOCK_MEMORY_SPACE memory, QByteArray passwd );
	virtual Gen2Result killTag ( QString tagId, QByteArray killPassword  );

	virtual QStringList getRssiChildren();
	//Mod by yingwei tseng for new NXP function, 2010/04/01
	#if 0
	Gen2Result nxpCommand ( QString tagId, QByteArray command, QByteArray password );
	#endif
	Gen2Result nxpCommand ( QString tagId, QByteArray command, QByteArray password, QByteArray config );
	//End by yingwei tseng for new NXP function, 2010/04/01
	Gen2Result nxpEasAlarmCommand (QByteArray &easCode);    //Add by yingwei tseng for NXP alarm message, 2010/03/15
	//Mod by yingwei tseng for frequency, 2010/01/04
	//Result setFrequency ( ulong frequencyKHz, uchar mode, uchar rssi, QByteArray& result );
	#if 0
	Result setFrequency ( ulong frequencyKHz, uchar mode, uchar rssi, uchar profile, QByteArray& result );
	#endif
	Result setFrequency (ulong frequencyKHz, uchar rssi);
	//End by yingwei tseng for frequency, 2010/01/04
	Result getFrequencies ( ulong *start, ulong *stop, uchar *num_freqs, ushort *idleTime, ushort* listenTime, ushort* allocationTime, signed char *rssi, uchar *activeProfile);

	double getG_rfp(void);

	Result setAllocationParams( ushort listeningTime_ms, ushort allocationTime_ms, ushort idleTime_ms  );
	Result continuousSend( ulong freqKHz, ushort timeout_ms );

	quint32 intervalTime();
	void 	setIntervalTime(quint32 time);

	bool rssiEnabled ();
	void setRssiEnabled ( bool on );

	virtual QrfeGen2ReaderInterface* getGen2Reader();

    //Add by yingwei tseng for auto-select multi rate in UART mode, 2010/09/08
    #if 0
	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
    virtual void setDeviceFlag(int flag);
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	#endif
	//End by yingwei tseng for auto-select multi rate in UART mode, 2010/09/08
public slots:
	virtual void deviceWasRemoved();

	void cyclicInventoryRunner();

private:
	virtual QByteArray interpretRSSItoPercent(QByteArray rssi);

	Gen2Result translateGen2ProtocolHandlerResult(QrfeProtocolHandler::Result res);
	Gen2Result translateGen2ReturnCode(eAMS_RET_VALUE ret);
	//Add by yingwei tseng for antenna power, 2010/01/04
	int m_powerLevel;
	//End by yingwei tseng for antenna power, 2010/01/04

	// Helper
	uint	m_notRespondingPauseCounter;

	uint 						m_cyclicInventoryInterval;
	bool						m_cyclicInventoryRunning;
	bool 						m_cyclicInventoryStop;
	CyclicInventoryRunner* 		m_runner;
	bool 						m_runnerRunning;

	// Reader specific settings
	bool 	m_rssiActive;

	QrfeProtocolHandlerAMS* m_ph;

	const static QStringList	m_rssiChildNames;

	friend class CyclicInventoryRunner;

};


class CyclicInventoryRunner : public QThread
{
	Q_OBJECT

public:
	CyclicInventoryRunner(QrfeAmsReader* parent);
	~CyclicInventoryRunner();

protected:
	virtual void run();

private:
	QrfeAmsReader* m_reader;
};


#endif /* QRFEPAMSREADER_H_ */

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
 * QrfeProtocollHandlerBase.h
 *
 *  Created on: 15.01.2009
 *      Author: stefan.detter
 */

#ifndef QRFEPROTOCOLLHANDLER_H_
#define QRFEPROTOCOLLHANDLER_H_

#include <QObject>
#include <QIODevice>
#include <QStringList>
#include <QMutex>
#include <QThread>

#include "protocoll/QrfeProtocolHandler.h"

class QrfeGen2ReaderInterface;

class QrfeReaderInterface : public QObject
{
	Q_OBJECT
	Q_ENUMS( Result ReaderType TagType ReaderVersion HandlerState HandlerAction )


public:
	QrfeReaderInterface(QrfeProtocolHandler* ph, QObject* parent = 0);
	virtual ~QrfeReaderInterface();

	typedef enum {
		OK,
		ERROR,
		COMMERROR,
		NORESPONSE,
		NA
	} Result;

	typedef enum {
		TYPE_ACTIVE,
		TYPE_PASSIVE,
		TYPE_UNKOWN
	} ReaderType;

	typedef enum{
		TAG_ISO6B,
		TAG_GEN2,
		TAG_RFE_AUR1,
		TAG_UNKWON
	} TagType;

	typedef enum {
		VERSION_AMS_LEO,
		VERSION_RFE_PUR_RM1,
		VERSION_RFE_PUR_RM2,
		VERSION_RFE_AUR
	} ReaderVersion;

	typedef enum  {
		STATE_ONLINE,
		STATE_OFFLINE,
		STATE_NOTRESPONDING,
		STATE_UNKOWN_ERROR
	} HandlerState;

	typedef enum  {
		ACTION_OFFLINE,
		ACTION_IDLE,
		ACTION_SCANNING,
		ACTION_WAITING
	} HandlerAction;

	typedef enum {
		USB_MODULE,
		USB_DONGLE,
		RU_865P,
		UNKNOW
	}ENUM_PRODUCT;


	typedef enum {
		USB_MODULE_MIN = 5,
		USB_MODULE_MAX = 24,
		USB_DONGLE_MIN = 5,
		USB_DONGLE_MAX = 18,
		RU_865P_MIN = 10,
		RU_865P_MAX = 27
	}ENUM_POWER;
	

public:
	virtual Result initDevice( ) = 0;											/*!<@brief Pure virtual function to initialize the device */

	virtual Result getReaderID ( QString &readerID ) = 0;						/*!<@brief Pure virtual function to get reader id */
	virtual Result getHardwareRevision ( QString &hardwareRevision ) = 0;		/*!<@brief Pure virtual function to get hardware revision */
	virtual Result getSoftwareRevision ( QString &softwareRevision ) = 0;		/*!<@brief Pure virtual function to get software revision */
	virtual ENUM_PRODUCT getDevType( ) = 0;										/*!<@brief Pure virtual function to get device type */

	virtual Result getMaxAttenuation ( ushort &maxAttenuation ) = 0;			/*!<@brief Pure virtual function to get max attenuation */
	virtual Result getCurrentAttenuation ( ushort &currentAttenuation ) = 0;	/*!<@brief Pure virtual function to get current attenuation */

    //Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//virtual Result reboot ( ) = 0;												/*!<@brief Pure virtual function to reboot the reader */
    //End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	virtual Result setHeartBeat ( bool on, ushort interval = 0 ) = 0;			/*!<@brief Pure virtual function to set the heart beat interval */
	//Add by yingwei tseng for antenna power, 2010/01/04
	#if 0
	virtual Result setAntennaPower ( bool on ) = 0;								/*!<@brief Pure virtual function to set antenna power */
	#endif
	//Mod by yingwei tseng for get power level, 2010/08/02
	//virtual Result setAntennaPortSetPowerLevel (int pLevel) = 0;
    virtual Result AntennaPortSetPowerLevel (int pLevel) = 0;
	//End by yingwei tseng for get power level, 2010/08/02
	//End by yingwei tseng for antenna power, 2010/01/04
	virtual Result setAttenuation ( ushort value ) = 0;							/*!<@brief Pure virtual function to set attenuation */

	virtual Result getParam ( ushort address, QByteArray &value ) = 0;			/*!<@brief Pure virtual function to get a param */
	virtual Result setParam ( ushort address, QByteArray value ) = 0;			/*!<@brief Pure virtual function to set a param */

	virtual Result setTagType(TagType type) = 0;								/*!<@brief Pure virtual function to set the tag type */

	virtual Result doSingleInventory ( QStringList &epc ) = 0;					/*!<@brief Pure virtual function to do a single inventory */

	virtual Result startCyclicInventory ( ) = 0;								/*!<@brief Pure virtual function to start cyclic inventory */
	virtual Result stopCyclicInventory ( ) = 0;									/*!<@brief Pure virtual function to stop cyclic inventory */

	virtual QrfeGen2ReaderInterface* getGen2Reader();							/*!<@brief Virtual function to get the pointer to the gen2 reader */

	virtual QStringList getRssiChildren();										/*!<@brief Virtual function to get the pointer to the gen2 reader */

    //Del by yingwei tseng for auto-select multi rate in UART mode, 2010/09/08
	//virtual void setDeviceFlag(int flag) = 0;    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	//End by yingwei tseng for auto-select multi rate in UART mode, 2010/09/08
	
public:
	QString 		readerId();
	ReaderType		readerType();
	ReaderVersion	readerVersion();
	TagType			tagType();
	ENUM_PRODUCT	devType();

	HandlerState 	currentState();
	QString 		currentStateString();

	HandlerAction 	currentAction();
	QString 		currentActionString();

	QString			deviceName();

protected:
	void setState(HandlerState state);
	void setAction(HandlerAction action);

	Result translateProtocolHandlerError(QrfeProtocolHandler::Result res);
	void   handleProtocolHandlerError(QrfeProtocolHandler::Result res);


public slots:
	virtual void deviceWasRemoved();

	void releaseDeviceImmediatly();

signals:
	void heartBeat(QString readerId);
	void cyclicInventory(QString readerId, QString epc);
	void cyclicRSSIInformation(QString readerId, QString epc, QByteArray rssiPercent);

	void changedState(QString state);
	void changedAction(QString action);

	void changedRssiEnable(QString readerId, bool on, uchar rssiChildCount, QStringList rssiChildNames);

    void lostConnection();

protected:

	QrfeProtocolHandler*	m_ph;

	QString 		m_readerID;
	ReaderType		m_readerType;
	ReaderVersion	m_readerVersion;
	TagType			m_tagType;

	HandlerState 	m_currentState;
	HandlerAction 	m_currentAction;

	ENUM_PRODUCT	m_enumDevType;

};


#endif /* QRFEPROTOCOLLHANDLER_H_ */

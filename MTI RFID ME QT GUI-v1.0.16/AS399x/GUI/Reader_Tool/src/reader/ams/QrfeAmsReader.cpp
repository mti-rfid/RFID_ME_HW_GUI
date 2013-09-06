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
 * QrfeProtocollHandlerRFE.cpp
 *
 *  Created on: 15.01.2009
 *      Author: stefan.detter
 */

#include "QrfeAmsReader.h"

#include "../epc/EPC_Defines.h"

#include <QrfeSleeper.h>
#include <QCoreApplication>
#include <math.h>

const QStringList QrfeAmsReader::m_rssiChildNames = QStringList() << "Channel Q" << "Channel I";

/*!
 * @brief 	Consturctor of the AMS reader object
 */
QrfeAmsReader::QrfeAmsReader(QrfeProtocolHandlerAMS* ph, QObject* parent)
	: QrfeReaderInterface(ph, parent)
	, QrfeTraceModule("QrfeAmsReader")
	, m_ph(ph)
{
	// initialize varaibles
	m_notRespondingPauseCounter = 0;

	/* Create cyclic inventory thread */
	m_runner = new CyclicInventoryRunner(this);
	m_runnerRunning = true;
	m_cyclicInventoryStop = false;
	m_cyclicInventoryInterval = 10;
	m_cyclicInventoryRunning = false;

	m_rssiActive = true;

	m_tagType = TAG_GEN2;

	//del by yo chen, for Panasonic need not set default power but auto set power from OEM, 2011/11/03
	//readerSetDefaultPowerLevel();    //Add by yingwei tseng for get region from OEM Cfg, 2010/12/09	

	/* Start thread */
	m_runner->start(QThread::TimeCriticalPriority);
	while(!m_runner->isRunning());
}

/*!
 * @brief 	Desturctor of the AMS reader object
 */
QrfeAmsReader::~QrfeAmsReader()
{
	/* Stop runner thread */
	m_runnerRunning = false;
	m_cyclicInventoryRunning = false;

	/* Wait for thread finished */
	while(m_runner->isRunning());

	/* Delete thread */
	m_runner->deleteLater();

	/* Set state to offline */
	setState(STATE_OFFLINE);

}

//Add by yingwei tseng for get region from OEM Cfg, 2010/12/09
void QrfeAmsReader::readerSetDefaultPowerLevel()
{
    QrfeReaderInterface::Result resPowerState;

	switch( m_enumDevType )		
    {
	    case USB_MODULE:				
			resPowerState = AntennaPortSetPowerLevel( (int)ENUM_POWER::USB_MODULE_MAX );
			break;
	    case USB_DONGLE:
			resPowerState = AntennaPortSetPowerLevel( (int)ENUM_POWER::USB_DONGLE_MAX );
			break;
		default:		
			break;
    }
}
//End by yingwei tseng for get region from OEM Cfg, 2010/12/09

/*!
 * @brief	Implementation of the virtual function to initialize the device
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::initDevice( )
{
	/* Normally get the serial number, but is not implemented on the reader at the moment, so create an id */
	static QByteArray serial(4, 0x00);
	QString s;
	QStringList sl;
	serial[3] = serial[3] + 1;
	getHardwareRevision(s);
	sl = s.split(" ");
	if (sl.size() < 2) s = "UNKNOWN";
	else s = sl.at(1);

	//Add by yingwei tseng for modifying GUI style, 2010/08/30
    s = "MTI RFID ME";
	//End by yingwei tseng for modifying GUI style, 2010/08/30	
	m_readerID 			= s + " " + QrfeProtocolHandler::epcToString(serial);
	m_readerType 		= TYPE_PASSIVE;
	m_readerVersion 	= VERSION_AMS_LEO;
	m_enumDevType		= getDevType();

	//clark 2011.12.15. Wait for firmware team. 
	//Set Init Power level
	readerSetDefaultPowerLevel();

	return QrfeReaderInterface::OK;
}


/*!
 * @brief	Implementation of the virtual function to get the reader type
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::ENUM_PRODUCT QrfeAmsReader::getDevType( )
{
	QByteArray selProductAddr = QByteArray::fromHex("0080");
	QByteArray selProductValue;	

	if ( QrfeReaderInterface::OK != m_ph->getMacFirmwareOemCfg(selProductAddr, selProductValue) )
		return ENUM_PRODUCT::UNKNOW;		

	m_enumDevType = (QrfeReaderInterface::ENUM_PRODUCT) selProductValue.at(0);

	return m_enumDevType;
}



/*!
 * @brief	Implementation of the virtual function to get the reader id
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::getReaderID ( QString &readerID )
{
	trc(9, "Trying to get hardware ID");
	readerID = m_readerID;

	return QrfeReaderInterface::OK;
}

/*!
 * @brief	Implementation of the virtual function to get the hardware revision
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::getHardwareRevision ( QString &hardwareRevision )
{
	trc(9, "Trying to get hardware ID");
	QrfeProtocolHandler::Result res = m_ph->getHardwareRevision(hardwareRevision);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

/*!
 * @brief	Implementation of the virtual function to get the software revision
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::getSoftwareRevision ( QString &softwareRevision )
{
	trc(9, "Trying to get software ID");
	QrfeProtocolHandler::Result res = m_ph->getSoftwareRevision(softwareRevision);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

/*!
 * @brief	Implementation of the virtual function to get the max attenuation
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::getMaxAttenuation( ushort &maxOutputPower )
{
	maxOutputPower = 0;
	return QrfeReaderInterface::NA;
}

/*!
 * @brief	Implementation of the virtual function to get the current attenuation
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::getCurrentAttenuation( ushort &currentOutputPower )
{
	currentOutputPower = 0;
	return QrfeReaderInterface::NA;
}

/*!
 * @brief	Implementation of the virtual function to reboot the reader
 * @return	Returns the result of the operation coded in an enum
 */
//Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04  
#if 0
QrfeReaderInterface::Result QrfeAmsReader::reboot ( )
{
	trc(9, "Trying to reset CPU");

	m_ph->reboot();

	setState(STATE_OFFLINE);

	return QrfeReaderInterface::OK;
}
#endif
//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 


/*!
 * @brief	Implementation of the virtual function to set the heart beat of the reader
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::setHeartBeat(bool /*on*/, ushort /*interval*/)
{
	return QrfeReaderInterface::NA;
}


/*!
 * @brief	Implementation of the virtual function to set the antenna power of the reader
 * @return	Returns the result of the operation coded in an enum
 */
//Add by yingwei tseng for antenna power, 2010/01/04
#if 0
QrfeReaderInterface::Result QrfeAmsReader::setAntennaPower ( bool on )
{
	QString status = (on)?"ON":"OFF";

	trc(9, "Trying to set antenna power " + status);

	QrfeProtocolHandler::Result res = m_ph->setAntennaPower(on);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}
#endif
QrfeReaderInterface::Result QrfeAmsReader::setAntennaPortSetPowerState ( bool on )
{
	QString status = (on)?"ON":"OFF";

	trc(9, "Trying to set antenna power " + status);

	QrfeProtocolHandler::Result res = m_ph->setAntennaPortSetPowerState(on);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

//Mod by yingwei tseng for get power level, 2010/08/02
//QrfeReaderInterface::Result QrfeAmsReader::setAntennaPortSetPowerLevel (int pLevel)
QrfeReaderInterface::Result QrfeAmsReader::AntennaPortSetPowerLevel (int pLevel)
//End by yingwei tseng for get power level, 2010/08/02
{
    //Mod by yingwei tseng for get power level, 2010/08/02
	//QrfeProtocolHandler::Result res = m_ph->setAntennaPortSetPowerLevel(pLevel);
	QrfeProtocolHandler::Result res = m_ph->AntennaPortSetPowerLevel(pLevel);
	//End by yingwei tseng for get power level, 2010/08/02
	handleProtocolHandlerError(res);
	//m_freqType = freqType;	
	m_powerLevel = pLevel;
	return translateProtocolHandlerError(res);
}

//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
QrfeReaderInterface::Result QrfeAmsReader::AntennaPortGetPowerLevel (int &pLevel)
{
	QrfeProtocolHandler::Result res = m_ph->AntennaPortGetPowerLevel(pLevel);
	handleProtocolHandlerError(res);
	return translateProtocolHandlerError(res);
}
//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

//Mod by yingwei tseng for modulation tseting, 2010/05/10
QrfeReaderInterface::Result QrfeAmsReader::setAntennaPortSetState(bool modulationMode, bool operationMode)
//QrfeReaderInterface::Result QrfeAmsReader::setAntennaPortSetState(bool modulationMode)
//End by yingwei tseng for modulation tseting, 2010/05/10
{
	//Mod by yingwei tseng for modulation tseting, 2010/05/10
	QrfeProtocolHandler::Result res = m_ph->setAntennaPortSetState(modulationMode, operationMode);
	//QrfeProtocolHandler::Result res = m_ph->setAntennaPortSetState(modulationMode);
	//End by yingwei tseng for modulation tseting, 2010/05/10
	handleProtocolHandlerError(res);
	return translateProtocolHandlerError(res);
}

//Add by yingwei tseng for modulation tseting, 2010/05/10
//mod by yo chen for the type of loopNumber from ulong to ushort, 2010/11/10
QrfeReaderInterface::Result QrfeAmsReader::setAntennaPortTransmitPattern (ushort loopNumber)
{
	QrfeProtocolHandler::Result res = m_ph->setAntennaPortTransmitPattern(loopNumber);
	handleProtocolHandlerError(res);
	return translateProtocolHandlerError(res);
}
//End by yingwei tseng for modulation tseting, 2010/05/10
//End by yingwei tseng for antenna power, 2010/01/04

//Add by yingwei tseng for pulse modulation tseting, 2010/10/12
QrfeReaderInterface::Result QrfeAmsReader::setAntennaPortTransmitPulse(ushort loopSecond)
{
    QrfeProtocolHandler::Result res = m_ph->setAntennaPortTransmitPulse(loopSecond);
	handleProtocolHandlerError(res);
	return translateProtocolHandlerError(res);		
}
//End by yingwei tseng for pulse modulation tseting, 2010/10/12

//Add by yingwei tseng for total times, 2010/02/03
int QrfeAmsReader::getCurrentTime()
{
	//QTime startTime;
	QTime startTime = QTime::currentTime();
	int msecTime = 0, secTime = 0, secTimeToMsec = 0, minTime = 0, minTimeToMsec = 0;
	int result = 0;

	msecTime = startTime.msec();
	secTime = startTime.second();
    secTimeToMsec = secTime*1000;
	minTime = startTime.minute(); 
    minTimeToMsec = minTime*60*1000;
	result = msecTime + secTimeToMsec + minTimeToMsec;

	return result;
}	
//End by yingwei tseng for total times, 2010/02/03

/*!
 * @brief	Implementation of the virtual function to set the attenuation
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::setAttenuation( ushort /*value*/ )
{
	return QrfeReaderInterface::NA;
}

/*!
 * @brief	Implementation of the virtual function to get a param of the reader
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::getRegs ( QByteArray &value )
{
	QrfeProtocolHandler::Result res = m_ph->getRegs(value);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

/*!
 * @brief	Implementation of the virtual function to get a param of the reader
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::getParam ( ushort address, QByteArray &value )
{
	QrfeProtocolHandler::Result res = m_ph->getParam(address, value);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

/*!
 * @brief	Implementation of the virtual function to set a param of the reader
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::setParam ( ushort address, QByteArray value )
{
	if(value.size() != 3)
		return QrfeReaderInterface::ERROR;

	QrfeProtocolHandler::Result res = m_ph->setParam(address, value);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

QrfeReaderInterface::Result QrfeAmsReader::setGen2Config (bool set_lf, int *lf_khz, bool set_cd, int *coding, bool set_sess, int *session, bool set_pilot, bool *longPilot, bool set_q, int *qbegin )
{
	QrfeProtocolHandler::Result res = m_ph->setGen2Config(set_lf, lf_khz, set_cd, coding, set_sess, session, set_pilot, longPilot, set_q, qbegin );
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

QrfeReaderInterface::Result QrfeAmsReader::setGetSensitivity ( signed char *sensitivity, bool set )
{
	QrfeProtocolHandler::Result res = m_ph->setGetSensitivity( sensitivity, set);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

/*!
 * @brief	Implementation of the virtual function to set the tag type
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::setTagType(QrfeReaderInterface::TagType type)
{
	if(type != TAG_GEN2 && type != TAG_ISO6B){
		return QrfeReaderInterface::ERROR;
	}

	m_tagType = type;

	return QrfeReaderInterface::OK;
}
//Add by yingwei tseng for region, 2009/12/03 
/*!
 * @brief	Implementation of the virtual function to set a region param of the reader
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::setRegion ( QByteArray value )
{
	if(value.size() != 1)
		return QrfeReaderInterface::ERROR;

	QrfeProtocolHandler::Result res = m_ph->setRegion(value);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

QrfeReaderInterface::Result QrfeAmsReader::getRegion (QByteArray &region)
{
	QrfeProtocolHandler::Result res = m_ph->getRegion(region);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}
//End by yingwei tseng for region, 2009/12/03 

//Add by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12
QrfeReaderInterface::Result QrfeAmsReader::getMacFirmwareOemCfg ( QByteArray address, QByteArray &value )
{
	QrfeProtocolHandler::Result res = m_ph->getMacFirmwareOemCfg(address, value);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

QrfeReaderInterface::Result QrfeAmsReader::setMacFirmwareOemCfg(QByteArray address, QByteArray value)
{
	QrfeProtocolHandler::Result res = m_ph->setMacFirmwareOemCfg(address, value);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}
//End by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12

/*!
 * @brief	Implementation of the virtual function to do a single inventory
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::doSingleInventory ( QStringList &epc )
{

	trc(9, "Trying to do an inventory");
	QrfeProtocolHandler::Result res;

	if(m_tagType == TAG_GEN2)
		res = m_ph->doGen2SingleInventory(epc);
	else if(m_tagType == TAG_ISO6B)
		res = m_ph->doISO6BSingleInventory(epc);

	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}


/*!
 * @brief	Implementation of the virtual function to start a cyclic inventory
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::startCyclicInventory ( )
{
	/* If is already running skip */
	if(m_cyclicInventoryRunning)
		return QrfeReaderInterface::OK;

	setAction(QrfeReaderInterface::ACTION_SCANNING);

	m_cyclicInventoryStop = false;
	m_cyclicInventoryRunning = true;

	return QrfeReaderInterface::OK;
}

/*!
 * @brief	Implementation of the virtual function to stop the cyclic inventory
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::stopCyclicInventory ( )
{
	/* Set variable to stop and wait until finished */
	m_cyclicInventoryStop = true;
	while(m_cyclicInventoryRunning)
		QrfeSleeper::MSleepAlive(5);

	return QrfeReaderInterface::OK;
}



/*!
 * @brief	Implementation of the virtual function to read data from a tag
 * @return	Returns the result of the operation coded in an enum
 */
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::readFromTag ( QString tagId, uchar mem_bank, ushort address, QByteArray passwd, uchar count, QByteArray &data )
{
	QrfeGen2ReaderInterface::Gen2Result gen2Result;
	QrfeProtocolHandler::Result phResult;

	/* Select tag */
	phResult = m_ph->selectTag(tagId);
	handleProtocolHandlerError(phResult);
	gen2Result = translateGen2ProtocolHandlerResult(phResult);
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
    //Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	if(gen2Result != QrfeGen2ReaderInterface::Gen2_OK){
		if(gen2Result == QrfeGen2ReaderInterface::Gen2_ERROR)
			return translateGen2ReturnCode(m_ph->lastReturnCode());
		return gen2Result;
	}
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14

	/* Read from tag */
	//Mod by yingwei tseng for modify read/write tag format, 2010/10/7 
	//phResult = m_ph->readFromTag(mem_bank, address, count, data);
	phResult = m_ph->readFromTag(mem_bank, address, passwd, count, data);
	//End by yingwei tseng for modify read/write tag format, 2010/10/7 
	handleProtocolHandlerError(phResult);
	gen2Result = translateGen2ProtocolHandlerResult(phResult);
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
    //End by yingwei tseng for fixed mem bank length, 2010/09/14
    return QrfeGen2ReaderInterface::RFID_STATUS_OK;
	//Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	if(gen2Result != QrfeGen2ReaderInterface::Gen2_OK){
		if(gen2Result == QrfeGen2ReaderInterface::Gen2_ERROR)
			return translateGen2ReturnCode(m_ph->lastReturnCode());
		return gen2Result;
	}
	return QrfeGen2ReaderInterface::Gen2_OK;
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
}

/*!
 * @brief	Implementation of the virtual function to write data to a tag
 * @return	Returns the result of the operation coded in an enum
 */
//Mod by yingwei tseng for memory bank block write, 2011/03/14 
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::writeToTag ( QString tagId, uchar mem_bank, ushort address, QByteArray passwd, QByteArray data, int writeMode,int *writtenBytes )
{
	int wb = 0;
	int to_write = data.size();
    QrfeGen2ReaderInterface::Gen2Result gen2Result, lastError = QrfeGen2ReaderInterface::RFID_STATUS_OK;
	QrfeProtocolHandler::Result phResult;

	/* Select tag */
	phResult = m_ph->selectTag(tagId);
	handleProtocolHandlerError(phResult);
	gen2Result = translateGen2ProtocolHandlerResult(phResult);
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
	
	/* Write to tag in maximum byte count of 14 byte */
	uchar tempAddr = address;
	int tries = 0, maxtries = data.size()/8+2, tmpWb;
	while(data.size() > 0 && tries++ < maxtries)
	{
		QByteArray ba = data.left(58);	//mod by yo chen, let the "SetEPC" function can set at most 27 words (match with the API Guide), 2011/06/17
										//the API Guide provide at most 27 words -> 27+2=29, 29*2=58
		phResult = m_ph->writeToTag(mem_bank, tempAddr, ba, passwd, writeMode, &tmpWb);
		wb += tmpWb;
		handleProtocolHandlerError(phResult);
		gen2Result = translateGen2ProtocolHandlerResult(phResult);
		if (gen2Result) 
	        lastError = gen2Result;
		else
			break;
		trc(1,QString("Wrote at %1 %2 bytes.").arg(tempAddr).arg(tmpWb));
		tempAddr += tmpWb/2;
		data = data.remove(0, tmpWb);
	}

	if ( writtenBytes != NULL)
		*writtenBytes = wb;
	
	if ( wb != to_write )
		gen2Result = lastError;

	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
    return QrfeGen2ReaderInterface::RFID_STATUS_OK;
}
//End by yingwei tseng for memory bank block write, 2011/03/14 


/*!
 * @brief	Implementation of the virtual function to set the epc of a tag
 * @return	Returns the result of the operation coded in an enum
 */
//Del by yingwei tseng for check EPC, 2010/07/06 
#if 0
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::writeTagId ( QString tagId_Before, QString tagId_After, QByteArray passwd )
#endif
//End by yingwei tseng for check EPC, 2010/07/06
//Add by yingwei tseng for check EPC, 2010/07/06
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::writeTagId ( QString tagId_Before, QString tagId_After, QByteArray passwd, int *writtenBytes )
//End by yingwei tseng for check EPC, 2010/07/06	
{
	trc(9, "Trying to set tag id from " + tagId_Before + " to " + tagId_After + ".");

	QByteArray epc = QrfeProtocolHandler::stringToEpc(tagId_After);

	//Mod by yingwei tseng for check EPC, 2010/07/06 
#if 0
	return writeToTag(tagId_Before, MEM_EPC, MEMADR_EPC, passwd, epc);
#endif
    //Add by yingwei tseng for memory bank block write, 2011/03/14 
	//return writeToTag(tagId_Before, MEM_EPC, MEMADR_EPC, passwd, epc, writtenBytes);
	return writeToTag(tagId_Before, MEM_EPC, MEMADR_EPC, passwd, epc, MEM_SET, writtenBytes);
    //End by yingwei tseng for memory bank block write, 2011/03/14 
	//End by yingwei tseng for check EPC, 2010/07/06 
}

//Add by yingwei tseng for check EPC, 2010/07/19
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::selectTagId ( QString tagId_After )
{
    //Mod by yingwei tseng for fixed mem bank length, 2010/09/14
	//QrfeGen2ReaderInterface::Gen2Result gen2Result = QrfeGen2ReaderInterface::Gen2_OK;
	QrfeGen2ReaderInterface::Gen2Result gen2Result = QrfeGen2ReaderInterface::RFID_STATUS_OK;
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
	QrfeProtocolHandler::Result phResult;

	trc(9, "Trying to select tag id " + tagId_After + ".");
	
	/* Select tag */
	phResult = m_ph->selectTag(tagId_After);
	handleProtocolHandlerError(phResult);
	gen2Result = translateGen2ProtocolHandlerResult(phResult);
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
    //End by yingwei tseng for fixed mem bank length, 2010/09/14
    return QrfeGen2ReaderInterface::RFID_STATUS_OK;
	//Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	if(gen2Result != QrfeGen2ReaderInterface::Gen2_OK){
		if(gen2Result == QrfeGen2ReaderInterface::Gen2_ERROR)
			return translateGen2ReturnCode(m_ph->lastReturnCode());
		return gen2Result;
	}
	return QrfeGen2ReaderInterface::Gen2_OK;
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
}
//End by yingwei tseng for check EPC, 2010/07/19

/*!
 * @brief	Implementation of the virtual function to lock a tag
 * @return	Returns the result of the operation coded in an enum
 */
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::lockTag ( QString tagId, LOCK_MODE mode, LOCK_MEMORY_SPACE memory, QByteArray passwd )
{
	QrfeGen2ReaderInterface::Gen2Result gen2Result;
	QrfeProtocolHandler::Result phResult;

	/* Select tag */
	phResult = m_ph->selectTag(tagId);
	handleProtocolHandlerError(phResult);
	gen2Result = translateGen2ProtocolHandlerResult(phResult);
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
    //Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	if(gen2Result != QrfeGen2ReaderInterface::Gen2_OK){
		if(gen2Result == QrfeGen2ReaderInterface::Gen2_ERROR)
			return translateGen2ReturnCode(m_ph->lastReturnCode());
		return gen2Result;
	}
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
	
	/* Lock the tag */
	phResult = m_ph->lockTag(mode, memory, passwd);
	handleProtocolHandlerError(phResult);
	gen2Result = translateGen2ProtocolHandlerResult(phResult);
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
    //End by yingwei tseng for fixed mem bank length, 2010/09/14
    return QrfeGen2ReaderInterface::RFID_STATUS_OK;
	//Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	if(gen2Result != QrfeGen2ReaderInterface::Gen2_OK){
		if(gen2Result == QrfeGen2ReaderInterface::Gen2_ERROR)
			return translateGen2ReturnCode(m_ph->lastReturnCode());
		return gen2Result;
	}
	return QrfeGen2ReaderInterface::Gen2_OK;
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
}

/*!
 * @brief	Implementation of the virtual function to kill a tag
 * @return	Returns the result of the operation coded in an enum
 */
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::killTag ( QString tagId, QByteArray killPassword )
{
	QrfeGen2ReaderInterface::Gen2Result gen2Result;
	QrfeProtocolHandler::Result res;

	/* Select tag */
	res = m_ph->selectTag(tagId);
	handleProtocolHandlerError(res);
	gen2Result = translateGen2ProtocolHandlerResult(res);
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
    //Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	if(gen2Result != QrfeGen2ReaderInterface::Gen2_OK){
		if(gen2Result == QrfeGen2ReaderInterface::Gen2_ERROR)
			return translateGen2ReturnCode(m_ph->lastReturnCode());
		return gen2Result;
	}
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14

	/* Kill tag */
	res = m_ph->killTag(killPassword);
	handleProtocolHandlerError(res);
	gen2Result = translateGen2ProtocolHandlerResult(res);
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
    //End by yingwei tseng for fixed mem bank length, 2010/09/14
    return QrfeGen2ReaderInterface::RFID_STATUS_OK;
	//Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	if(gen2Result != QrfeGen2ReaderInterface::Gen2_OK){
		if(gen2Result == QrfeGen2ReaderInterface::Gen2_ERROR)
			return translateGen2ReturnCode(m_ph->lastReturnCode());
		return gen2Result;
	}
	return QrfeGen2ReaderInterface::Gen2_OK;
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
}

//Add by yingwei tseng for NXP alarm message, 2010/03/15
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::nxpEasAlarmCommand (QByteArray &easCode)
{
	QrfeGen2ReaderInterface::Gen2Result gen2Result;
	QrfeProtocolHandler::Result res;

	/* Set nxp alarm command */
	res = m_ph->nxpEasAlarmCommand(easCode);
	handleProtocolHandlerError(res);
	gen2Result = translateGen2ProtocolHandlerResult(res);
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
    return QrfeGen2ReaderInterface::RFID_STATUS_OK;
}
//End by yingwei tseng for NXP alarm message, 2010/03/15
/*!
 * @brief	Function to set a nxp command to a tag
 * @return	Returns the result of the operation coded in an enum
 */
//Mod by yingwei tseng for new NXP function, 2010/04/01 
#if 0
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::nxpCommand ( QString tagId, QByteArray command, QByteArray password )
#endif
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::nxpCommand ( QString tagId, QByteArray command, QByteArray password, QByteArray config )
//End by yingwei tseng for new NXP function, 2010/04/01
{
	QrfeGen2ReaderInterface::Gen2Result gen2Result;
	QrfeProtocolHandler::Result res;

	/* Select the used tag */
	res = m_ph->selectTag(tagId);
	handleProtocolHandlerError(res);
	gen2Result = translateGen2ProtocolHandlerResult(res);
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
    //Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	if(gen2Result != QrfeGen2ReaderInterface::Gen2_OK){
		if(gen2Result == QrfeGen2ReaderInterface::Gen2_ERROR)
			return translateGen2ReturnCode(m_ph->lastReturnCode());
		return gen2Result;
	}
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14

	/* Set nxp command */
	//Mod by yingwei tseng for new NXP function, 2010/04/01
	#if 0
	res = m_ph->nxpCommand(command, password);
	#endif
	res = m_ph->nxpCommand(command, password, config);
	//End by yingwei tseng for new NXP function, 2010/04/01
	handleProtocolHandlerError(res);
	gen2Result = translateGen2ProtocolHandlerResult(res);
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	if(gen2Result != QrfeGen2ReaderInterface::RFID_STATUS_OK)
        return translateGen2ReturnCode(m_ph->lastReturnCode());
    //End by yingwei tseng for fixed mem bank length, 2010/09/14
    return QrfeGen2ReaderInterface::RFID_STATUS_OK;
	//Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	if(gen2Result != QrfeGen2ReaderInterface::Gen2_OK){
		if(gen2Result == QrfeGen2ReaderInterface::Gen2_ERROR)
			return translateGen2ReturnCode(m_ph->lastReturnCode());
		return gen2Result;
	}
	return QrfeGen2ReaderInterface::Gen2_OK;
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
}
/*!
 * @brief	Function to get the G value for reflected power according AN2 ?Reflected RF Level Indicator
 * @return	Returns the G value for subsequent usage.
 */
double QrfeAmsReader::getG_rfp(void)
{
	double G;
	QByteArray reg0a,reg13;
	getParam(0x0a,reg0a);
	getParam(0x13,reg13);
	if (reg13.size() == 0 || reg0a.size()==0)
		return 0.0;

	if ((reg13.at(0) & 0xf0) == 0x50)
	{
		switch (reg0a.at(0) & 0x07)
		{
			case 0x00: G=26; break;
			case 0x01: G=10; break;
			case 0x02: G=90; break;
			case 0x03: G=26; break; /* repeat 0x00 case, attenuator and gain should compensate */
			case 0x04: G=46; break;
			case 0x05: G=25; break;
			case 0x06: G=46; break; /* repeat 0x04 case, mixer gain should have no effect on single ended */
			case 0x07: G=25; break; /* repeat 0x05 case, mixer gain should have no effect on single ended */
		}
	}
	else
	{
		/* This code assumes ir<1> = vext_low if not we are anyway in trouble*/
		switch (reg0a.at(0) & 0x07)
		{
			case 0x00: G=23; break;
			case 0x01: G= 9; break;
			case 0x02: G=18; break;
			case 0x03: G= 7; break;
			case 0x04: G=42; break;
			case 0x05: G=23; break;
			case 0x06: G=42; break; /* repeat 0x04 case */
			case 0x07: G=23; break; /* repeat 0x05 case */
		}
	}
	
	return G;
}

/*!
 * @brief	Function to get the complete frequency settings
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::getFrequencies ( ulong *start, ulong *stop, uchar *num_freqs, ushort *idleTime, ushort* listenTime, ushort* allocationTime, signed char *rssi, uchar *activeProfile)
{
	QrfeProtocolHandler::Result res = m_ph->getFrequencies ( start, stop, num_freqs, idleTime, listenTime, allocationTime, rssi, activeProfile);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}


/*!
 * @brief	Function to set the frequency of the reader
 * @return	Returns the result of the operation coded in an enum
 */
//Mod by yingwei tseng for frequency, 2010/01/04
#if 0
QrfeReaderInterface::Result QrfeAmsReader::setFrequency ( ulong frequencyKHz, uchar mode, uchar rssi, QByteArray& result )
{
	QrfeProtocolHandler::Result res = m_ph->setFrequency(frequencyKHz, mode, rssi, result);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}
#endif

#if 0
QrfeReaderInterface::Result QrfeAmsReader::setFrequency ( ulong frequencyKHz, uchar mode, uchar rssi, uchar profile, QByteArray& result )
{
	QrfeProtocolHandler::Result res = m_ph->setFrequency(frequencyKHz, mode, rssi, profile, result);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}
#endif

QrfeReaderInterface::Result QrfeAmsReader::setFrequency (ulong frequencyKHz, uchar rssi)
{
	QrfeProtocolHandler::Result res = m_ph->setFrequency(frequencyKHz, rssi);
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}
//End by yingwei tseng for frequency, 2010/01/04

/*!
 * @brief	Function to set the frequency of the reader
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::setAllocationParams( ushort listeningTime_ms, ushort allocationTime_ms, ushort idleTime_ms  )
{
	QrfeProtocolHandler::Result res = m_ph->setAllocationParams( listeningTime_ms, allocationTime_ms, idleTime_ms  );
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}

/*!
 * @brief	Function to send continuously a modulation into air
 * @return	Returns the result of the operation coded in an enum
 */
QrfeReaderInterface::Result QrfeAmsReader::continuousSend( ulong freqKHz, ushort timeout_ms )
{
	QrfeProtocolHandler::Result res = m_ph->continuousSend(freqKHz,timeout_ms );
	handleProtocolHandlerError(res);

	return translateProtocolHandlerError(res);
}


/*!
 * @brief	Function to get the configured cyclic inventory interval
 * @returns	Returns the configured cyclic inventory interval
 */
quint32 QrfeAmsReader::intervalTime()
{
	return m_cyclicInventoryInterval;
}

/*!
 * @brief	Function to set the cyclic inventory interval
 * @param	time	The cyclic inventory interval
 */
void QrfeAmsReader::setIntervalTime(quint32 time)
{
	m_cyclicInventoryInterval = time;
}


/*!
 * @brief	Function to get if rssi is enabled
 * @returns	Returns true if rssi is enabled
 */
bool QrfeAmsReader::rssiEnabled ( )
{
	return m_rssiActive;
}

/*!
 * @brief	Function to set if rssi should be enabled
 * @param	on		If the rssi should be enabled
 */
void QrfeAmsReader::setRssiEnabled ( bool on )
{
	m_rssiActive = on;
	emit changedRssiEnable(m_readerID, on, m_rssiChildNames.size(), m_rssiChildNames);
}

/*!
 * @brief	Slot that is called if the used device was removed
 */
void QrfeAmsReader::deviceWasRemoved()
{
	m_cyclicInventoryRunning = false;
	QrfeReaderInterface::deviceWasRemoved();
}

//Del by yingwei tseng for auto-select multi rate in UART mode, 2010/09/08
#if 0
//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
void QrfeAmsReader::setDeviceFlag(int flag)
{
    m_ph->setDeviceFlag(flag);
}
//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
#endif
//End by yingwei tseng for auto-select multi rate in UART mode, 2010/09/08

/*!
 * @brief	Function to return the pointer to this gen2 reader
 * @return 	Returns the pointer to this gen2 reader.
 */
QrfeGen2ReaderInterface* QrfeAmsReader::getGen2Reader()
{
	return (QrfeGen2ReaderInterface*) this;
}


/*!
 * @brief	Function that is cyclic called from the external thread
 */
void QrfeAmsReader::cyclicInventoryRunner()
{
	/* If cyclic inventory is running */
	if(m_cyclicInventoryRunning)
	{
		/* If the cyclic inventory should be stoped, change action and turn antenna off */
		if(m_cyclicInventoryStop)
		{
			setAction(QrfeReaderInterface::ACTION_IDLE);

			m_cyclicInventoryRunning = false;
			
			m_ph->setInventoryAction();    //Add by yingwei tseng for USB/UART offline problem, 2010/11/11

			return;
		}
		else
		{
			/* Get the interval time */
			uint interval = m_cyclicInventoryInterval;
			if(interval < 1)
				interval = 1;

			/* If the reader is not responding, wait a defined count of timeout cycles */
			if(m_currentState == QrfeReaderInterface::STATE_NOTRESPONDING &&
				(m_notRespondingPauseCounter+=interval) < NOT_RESPONDING_WAIT_TIME_MS)
			{
				setAction(QrfeReaderInterface::ACTION_WAITING);
				return;
			}

			/* Set action to scanning */
			setAction(QrfeReaderInterface::ACTION_SCANNING);

			/* Reset the no response counter */
			m_notRespondingPauseCounter = 0;

			QStringList 				epcs;
			QMap<QString, QByteArray> 	rssi;

			/* Do the single inventory */
			QrfeProtocolHandler::Result res;
			if(m_tagType == TAG_GEN2)
			{
				if(m_rssiActive)
					res = m_ph->doGen2SingleInventoryRSSI(epcs, rssi);
				else
					res = m_ph->doGen2SingleInventory(epcs);
			}
			else if(m_tagType == TAG_ISO6B)
				res = m_ph->doISO6BSingleInventory(epcs);

			handleProtocolHandlerError(res);

			/* If everything is ok, handle the result and emit the found tags */
			if(res == QrfeProtocolHandler::OK)
			{
				trc(9, "Found " +  QString::number(epcs.size()) + " tags: " + epcs.join(" "));

				/* Emit the results */
				foreach(QString epc, epcs){
				    if (epc.size() != 0){    //Add by yingwei tseng for resolving zero tag problem, 2010/10/01
					    emit cyclicInventory(m_readerID, epc);
					    if(m_rssiActive)
						    emit cyclicRSSIInformation(m_readerID, epc, interpretRSSItoPercent(rssi.value(epc)));
				    }    //Add by yingwei tseng for resolving zero tag problem, 2010/10/01
				}
			}
		}
	}
}

/*!
 * @brief Function interprets the byte given from the reader into the two rssi channel values
 */
QByteArray QrfeAmsReader::interpretRSSItoPercent(QByteArray rssi)
{
	if(rssi.size() != 1)
		return QByteArray(3, 0x00);
	uchar q = (rssi.at(0) & 0xF0) >> 4;
	uchar i = (rssi.at(0) & 0x0F);
	
	//double rssiVal = sqrt ((double)(q*q + i*i))/sqrt(2.0);
        // the following formula is taken from the proper RSSI related application notes.
	double highRSSI = q>i?q:i;
	double lowRSSI = q<i?q:i;
	double deltaRSSI = highRSSI - lowRSSI;
	double rssiVal = 2*highRSSI + 10 * log10(1+pow(10,-deltaRSSI/10));
        //         33  = 2 * 15     + 10 * log10(1+1)
        // maximum value of the above formala goes intto percentage calculation

	QByteArray res;
	res.append((uchar) ((rssiVal / 33) * 100.0));
	res.append(q*2);
	res.append(i*2);

	return res;
}

/*!
 * @brief Function translates the protocol handler result into a gen2 return code
 */
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::translateGen2ProtocolHandlerResult(QrfeProtocolHandler::Result res)
{
	switch(res)
	{
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	case RFID_STATUS_OK:
		return QrfeGen2ReaderInterface::RFID_STATUS_OK;
	case RFID_ERROR_CMD_INVALID_DATA_LENGTH:
		return QrfeGen2ReaderInterface::RFID_ERROR_CMD_INVALID_DATA_LENGTH;
	case RFID_ERROR_INVALID_PARAMETER :
		return QrfeGen2ReaderInterface::RFID_ERROR_INVALID_PARAMETER;
	//Add by yingwei tseng for NXP alarm message, 2010/03/15	
    case RFID_ERROR_SYS_CHANNEL_TIMEOUT :
		return QrfeGen2ReaderInterface::RFID_ERROR_SYS_CHANNEL_TIMEOUT;
	case RFID_ERROR_HWOPT_READONLY_ADDRESS :	
		return QrfeGen2ReaderInterface::RFID_ERROR_HWOPT_READONLY_ADDRESS;
	case RFID_ERROR_HWOPT_UNSUPPORTED_REGION :
		return QrfeGen2ReaderInterface::RFID_ERROR_HWOPT_UNSUPPORTED_REGION;
	case RFID_ERROR_SYS_SECURITY_FAILURE :
		return QrfeGen2ReaderInterface::RFID_ERROR_SYS_SECURITY_FAILURE;
	//End by yingwei tseng for NXP alarm message, 2010/03/15			
	case RFID_ERROR_SYS_MODULE_FAILURE:
		return QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	case RFID_ERROR_18K6C_REQRN:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_REQRN;
	case RFID_ERROR_18K6C_ACCESS:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_ACCESS;
	case RFID_ERROR_18K6C_KILL:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_KILL;
	case RFID_ERROR_18K6C_NOREPLY:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_NOREPLY;
	case RFID_ERROR_18K6C_LOCK:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_LOCK;
	case RFID_ERROR_18K6C_BLOCKWRITE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_BLOCKWRITE;
	case RFID_ERROR_18K6C_BLOCKERASE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_BLOCKERASE;
	case RFID_ERROR_18K6C_READ:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_READ;
	case RFID_ERROR_18K6C_SELECT:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_SELECT;
	case RFID_ERROR_18K6B_INVALID_CRC:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6B_INVALID_CRC;
	case RFID_ERROR_18K6B_RFICREG_FIFO:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6B_RFICREG_FIFO;
	case RFID_ERROR_18K6B_NO_RESPONSE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6B_NO_RESPONSE;
	case RFID_ERROR_18K6B_NO_ACKNOWLEDGE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6B_NO_ACKNOWLEDGE;
	case RFID_ERROR_18K6B_PREAMBLE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6B_PREAMBLE;
	//Add by yingwei tseng for NXP alarm message, 2010/03/15	
	case RFID_ERROR_18K6C_EASCODE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_EASCODE;
	//End by yingwei tseng for NXP alarm message, 2010/03/15		
	case RFID_ERROR_6CTAG_OTHER_ERROR:
		return QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_OTHER_ERROR;
	case RFID_ERROR_6CTAG_MEMORY_OVERRUN:
		return QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_MEMORY_OVERRUN;
	case RFID_ERROR_6CTAG_MEMORY_LOCKED:
		return QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_MEMORY_LOCKED;	
	case RFID_ERROR_6CTAG_INSUFFICIENT_POWER:
		return QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_INSUFFICIENT_POWER;				
	case RFID_ERROR_6CTAG_NONSPECIFIC_ERROR:
		return QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_NONSPECIFIC_ERROR;			
	default:
		return QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;	
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
	//Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	case QrfeProtocolHandler::OK:
		return QrfeGen2ReaderInterface::Gen2_OK;
	case QrfeProtocolHandler::ERROR:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case QrfeProtocolHandler::COMMERROR:
		return QrfeGen2ReaderInterface::Gen2_COMMERROR;
	case QrfeProtocolHandler::NORESPONSE:
		return QrfeGen2ReaderInterface::Gen2_NORESPONSE;
	default:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	#endif	
	//End by yingwei tseng for fixed mem bank length, 2010/09/14	
	}
}

/*!
 * @brief Function translates the gen2 return code of the reader into a gen2 return code
 */
QrfeGen2ReaderInterface::Gen2Result QrfeAmsReader::translateGen2ReturnCode(eAMS_RET_VALUE ret)
{
	switch(ret)
	{
	//Add by yingwei tseng for fixed mem bank length, 2010/09/14
	case RFID_STATUS_OK:
		return QrfeGen2ReaderInterface::RFID_STATUS_OK;
	case RFID_ERROR_CMD_INVALID_DATA_LENGTH:
		return QrfeGen2ReaderInterface::RFID_ERROR_CMD_INVALID_DATA_LENGTH;
	case RFID_ERROR_INVALID_PARAMETER :
		return QrfeGen2ReaderInterface::RFID_ERROR_INVALID_PARAMETER;
	//Add by yingwei tseng for NXP alarm message, 2010/03/15	
    case RFID_ERROR_SYS_CHANNEL_TIMEOUT :
		return QrfeGen2ReaderInterface::RFID_ERROR_SYS_CHANNEL_TIMEOUT;
	case RFID_ERROR_HWOPT_READONLY_ADDRESS :

		return QrfeGen2ReaderInterface::RFID_ERROR_HWOPT_READONLY_ADDRESS;
	case RFID_ERROR_HWOPT_UNSUPPORTED_REGION :
		return QrfeGen2ReaderInterface::RFID_ERROR_HWOPT_UNSUPPORTED_REGION;
	case RFID_ERROR_SYS_SECURITY_FAILURE :
		return QrfeGen2ReaderInterface::RFID_ERROR_SYS_SECURITY_FAILURE;
	//End by yingwei tseng for NXP alarm message, 2010/03/15	
	case RFID_ERROR_SYS_MODULE_FAILURE:
		return QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	case RFID_ERROR_18K6C_REQRN:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_REQRN;
	case RFID_ERROR_18K6C_ACCESS:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_ACCESS;
	case RFID_ERROR_18K6C_KILL:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_KILL;
	case RFID_ERROR_18K6C_NOREPLY:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_NOREPLY;
	case RFID_ERROR_18K6C_LOCK:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_LOCK;
	case RFID_ERROR_18K6C_BLOCKWRITE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_BLOCKWRITE;
	case RFID_ERROR_18K6C_BLOCKERASE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_BLOCKERASE;
	case RFID_ERROR_18K6C_READ:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_READ;
	case RFID_ERROR_18K6C_SELECT:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_SELECT;
	case RFID_ERROR_18K6B_INVALID_CRC:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6B_INVALID_CRC;
	case RFID_ERROR_18K6B_RFICREG_FIFO:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6B_RFICREG_FIFO;
	case RFID_ERROR_18K6B_NO_RESPONSE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6B_NO_RESPONSE;
	case RFID_ERROR_18K6B_NO_ACKNOWLEDGE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6B_NO_ACKNOWLEDGE;
	case RFID_ERROR_18K6B_PREAMBLE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6B_PREAMBLE;
	//Add by yingwei tseng for NXP alarm message, 2010/03/15	
	case RFID_ERROR_18K6C_EASCODE:
		return QrfeGen2ReaderInterface::RFID_ERROR_18K6C_EASCODE;
	//End by yingwei tseng for NXP alarm message, 2010/03/15				
	case RFID_ERROR_6CTAG_OTHER_ERROR:
		return QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_OTHER_ERROR;
	case RFID_ERROR_6CTAG_MEMORY_OVERRUN:
		return QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_MEMORY_OVERRUN;
	case RFID_ERROR_6CTAG_MEMORY_LOCKED:
		return QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_MEMORY_LOCKED;	
	case RFID_ERROR_6CTAG_INSUFFICIENT_POWER:
		return QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_INSUFFICIENT_POWER;				
	case RFID_ERROR_6CTAG_NONSPECIFIC_ERROR:
		return QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_NONSPECIFIC_ERROR;			
	default:
		return QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;	
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
	//Del by yingwei tseng for fixed mem bank length, 2010/09/14
	#if 0
	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	case RFID_STATUS_OK:
		return QrfeGen2ReaderInterface::Gen2_OK;
	case RFID_ERROR_6CTAG_INSUFFICIENT_POWER:
		return QrfeGen2ReaderInterface::Gen2_INSUFFICIENT_POWER;
	case RFID_ERROR_6CTAG_MEMORY_LOCKED:
		return QrfeGen2ReaderInterface::Gen2_MEM_LOCKED;
	case RFID_ERROR_6CTAG_MEMORY_OVERRUN:
		return QrfeGen2ReaderInterface::Gen2_MEM_OVERRUN;
	case RFID_ERROR_6CTAG_NONSPECIFIC_ERROR:
		return QrfeGen2ReaderInterface::Gen2_NON_SPECIFIC;
	case RFID_ERROR_6CTAG_OTHER_ERROR:
		return QrfeGen2ReaderInterface::Gen2_OTHER;
	case RFID_ERROR_GEN2_REQRN:
		return QrfeGen2ReaderInterface::Gen2_REQRN_FAILED;
	case RFID_ERROR_GEN2_ACCESS:
		return QrfeGen2ReaderInterface::Gen2_ACCESS_FAILED;
	case RFID_ERROR_GEN2_KILL:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case RFID_ERROR_GEN2_NOREPLY:
		return QrfeGen2ReaderInterface::Gen2_NOREPLY;
	case RFID_ERROR_GEN2_LOCK:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case RFID_ERROR_GEN2_BLOCKWRITE:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case RFID_ERROR_GEN2_BLOCKERASE:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case RFID_ERROR_GEN2_READ:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case RFID_ERROR_GEN2_SELECT:
		return QrfeGen2ReaderInterface::Gen2_TAG_UNREACHABLE;
	case RFID_ERROR_GEN2_CHANNEL_TIMEOUT:
		return QrfeGen2ReaderInterface::Gen2_CHANNEL_TIMEOUT;
	default:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
	//Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	#if 0    
	case AMS_RET_GEN2_NO_ERROR:
		return QrfeGen2ReaderInterface::Gen2_OK;
	case AMS_RET_GEN2_INSUFFICIENT_POWER:
		return QrfeGen2ReaderInterface::Gen2_INSUFFICIENT_POWER;
	case AMS_RET_GEN2_MEM_LOCKED:
		return QrfeGen2ReaderInterface::Gen2_MEM_LOCKED;
	case AMS_RET_GEN2_MEM_OVERRUN:
		return QrfeGen2ReaderInterface::Gen2_MEM_OVERRUN;
	case AMS_RET_GEN2_NON_SPECIFIC:
		return QrfeGen2ReaderInterface::Gen2_NON_SPECIFIC;
	case AMS_RET_GEN2_OTHER_ERROR:
		return QrfeGen2ReaderInterface::Gen2_OTHER;
	case AMS_RET_ERR_REQRN:
		return QrfeGen2ReaderInterface::Gen2_REQRN_FAILED;
	case AMS_RET_ERR_ACCESS:
		return QrfeGen2ReaderInterface::Gen2_ACCESS_FAILED;
	case AMS_RET_ERR_KILL:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case AMS_RET_ERR_NOREPLY:
		return QrfeGen2ReaderInterface::Gen2_NOREPLY;
	case AMS_RET_ERR_LOCK:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case AMS_RET_ERR_BLOCKWRITE:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case AMS_RET_ERR_BLOCKERASE:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case AMS_RET_ERR_READ:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	case AMS_RET_ERR_SELECT:
		return QrfeGen2ReaderInterface::Gen2_TAG_UNREACHABLE;
	case AMS_RET_ERR_CHANNEL_TIMEOUT:
		return QrfeGen2ReaderInterface::Gen2_CHANNEL_TIMEOUT;
	default:
		return QrfeGen2ReaderInterface::Gen2_ERROR;
	#endif
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 	    
	}
}

QStringList QrfeAmsReader::getRssiChildren()
{
	return m_rssiChildNames;
}

/*!
 * @brief Constructor of the thread that initiates cyclic the inventory procedure
 */
CyclicInventoryRunner::CyclicInventoryRunner(QrfeAmsReader* parent)
	: QThread(parent)
	, m_reader(parent)
{
}

/*!
 * @brief Destructor of the thread
 */
CyclicInventoryRunner::~CyclicInventoryRunner()
{
}

/*!
 * @brief Thread main function calls cyclic inventory procedure
 */
void CyclicInventoryRunner::run()
{
	while(m_reader->m_runnerRunning)
	{
		msleep(m_reader->m_cyclicInventoryInterval);
		m_reader->cyclicInventoryRunner();
	}
}

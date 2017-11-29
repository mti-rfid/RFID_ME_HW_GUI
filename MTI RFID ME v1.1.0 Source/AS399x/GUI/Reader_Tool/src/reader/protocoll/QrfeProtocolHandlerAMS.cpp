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

#include "QrfeProtocolHandlerAMS.h"

#include "../epc/EPC_Defines.h"

#include <QrfeSleeper.h>
#include <QCoreApplication>

/*!
 * @brief 	Constructor of the AMS implementation of the protocol handler
 */
QrfeProtocolHandlerAMS::QrfeProtocolHandlerAMS(QIODevice* dev, DeviceType deviceType, QObject* parent)
	: QrfeProtocolHandler(dev, deviceType, parent)
	, QrfeTraceModule("QrfeProtocollHandlerAMS")
{
	m_MAX_WAIT_TIME_IN_MS = 1000;
	m_deviceFlag = deviceType;     //Add by yingwei tseng for auto-select multi rate in UART mode, 2010/09/08
}

/*!
 * @brief 	Destructor
 */
QrfeProtocolHandlerAMS::~QrfeProtocolHandlerAMS()
{
}

/*!
 * @brief 	Function to get the last return code of the reader
 */
eAMS_RET_VALUE QrfeProtocolHandlerAMS::lastReturnCode()
{
	return m_lastReturnCode;
}

/*!
 * @brief 	Implementation of the get hardware revision command
 * @param	hardwareRevision		String with the hardware revision is returned
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::getHardwareRevision ( QString &hardwareRevision )
{
	trc(9, "Trying to get hardware ID");
	// build up the command byte array
	QByteArray buf;
	buf.resize(3);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_FIRM_HARDW_ID;
	buf [0] = RFID_MacGetModuleID;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	buf [2] = 1;

	// reset the flag
	m_hardsoftwareRevisionFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;


	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_hardsoftwareRevisionFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	// set the variable to the new value
	hardwareRevision = m_hardsoftwareRevision;

	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	if(m_getRevisionError != 0)
		return QrfeProtocolHandler::ERROR;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

	return QrfeProtocolHandler::OK;
}

/*!
 * @brief 	Implementation of the get software revision command
 * @param	softwareRevision	String with the software revision is returned
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::getSoftwareRevision ( QString &softwareRevision )
{
	trc(9, "Trying to get hardware ID");
	// build up the command byte array
	QByteArray buf;
	buf.resize(3);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_FIRM_HARDW_ID;
	buf [0] = RFID_MacGetModuleID;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	buf [2] = 0;

	// reset the flag
	m_hardsoftwareRevisionFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_hardsoftwareRevisionFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	softwareRevision = m_hardsoftwareRevision;

	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	if(m_getRevisionError != 0)
		return QrfeProtocolHandler::ERROR;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 	

	return QrfeProtocolHandler::OK;
}

/*!
 * @brief 	Implementation of the reboot command
 * @return	Returns the result of the operation coded in an enum
 */
//Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
#if 0
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::reboot ( )
{
	trc(9, "Trying to reset CPU");
	// build up the command byte array and send it
	QByteArray buf;
	buf.resize(2);
	buf [0] = OUT_CPU_RESET;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	return QrfeProtocolHandler::OK;
}
#endif
//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 


/*!
 * @brief 	Implementation of the set antenna power command
 * @param	on		Specifies if antenna should be set on or off
 * @return	Returns the result of the operation coded in an enum
 */
//Del by yingwei tseng for antenna power, 2010/01/04 
#if 0
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setAntennaPower ( bool on )
{
	QString status = (on)?"ON":"OFF";
	trc(9, "Trying to set antenna power " + status);
	// build up the command byte array and send it
	QByteArray buf;

	buf.resize(3);
	buf [0] = OUT_ANTENNA_POWER;
	buf [2] = (uchar) (on)?0xFF:0x00;

	m_antennaPowerFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_antennaPowerFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	trc(9, "Set antenna power " + status);
	return QrfeProtocolHandler::OK;
}
#endif
//End by yingwei tseng for antenna power, 2010/01/04
//Add by yingwei tseng for antenna power, 2010/01/04
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setAntennaPortSetPowerState ( bool on )
{
	QString status = (on)?"ON":"OFF";
	trc(9, "Trying to set antenna power " + status);
	// build up the command byte array and send it
	QByteArray buf;

	buf.resize(3);
	buf [0] = RFID_AntennaPortSetPowerState;
	buf [2] = (uchar) (on)?0xFF:0x00;

	m_AntennaPortSetPowerState = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_AntennaPortSetPowerState, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	if(m_AntennaPortSetPowerStateError != 0)
		return QrfeProtocolHandler::ERROR;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 	

	trc(9, "Set antenna power " + status);
	return QrfeProtocolHandler::OK;
} 

//Mod by yingwei tseng for get power level, 2010/08/02
//QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setAntennaPortSetPowerLevel (int pLevel)
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::AntennaPortSetPowerLevel (int pLevel)
//End by yingwei tseng for get power level, 2010/08/02
{
	// build up the command byte array and send it
	QByteArray buf;
    QString str = QString( "%1" )
                .arg( pLevel, 2, 16 );	
	QByteArray pLevelConv = QByteArray::fromHex(str.toAscii());	

	buf.resize(2);
	buf [0] = RFID_AntennaPortSetPowerLevel;
	buf [1] = 0x03;
	buf.append(pLevelConv);
	m_AntennaPortSetPowerLevel = false;		
	
	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_AntennaPortSetPowerLevel, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

    //Add by yingwei tseng for get power level, 2010/08/02
	if (m_setPowerLevelError != 0)
		return QrfeProtocolHandler::ERROR;
	//End by yingwei tseng for get power level, 2010/08/02

	trc(9, "Trying to set power level" + QString::number(pLevel));
	return QrfeProtocolHandler::OK;
}

//Add by yingwei tseng for get power level, 2010/08/02
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::AntennaPortGetPowerLevel(int &pLevel)
{
    QByteArray buf;
	buf.resize(2);
	buf [0] = RFID_AntennaPortGetPowerLevel;
	buf [1] = 0x02;

	m_AntennaPortGetPowerLevel = false;

	Result res = send2Reader(buf);
	if (res != QrfeProtocolHandler::OK)
		return res;

	bool result = waitForFlag(&m_AntennaPortGetPowerLevel, m_MAX_WAIT_TIME_IN_MS);
	pLevel = m_getPowerLevel;
	if (!result)
		return QrfeProtocolHandler::NORESPONSE;

	if (m_getPowerLevelError != 0)
		return QrfeProtocolHandler::ERROR;
	
    return QrfeProtocolHandler::OK;
}
//End by yingwei tseng for get power level, 2010/08/02

//Add by yingwei tseng for antenna power, 2010/01/04
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setAntennaPortSetState (bool modulationMode, bool operationMode)
{
	QByteArray buf;

    //Add by yingwei tseng for setting command length, 2010/10/05
	//buf.resize(3);
	buf.resize(4);
	//End by yingwei tseng for setting command length, 2010/10/05
	buf [0] = RFID_AntennaPortSetOperation;
	buf [1] = 0x04;
	buf [2] = (uchar) (modulationMode)?0x01:0x00;
	buf [3] = (uchar) (operationMode)?0x01:0x00;    //Add by yingwei tseng for modulation tseting, 2010/05/10
	
	m_AntennaPortSetState = false;	

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_AntennaPortSetState, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	return QrfeProtocolHandler::OK;	
}
//End by yingwei tseng for antenna power, 2010/01/04
//Add by yingwei tseng for modulation tseting, 2010/05/10
//mod by yo chen for the type of loopNumber from ulong to ushort, 2010/11/10
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setAntennaPortTransmitPattern (ushort loopNumber)
{
	QByteArray buf;

	//mod by yo chen for RFID_AntennaPortTransmitPattern from 6 bytes to 4 bytes to match F/W version, 2010/11/10
	/*
	buf.resize(6);
	buf [0] = RFID_AntennaPortTransmitPattern;
	buf [1] = 0x06;
	buf [2] = (loopNumber & 0xFF000000) >> 24;
	buf [3] = (loopNumber & 0x00FF0000) >> 16;
	buf [4] = (loopNumber & 0x0000FF00) >> 8;
	buf [5] = loopNumber & 0x000000FF;
	*/
	buf.resize(4);
	buf [0] = RFID_AntennaPortTransmitPattern;
	buf [1] = 0x04;
	buf [2] = (loopNumber & 0x0000FF00) >> 8;
	buf [3] = loopNumber & 0x000000FF;
	//end by yo chen, 2010/11/10
	m_AntennaPortTransmitPatternFLAG = false;	

	trcBytes(2, "Sending bytes ", buf);
		
	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	//bool result = waitForFlag(&m_AntennaPortTransmitPatternFLAG, (loopNumber/100)*1000*3);
	bool result = waitForFlag(&m_AntennaPortTransmitPatternFLAG, loopNumber*1000);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	if(m_AntennaPortTransmitPattern != 0)
		return QrfeProtocolHandler::ERROR;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 		
	
	return QrfeProtocolHandler::OK;	
}
//End by yingwei tseng for modulation tseting, 2010/05/10

//Add by yingwei tseng for pulse modulation tseting, 2010/10/12
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setAntennaPortTransmitPulse(ushort loopSecond)
{
    QByteArray buf;

	buf.resize(4);

	buf [0] = RFID_AntennaPortTransmitPulse;
	buf [1] = 0x04;
	buf [2] = (loopSecond & 0x0000FF00) >> 8;
	buf [3] = (loopSecond & 0x000000FF);
  
	m_AntennaPortTransmitPulseFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;	

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_AntennaPortTransmitPulseFLAG, loopSecond*1000);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;	

	if(m_AntennaPortTransmitPulse != 0)
		return QrfeProtocolHandler::ERROR;
	
    trc(9, "Set antenna port transmit pulse is " + QString::number(loopSecond));
    return QrfeProtocolHandler::OK;		
}
//End by yingwei tseng for pulse modulation tseting, 2010/10/12

QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setGetSensitivity(signed char *sensitivity, bool set )
{
	// build up the command byte array and send it
	//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//QByteArray buf(64,'\0');
	QByteArray buf(14,'\0');
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	QByteArray value;

	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf[0] = OUT_GEN2_CONFIG;
	buf[0] = RFID_18K6CSetQueryParameter;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf[1] will be filled by send2Reader
	buf[12] = (set==true)?1:0;
	buf[13] = *sensitivity;
	
	m_gen2ConfigFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_gen2ConfigFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	value = m_gen2ConfigData;

	trc(9, QString("SetGetSensitiviy sens=%1 set=%2: %3 ").arg(*sensitivity).arg(set).arg(value.at(11)));

	*sensitivity = value.at(11);

    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	if(m_setGen2ConfigError != 0)
		return QrfeProtocolHandler::ERROR;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 	

	return QrfeProtocolHandler::OK;
}
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setGen2Config 
(bool set_lf, int *lf_khz, bool set_cd, int *miller, bool set_sess, int *session,
 bool set_pilot, bool *longPilot, bool set_q, int *qbegin )
{
	int lf;
	int coding;
	// build up the command byte array and send it
	//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//QByteArray buf(64,'\0');
	QByteArray buf(14,'\0');
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	QByteArray value;

	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf[0] = OUT_GEN2_CONFIG;
	buf[0] = RFID_18K6CSetQueryParameter;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf[1] will be filled by send2Reader
	switch(*lf_khz){
		case 40:  lf = 0;  break;
		case 80:  lf = 3;  break;
		case 160: lf = 6;  break;
		case 213: lf = 8;  break;
		case 256: lf = 9;  break;
		case 320: lf = 12; break;
		case 640: lf = 15; break;
		default:  lf = 6;  break;
	}
	switch(*miller){
		case 0: coding = 0; break;
		case 2: coding = 1; break;
		case 4: coding = 2; break;
		case 8: coding = 3; break;
		default: coding = 1;
	}

	buf[2] = set_lf;
	buf[3] = lf;
	buf[4] = set_cd;
	buf[5] = coding;
	buf[6] = set_sess;
	buf[7] = *session;
	//Mod by yingwei tseng for modfying gen2 setting pilot tone, 2010/07/23
	#if 0
	buf[8] = set_pilot;
	buf[9] = *longPilot;
	#endif
	buf[8] = 0x00;
	buf[9] = 0x01;
	//End by yingwei tseng for modfying gen2 setting pilot tone, 2010/07/23
	buf[10] = set_q;
	buf[11] = *qbegin;
	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
    buf[12] = 0;
	buf[13] = 0;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	
	m_gen2ConfigFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_gen2ConfigFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	value = m_gen2ConfigData;

	QString msg = QString("Set gen2config lf=%1 cod=%2 session=%3 pilot=%4 qbegin=%5 ").arg(lf).arg(coding).arg(*session).arg(*longPilot).arg(*qbegin);

	lf = value[1];
	coding = value.at(3);
	switch(lf){
		case 0 : *lf_khz = 40 ;  break;
		case 3 : *lf_khz = 80 ;  break;
		case 6 : *lf_khz = 160;  break;
		case 8 : *lf_khz = 213;  break;
		case 9 : *lf_khz = 256;  break;
		case 12: *lf_khz = 320; break;
		case 15: *lf_khz = 640; break;
		default: *lf_khz = 777;  break;
	}
	switch(coding){
		case 0:  *miller = 0; break;
		case 1:  *miller = 2; break;
		case 2:  *miller = 4; break;
		case 3:  *miller = 8; break;
		default: *miller = 42;
	}
	
	*session = value.at(5);
	*longPilot = value.at(7);
	*qbegin = value.at(9);

	msg += QString("to lf=%1 cod=%2 session=%3 pilot=%4 qbegin=%5 ").arg(lf).arg(coding).arg(*session).arg(*longPilot).arg(*qbegin);

    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	if(m_setGen2ConfigError != 0)
		return QrfeProtocolHandler::ERROR;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	
	trc(9, msg);
	return QrfeProtocolHandler::OK;
}
/*!
 * @brief 	Implementation of the set param command
 * @param	address		The address of the param
 * @param	value		The data that should be set
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setParam ( ushort address, QByteArray value )
{
	if(value.size() != 3)
		return QrfeProtocolHandler::ERROR;

	// build up the command byte array and send it
	QByteArray buf;

	buf.resize(3);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_WRITE_REG;
	buf [0] = RFID_MacBypassWriteRegister;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	buf [2] = (uchar)address;
	buf.append(value);

	m_setParamFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_setParamFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	// set the variable to the new value
	if(m_setParamError != 0)
		return QrfeProtocolHandler::ERROR;

	trc(9, "Set param at " + QString::number(address) + " to " + QString("0x%1%2%3")
			.arg((unsigned char)value.at(2), 2, 16, QChar('0'))
			.arg((unsigned char)value.at(1), 2, 16, QChar('0'))
			.arg((unsigned char)value.at(0), 2, 16, QChar('0')));
	return QrfeProtocolHandler::OK;
}

/*!
 * @brief 	Implementation of the set param command
 * @param	address		The address of the param
 * @param	value		The data that should be set
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::getParam ( ushort address, QByteArray& value )
{
	// build up the command byte array and send it
	QByteArray buf;

	buf.resize(3);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_READ_REG;
	buf [0] = RFID_MacBypassReadRegister;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	buf [2] = (uchar)address;

	m_getParamFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_getParamFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	// set the variable to the new value
	if(m_getParamData.size() == 0)
		return QrfeProtocolHandler::ERROR;

	if(m_getParamData.size() < 3)
	{
		QByteArray a(3 - m_getParamData.size(), (char)0x00);
		m_getParamData.append(a);
	}

	value = m_getParamData;

	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	if(m_getParamError != 0)
		return QrfeProtocolHandler::ERROR;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 			

	trc(9, "Get param of " + QString::number(address) + " is " + m_getParamData.toHex());
	return QrfeProtocolHandler::OK;
}

//Add by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setMacFirmwareOemCfg(QByteArray address, QByteArray value)
{	
    if (value.size() != 1)
		return QrfeProtocolHandler::ERROR;


	// build up the command byte array and send it
	QByteArray buf;

	buf.resize(2);
	buf [0] = RFID_MacWriteOemData;
	buf [1] = 0x05;
	buf.append(address);
	buf.append(value);

	m_setMacFirmwareOemCfgFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;
	
	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_setMacFirmwareOemCfgFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	// set the variable to the new value
	if(m_setMacFirmwareOemCfgError != 0)
		return QrfeProtocolHandler::ERROR;

	trc(0, "Set MAC firmware OEM config param at " + address.toHex() + " to " + value.toHex());
	return QrfeProtocolHandler::OK;
}

QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::getMacFirmwareOemCfg(QByteArray address, QByteArray& value)
{
	// build up the command byte array and send it
	QByteArray buf;

	buf.resize(2);
	buf [0] = RFID_MacReadOemData;
	buf [1] = 0x04;
	buf.append(address);

	m_getMacFirmwareOemCfgFLAG = false;

	//send the command
	Result res = send2Reader(buf);
	if (res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_getMacFirmwareOemCfgFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;
	
	// set the variable to the new value
	if(m_getMacFirmwareOemCfgData.size() == 0)
		return QrfeProtocolHandler::ERROR;

	if(m_getMacFirmwareOemCfgData.size() < 2)
	{
		QByteArray a(2 - m_getMacFirmwareOemCfgData.size(), (char)0x00);
		m_getMacFirmwareOemCfgData.append(a);
	}

	value = m_getMacFirmwareOemCfgData;
	
	trc(0, "Get MAC firmware OEM config param of " + address.toHex() + " is " + m_getMacFirmwareOemCfgData.toHex());
	return QrfeProtocolHandler::OK;	
}
//End by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12

/*!
 * @brief 	Implementation of the set param command
 * @param	value		The data that should be set
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::getRegs (QByteArray& value )
{
	// build up the command byte array and send it
	QByteArray buf;

	buf.resize(2);
	buf [0] = OUT_REGS_COMPLETE;

	m_getParamFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_getParamFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	// set the variable to the new value
	if(m_getParamData.size() == 0)
		return QrfeProtocolHandler::ERROR;

	value = m_getParamData;

	trc(9, "Get all regs is " + m_getParamData.toHex());
	return QrfeProtocolHandler::OK;
}

/*!
 * @brief 	Implementation of the single inventory gen2 command
 * @param	epc		A list of epcs that were read is returned
 * @return	Returns the result of the operation coded in an enum
 */
//Add by yingwei tseng for USB/UART offline problem, 2010/11/11
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::doGen2SingleInventory( QStringList &epc )
{
    trc(9, "Trying to do an inventory");

	unsigned i=0;
	int j = 2;
	// build up the command byte array
	QByteArray buf;
	buf.resize(3);
	buf [0] = RFID_18K6CTagInventory;
	buf [2] = START_INVENTORY_ROUND;

	// clear the receive buffer
	m_singleInventoryResult.clear();

	do{		
		// reset the flags
		m_singleInventoryFLAG = false;
		m_singleInventoryEpcsToRead = false;

		if (m_deviceFlag == DEV_SERIAL)
		{
            if (buf.at(2) == START_INVENTORY_ROUND)
			    m_inventoryAction = START_INVENTORY_ROUND;
            else
                m_inventoryAction = GET_ALL_TAGS_INFO;
		}		
        

		// send the command
		Result res = send2Reader(buf);
		if(res != QrfeProtocolHandler::OK){
			return res;
		}

        // wait for either the flag or a timeout		
	    while(i++ < m_MAX_WAIT_TIME_IN_MS && !m_singleInventoryFLAG)
		    QrfeSleeper::MSleepAlive(1, QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);	

	    i = 0 ;
       		
 		// set command to get the next tag
 		if (m_singleInventoryEpcsToRead)
		    buf[2] = GET_ALL_TAGS_INFO;
		else
			break;

 		j--;
	}while(j);	


	while(m_singleInventoryEpcsToRead){
 		m_singleInventoryFLAG = false;

	    // check for the flag for the given time
	    while(i++ < m_MAX_WAIT_TIME_IN_MS && !m_singleInventoryFLAG)
		    QrfeSleeper::MSleepAlive(1, QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);

		i = 0;
		
		if (m_singleInventoryFLAG == false)
			break;
	}

	// save the result in the variable
	epc = m_singleInventoryResult;

	if(m_doGen2SingleInventoryError != 0)
		return QrfeProtocolHandler::ERROR;

	return QrfeProtocolHandler::OK;
}

/*!
 * @brief 	Implementation of the single inventory gen2 command with rssi info
 * @param	epc		A list of epcs that were read is returned
 * @param	rssi	A map of epcs and the measured rssi value
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::doGen2SingleInventoryRSSI ( QStringList &epc, QMap<QString, QByteArray> &rssi )
{
    trc(9, "Trying to do an inventory");
	unsigned i=0;
	int j = 2;

	QByteArray buf;
	buf.resize(3);
	buf [0] = RFID_18K6CTagInventoryRSSI;
	buf [2] = START_INVENTORY_ROUND;

	// clear the receive buffer
	m_singleInventoryResult.clear();
	m_singleInventoryRSSI.clear();

	do{		
		// reset the flags
		m_singleInventoryFLAG = false;
		m_singleInventoryEpcsToRead = false;

		if (m_deviceFlag == DEV_SERIAL)
		{ 
            if (buf.at(2) == START_INVENTORY_ROUND)
			    m_inventoryAction = START_INVENTORY_ROUND;
            else
                m_inventoryAction = GET_ALL_TAGS_INFO;
		}		
        
		// send the command
		Result res = send2Reader(buf);
		if(res != QrfeProtocolHandler::OK){
			return res;
		}

        // wait for either the flag or a timeout		
	    while(i++ < m_MAX_WAIT_TIME_IN_MS && !m_singleInventoryFLAG)
		    QrfeSleeper::MSleepAlive(1, QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);	

        i = 0 ;
		
 		// set command to get the next tag
 		if (m_singleInventoryEpcsToRead)
		    buf[2] = GET_ALL_TAGS_INFO;
		else
			break;

 		j--;
	}while(j);	


	while(m_singleInventoryEpcsToRead){
 		m_singleInventoryFLAG = false;

	    // check for the flag for the given time
	    while(i++ < m_MAX_WAIT_TIME_IN_MS && !m_singleInventoryFLAG)
		    QrfeSleeper::MSleepAlive(1, QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);

		i = 0;
		
		if (m_singleInventoryFLAG == false)
			break;
	} 	

	// save the result in the variable
	epc = m_singleInventoryResult;
	rssi = m_singleInventoryRSSI;

	if(m_doGen2SingleInventoryError != 0)
		return QrfeProtocolHandler::ERROR;

	return QrfeProtocolHandler::OK;
}

/*!
 * @brief 	Implementation of the single inventory iso 6b command
 * @param	epc		A list of epcs that were read is returned
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::doISO6BSingleInventory( QStringList &epc )
{
	trc(9, "Trying to do an inventory");

	unsigned i=0;
	int j = 2;

	// build up the command byte array
	QByteArray buf;
	buf.resize(13);
	buf [0] = RFID_18K6BTagInventory;
	buf [2] = START_INVENTORY_ROUND;
	buf [3] = buf [4] = buf [5] = buf [6] = buf [7] = buf [8] = buf [9] = buf [10] = buf [11] = buf [12] = 0;

	// clear the receive buffer
	m_singleInventoryResult.clear();

	do{		
		// reset the flags
		m_singleInventoryFLAG = false;
		m_singleInventoryEpcsToRead = false;

		if (m_deviceFlag == DEV_SERIAL)
		{ 
            if (buf.at(2) == START_INVENTORY_ROUND)
			    m_inventoryAction = START_INVENTORY_ROUND;
            else
                m_inventoryAction = GET_ALL_TAGS_INFO;
		}		
        
		// send the command
		Result res = send2Reader(buf);
		if(res != QrfeProtocolHandler::OK){
			return res;
		}

        // wait for either the flag or a timeout		
	    while(i++ < m_MAX_WAIT_TIME_IN_MS && !m_singleInventoryFLAG)
		    QrfeSleeper::MSleepAlive(1, QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);	

        i = 0 ; 
		
 		// set command to get the next tag
 		if (m_singleInventoryEpcsToRead)
		    buf[2] = GET_ALL_TAGS_INFO;
		else
			break;

 		j--;
	}while(j);	


	while(m_singleInventoryEpcsToRead){
 		m_singleInventoryFLAG = false;

	    // check for the flag for the given time
	    while(i++ < m_MAX_WAIT_TIME_IN_MS && !m_singleInventoryFLAG)
		    QrfeSleeper::MSleepAlive(1, QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);

		i = 0;
		
		if (m_singleInventoryFLAG == false)
			break;
	} 	

	// save the result in the variable
	epc = m_singleInventoryResult;

	if(m_doGen2SingleInventoryError != 0)
		return QrfeProtocolHandler::ERROR;

	return QrfeProtocolHandler::OK;
}
 
void QrfeProtocolHandlerAMS::setInventoryAction()
{
    //m_inventoryAction = START_INVENTORY_ROUND;
    m_inventoryAction = 0x00;
}

//End by yingwei tseng for USB/UART offline problem, 2010/11/11		 
/*!
 * @brief 	Implementation of the select tag command
 * @param	epc		The epc of the tag that should be selected
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::selectTag( QString epc )
{
	trc(9, "Trying to select the tag " + epc);
	// TODO: TEST
	//return QrfeProtocolHandler::OK;

	bool ok = false;
	QByteArray epcBytes = stringToEpc(epc, &ok);

        /* Allow zero length tags.... */
#if 0
	if(!ok)
		return QrfeProtocolHandler::ERROR;
#endif

	// build up the command byte array and send it
	QByteArray buf;

	buf.resize(3);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_SELCET_TAG; // Report ID;
	buf [0] = RFID_18K6CTagSelect; // Report ID;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	buf [2] = epcBytes.size();	// EPC size

	buf.append(epcBytes);
	
	buf [1] = buf.length();    //Add by yingwei tseng for fixed select/write command length, 2010/12/02

    //Del by yingwei tseng for fixed select/write command length, 2010/12/02
	//buf.resize(64);    //Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	//End by yingwei tseng for fixed select/write command length, 2010/12/02

	m_selectTagFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_selectTagFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	// set the variable to the new value
	if(m_selectTagError != 0)
		return QrfeProtocolHandler::ERROR;

	return QrfeProtocolHandler::OK;
}


/*!
 * @brief 	Implementation of the read from tag command
 * @param	mem_bank	The memory bank from which should be read
 * @param	address		The address where data should be read
 * @param	count		The count of data
 * @param	data		The data that was read is returned
 * @return	Returns the result of the operation coded in an enum
 */
//Mod by yingwei tseng for modify read/write tag format, 2010/10/7  
//QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::readFromTag ( uchar mem_bank, ushort address, uchar count, QByteArray &data )
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::readFromTag ( uchar mem_bank, ushort address, QByteArray password, uchar count, QByteArray &data )
//End by yingwei tseng for modify read/write tag format, 2010/10/7 
{
    //Mod by yingwei tseng for modify read/write tag format, 2010/10/7 
    //trc(1, "Trying to read from tag at membank " + QString::number(mem_bank) + " address " + QString::number(address) +  " count " + QString::number(count));
	trc(1, "Trying to read from tag at membank " + QString::number(mem_bank) + " address " + QString::number(address) + " password " + password.toHex() + " count " + QString::number(count));
    //End by yingwei tseng for modify read/write tag format, 2010/10/7 
    
	// build up the command byte array and send it
	QByteArray buf;

    //Add by yingwei tseng for modify read/write tag format, 2010/10/7 
	if(password.size() != 4)
		return QrfeProtocolHandler::ERROR;	
	//End by yingwei tseng for modify read/write tag format, 2010/10/7 

    //Mod by yingwei tseng for modify read/write tag format, 2010/10/7 
    //Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//buf.resize(9);
	//buf.resize(5);
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	buf.resize(4);
	//End by yingwei tseng for modify read/write tag format, 2010/10/7 
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_READ_FROM_TAG; // Report ID;
	buf [0] = RFID_18K6CTagRead; // Report ID;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	//Mod by yingwei tseng for modify read/write tag format, 2010/10/7 
	//buf [1] = 0x05;//Add by yingwei tseng for setting command length, 2010/10/05
	buf [1] = 0x09;
	//End by yingwei tseng for modify read/write tag format, 2010/10/7 
	buf [2] = mem_bank;  		// Bank: EPC
	buf [3] = address;			// Address
	//Mod by yingwei tseng for modify read/write tag format, 2010/10/7 
	//buf [4] = count;			// Data Length
	buf.append(password);
	buf.append(count);        // Data Length 
	//End by yingwei tseng for modify read/write tag format, 2010/10/7 
    //Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
    #if 0
	buf [5] = 0x00;		// Password, not used
	buf [6] = 0x00;		// Password, not used
	buf [7] = 0x00;		// Password, not used
	buf [8] = 0x00;		// Password, not used
	#endif
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

	m_readFromTagFLAG = false;
	m_readFromTagResult.clear();

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_readFromTagFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	data = m_readFromTagResult;

	// set the variable to the new value
	if(m_readFromTagError != 0)
		return QrfeProtocolHandler::ERROR;

	return QrfeProtocolHandler::OK;

}

/*!
 * @brief 	Implementation of the swrite to tag command
 * @param	mem_bank	The memory bank to which should be written
 * @param	address		The address where data should be written
 * @param	data		The data that should be written
 * @param 	password	The access password for write operation
 * @return	Returns the result of the operation coded in an enum
 */
//Mod by yingwei tseng for memory bank block write, 2011/03/14  
//QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::writeToTag ( uchar mem_bank, ushort address, QByteArray data, QByteArray password, int *writtenBytes )
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::writeToTag ( uchar mem_bank, ushort address, QByteArray data, QByteArray password, int writeMode, int *writtenBytes )
//End by yingwei tseng for memory bank block write, 2011/03/14 
{
	if (writtenBytes != NULL) *writtenBytes = 0;
	trc(9, "Trying to write to tag.");

	if(password.size() != 4)
		return QrfeProtocolHandler::ERROR;

	if(data.size()%2 == 1)
		data.push_back((char)0x00);


	// build up the command byte array and send it
	QByteArray buf;

	buf.resize(4);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_WRITE_TO_TAG; // Report ID;
	//Mod by yingwei tseng for memory bank block write, 2011/03/14 
	//buf [0] = RFID_18K6CTagWrite; // Report ID;
	if (writeMode == MEM_SET)
	    buf [0] = RFID_18K6CTagWrite;
	else
		buf [0] = RFID_18K6CTagBlockWrite;
	//End by yingwei tseng for memory bank block write, 2011/03/14
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	buf [2] = mem_bank;  		// Bank: EPC
	buf [3] = address;			// Address
	buf.append(password);		// Password, not used
	buf.append(data.size()/2);	// Data Length
	buf.append(data);

	buf [1] = buf.length();    //Add by yingwei tseng for fixed select/write command length, 2010/12/02

    //Del by yingwei tseng for fixed select/write command length, 2010/12/02
	//buf.resize(64);    //Del by yingwei tseng for setting command length, 2010/10/05
	//End by yingwei tseng for fixed select/write command length, 2010/12/02

	m_writeTagIDFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_writeTagIDFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	if (writtenBytes != NULL)
		*writtenBytes = m_writtenWords * 2;

	// set the variable to the new value
	if(m_writeToTagError != 0)
		return QrfeProtocolHandler::ERROR;

	if (writtenBytes != NULL){
		*writtenBytes = m_writtenWords * 2;
	}	

	return QrfeProtocolHandler::OK;
}

/*!
 * @brief 	Implementation of the lock command
 * @param	mode		The specified lock mode
 * @param	memory 		The specified memory space
 * @param	password	The access password for the lock operation
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::lockTag ( LOCK_MODE mode, LOCK_MEMORY_SPACE memory, QByteArray password )
{
	trc(9, "Trying to get lock tag");
	// build up the command byte array
	QByteArray buf;
	buf.resize(4);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_LOCK_UNLOCK;
	buf [0] = RFID_18K6CTagLock;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	buf [2] = mode;
	buf [3] = memory;
	buf.append(password);

	// reset the flag
	m_lockTagFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;


	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_lockTagFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	if(m_lockTagError != 0x00)
		return QrfeProtocolHandler::ERROR;

	return QrfeProtocolHandler::OK;
}

/*!
 * @brief 	Implementation of the kill command
 * @param	password	The kill password
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::killTag ( QByteArray password )
{
	trc(9, "Trying to get lock tag");
	if(password.size() != 4)
		return QrfeProtocolHandler::ERROR;

	// build up the command byte array
	QByteArray buf;
	//Mod by yingwei tseng for kill/lock bug, 2010/09/23
	//buf.resize(7);
	buf.resize(6);
	//End by yingwei tseng for kill/lock bug, 2010/09/23
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_KILL_TAG;
	buf [0] = RFID_18K6CTagKill;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	buf [2] = password.at(0);
	buf [3] = password.at(1);
	buf [4] = password.at(2);
	buf [5] = password.at(3);
	//buf [6] = 0;    //Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

	// reset the flag
	m_killTagFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;


	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_killTagFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	if(m_killTagError != 0x00)
		return QrfeProtocolHandler::ERROR;

	return QrfeProtocolHandler::OK;
}

/*!
 * @brief 	Implementation of the nxp command
 * @param	command		The command that should be executed
 * @param	password	The access password for this command
 * @return	Returns the result of the operation coded in an enum
 */
//Mod by yingwei tseng for new NXP function, 2010/04/01 
#if 0
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::nxpCommand (QByteArray command, QByteArray password )
#endif 
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::nxpCommand (QByteArray command, QByteArray password, QByteArray config )
//End by yingwei tseng for new NXP function, 2010/04/01 
{
	trc(9, "Trying to set nxp Command");
	if(command.size() != 2)
		return QrfeProtocolHandler::ERROR;

	if(password.size() != 4)
		return QrfeProtocolHandler::ERROR;

	// build up the command byte array
	QByteArray buf;
	//Mod by yingwei tseng for setting command length, 2010/10/05
	//buf.resize(64);
	buf.resize(10);
	//End by yingwei tseng for setting command length, 2010/10/05
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_NXP_COMMAND;
	buf [0] = RFID_18K6CTagNXPCommand;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	buf [2] = command.at(0);
	buf [3] = command.at(1);
	buf [4] = password.at(0);
	buf [5] = password.at(1);
	buf [6] = password.at(2);
	buf [7] = password.at(3);
	//Add by yingwei tseng for new NXP function, 2010/04/01 
	buf [8] = config.at(0);
	buf [9] = config.at(1);
	//End by yingwei tseng for new NXP function, 2010/04/01 

	// reset the flag
	m_nxpCommandFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;


	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_nxpCommandFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	if(m_nxpCommandError != 0x00)
		return QrfeProtocolHandler::ERROR;

	return QrfeProtocolHandler::OK;
}

//Add by yingwei tseng for NXP alarm message, 2010/03/15
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::nxpEasAlarmCommand (QByteArray &easCode)
{
    trc(9, "Trying to set NXP EAS alarm command");
    QByteArray buf;
	buf.resize(2);
	buf [0] = RFID_18K6CTagNxpEasAlarmCommand;
	buf [1] = 0x02;

	// reset the flag
	m_nxpEasAlarmCommandFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_nxpEasAlarmCommandFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	if(m_nxpEasAlarmCommandError != 0x00)
		return QrfeProtocolHandler::ERROR;

    easCode = m_nxpEasAlarmCommandData;
	
	return QrfeProtocolHandler::OK;
}
//End by yingwei tseng for NXP alarm message, 2010/03/15
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::getFrequencies ( ulong *start, ulong *stop, uchar *num_freqs, ushort *idleTime, ushort* listenTime, ushort* allocationTime, signed char *rssi, uchar *activeProfile)
{
	trc(1, "Trying to get frequency settings ");

	// build up the command byte array
	QByteArray buf(64,0);
	buf.resize(64);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_CHANGE_FREQ;
	buf [0] = RFID_AntennaPortSetFrequency;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	buf [1] = 7;
	buf [2] = 17;

	// reset the flag
	m_setFrequencyFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;


	// wait for either the flag or a timeout
	bool waitOk = waitForFlag(&m_setFrequencyFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!waitOk)
		return QrfeProtocolHandler::NORESPONSE;

	*activeProfile  = ((uchar)m_setFrequencyResult[2] );
	*listenTime     = ((uchar)m_setFrequencyResult[3] ) |( (uchar)m_setFrequencyResult[4]<<8);
	*allocationTime = ((uchar)m_setFrequencyResult[5] ) |( (uchar)m_setFrequencyResult[6]<<8);
	*idleTime       = ((uchar)m_setFrequencyResult[7] ) |( (uchar)m_setFrequencyResult[8]<<8);
	*start          = ((uchar)m_setFrequencyResult[9] ) |( (uchar)m_setFrequencyResult[10]<<8) | ((uchar)m_setFrequencyResult[11]<<16);
	*stop           = ((uchar)m_setFrequencyResult[12]) |( (uchar)m_setFrequencyResult[13]<<8) | ((uchar)m_setFrequencyResult[14]<<16);
	*num_freqs      = ((uchar)m_setFrequencyResult[15]);
	*rssi           = ((uchar)m_setFrequencyResult[16]);

	return QrfeProtocolHandler::OK;
}


/*!
 * @brief 	Implementation of the set frequency command
 * @param	frequencyKHz	The frequency in kHz
 * @param	mode			1: set freq and return rssi
 *                          2: set freq and return reflected power
 *                          4: add freq to the frequency hopping list
 *                          8: clear frequency hopping list and add freq
 * @param	result			Result of the operation
 * @return	Returns the result of the operation coded in an enum
 */
//Mod by yingwei tseng for frequency, 2010/01/04
#if 0
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setFrequency( ulong frequencyKHz, uchar mode, uchar rssi, uchar profile, QByteArray& result)
{
	frequencyKHz = 0x00FFFFFF & frequencyKHz;

	trc(1, "Trying to set frequency to " + QString::number(frequencyKHz / 1000.0) + " MHz with the " + QString(mode) + " result");
	// build up the command byte array
	QByteArray buf;
	buf.resize(64);
	buf [0] = OUT_CHANGE_FREQ;
	buf [1] = 7;
	buf [2] = mode;
	buf[3] = frequencyKHz & 0x000000FF;
	buf[4] = (frequencyKHz & 0x0000FF00) >> 8;
	buf[5] = (frequencyKHz & 0x00FF0000) >> 16;
	buf[6] = rssi;
	buf[7] = profile;

	// reset the flag
	m_setFrequencyFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;


	// wait for either the flag or a timeout
	bool waitOk = waitForFlag(&m_setFrequencyFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!waitOk)
		return QrfeProtocolHandler::NORESPONSE;

	// set the variable to the new value
	result = m_setFrequencyResult;

	return QrfeProtocolHandler::OK;

}
#endif
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setFrequency(ulong frequencyKHz, uchar rssi)
{	
	frequencyKHz = 0x00FFFFFF & frequencyKHz;

	// build up the command byte array
	QByteArray buf;
	//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	//buf.resize(64);
	buf.resize(9);
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_CHANGE_FREQ;
	buf [0] = RFID_AntennaPortSetFrequency;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//buf [1] = 0x07;
	buf [1] = 0x09;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	buf [2] = 0x08;
	buf [3] = frequencyKHz & 0x000000FF;
	buf [4] = (frequencyKHz & 0x0000FF00) >> 8;
	buf [5] = (frequencyKHz & 0x00FF0000) >> 16;
	buf [6] = rssi;
	buf [7] = 0;
	buf [8] = 0;    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

	// reset the flag
	m_setFrequencyFLAG = false;

	trcBytes(2, "Sending bytes ", buf);

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool waitOk = waitForFlag(&m_setFrequencyFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!waitOk)
		return QrfeProtocolHandler::NORESPONSE;

	return QrfeProtocolHandler::OK;

}
//End by yingwei tseng for frequency, 2010/01/04
//Add by yingwei tseng for region, 2009/12/03 
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setRegion (QByteArray value)
{	
	if(value.size() != 1)
		return QrfeProtocolHandler::ERROR;

	QByteArray buf;
	buf.resize(2);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_SET_REGION;
	buf [0] = RFID_RadioSetRegion;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	buf [1] = 0x03;
	buf.append(value);

	m_setRegionFLAG = false;
	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	bool result = waitForFlag(&m_setRegionFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	if(m_setRegionError != 0)
		return QrfeProtocolHandler::ERROR;

	trc(9, "Set region to " + QString("0x%1")
			.arg((unsigned char)value.at(0), 2, 16, QChar('0')));
			
	return QrfeProtocolHandler::OK;
}

QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::getRegion ( QByteArray &region, QByteArray &value )
{
	trc(9, "Trying to get region");
	// build up the command byte array
	QByteArray buf;
	buf.resize(2);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_GET_REGION;
	buf [0] = RFID_RadioGetRegion;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	buf [1] = 0x02;

	// reset the flag
	m_getRegionFLAG = false;
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	bool result = waitForFlag(&m_getRegionFLAG, m_MAX_WAIT_TIME_IN_MS);
	region = m_getRegion;
	value=region;
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	if(m_getRegionError != 0)
		return QrfeProtocolHandler::ERROR;
	
	return QrfeProtocolHandler::OK;
}
//End by yingwei tseng for region, 2009/12/03 

/*!
 * @brief 	Implementation of the set frequency command mode 16
 * @param	freqKHz : the frequency to be used for modulation
 * @param	timeout_ms : to limit the time...
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::continuousSend( ulong frequencyKHz, ushort timeout_ms )
{
	frequencyKHz = 0x00FFFFFF & frequencyKHz;
	trc(1, "Trying to send continuous send");
	// build up the command byte array
	QByteArray buf;
	buf.resize(64);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_CHANGE_FREQ;
	buf [0] = RFID_AntennaPortSetFrequency;	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//buf [1] = 7;
	buf [1] = 9;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	buf [2] = 32;
	buf [3] = frequencyKHz & 0x000000FF;
	buf [4] = (frequencyKHz & 0x0000FF00) >> 8;
	buf [5] = (frequencyKHz & 0x00FF0000) >> 16;
	buf [6] = timeout_ms & 0x000000FF;
	buf [7] = (timeout_ms & 0x0000FF00) >> 8;
    buf [8] = 0;    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	
	// reset the flag
	m_setFrequencyFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;


	// wait for either the flag or a timeout
	bool waitOk = waitForFlag(&m_setFrequencyFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!waitOk)
		return QrfeProtocolHandler::NORESPONSE;

	return QrfeProtocolHandler::OK;
}

/*!
 * @brief 	Implementation of the set frequency command mode 16
 * @param	idleTime_ms : the time after sending before rescanning a channel in milliseconds
 * @param	listeningTime_ms : the LBT time in milliseconds
 * @param	allocationTime_ms : the maximal duration of sending on a channel in milliseconds
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::setAllocationParams( ushort listeningTime_ms, ushort allocationTime_ms, ushort idleTime_ms  )
{
	trc(1, "Trying to set frequency params");
	// build up the command byte array
	QByteArray buf;
	buf.resize(64);
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//buf [0] = OUT_CHANGE_FREQ;
	buf [0] = RFID_AntennaPortSetFrequency;
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//buf [1] = 7;
	buf [1] = 9;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	buf [2] = 16;
	buf[3] = listeningTime_ms & 0x000000FF;
	buf[4] = (listeningTime_ms & 0x0000FF00) >> 8;
	buf[5] = allocationTime_ms & 0x000000FF;
	buf[6] = (allocationTime_ms & 0x0000FF00) >> 8;
	buf[7] = idleTime_ms & 0x000000FF;
	buf[8] = (idleTime_ms & 0x0000FF00) >> 8;

	// reset the flag
	m_setFrequencyFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;


	// wait for either the flag or a timeout
	bool waitOk = waitForFlag(&m_setFrequencyFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!waitOk)
		return QrfeProtocolHandler::NORESPONSE;

	return QrfeProtocolHandler::OK;

}

//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
//Del by yingwei tseng for auto-select multi rate in UART mode, 2010/09/08
#if 0
void QrfeProtocolHandlerAMS::setDeviceFlag(int flag)
{
    m_deviceFlag = flag;
}
#endif
//End by yingwei tseng for auto-select multi rate in UART mode, 2010/09/08

unsigned short QrfeProtocolHandlerAMS::crc16(unsigned char *buf, unsigned short bit_length)
{
    unsigned short shift, data, val;
    int i;

	shift = 0xFFFF;

	for ( i = 0; i < bit_length; i++ ){
        if ((i % 8) == 0)
			data = (*buf++) << 8;

		val = shift ^ data;
		shift = shift << 1;
		data = data << 1;

		if (val & 0x8000)
			shift = shift ^ POLY;		
	}	

	return shift;
}
//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

/*!
 * @brief 	Helper function that sends the given bytes to the reader
 * @param	msg		The message that should be sent
 * @return	Returns the result of the operation coded in an enum
 */
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::send2Reader(QByteArray msg)
{
    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
    if (m_deviceFlag == DEV_USB_HID){
		//Mod by yingwei tseng for fixed select/write command length, 2010/12/02
		int len = msg.length();
		QByteArray fZero;
		fZero.resize(1);
		fZero[0] = 0x00;		
		
		switch(msg[0])
		{
            case RFID_18K6CTagSelect:
			case RFID_18K6CTagWrite:				
			case RFID_18K6CTagBlockWrite:    //Add by yingwei tseng for memory bank block write, 2011/03/14  
				for (int i = 0; i < (64 - len); i++)
				{
                    msg.append(fZero);
				}
				break;	
			default:
				msg[1] = msg.length();
				break;
		}
		//End by yingwei tseng for fixed select/write command length, 2010/12/02

		if(m_deviceRemoved)
			return QrfeProtocolHandler::COMMERROR;

		trcBytes(1, "Sending bytes: ", msg);
		if(m_device->write(msg) != msg.size()){
			trc(1, "Could not send bytes...");
			return QrfeProtocolHandler::COMMERROR;
		}
    }else{	
	    QByteArray buf;
		unsigned short crc;

		buf.resize(5);
		buf [0] = 0x4D;
		buf [1] = 0x54;
		buf [2] = 0x49;
		buf [3] = 0x43;
		buf [4] = 0xFF;
		buf.append(msg);
	    buf [6] = msg.length();

	    int len = buf.length();
	    crc = crc16((unsigned char *)buf.data(), (unsigned short)len*8);
		buf [len] = ~crc >> 8;
		buf [len + 1] = ~crc & 0xFF;

		if(m_deviceRemoved)
		    return QrfeProtocolHandler::COMMERROR;

		trcBytes(1, "Sending bytes: ", buf);
		if(m_device->write(buf) != buf.size()){
			trc(1, "Could not send bytes...");
			return QrfeProtocolHandler::COMMERROR;
		}
    }
    //End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04

    //Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
#if 0
	msg[1] = msg.length();

	if(m_deviceRemoved)
		return QrfeProtocolHandler::COMMERROR;

	trcBytes(1, "Sending bytes: ", msg);
	if(m_device->write(msg) != msg.size()){
		trc(1, "Could not send bytes...");
		return QrfeProtocolHandler::COMMERROR;
	}
#endif
    //End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 	
	return QrfeProtocolHandler::OK;
}


/*!
 * @brief 	Call through function to base implementation
 */
void QrfeProtocolHandlerAMS::deviceWasRemoved()
{
	QrfeProtocolHandler::deviceWasRemoved();
}

/*!
 * @brief 	Function that should parse the given data and extract messages out of it. Is not needed here
 * @param	data		The data that should be parsed
 */
int QrfeProtocolHandlerAMS::computeData 	( const QByteArray& data )
{
    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
    if (m_deviceFlag == DEV_USB_HID)
		return computeMessage(data);
	else{
		//Add by yingwei tseng for USB/UART offline problem, 2010/11/11
		unsigned short verifcation = 0;
		
	    trcBytes(1, "Receive bytes(computeData): ", data);

        int len = (unsigned char)data.at(6) + 7;    //header length + data length + checksum length

        if ((data.at(0) == 0x4D) && (len > 0))
	    {			
            if (m_inventoryAction == START_INVENTORY_ROUND)
		        m_totalTags = ((unsigned char)data.at(8) - 1) * len;    //total tags except first tag 
				
			verifcation = crc16((unsigned char *)data.mid(0, len).data(), (unsigned short)len*8);
			if (verifcation == 0x1D0F)
		    {
				if (m_inventoryAction == GET_ALL_TAGS_INFO)
				{
			        int cycle = 0, totalMsg = 0, dataStart = 5, dataLen = 6, verifcationStart = 0;
					cycle = m_totalTags/len;
                    while(cycle)
                    {
                        verifcation = crc16((unsigned char *)data.mid(verifcationStart, len).data(), (unsigned short)len*8);
						if (verifcation == 0x1D0F)						
                            totalMsg += computeMessage(data.mid(dataStart, data.at(dataLen)));
						else						    
                            totalMsg += len;
						
    
						dataStart += len;
						dataLen += len;
                        verifcationStart += len; 
					    cycle--;
                    }

					return totalMsg;
				}else
		            return computeMessage(data.mid(5, data.at(6)));
					
			}else
			    return 0;
                //return data.length();
				
        }else
            return 0;
            //return data.length(); 	
		//End by yingwei tseng for USB/UART offline problem, 2010/11/11	
		//Del by yingwei tseng for USB/UART offline problem, 2010/11/11	
#if 0	
	    unsigned short verifcation;
		
		trcBytes(1, "Receive bytes(computeData): ", data);

	    int len = data.length();
		verifcation = crc16((unsigned char *)data.data(), (unsigned short)len*8);
		if (verifcation == 0x1D0F){
		    return computeMessage(data.mid(5, data.at(6)));
		}
#endif		
        //End by yingwei tseng for USB/UART offline problem, 2010/11/11	
    }
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
    //Del by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
#if 0
	// no checksum or protocol parser so compute the message
	return computeMessage(data);
#endif
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
}

/*!
 * @brief 	Function that parses a given message and sets the appropriate flags
 * @param	message		A message from the reader
 */
int QrfeProtocolHandlerAMS::computeMessage ( const QByteArray& message )
{
	int ret_length = 0;
	// get the command id from the message
	if ( message.size() < 2 ) return 0;
	uchar id = message.at(0);
	trcBytes(1, "Received Message from reader", message);
	// get the payload of the message
	ret_length = (unsigned char) message.at(1);
	if ( message.size() < ret_length ) return 0;
	
	//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	//QByteArray msg = message.mid(2, message.at(1) - 2);
	QByteArray msg = message.mid(3, message.at(1));
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

	trc(9, "Message from reader");
	trcBytes(1, "Extracted the id 0x" + QString::number(id,16) + " and the message", msg);
	switch(id)
	{
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_FIRM_HARDW_ID:
	case RFID_MacGetModuleID_Resp:
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
		trc(9, "-Read Hardware/Firmware ID: " + msg);
		// save the result
		m_hardsoftwareRevision = msg;
		// set the flag
		m_hardsoftwareRevisionFLAG = true;
	    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
        m_getRevisionError = message.at(2);
		m_lastReturnCode = (eAMS_RET_VALUE)m_getRevisionError;
	    //End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 			
		break;

	//Del by yingwei tseng for antenna power, 2010/01/04
	#if 0
	case IN_ANTENNA_POWER:
		m_antennaPowerFLAG = true;
		break;
	#endif	
	//End by yingwei tseng for antenna power, 2010/01/04

	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_INVENTORY:
	//case IN_INVENTORY_RSSI:
	//case IN_INVENTORY_6B_ID:
	case RFID_18K6CTagInventory_Resp:
	case RFID_18K6CTagInventoryRSSI_Resp:
	case RFID_18K6BTagInventory_Resp:	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	{
		// get the count of epcs that are still to fetch
		uchar epcsToDo = msg.at(0);
		trc(1, "EPCs to do:" + QString::number(epcsToDo));
		if(epcsToDo == 0)
			trc(9, "Nothing found");
		else
			trc(9, "-Inventory found " + QString::number(epcsToDo) + " EPCs");
		if(epcsToDo > 0){
			//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
			//if(id == IN_INVENTORY){
			if(id == RFID_18K6CTagInventory_Resp){	
			//End by yingwei tseng for REPORT_ID table, 2010/05/10	
				trc(0, "--Found EPC: " + epcToString(msg.mid(4, msg.at(1)- 2)));
				m_singleInventoryResult << epcToString(msg.mid(4, msg.at(1)- 2));
			}
			//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
			//else if(id == IN_INVENTORY_RSSI){
			else if(id == RFID_18K6CTagInventoryRSSI_Resp){	
			//End by yingwei tseng for REPORT_ID table, 2010/05/10
				trc(1, 
					QString("--Found EPC: " + epcToString(msg.mid(8, msg.at(5)- 2)) + 
					QString(48-3*msg.at(5),QChar(' ')) +
					" freq=%1 kHz, RSSI %2 ").arg( (unsigned)(((unsigned char)msg.at(2))
					                              |(((unsigned char)msg.at(3))<<8)
												  |(((unsigned char)msg.at(4))<<16)))
					                         .arg((unsigned char)msg.at(1),0,16)
					);
				m_singleInventoryResult << epcToString(msg.mid(8, msg.at(5)- 2));
				m_singleInventoryRSSI.insert(epcToString(msg.mid(8, msg.at(5)- 2)), QByteArray().append(msg.at(1)));
			}
			//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
			//else if(id == IN_INVENTORY_6B_ID){
			else if(id == RFID_18K6BTagInventory_Resp){	
			//End by yingwei tseng for REPORT_ID table, 2010/05/10	
				trc(0, "--Found 6B ID: " + epcToString(msg.mid(3, msg.at(2))));
				m_singleInventoryResult << epcToString(msg.mid(3, msg.at(2)));
				m_singleInventoryRSSI.insert(epcToString(msg.mid(3, msg.at(2))), QByteArray().append(msg.at(1)));
			}
		}

		// save in member if there are still epcs to fetch
		if(epcsToDo > 1)
			m_singleInventoryEpcsToRead = true;
		else
			m_singleInventoryEpcsToRead = false;
  
		// set the flag
		m_singleInventoryFLAG = true;

	    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
        m_doGen2SingleInventoryError = message.at(2);
		m_lastReturnCode = (eAMS_RET_VALUE)m_doGen2SingleInventoryError;
	    //End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 			

		break;
	}

	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_WRITE_TO_TAG:
	case RFID_18K6CTagWrite_Resp:	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	case RFID_18K6CTagBlockWrite_Resp:    //Add by yingwei tseng for memory bank block write, 2011/03/14 
	{
	    //Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		//m_writeToTagError = msg.at(0);
		m_writeToTagError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		//Mod by yingwei tseng for handle error message, 2010/09/16
		//m_writtenWords = msg.at(1);		
		m_writtenWords = msg.at(0);	
		//End by yingwei tseng for handle error message, 2010/09/16
		m_lastReturnCode = (eAMS_RET_VALUE)m_writeToTagError;
		m_writeTagIDFLAG = true;

		trc(1, "Received result for write to tag: " + QString::number(m_writeToTagError));
		break;
	}

	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_READ_FROM_TAG:
	case RFID_18K6CTagRead_Resp:	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	{
		m_readFromTagFLAG = true;
		//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		//m_readFromTagError = msg.at(0);
		m_readFromTagError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		m_lastReturnCode = (eAMS_RET_VALUE)m_readFromTagError;
		//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		//m_readFromTagResult = msg.mid(2, msg.at(1)*2);
		m_readFromTagResult = msg.mid(1, msg.at(0)*2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

		trc(9, "Received error of read from tag: " + QString::number(m_readFromTagError));
		trcBytes(9, "Received result of read from tag: ", m_readFromTagResult);
		break;
	}

    //Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_WRITE_REG:
	case RFID_MacBypassWriteRegister_Resp:
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	{
	    //Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		//m_setParamError = msg.at(0);
		m_setParamError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		m_lastReturnCode = (eAMS_RET_VALUE)m_setParamError;
		m_setParamFLAG = true;

		trc(9, "Received result for write register: " + QString::number(m_writeToTagError));
		break;
	}

    //Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_READ_REG:
	case RFID_MacBypassReadRegister_Resp:
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	{
		m_getParamData = msg.left(3);
		m_getParamFLAG = true;

	    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
        m_getParamError = message.at(2);
        m_lastReturnCode = (eAMS_RET_VALUE)m_getParamError;
	    //End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 		

		trc(9, "Received result for read register: " + m_getParamData.toHex());
		break;
	}
	//Add by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12
	case RFID_MacWriteOemData_Resp:
	{   
		//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		//m_setMacFirmwareOemCfgError = msg.at(0);
		m_setMacFirmwareOemCfgError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		m_lastReturnCode = (eAMS_RET_VALUE)m_setParamError;
		m_setMacFirmwareOemCfgFLAG = true;

		trc(9, "Received result for write MAC firmware OEM configuration data area : " + QString::number(m_setMacFirmwareOemCfgError));
		break;
		
	}	
	case RFID_MacReadOemData_Resp:
	{
		m_getMacFirmwareOemCfgData = msg.left(4);
		m_getMacFirmwareOemCfgFLAG = true;

		trc(0, "Received result for read MAC firmware OEM configuration data area : " + m_getMacFirmwareOemCfgData.toHex());
		break;
	}
	//End by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/07/12
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_GEN2_CONFIG:
	case RFID_18K6CSetQueryParameter_Resp:	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	{
		m_gen2ConfigData = msg;
		m_gen2ConfigFLAG = true;

	    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
        m_setGen2ConfigError = message.at(2);
		m_lastReturnCode = (eAMS_RET_VALUE)m_setGen2ConfigError;
	    //End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 		

		trc(9, "Received result gen2 config: " + m_gen2ConfigData.toHex());
		break;
	}

	case IN_REGS_COMPLETE:
	{
		m_getParamData = msg;
		m_getParamFLAG = true;

		trc(9, "Received result for read all registers: " + m_getParamData.toHex());
		break;
	}

	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_CHANGE_FREQ:
	case RFID_AntennaPortSetFrequency_Resp:	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	{
		m_setFrequencyFLAG = true;
		m_setFrequencyResult = msg;

		trcBytes(9, "Received result for set frequency: ", m_setFrequencyResult);
		break;
	}

    //Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_LOCK_UNLOCK:
	case RFID_18K6CTagLock_Resp:	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	{
		m_lockTagFLAG = true;
		//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		//m_lockTagError = msg.at(0);
		m_lockTagError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		m_lastReturnCode = (eAMS_RET_VALUE)m_lockTagError;

		trc(9, "Received result for lock tag: " + QString::number(m_lockTagError));
		break;
	}

	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_KILL_TAG:
	case RFID_18K6CTagKill_Resp:	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	{
		m_killTagFLAG = true;
		//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		//m_killTagError = msg.at(0);
		m_killTagError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		m_lastReturnCode = (eAMS_RET_VALUE)m_killTagError;

		trc(1, "Received result for kill tag: " + QString::number(m_lockTagError));
		break;
	}

	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_SELCET_TAG:
	case RFID_18K6CTagSelect_Resp:	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	{
		m_selectTagFLAG = true;
		//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		//m_selectTagError = msg.at(0);
		m_selectTagError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		m_lastReturnCode = (eAMS_RET_VALUE)m_selectTagError;

		trc(9, "Received result for select tag: " + QString::number(m_selectTagError));
		break;
	}

    //Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_NXP_COMMAND:
	case RFID_18K6CTagNXPCommand_Resp:
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	{
		m_nxpCommandFLAG = true;
		//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		//m_nxpCommandError = msg.at(0);
		m_nxpCommandError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		m_lastReturnCode = (eAMS_RET_VALUE)m_nxpCommandError;

		trc(9, "Received result for select tag: " + QString::number(m_selectTagError));
		break;
	}
	//Add by yingwei tseng for region, 2009/12/03 
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_SET_REGION:
	case RFID_RadioSetRegion_Resp:	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
	{
	    //Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
        //m_setRegionError = msg.at(0);
		m_setRegionError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		m_lastReturnCode = (eAMS_RET_VALUE)m_setRegionError;
		m_setRegionFLAG = true;
		break;
	}
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10
	//case IN_GET_REGION:
	case RFID_RadioGetRegion_Resp:	
	//End by yingwei tseng for REPORT_ID table, 2010/05/10
	{		
		m_getRegionFLAG = true;	
		//Mod by yingwei tseng for adding 869.85(EU) from frequency bug in REG_EU_300220, 2010/08/18
		//m_getRegion = msg.mid(1);
		m_getRegion = msg.mid(0);
		//End by yingwei tseng for adding 869.85(EU) from frequency bug in REG_EU_300220, 2010/08/18
        //Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		//m_getRegionError = msg.at(0);
		m_getRegionError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		break;
	}		
	//End by yingwei tseng for region, 2009/12/03 
	//Add by yingwei tseng for antenna power, 2010/01/04
	#if 0
	case IN_SET_ANTENNA_POWER:
		m_setPowerFlag = true;
		break;
	case IN_CTRL_ANTENNA_PORT:
		m_antennaPowerCtrlPortFLAG = true;
		break;			
	#endif
	case RFID_AntennaPortSetOperation_Resp:
		m_AntennaPortSetState = true;
		break;
	//Add by yingwei tseng for modulation tseting, 2010/05/10	
	case RFID_AntennaPortTransmitPattern_Resp:
		m_AntennaPortTransmitPatternFLAG = true;
		m_AntennaPortTransmitPatternResult = msg;

		//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	    m_AntennaPortTransmitPattern = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04		

		trcBytes(9, "Received result for set Antenna Port Transmit Pattern: ", m_AntennaPortTransmitPatternResult);	
		break;
	//End by yingwei tseng for modulation tseting, 2010/05/10
	//Add by yingwei tseng for pulse modulation tseting, 2010/10/12
    case RFID_AntennaPortTransmitPulse_Resp:
		m_AntennaPortTransmitPulseFLAG = true;

		m_AntennaPortTransmitPulse = message.at(2);

		trcBytes(9, "Received result for set Antenna Port Transmit Pulse: ", msg);	
		break;
	//End by yingwei tseng for pulse modulation tseting, 2010/10/12	
	case RFID_AntennaPortSetPowerLevel_Resp:
		m_AntennaPortSetPowerLevel = true;
		//Mod by yingwei tseng for get power level, 2010/08/02
		//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	    //m_setPowerLevelError = msg.at(0);
	    m_setPowerLevelError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		m_lastReturnCode = (eAMS_RET_VALUE)m_setPowerLevelError;	
		//End by yingwei tseng for get power level, 2010/08/02
		break;				
    //Mod by yingwei tseng for get power level, 2010/08/02
	case RFID_AntennaPortGetPowerLevel_Resp:
		m_AntennaPortGetPowerLevel = true;	
		//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	    //m_getPowerLevelError = msg.at(0);
	    m_getPowerLevelError = message.at(2);
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
		m_lastReturnCode = (eAMS_RET_VALUE)m_getPowerLevelError;
		m_getPowerLevel = (int)msg.at(0);		
		break;
	//End by yingwei tseng for get power level, 2010/08/02	
	//Mod by yingwei tseng for REPORT_ID table, 2010/05/10	
	//case RFID_AntennaPortSetPowerStateResp:
	case RFID_AntennaPortSetPowerState_Resp:
	//End by yingwei tseng for REPORT_ID table, 2010/05/10	
		m_AntennaPortSetPowerState = true;

	    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
        m_AntennaPortSetPowerStateError = message.at(2);
		m_lastReturnCode = (eAMS_RET_VALUE)m_setRegionError;
	    //End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		break;	
	//End by yingwei tseng for antenna power, 2010/01/04	
	//Add by yingwei tseng for NXP alarm message, 2010/03/15
	case RFID_18K6CTagNxpEasAlarmCommand_Resp:
	{
		m_nxpEasAlarmCommandFLAG = true;
		m_nxpEasAlarmCommandError = message.at(2);
		m_nxpEasAlarmCommandData = message.mid(3, 8);
		m_lastReturnCode = (eAMS_RET_VALUE)m_nxpEasAlarmCommandError;

		trc(9, "Received result for NXP EAS alarm: " + QString::number(m_nxpEasAlarmCommandError));
		break;
	}	
	//End by yingwei tseng for NXP alarm message, 2010/03/15
	//Add by rick for softReset, 2014-04-03
	case RFID_MacSoftReset_Resp:
		{
			m_softwareResetFLAG = true;
			break;
		}
	//End by rick for softReset, 2014-04-03
	default:
		trc(9, "--Do not know what the identifier " + QString::number(id) + " means.");
		break;
	}
	
	//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	if (m_deviceFlag == DEV_USB_HID)
	    return ret_length;
	else	
	    return (ret_length + 7);
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
}

//rick¡õ
QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::getOEMCfgMVN ( QString &softwareRevision )
{
	//trc(9, "Trying to get hardware ID");
	// build up the command byte array
	QByteArray buf;
	buf.resize(3);
	buf [0] = RFID_MacGetModuleID;
	buf [2] = 2;

	// reset the flag
	m_hardsoftwareRevisionFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_hardsoftwareRevisionFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	softwareRevision = m_hardsoftwareRevision;

	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	if(m_getRevisionError != 0)
		return QrfeProtocolHandler::ERROR;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 	

	return QrfeProtocolHandler::OK;
}

QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::getOEMCfgUPIC ( QString &softwareRevision )
{
	QByteArray buf;
	buf.resize(3);
	buf [0] = RFID_MacGetModuleID;
	buf [2] = 3;

	// reset the flag
	m_hardsoftwareRevisionFLAG = false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	// wait for either the flag or a timeout
	bool result = waitForFlag(&m_hardsoftwareRevisionFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	softwareRevision = m_hardsoftwareRevision;

	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	if(m_getRevisionError != 0)
		return QrfeProtocolHandler::ERROR;
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 	

	return QrfeProtocolHandler::OK;
}

QrfeProtocolHandler::Result QrfeProtocolHandlerAMS::softReset()
{
	trc(9, "Trying to SoftwareReset");
	// build up the command byte array
	QByteArray buf;
	buf.resize(2);
	buf [0] = RFID_MacSoftReset ;
	buf [1] = 0x02;

	// reset the flag
	m_softwareResetFLAG=false;

	// send the command
	Result res = send2Reader(buf);
	if(res != QrfeProtocolHandler::OK)
		return res;

	bool result = waitForFlag(&m_softwareResetFLAG, m_MAX_WAIT_TIME_IN_MS);
	if(!result)
		return QrfeProtocolHandler::NORESPONSE;

	return QrfeProtocolHandler::OK;
}

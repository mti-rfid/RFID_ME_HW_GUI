/*
*****************************************************************************
* Copyright @ 2009 by austriamicrosystems AG                                *
* All rights are reserved.                                                  *
*                                                                           *
* Reproduction in whole or in part is prohibited without the written consent*
* of the copyright owner. Austriamicrosystems reserves the right to make    *
* changes without notice at any time. The software is provided as is and    *
* Austriamicrosystems makes no warranty, expressed, implied or statutory,   *
* including but not limited to any implied warranty of merchantability or   *
* fitness for any particular purpose, or that the use will not infringe any *
* third party patent, copyright or trademark. Austriamicrosystems should    *
* not be liable for any loss or damage arising from its use.                *
*****************************************************************************
*/

/*
 *      PROJECT:   AMSCommunication
 *      $Revision: 1.1 $
 *      LANGUAGE: QT C++
 */

/*! \file
 *
 *  \author S. Puri
 *
 *  \brief  Communication class for communication via USB HID
 *
 *  This controls the communication to the SiLabs C8051F34x.
 *  Actual USB access is handled via SLABHIDDevice.dll.
 *  Human Interface Device (HID) Tutorials (AN249).
 */

#ifndef USB_HID_WRAPPER_H
#define USB_HID_WRAPPER_H

#include <AMSCommunication.hxx>
#include "reader\ams\QrfeAmsReader.h"


class USBHIDWrapper : public AMSCommunication
{
	Q_OBJECT
public:
	USBHIDWrapper(unsigned char devAddr);
	~USBHIDWrapper();

	AMSCommunication::Error hwConnect();
	void hwDisconnect();
	AMSCommunication::Error hwReadRegister(unsigned char reg, unsigned char *val);
	AMSCommunication::Error hwWriteRegister(unsigned char reg, unsigned char val);
	AMSCommunication::Error hwSendCommand(QString command, QString * answer);
	AMSCommunication::Error writeSubRegister(unsigned char reg, unsigned char val, unsigned char subAddress, bool verify=false, bool doemit=false);
	AMSCommunication::Error readSubRegister(unsigned char reg, unsigned char *val, unsigned char subAddress, bool doemit=false);

public slots:
	void gotReader(QrfeReaderInterface* ph);
	void lostReader(QrfeReaderInterface* ph);

protected:
	void setConnectionProperties(void *);

private:
	void update(void);
	AMSCommunication::Error hwReadMultiByteRegister(unsigned char, QByteArray*, unsigned short);

	unsigned char devAddr;

	QTime lastUpdate;
	QByteArray valArray;
	int inputReportBufferLength;
	int outputReportBufferLength;
	int featureReportBufferLength;
	int maxReportRequest;

	QrfeAmsReader* ph;
};

#endif // USB_HID_WRAPPER_H

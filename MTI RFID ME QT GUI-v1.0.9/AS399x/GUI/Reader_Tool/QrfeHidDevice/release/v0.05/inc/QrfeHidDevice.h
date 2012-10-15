/*
 * QrfeHidDevice.h
 *
 *  Created on: 30.10.2008
 *      Author: stefan.detter
 */

#ifndef QRFEHIDDEVICE_H_
#define QRFEHIDDEVICE_H_

/*POSIX CODE*/
#ifdef _HID_POSIX_
#include "Unix_QrfeHidDevice.h"
#define QrfeHidBaseType Unix_QrfeHidDevice

/*MS WINDOWS CODE*/
#else
#include "Win_QrfeHidDevice.h"
#define QrfeHidBaseType Win_QrfeHidDevice
#endif


class QrfeHidDevice : public QrfeHidBaseType
{
public:
	QrfeHidDevice(QObject* parent = 0);
	QrfeHidDevice(QString devicePath, QObject* parent = 0);
	virtual ~QrfeHidDevice();
};

#endif /* QRFEHIDDEVICE_H_ */

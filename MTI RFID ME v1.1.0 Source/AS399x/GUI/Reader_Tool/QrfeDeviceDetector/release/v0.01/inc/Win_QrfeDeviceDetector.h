/*
 * Win_QrfeDeviceDetector.h
 *
 *  Created on: 19.12.2008
 *      Author: stefan.detter
 */

#ifndef WIN_QRFEDEVICEDETECTOR_H_
#define WIN_QRFEDEVICEDETECTOR_H_

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>

#include <windows.h>
extern "C"
{
#include <hidsdi.h>
}
#include <setupapi.h>
#include <stdlib.h>
#include <usbiodef.h>

#ifdef QrfeDEVICEDETECTOR_DEBUG
#include <QrfeTrace.h>
#endif

#include "QrfeDeviceDetectorBase.h"
#include "Win_QrfeDeviceChangeDetector.h"

class Win_QrfeDeviceDetector: public QrfeDeviceDetectorBase
#ifdef QrfeDEVICEDETECTOR_DEBUG
, QrfeTraceModule
#endif
{
Q_OBJECT

public:
	Win_QrfeDeviceDetector(QObject* parent = 0)	;
	virtual~ Win_QrfeDeviceDetector();

	virtual bool registerForUSBDevice(quint16 vendorID, quint16 productID);
	virtual bool registerForHIDDevice(quint16 vendorID, quint16 productID);

	virtual QStringList getConnectedUSBDevicePaths(quint16 vid, quint16 pid);
	virtual QStringList getConnectedHIDDevicePaths(quint16 vid, quint16 pid);

private slots:
	void win_usbDeviceAttached (QString devicePath, quint16 vendorID, quint16 productID);
	void win_usbDeviceRemoved (QString devicePath, quint16 vendorID, quint16 productID);

	void win_hidDeviceAttached (QString devicePath, quint16 vendorID, quint16 productID);
	void win_hidDeviceRemoved (QString devicePath, quint16 vendorID, quint16 productID);

private:
	Win_QrfeDeviceChangeDetector m_detector;
};

#endif /* WIN_QRFEDEVICEDETECTOR_H_ */

/*
 * Win_QrfeDeviceChangeDetector.h
 *
 *  Created on: 25.11.2008
 *      Author: stefan.detter
 */

#ifndef WIN_QRFEDEVICECHANGEDETECTOR_H_
#define WIN_QRFEDEVICECHANGEDETECTOR_H_

#include <QObject>
#include <QAbstractEventDispatcher>
#include <QTimer>
#include <QMainWindow>

#include <windows.h>
extern "C"
{
#include <hidsdi.h>
}

#ifdef QrfeDEVICEDETECTOR_DEBUG
#include <QrfeTrace.h>
#endif

class Win_QrfeDeviceChangeDetector: public QWidget
#ifdef QrfeDEVICEDETECTOR_DEBUG
, QrfeTraceModule
#endif
{
Q_OBJECT
public:
	Win_QrfeDeviceChangeDetector ()	;
~	Win_QrfeDeviceChangeDetector ( );

	virtual bool winEvent ( MSG * msg, long * result );

	bool registerNotification();
	bool unregisterNotification();

	signals:
	void usbDeviceAttached(QString devicePath, quint16 vendorID, quint16 productID);
	void usbDeviceRemoved(QString devicePath, quint16 vendorID, quint16 productID);

	void hidDeviceAttached(QString devicePath, quint16 vendorID, quint16 productID);
	void hidDeviceRemoved(QString devicePath, quint16 vendorID, quint16 productID);

private:

	HANDLE m_notifyDeviceInterface_USB;
	HANDLE m_notifyDeviceInterface_HID;
};

#endif /* WIN_QRFEDEVICECHANGEDETECTOR_H_ */

/*
 * QrfeDeviceDetectorBase.h
 *
 *  Created on: 19.12.2008
 *      Author: stefan.detter
 */

#ifndef QRFEDEVICEDETECTORBASE_H_
#define QRFEDEVICEDETECTORBASE_H_

#include <QObject>
#include <QString>
#include <QMap>

#define MAX_USB_DEVICES 64

class QrfeDeviceDetectorBase: public QObject
{
Q_OBJECT
public:
	QrfeDeviceDetectorBase(QObject* parent = 0)	;
	virtual~ QrfeDeviceDetectorBase();

	virtual bool registerForUSBDevice(quint16 vendorID, quint16 productID) = 0;
	virtual bool registerForHIDDevice(quint16 vendorID, quint16 productID) = 0;

	virtual QStringList getConnectedUSBDevicePaths(quint16 vid, quint16 pid) = 0;
	virtual QStringList getConnectedHIDDevicePaths(quint16 vid, quint16 pid) = 0;

	signals:
	void hidDeviceAttached ( QString devicePath, quint16 vendorID, quint16 productID );
	void hidDeviceRemoved ( QString devicePath, quint16 vendorID, quint16 productID );

	void usbDeviceAttached ( QString devicePath, quint16 vendorID, quint16 productID );
	void usbDeviceRemoved ( QString devicePath, quint16 vendorID, quint16 productID );

protected:
	QMap<quint16, quint16> m_usbDevices;
	QMap<quint16, quint16> m_hidDevices;

};

#endif /* QRFEDEVICEDETECTORBASE_H_ */

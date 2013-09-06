/*
 * QrfeHidBase.h
 *
 *  Created on: 30.10.2008
 *      Author: stefan.detter
 */

#ifndef QRFEHIDBASE_H_
#define QRFEHIDBASE_H_

#include <QIODevice>

enum HidReturnCode{
	HID_DEVICE_SUCCESS,
	HID_DEVICE_NO_DEVICE,
	HID_DEVICE_NOT_FOUND,
	HID_DEVICE_NOT_OPENED,
	HID_DEVICE_ALREADY_OPENED,
	HID_DEVICE_TRANSFER_TIMEOUT,
	HID_DEVICE_TRANSFER_FAILED,
	HID_DEVICE_CANNOT_GET_HID_INFO,
	HID_DEVICE_HANDLE_ERROR,
	HID_DEVICE_INVALID_BUFFER_SIZE,
	HID_DEVICE_SYSTEM_CODE,
	HID_DEVICE_UNKNOWN_ERROR
};

class QrfeHidBase  : public QIODevice
{
	Q_OBJECT

public:
	QrfeHidBase(QObject* parent = 0);
	QrfeHidBase(QString devicePath, QObject* parent = 0);
	virtual ~QrfeHidBase();

	QString 	devicePath() const;
	void 		setDevicePath(QString devicePath);

	static quint8 lastError();
	QString errorString () const;

	virtual void timeouts(quint32 &getReportTimeout, quint32 &setReportTimeout) = 0;
	virtual void setTimeouts(quint32 getReportTimeout, quint32 setReportTimeout) = 0;

	virtual bool open ( OpenMode mode = QIODevice::ReadWrite ) = 0;
	virtual bool open ( QString devicePath, OpenMode mode = QIODevice::ReadWrite ) = 0;
	virtual void close () = 0;

	virtual bool isOpen() = 0;

	virtual qint64 bytesAvailable () const = 0;
	virtual qint64 bytesToWrite () const = 0;
	virtual bool canReadLine () const = 0;
	virtual bool isSequential () const = 0;

	virtual bool flush() = 0;

protected:
	virtual qint64 readData ( char * data, qint64 maxSize ) = 0;
	virtual qint64 writeData ( const char * data, qint64 maxSize ) = 0;

	virtual void resetDeviceData();

protected:
	static HidReturnCode  m_lastError;

	QString m_devicePath;

	quint32 m_getReportTimeout;
	quint32 m_setReportTimeout;

	bool 	m_deviceOpened;

	QList<QByteArray> m_readBuffer;
};

#endif /* QRFEHIDBASE_H_ */

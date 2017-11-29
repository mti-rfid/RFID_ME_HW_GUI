/*
 * win_QrfeHidPort.h
 *
 *  Created on: 30.10.2008
 *      Author: stefan.detter
 */

#ifndef WIN_QRFEHIDPORT_H_
#define WIN_QRFEHIDPORT_H_

#include "QrfeHidBase.h"

#include <windows.h>
extern "C" {
	#include <hidsdi.h>
}
#include <setupapi.h>
#include <stdlib.h>

#include <QMutex>
#include <QStringList>
#include <QThread>
#include <QTime>
#include <QCoreApplication>
#include <QrfeTrace.h>


// Max number of USB Devices allowed
#define MAX_USB_DEVICES					64

// Max number of reports that can be requested at time
#define MAX_REPORT_REQUEST_XP			512
#define MAX_REPORT_REQUEST_2K			200

#define DEFAULT_REPORT_INPUT_BUFFERS	0

#define MAX_SERIAL_STRING_LENGTH		256

class Win_QrfeHidDevice : public QrfeHidBase, QrfeTraceModule
{
	Q_OBJECT

public:
	Win_QrfeHidDevice(QObject* parent = 0);
	Win_QrfeHidDevice(QString devicePath, QObject* parent = 0);
	virtual ~Win_QrfeHidDevice();

	virtual void timeouts(quint32 &getReportTimeout, quint32 &setReportTimeout);
	virtual void setTimeouts(quint32 getReportTimeout, quint32 setReportTimeout);

	QString getSerialString();

	virtual bool open ( OpenMode mode = QIODevice::ReadWrite );
	virtual bool open ( QString devicePath, OpenMode mode = QIODevice::ReadWrite );
	virtual void close ();

	virtual bool isOpen();

	virtual qint64 bytesAvailable () const;
	virtual qint64 bytesToWrite () const;
	virtual bool canReadLine () const;
	virtual bool isSequential () const;

	virtual bool flush();

protected:
	virtual qint64 readData ( char * data, qint64 maxSize );
	virtual qint64 writeData ( const char * data, qint64 maxSize );

	virtual void resetDeviceData();

protected:
	bool setFeatureReport(QByteArray buffer);
	bool getFeatureReport(QByteArray &buffer, quint32 buffSize);
	bool setReportInterrupt(QByteArray buffer);
	bool getReportInterrupt(QByteArray &buffer, quint16 numReports);
	bool setReportControl(QByteArray buffer);
	bool getReportControl(QByteArray &buffer);

	HANDLE openDevice(QString devicePath);

protected:
	virtual void timerEvent ( QTimerEvent * event );

private:
	quint16 m_InputReportBufferLength;
	quint16 m_OutputReportBufferLength;
	quint16 m_FeatureReportBufferLength;
	quint16 m_MaxReportRequest;

	HANDLE m_Handle;
};

#endif /* WIN_QRFEHIDPORT_H_ */

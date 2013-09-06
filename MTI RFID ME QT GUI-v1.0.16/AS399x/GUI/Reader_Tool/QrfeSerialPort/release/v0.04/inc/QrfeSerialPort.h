#ifndef QRFESERIALPORT_H_
#define QRFESERIALPORT_H_

#include "qextserialport.h"

class QrfeSerialPort : public QextSerialPort
{
	Q_OBJECT

public:
	QrfeSerialPort(QObject* parent = 0);
	QrfeSerialPort(const QString & name, QObject* parent = 0);
	QrfeSerialPort(PortSettings const& s, QObject* parent = 0);
	QrfeSerialPort(const QString & name, PortSettings const& s, QObject* parent = 0);
	QrfeSerialPort(const QrfeSerialPort& s);
	virtual ~QrfeSerialPort();

	virtual bool open ( OpenMode mode );
	virtual void close ();

protected:
	virtual void timerEvent ( QTimerEvent * event );
};


#endif /*QRFESERIALPORT_H_*/

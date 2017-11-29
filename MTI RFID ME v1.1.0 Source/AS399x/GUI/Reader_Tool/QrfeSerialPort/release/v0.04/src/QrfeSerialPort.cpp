#include "../inc/QrfeSerialPort.h"

QrfeSerialPort::QrfeSerialPort(QObject* parent)
	:QextSerialPort(parent)
{
}

QrfeSerialPort::QrfeSerialPort(const QString & name, QObject* parent)
	:QextSerialPort(name, parent)
{
}

QrfeSerialPort::QrfeSerialPort(PortSettings const& s, QObject* parent)
	:QextSerialPort(s, parent)
{
}

QrfeSerialPort::QrfeSerialPort(const QString & name, PortSettings const& s, QObject* parent)
	:QextSerialPort(name, s, parent)
{
}

QrfeSerialPort::QrfeSerialPort(const QrfeSerialPort& s)
	:QextSerialPort(s)
{
}

QrfeSerialPort::~QrfeSerialPort()
{
}

bool QrfeSerialPort::open ( OpenMode mode )
{
	if(QextSerialPort::open(mode) == true){
		startTimer(20);
		return true;
	}
	return false;
}


void QrfeSerialPort::close ()
{
	if(QextSerialPort::isOpen() == true)
		QextSerialPort::close();
}

void QrfeSerialPort::timerEvent ( QTimerEvent * /*event*/ )
{
	if(this->isOpen() && this->bytesAvailable() > 0)
	    emit readyRead();
}


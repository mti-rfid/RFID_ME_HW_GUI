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
 * @file	QrfeProtocolHandler.cpp
 * @brief	The base implementation for a protocol handler
 */

#include "QrfeProtocolHandler.h"

#include <QrfeSleeper.h>
#include <QrfeSerialPort.h>
#include <QrfeHidDevice.h>

/*!
 * @brief	Constructor of the base implementation of a protocol handler
 * The protocol handler implements a thread. This thread checks if there is data available on the given device. If there is data available
 * the thread reads the data and calls a virtual function of the implementation that can parse the data.
 * @param	dev			IODevice to communicate with the reader
 * @param 	deviceType	Type of the device
 * @param	parent		Pointer to the parent object
 */
QrfeProtocolHandler::QrfeProtocolHandler(QIODevice* dev, DeviceType deviceType, QObject* parent)
	: QThread(parent)
	, m_device(dev)
	, m_deviceType(deviceType)
{
	m_deviceRemoved = false;

	m_waitingMutex = false;
	m_MAX_WAIT_TIME_IN_MS = 500;

	/* Start this thread */
	this->start();

	/* Move the device to the thread */
	if(this != QThread::currentThread ())
		m_device->moveToThread(this);

}

/*!
 * @brief	Destructor of the base implementation
 */
QrfeProtocolHandler::~QrfeProtocolHandler()
{
	/* Stop thread and wait for finished */
	this->quit();
	while(!isFinished ())
		;
	m_deviceRemoved = true;
}

/*!
 * @brief	Function that returns the device name
 */
QString QrfeProtocolHandler::deviceName()
{
	switch(m_deviceType)
	{
	case DEV_SERIAL:
		return ((QrfeSerialPort*)m_device)->portName();
	case DEV_USB_HID:
		return ((QrfeHidDevice*)m_device)->devicePath();
	default:
		return "Unknown";
	}
}

/*!
 * @brief	Function that overwrites the function of QObject
 * This function overwrites the deleteLater method of QObject, to ensure that there is no one waiting while the object is destroyed.
 */
void QrfeProtocolHandler::deleteLater ()
{
	/* Wait until no one is waiting in this object */
	waitForNoOneWaiting();

	/* Block all signals of this object */
	blockSignals(true);

	/* Delete the object */
	QObject::deleteLater();
}

/*!
 * @brief	Function returns if device is removed.
 */
bool QrfeProtocolHandler::deviceRemoved()
{
	return m_deviceRemoved;
}

/*!
 * @brief	Function is called if device was removed.
 */
void QrfeProtocolHandler::deviceWasRemoved()
{
	// note in a variable that the device was removed
	m_deviceRemoved = true;

	// set the state of the reader
	this->blockSignals(true);
}

/*!
 * @brief	Function is called if the device should be released
 */
void QrfeProtocolHandler::releaseDeviceImmediatly()
{
	this->quit();
	while(!isFinished ())
		;
}

/*!
 * @brief	Function is waits for the given maximum time until there is no one waiting in this object.
 * @param	msecs		Maximum time to wait, if is zero wait is infinite
 * @return	Returns if the operation was successful
 */
bool QrfeProtocolHandler::waitForNoOneWaiting(uint msecs)
{
	if(msecs == 0){
		while(!m_deviceRemoved && m_waitingMutex == true){
			QrfeSleeper::MSleepAlive(100);
		}
	}
	else
	{
		uint cycles = msecs / 10;
		uint i = 0;
		// wait until there is no function waiting for a flag
		while(!m_deviceRemoved && i++ < cycles && m_waitingMutex == true){
			QrfeSleeper::MSleepAlive(5);
		}
	}

	if(m_waitingMutex)
		return false;

	m_waitingMutex = false;
	return true;
}

/*!
 * @brief 	Function is called if there is data available at the io device
 * If data is available the functions reads the data and calls the computeData function of the protocol handler implementation.
 */
//Add by yingwei tseng for USB/UART offline problem, 2010/11/11 
void QrfeProtocolHandler::readFromDevice ( )
{
	/* read from device into the buffer */
	QByteArray buff;
	int consumed;

	do{
		if (m_deviceType == DEV_SERIAL)    //Serial Port
		{
			/*GET_ALL_TAGS_INFO = 0x03 for UART get all inventory*/
            if (m_inventoryAction == 0x03)
            {
                if(m_device->bytesAvailable() != m_totalTags)
                    break;
            }
           
			buff.append(m_device->read(m_device->bytesAvailable()));
		    consumed = computeData(buff);
		    buff.remove(0,consumed);
			if ( buff.size() ) msleep(1); /* Give time for upcoming data */
		}else{    //USB HID
		    /* let the specific protocolhandler compute the data */
		    buff.append(m_device->read(m_device->bytesAvailable()));			
		    consumed = computeData(buff);
		    buff.remove(0,consumed);
		    if ( buff.size() ) msleep(1); /* Give time for upcoming data */
		}		
	} while( m_device->bytesAvailable() || consumed);
}
//End by yingwei tseng for USB/UART offline problem, 2010/11/11	
 
#if 0 
void QrfeProtocolHandler::readFromDevice ( )
{
	/* read from device into the buffer */
	QByteArray buff;
	int consumed;

	do{
		/* let the specific protocolhandler compute the data */
		buff.append(m_device->read(m_device->bytesAvailable()));
		consumed = computeData(buff);
		buff.remove(0,consumed);
		if ( buff.size() ) msleep(1); /* Give time for upcoming data */
	} while( m_device->bytesAvailable() || consumed);
}
#endif

/*!
 * @brief	Function to wait for a flag
 * Function waits until the flag is set to true for a maximal time.
 * @param	flag	Pointer to the flag for which should be waited
 * @param	msecs	Count of msecs to wait maximal
 * @return	Returns if the flag was set to true.
 */
bool QrfeProtocolHandler::waitForFlag( bool* flag, uint msecs )
{
	uint i = 0;
	uint cycles = msecs/1;

	// lock the mutex, so that everyone knows that we are now waiting
	m_waitingMutex = true;

	// check for the flag for the given time
	while(i++ < cycles && !(*flag))
		QrfeSleeper::MSleepAlive(1, QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);

	// unlock mutex to signal, that we are finished with waiting
	m_waitingMutex = false;

	// if device was removed, there can be no answer
	if(m_deviceRemoved)
		return false;

	// if the flag was not set return the result
	if(!(*flag))
		return false;

	return true;
}

/*!
 * @brief 	Main function of the thread. Just connects to the given device and the starts event loop.
 */
void QrfeProtocolHandler::run()
{
	// connect to the ready read signal of the device
	QObject::connect(m_device, SIGNAL(readyRead()), this, SLOT(readFromDevice())/*, Qt::DirectConnection*/);
	this->setPriority(QThread::TimeCriticalPriority	);

	exec();

	if(!m_deviceRemoved || m_device->isOpen())
		m_device->close();

	delete m_device;
}

/*!
 * @brief	Simple helper function to transform epc byte array to a string
 */
QString QrfeProtocolHandler::epcToString ( const QByteArray epc )
{
	QString epcString;
	for(int i = 0; i < epc.size(); i++)
	{
		if(i != 0)
			epcString += "-";
		epcString += QString("%1").arg((unsigned char)epc.at(i), 2, 16, QChar('0'));
	}
	return epcString;
}

/*!
 * @brief	Simple helper function to transform a string to epc byte array
 */
QByteArray QrfeProtocolHandler::stringToEpc ( const QString epcString, bool * ok )
{
	QByteArray epc;
	QStringList bytes = epcString.split(QChar('-'));
	bool local_ok = false;
	if(!ok)
		ok = &local_ok;
	for(int i = 0; i < bytes.size(); i++)
	{
		(*ok) = false;
		epc.append((uchar) bytes.at(i).toUShort(ok, 16 ) );
		if(!(*ok))
			return QByteArray();
	}
	return epc;
}

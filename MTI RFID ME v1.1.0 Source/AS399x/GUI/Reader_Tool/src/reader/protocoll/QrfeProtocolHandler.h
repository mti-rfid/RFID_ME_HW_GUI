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
 */
/*
 * QrfeProtocolHandler.h
 *
 *  Created on: 14.05.2009
 *      Author: stefan.detter
 */

#ifndef QRFEPROTOCOLHANDLER_H_
#define QRFEPROTOCOLHANDLER_H_

#include <QObject>
#include <QIODevice>
#include <QStringList>
#include <QThread>

class QrfeProtocolHandler : public QThread
{
	Q_OBJECT
	Q_ENUMS ( Result DeviceType )

public:

	typedef enum {
		OK,
		ERROR,
		COMMERROR,
		NORESPONSE,
		NA
	} Result;

	typedef enum {
		DEV_USB_HID,
		DEV_SERIAL,
		DEV_TCP
	} DeviceType;

	QrfeProtocolHandler(QIODevice* dev, DeviceType deviceType, QObject* parent = 0);
	virtual ~QrfeProtocolHandler();

	QString deviceName();


public:
	bool deviceRemoved();

public slots:
	virtual void deleteLater();
	virtual void deviceWasRemoved();

	void releaseDeviceImmediatly();


protected:
	virtual void run ();
	virtual int computeData 	( const QByteArray &data ) = 0;
	virtual int computeMessage ( const QByteArray &msg ) = 0;

	bool waitForNoOneWaiting(uint msecs = 0);

signals:
	void heartBeat();

	void cyclicInventory(QString epc);

	void cyclicRSSIInformation(QString epc, QByteArray rssi);


protected slots:
	void readFromDevice();

public:
	static QString 		epcToString  ( const QByteArray epc );
	static QByteArray 	stringToEpc  ( const QString epc, bool * ok = 0  );

protected:
	bool waitForFlag( bool* flag, uint msecs );

	QIODevice* 		m_device;
	DeviceType 		m_deviceType;

	bool			m_waitingMutex;

	uint 			m_MAX_WAIT_TIME_IN_MS;

	bool			m_deviceRemoved;

	bool            m_deviceFlag;    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	//Add by yingwei tseng for USB/UART offline problem, 2010/11/11 
	int             m_inventoryAction; 
	int             m_totalTags;
    QByteArray      tempBuf;		
	//End by yingwei tseng for USB/UART offline problem, 2010/11/11		
};

#endif /* QRFEPROTOCOLHANDLER_H_ */

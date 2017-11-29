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
 * CAmsReaderManager.h
 *
 *  Created on: 27.01.2009
 *      Author: stefan.detter
 */

#ifndef CAMSREADERMANAGER_H_
#define CAMSREADERMANAGER_H_

#include <QObject>
#include <QMap>

#define AMS_OLD_VID 0x10c4
#define AMS_OLD_PID 0x82cd

#define AMS_VID 0x1325
#define AMS_PID 0xC029

#define MTI_VID 0x24E9
#define MTI_PID	0x3993

#define MTI_EightSixFive_VID 0x24E9
#define MTI_EightSixFive_PID 0x0865

#include "reader/QrfeReaderInterface.h"
#include <QrfeDeviceDetector.h>
#include <QrfeHidDevice.h>
#include <QrfeTrace.h>
#include <QrfeSerialPort.h>
#include <QTcpSocket>

class CReaderManager : public QObject
	, QrfeTraceModule
{
	Q_OBJECT
public:
	CReaderManager(QObject* parent = 0);
	virtual ~CReaderManager();

	typedef enum{
		OK,
		ERROR_PORT,
		ERROR_PROTOCOLL,
		ERROR_DOUBLE_ID
	} Result;

public:
	void searchForReader();
	void serialReaderAttached ( QString devicePath, QString &msg );

	void cleanUp();

private slots:

	void readerAttached ( QString devicePath, quint16 vendorID, quint16 productID );
	void readerRemoved ( QString devicePath, quint16 vendorID, quint16 productID );

	void protocollHandlerLostConnection();

signals:
	void gotReader(QrfeReaderInterface* ph);
	void lostReader(QrfeReaderInterface* ph);

private:
	QrfeDeviceDetector m_deviceDetector;

	QMap<QString, QIODevice*>				m_devices;
	QMap<QIODevice*, QrfeReaderInterface*>	m_reader;

	QMap<QIODevice*, QrfeReaderInterface*>	m_toDelete;
};

#endif /* CAMSREADERMANAGER_H_ */

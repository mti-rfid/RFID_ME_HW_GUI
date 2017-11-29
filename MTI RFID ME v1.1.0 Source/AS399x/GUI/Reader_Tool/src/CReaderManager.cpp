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
/*!
 * @file	CReaderManager.cpp
 * @brief	Class that controls the reader and detects new plugged in reader.
 */

#include "CReaderManager.h"

#include "reader/QrfeReaderFactory.h"
#include "QrfeSerialPort.h"

#include <QHostAddress>

/*!
 * @brief	Constructor of the reader manager
 * The constructor of the reader manager registers for the specified VIDs and PIDs and connects its slots to the device detector.
 */
CReaderManager::CReaderManager(QObject* parent)
	: QObject(parent)
	, QrfeTraceModule("CReaderManager")
{
	m_deviceDetector.registerForHIDDevice(AMS_VID, AMS_PID);
	m_deviceDetector.registerForHIDDevice(MTI_VID, MTI_PID);
	m_deviceDetector.registerForHIDDevice(AMS_OLD_VID, AMS_OLD_PID);
	m_deviceDetector.registerForHIDDevice(MTI_EightSixFive_VID, MTI_EightSixFive_PID);//add by rick for RU-865 common sample
	QObject::connect(&m_deviceDetector, SIGNAL(hidDeviceAttached ( QString, quint16, quint16 )), this, SLOT(readerAttached ( QString, quint16, quint16 )));
	QObject::connect(&m_deviceDetector, SIGNAL(hidDeviceRemoved ( QString, quint16, quint16 )), this, SLOT(readerRemoved ( QString, quint16, quint16 )));
}

/*!
 * @brief 	Destructor of the reader manager
 * Deletes all reader objects that the reader manager manages.
 */
CReaderManager::~CReaderManager()
{
	foreach(QIODevice* dev, m_reader.keys())
	{
		m_reader.value(dev)->deleteLater();
	}
}

/*!
 * @brief 	Member function that searches for plugged in reader
 * This function searches for plugged in devices. If a device is found the function readerAttatched is called.
 * This function should only be called on start up. All devices that are plugged in while the application runs are detected by the
 * device detector.
 */
void CReaderManager::searchForReader()
{
	trc(4, "-> searchForReader");
	QStringList list;
	list << m_deviceDetector.getConnectedHIDDevicePaths(AMS_VID, AMS_PID);
	foreach(QString device, list)
	{
		readerAttached(device, AMS_VID, AMS_PID);
	}
	list.clear();
	list << m_deviceDetector.getConnectedHIDDevicePaths(MTI_VID, MTI_PID);
	foreach(QString device, list)
	{
		readerAttached(device, MTI_VID, MTI_PID);
	}
	list.clear();
	list << m_deviceDetector.getConnectedHIDDevicePaths(AMS_OLD_VID, AMS_OLD_PID);
	foreach(QString device, list)
	{
		readerAttached(device, AMS_VID, AMS_PID);
	}
	//add by rick for RU-865 common sample¡õ
	list.clear();
	list << m_deviceDetector.getConnectedHIDDevicePaths(MTI_EightSixFive_VID, MTI_EightSixFive_PID);
	foreach(QString device, list)
	{
		readerAttached(device, MTI_EightSixFive_VID, MTI_EightSixFive_PID);
	}
	//add by rick for RU-865 common sample¡ô
}
/*!
 * @brief 	function that is called if a matching device is attached.
 * This slot is called if a matching device is attached. It tries to create a reader object and broadcast this to the main application.
 * @param devicePath : e.g. COM1
 */
void CReaderManager::serialReaderAttached ( QString devicePath, QString &msg )
{
	trc(4, "-> serialReaderAttatched");
	struct PortSettings settings;
	settings.BaudRate = BAUD115200;
	settings.DataBits = DATA_8;
	settings.FlowControl = FLOW_OFF;
	settings.StopBits = STOP_1;
	settings.Parity = PAR_NONE;

	/* If the device is already in use, skip */
	if(m_devices.contains(devicePath))
		return;

	trc(1, "A reader with the devicePath " + devicePath + " was attached.");
	/* Create new hid device instance with the given path */
	QrfeSerialPort* dev = new QrfeSerialPort(devicePath,settings);
	if(!dev->open(QIODevice::ReadWrite | QIODevice::Unbuffered)){
		msg = QString("Failed to open COM port %1").arg(devicePath);
		delete dev;
		return;
	}
	m_devices.insert(devicePath, dev);

	/* Try to get the reader object from the factory with the given device */
	QrfeReaderInterface* reader = 0;
	reader = QrfeReaderFactory::getAmsReader(dev, QrfeProtocolHandler::DEV_SERIAL, this);    //Mod by yingwei tseng for uart mode, 2010/12/15
	
	/* If no reader was created or the reader can not be initialized, delete it an skip */
	if(reader == 0 || reader->initDevice() != QrfeReaderInterface::OK)
	{
		if(reader)
			reader->deleteLater();
		m_devices.remove(devicePath);
		return;
	}

	/* Connect to the signal of the reader, if it lost connection */
	QObject::connect(reader, SIGNAL(lostConnection()), this, SLOT(protocollHandlerLostConnection()));

	/* Store reader */
	m_reader.insert(dev, reader);

	/* Broadcast new reader */
	emit gotReader(reader);
}

/*!
 * @brief 	Slot that is called if a matching device is attached.
 * This slot is called if a matching device is attached. It tries to create a reader object and broadcast this to the main application.
 * @param	devicePath		The windows path to the hid device
 * @param	vendorID		The vendor id of the device
 * @param	productID		The product id of the device
 */
void CReaderManager::readerAttached ( QString devicePath, quint16 vendorID, quint16 productID )
{
	trc(4, "-> readerAttatched");

	/* Check the vendor and product id */
	if( (vendorID != AMS_VID || productID != AMS_PID) && (vendorID != MTI_VID || productID != MTI_PID) && (vendorID != AMS_OLD_VID || productID != AMS_OLD_PID) && (vendorID != MTI_EightSixFive_VID || productID != MTI_EightSixFive_PID) )//mod by rick for RU-865 common sample
		return;

	/* If the device is already in use, skip */
	if(m_devices.contains(devicePath))
		return;

	trc(1, "A reader with the devicePath " + devicePath + " was attached.");
	/* Create new hid device instance with the given path */
	QrfeHidDevice* dev = new QrfeHidDevice(devicePath);
	if(!dev->open(QIODevice::ReadWrite)){
		delete dev;
		return;
	}
	m_devices.insert(devicePath, dev);

	/* Try to get the reader object from the factory with the given device */
	QrfeReaderInterface* reader = 0;
	if(vendorID == AMS_VID && productID == AMS_PID)
	{
		reader = QrfeReaderFactory::getAmsReader(dev, QrfeProtocolHandler::DEV_USB_HID, this);
	}
	else if(vendorID == MTI_VID && productID == MTI_PID)
	{
		reader = QrfeReaderFactory::getAmsReader(dev, QrfeProtocolHandler::DEV_USB_HID, this);
	}
	else if(vendorID == AMS_OLD_VID && productID == AMS_OLD_PID)
	{
		reader = QrfeReaderFactory::getAmsReader(dev, QrfeProtocolHandler::DEV_USB_HID, this);
	}
	else if(vendorID == MTI_EightSixFive_VID && productID == MTI_EightSixFive_PID)//add by rick for RU-865 common sample
	{
		reader = QrfeReaderFactory::getAmsReader(dev, QrfeProtocolHandler::DEV_USB_HID, this);
	}

	/* If no reader was created or the reader can not be initialized, delete it an skip */
	if(reader == 0 || reader->initDevice() != QrfeReaderInterface::OK)
	{
		if(reader)
			reader->deleteLater();
		m_devices.remove(devicePath);
		return;
	}

	/* Connect to the signal of the reader, if it lost connection */
	QObject::connect(reader, SIGNAL(lostConnection()), this, SLOT(protocollHandlerLostConnection()));

	/* Store reader */
	m_reader.insert(dev, reader);

	/* Broadcast new reader */
	emit gotReader(reader);
}

/*!
 * @brief 	Slot that is called if a reader was plugged off
 * This slot is called if a device with a matching vendor id and product id is plugged off.
 * @param	devicePath		The windows path to the hid device
 * @param	vendorID		The vendor id of the device
 * @param	productID		The product id of the device
 */
void CReaderManager::readerRemoved ( QString devicePath, quint16 vendorID, quint16 productID )
{
	trc(4, "-> readerRemoved");

	/* Check the vendor and product id */
	//if( (vendorID != AMS_VID || productID != AMS_PID) && (vendorID != AMS_OLD_VID || productID != AMS_OLD_PID) )
	//if( (vendorID != AMS_VID || productID != AMS_PID) && (vendorID != MTI_VID || productID != MTI_PID) && (vendorID != AMS_OLD_VID || productID != AMS_OLD_PID))//mod by rick via James for Bug fixed of MTI RFID Me
	if( (vendorID != AMS_VID || productID != AMS_PID) && (vendorID != MTI_VID || productID != MTI_PID) && (vendorID != AMS_OLD_VID || productID != AMS_OLD_PID) && (vendorID != MTI_EightSixFive_VID || productID != MTI_EightSixFive_PID))//add by rick for RU-865 common sample
		return;

	/* Check if the device was handled by this reader manager */
	if(!m_devices.keys().contains(devicePath))
		return;

	trc(1, "The reader with the devicePath " + devicePath + " was removed.");

	/* Remove the reader from the used objects and notify the rest of the application */
	QIODevice* dev = m_devices.value(devicePath);
	QrfeReaderInterface* reader = m_reader.value(dev);

	if (reader != NULL)
	{
		reader->deviceWasRemoved();
		emit lostReader(reader);
	}

	m_devices.remove(devicePath);
	m_reader.remove(dev);

	/* Store the device in the map that is deleted if cleanup is called */
	m_toDelete.insert(dev, reader);
}

/*!
 * @brief	Slot is called from a reader if it losts connection to the reader.
 * This slot is called from a reader if it losts connection to the reader.
 */
void CReaderManager::protocollHandlerLostConnection()
{
	trc(4, "-> protocollHandlerLostConnection");
	/* Get the caller */
	QrfeReaderInterface* reader = qobject_cast<QrfeReaderInterface*>(sender());
	/* Notify the reader that the device is no plugged off */
	reader->deviceWasRemoved();

	if(!m_reader.values().contains(reader))
		return;

	QIODevice* dev = m_reader.key(reader);
	QString devicePath = m_devices.key(dev);

	/* Notify the rest of the application that the reader is removed */
	emit lostReader(reader);

	m_devices.remove(devicePath);
	m_reader.remove(dev);

	/* Store the device in the map that is deleted if cleanup is called */
	m_toDelete.insert(dev, reader);
}

/*!
 * @brief	Functions that cleans up the plugged off device objects
 */
void CReaderManager::cleanUp()
{
	/* Delete the devices that are to remove */
	foreach(QIODevice* dev, m_toDelete.keys()){
		QrfeReaderInterface* ph = m_toDelete.value(dev);
		ph->deleteLater();
	}
	m_toDelete.clear();
}

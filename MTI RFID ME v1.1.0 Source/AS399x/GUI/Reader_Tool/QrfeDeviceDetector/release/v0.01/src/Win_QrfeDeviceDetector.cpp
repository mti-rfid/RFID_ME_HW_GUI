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
 * Win_QrfeDeviceDetector.cpp
 *
 *  Created on: 19.12.2008
 *      Author: stefan.detter
 */

#include "../inc/Win_QrfeDeviceDetector.h"

#include <setupapi.h>
#include <hidsdi.h>
#include <hidclass.h>

Win_QrfeDeviceDetector::Win_QrfeDeviceDetector(QObject* parent) :
	QrfeDeviceDetectorBase(parent)
#ifdef QrfeDEVICEDETECTOR_DEBUG
, QrfeTraceModule("Win_QrfeDeviceManager")
#endif
{
	m_detector.registerNotification();
	QObject::connect(&m_detector, SIGNAL(usbDeviceAttached (QString, quint16, quint16)), this, SLOT(win_usbDeviceAttached (QString, quint16, quint16)));
	QObject::connect(&m_detector, SIGNAL(usbDeviceRemoved (QString, quint16, quint16)), this, SLOT(win_usbDeviceRemoved (QString, quint16, quint16)));
	QObject::connect(&m_detector, SIGNAL(hidDeviceAttached (QString, quint16, quint16)), this, SLOT(win_hidDeviceAttached (QString, quint16, quint16)));
	QObject::connect(&m_detector, SIGNAL(hidDeviceRemoved (QString, quint16, quint16)), this, SLOT(win_hidDeviceRemoved (QString, quint16, quint16)));
}

Win_QrfeDeviceDetector::~Win_QrfeDeviceDetector()
{
	m_detector.unregisterNotification();
}

bool Win_QrfeDeviceDetector::registerForUSBDevice(quint16 vendorID,
		quint16 productID)
{
#ifdef QrfeDEVICEDETECTOR_DEBUG
	trc(8, "Registering for usb devices with the VendorID " + QString("%1").arg(vendorID, 4, 16, QChar('0')) + " and the ProductID " + QString("%1").arg(productID, 4, 16, QChar('0')));
#endif

	if (!m_usbDevices.contains(vendorID)
			|| !m_usbDevices.values(vendorID).contains(productID))
		m_usbDevices.insertMulti(vendorID, productID);

	QStringList devicePathList =
			getConnectedUSBDevicePaths(vendorID, productID);
	foreach(QString devicePath, devicePathList){
#ifdef QrfeDEVICEDETECTOR_DEBUG
		trc(9, "Found usb device with the VendorID " + QString("%1").arg(vendorID, 4, 16, QChar('0')) + " and the ProductID " + QString("%1").arg(productID, 4, 16, QChar('0')) + " at " + devicePath);
#endif
		win_usbDeviceAttached(devicePath.toLower(), vendorID, productID);
	}

return true;
}

void Win_QrfeDeviceDetector::win_usbDeviceAttached(QString devicePath,
		quint16 vendorID, quint16 productID)
{
	if (!(m_usbDevices.contains(vendorID)
			&& m_usbDevices.values(vendorID).contains(productID)))
		return;

#ifdef QrfeDEVICEDETECTOR_DEBUG
	trc(8, "Usb device attached with the VendorID " + QString("%1").arg(vendorID, 4, 16, QChar('0')) + " and the ProductID " + QString("%1").arg(productID, 4, 16, QChar('0')) + " at " + devicePath);
#endif
	emit usbDeviceAttached(devicePath, vendorID, productID);
}

void Win_QrfeDeviceDetector::win_usbDeviceRemoved(QString devicePath,
		quint16 vendorID, quint16 productID)
{
	if (!(m_usbDevices.contains(vendorID)
			&& m_usbDevices.values(vendorID).contains(productID)))
		return;

#ifdef QrfeDEVICEDETECTOR_DEBUG
	trc(8, "Usb device removed with the VendorID " + QString("%1").arg(vendorID, 4, 16, QChar('0')) + " and the ProductID " + QString("%1").arg(productID, 4, 16, QChar('0')) + " at " + devicePath);
#endif
	emit usbDeviceRemoved(devicePath, vendorID, productID);
}

QStringList Win_QrfeDeviceDetector::getConnectedUSBDevicePaths(
		quint16 vendorID, quint16 productID)
{
	QStringList devicePaths;

	//Get a list of devices matching the criteria (hid interface, present)
	HDEVINFO hardwareDeviceInfoSet = SetupDiGetClassDevs(
			&GUID_DEVINTERFACE_USB_DEVICE, NULL, // Define no enumerator (global)
			NULL, // Define no
			(DIGCF_PRESENT | DIGCF_DEVICEINTERFACE)); // Function class devices.

	uint deviceIndex = 0;

	while (1)
	{
		SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
		deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		//Go through the list and get the interface data
		bool ok = SetupDiEnumDeviceInterfaces(hardwareDeviceInfoSet, NULL, //infoData,
				&GUID_DEVINTERFACE_USB_DEVICE, //interfaceClassGuid,
				deviceIndex, &deviceInterfaceData);

		/* Failed to get a device - possibly the index is larger than the number of devices */
		if (!ok)
		{
			break;
		}

		ulong requiredSize = 0;

		//Get the details with null values to get the required size of the buffer
		SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfoSet,
				&deviceInterfaceData, NULL, //interfaceDetail,
				0, //interfaceDetailSize,
				&requiredSize, 0); //infoData))

		//Allocate the buffer
		PSP_INTERFACE_DEVICE_DETAIL_DATA deviceDetail;
		deviceDetail
				= (PSP_INTERFACE_DEVICE_DETAIL_DATA) new char[requiredSize];
		deviceDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

		//Fill the buffer with the device details
		if (!SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfoSet,
				&deviceInterfaceData, deviceDetail, requiredSize,
				&requiredSize, NULL))
		{
			delete deviceDetail;
			break;
		}

		QString name = QString::fromWCharArray(deviceDetail->DevicePath);

#ifdef QrfeDEVICEDETECTOR_DEBUG
		trc(9, "Found USB device at " + name);
#endif

		quint16 vid = name.mid(
				name.indexOf("Vid_", 0, Qt::CaseInsensitive) + 4, 4).toUInt(
				&ok, 16);
		quint16 pid = name.mid(
				name.indexOf("Pid_", 0, Qt::CaseInsensitive) + 4, 4).toUInt(
				&ok, 16);

		if (vendorID == vid && productID == pid)
			devicePaths.append(name);

		delete deviceDetail;

		deviceIndex++;
	}

	SetupDiDestroyDeviceInfoList(hardwareDeviceInfoSet);

	return devicePaths;
}



bool Win_QrfeDeviceDetector::registerForHIDDevice(quint16 vendorID,
		quint16 productID)
{
#ifdef QrfeDEVICEDETECTOR_DEBUG
	trc(8, "Registering for hid devices with the VendorID " + QString("%1").arg(vendorID, 4, 16, QChar('0')) + " and the ProductID " + QString("%1").arg(productID, 4, 16, QChar('0')));
#endif

	if (!m_hidDevices.contains(vendorID)
			|| !m_hidDevices.values(vendorID).contains(productID))
		m_hidDevices.insertMulti(vendorID, productID);

	QStringList devicePathList =
			getConnectedHIDDevicePaths(vendorID, productID);
	foreach(QString devicePath, devicePathList){
#ifdef QrfeDEVICEDETECTOR_DEBUG
	trc(9, "Found hid device with the VendorID " + QString("%1").arg(vendorID, 4, 16, QChar('0')) + " and the ProductID " + QString("%1").arg(productID, 4, 16, QChar('0')) + " at " + devicePath);
#endif
	win_hidDeviceAttached(devicePath, vendorID, productID);
}

return true;
}

void Win_QrfeDeviceDetector::win_hidDeviceAttached(QString devicePath,
		quint16 vendorID, quint16 productID)
{
	if (!(m_hidDevices.contains(vendorID)
			&& m_hidDevices.values(vendorID).contains(productID)))
		return;

#ifdef QrfeDEVICEDETECTOR_DEBUG
	trc(8, "HID device attached with the VendorID " + QString("%1").arg(vendorID, 4, 16, QChar('0')) + " and the ProductID " + QString("%1").arg(productID, 4, 16, QChar('0')) + " at " + devicePath);
#endif
	emit hidDeviceAttached(devicePath, vendorID, productID);
}

void Win_QrfeDeviceDetector::win_hidDeviceRemoved(QString devicePath,
		quint16 vendorID, quint16 productID)
{
	if (!(m_hidDevices.contains(vendorID)
			&& m_hidDevices.values(vendorID).contains(productID)))
		return;

#ifdef QrfeDEVICEDETECTOR_DEBUG
	trc(8, "HID device removed with the VendorID " + QString("%1").arg(vendorID, 4, 16, QChar('0')) + " and the ProductID " + QString("%1").arg(productID, 4, 16, QChar('0')) + " at " + devicePath);
#endif
	emit hidDeviceRemoved(devicePath, vendorID, productID);
}

QStringList Win_QrfeDeviceDetector::getConnectedHIDDevicePaths(
		quint16 vendorID, quint16 productID)
{
	QStringList devicePaths;

	//Get a list of devices matching the criteria (hid interface, present)
	HDEVINFO hardwareDeviceInfoSet = SetupDiGetClassDevs(
			&GUID_DEVINTERFACE_HID, NULL, // Define no enumerator (global)
			NULL, // Define no
			(DIGCF_PRESENT | DIGCF_DEVICEINTERFACE)); // Function class devices.

	uint deviceIndex = 0;

	while (1)
	{
		SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
		deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		//Go through the list and get the interface data
		bool ok = SetupDiEnumDeviceInterfaces(hardwareDeviceInfoSet, NULL, //infoData,
				&GUID_DEVINTERFACE_HID, //interfaceClassGuid,
				deviceIndex, &deviceInterfaceData);

		/* Failed to get a device - possibly the index is larger than the number of devices */
		if (!ok)
		{
			break;
		}

		ulong requiredSize = 0;

		//Get the details with null values to get the required size of the buffer
		SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfoSet,
				&deviceInterfaceData, NULL, //interfaceDetail,
				0, //interfaceDetailSize,
				&requiredSize, 0); //infoData))

		//Allocate the buffer
		PSP_INTERFACE_DEVICE_DETAIL_DATA deviceDetail;
		deviceDetail
				= (PSP_INTERFACE_DEVICE_DETAIL_DATA) new char[requiredSize];
		deviceDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

		//Fill the buffer with the device details
		if (!SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfoSet,
				&deviceInterfaceData, deviceDetail, requiredSize,
				&requiredSize, NULL))
		{
			delete deviceDetail;
			break;
		}

		HANDLE hHidDeviceHandle = CreateFile(
				(LPCWSTR) deviceDetail->DevicePath, GENERIC_READ, NULL, NULL,
				OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

		// Check that the device opened successfully
		if (hHidDeviceHandle == INVALID_HANDLE_VALUE)
		{
			delete deviceDetail;
			deviceIndex++;
			continue;
		}

		QString name = QString::fromWCharArray(deviceDetail->DevicePath);
#ifdef QrfeDEVICEDETECTOR_DEBUG
		trc(9, "Found HID device at " + name);
#endif

		HIDD_ATTRIBUTES hidDeviceAttributes;

		// Get the attributes of the HID device
		if (HidD_GetAttributes(hHidDeviceHandle, &hidDeviceAttributes))
		{
			// Close the handle if either the VID or the PID don't match
			if ((hidDeviceAttributes.VendorID == vendorID)
					&& (hidDeviceAttributes.ProductID == productID))
			{
				devicePaths.append(name);
			}
		}

		CloseHandle(hHidDeviceHandle);
		hHidDeviceHandle = INVALID_HANDLE_VALUE;

		delete deviceDetail;

		deviceIndex++;
	}

	SetupDiDestroyDeviceInfoList(hardwareDeviceInfoSet);

	return devicePaths;
}


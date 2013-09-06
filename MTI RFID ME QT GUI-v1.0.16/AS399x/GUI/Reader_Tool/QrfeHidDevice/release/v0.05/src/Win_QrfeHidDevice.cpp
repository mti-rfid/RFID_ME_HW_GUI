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
 * win_QrfeHidPort.cpp
 *
 *  Created on: 30.10.2008
 *      Author: stefan.detter
 */

#include "../inc/Win_QrfeHidDevice.h"

Win_QrfeHidDevice::Win_QrfeHidDevice(QObject* parent)
:QrfeHidBase(parent),QrfeTraceModule("Win_QrfeHidDevice")
{
	resetDeviceData();
}

Win_QrfeHidDevice::Win_QrfeHidDevice(QString devicePath, QObject* parent)
	:QrfeHidBase(devicePath, parent),QrfeTraceModule("Win_qrfehiddevic")
{
	resetDeviceData();
}

Win_QrfeHidDevice::~Win_QrfeHidDevice() {
	close();
}


void Win_QrfeHidDevice::resetDeviceData() {
	// Reset the device handle
	m_Handle = INVALID_HANDLE_VALUE;

	// Set all buffer lengths to 0
	m_InputReportBufferLength = 0;
	m_OutputReportBufferLength = 0;
	m_FeatureReportBufferLength = 0;

	m_getReportTimeout = 0;
	m_setReportTimeout = 3000;

	// Obtain the OS version
	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);

	// Only set the max report requests if we are using 2K or later
	if ((osVer.dwPlatformId == 2) && (osVer.dwMajorVersion == 5)) {
		if (osVer.dwMinorVersion >= 1) {
			// XP or later supports 512 input reports
			m_MaxReportRequest = MAX_REPORT_REQUEST_XP;
		} else if (osVer.dwMinorVersion == 0) {
			// 2K supports 200 input reports
			m_MaxReportRequest = MAX_REPORT_REQUEST_2K;
		}
	} else {
		// Otherwise set the max reports to 0, and this will
		// only allow te default
		m_MaxReportRequest = 0;
	}
}

bool Win_QrfeHidDevice::open( OpenMode mode )
{
	m_lastError = HID_DEVICE_SUCCESS;

	// Ensure that the we don't already have an open device
	if (isOpen()){
		m_lastError = HID_DEVICE_ALREADY_OPENED;
		return false;
	}

	// Begin to look for the device if it is not opened
	HANDLE hHidDeviceHandle = openDevice(m_devicePath);

	// Check that the device is opened and the handle is valid
	if (hHidDeviceHandle == INVALID_HANDLE_VALUE){
		m_lastError = HID_DEVICE_NOT_FOUND;
		return false;
	}


	PHIDP_PREPARSED_DATA preparsedData;
	if (!HidD_GetPreparsedData(hHidDeviceHandle, &preparsedData)){
		CloseHandle(hHidDeviceHandle);
		m_lastError = HID_DEVICE_CANNOT_GET_HID_INFO;
		return false;
	}

	HIDP_CAPS capabilities;
	// Used the preparsed data structure to get the device capabilities
	if (!HidP_GetCaps(preparsedData, &capabilities)) {
		CloseHandle(hHidDeviceHandle);
		m_lastError = HID_DEVICE_CANNOT_GET_HID_INFO;
		return false;
	}

	// Allocate memory for the input, output and feature reports
	if (capabilities.InputReportByteLength)
		m_InputReportBufferLength = capabilities.InputReportByteLength;
	if (capabilities.OutputReportByteLength)
		m_OutputReportBufferLength = capabilities.OutputReportByteLength;
	if (capabilities.FeatureReportByteLength)
		m_FeatureReportBufferLength = capabilities.FeatureReportByteLength;
	HidD_GetNumInputBuffers(hHidDeviceHandle, (PULONG) (&m_MaxReportRequest));

	// Cleanup the preparesed data
	HidD_FreePreparsedData(preparsedData);

	// Set the member variables to an opened device and handle
	m_deviceOpened = true;
	m_Handle = hHidDeviceHandle;

	startTimer(1);

	return QIODevice::open(mode);
}

bool Win_QrfeHidDevice::open( QString devicePath, OpenMode mode )
{
	m_devicePath = devicePath;
	return open(mode);
}

void Win_QrfeHidDevice::close()
{
	QIODevice::close();
	// This function will close the HID Device and then calls ResetDeviceData
	// to reinitialize all of the members after the close completes

	m_lastError = HID_DEVICE_SUCCESS;

	// Check to see if the device is opened, otherwise return an error code
	if (!isOpen()){
		m_lastError = HID_DEVICE_NOT_OPENED;
		return;
	}

	// Check that we have valid handle values, otherwise return an error code
	if ((m_Handle != INVALID_HANDLE_VALUE) && (m_Handle != NULL))
		CloseHandle(m_Handle);
	else{
		m_lastError = HID_DEVICE_HANDLE_ERROR;
		return;
	}

	// Reset the device data
	resetDeviceData();

	return;
}

bool Win_QrfeHidDevice::isOpen()
{
	// Check if a device is opened, and the handle is valid
	if (m_deviceOpened && (m_Handle != INVALID_HANDLE_VALUE) && (m_Handle != NULL))
		return true;

	if (m_deviceOpened)
		resetDeviceData();

	return false;
}

qint64 Win_QrfeHidDevice::bytesAvailable () const
{
	qint64 bytesAvailable = 0;
	for(int i = 0; i < m_readBuffer.size(); i++)
		bytesAvailable += m_readBuffer.at(i).size();
	return bytesAvailable;
}

qint64 Win_QrfeHidDevice::bytesToWrite () const
{
	return 0;
}

bool Win_QrfeHidDevice::canReadLine () const
{
	return (m_readBuffer.size() > 0);
}

bool Win_QrfeHidDevice::isSequential () const
{
	return true;
}

qint64 Win_QrfeHidDevice::readData ( char * data, qint64 maxSize )
{
	if(!isOpen())
		return -1;

	if(m_readBuffer.size() == 0)
		return -1;

	QByteArray ba;
	if(m_readBuffer.at(0).size() <= maxSize)
		ba = m_readBuffer.takeFirst();
	else
	{
		ba = m_readBuffer.at(0).left(maxSize);
		m_readBuffer[0] = m_readBuffer.at(0).right(m_readBuffer.at(0).size() - ba.size());
	}

	qint64 runs = qMin((qint64)maxSize, (qint64)ba.size());
	for(qint64 i = 0; i < runs; i++ )
		data[i] = ba.at(i);

	return runs;
}

qint64 Win_QrfeHidDevice::writeData ( const char * data, qint64 maxSize )
{
	if(!isOpen())
		return 0;

	bool ret = setReportControl(QByteArray(data, maxSize));
	if(ret)
		return maxSize;
	else
		return 0;
}

void Win_QrfeHidDevice::timerEvent ( QTimerEvent * /*event*/ )
{
	//static QTime qtime;
	if(!isOpen())
		return;

	QByteArray buffer;
	if(getReportInterrupt(buffer, 1)){
		m_readBuffer.append(buffer);
		//trc(0,(QString("timerevent %1 %2")).arg((int)buffer.at(0),0,16).arg(qtime.restart()));
		emit readyRead();
	}
}

void Win_QrfeHidDevice::timeouts(quint32 &getReportTimeout, quint32 &setReportTimeout)
{
	// Will get the timeouts for Get/SetReport
	getReportTimeout = m_getReportTimeout;
	setReportTimeout = m_setReportTimeout;
}

void Win_QrfeHidDevice::setTimeouts(quint32 getReportTimeout, quint32 setReportTimeout)
{
	// Will set the timeouts for Get/SetReport
	m_getReportTimeout = getReportTimeout;
	m_setReportTimeout = setReportTimeout;
}


QString Win_QrfeHidDevice::getSerialString()
{
	// This function will obtain the serial string of a device by it's index within it's VID
	// and PID. So if only 1 device is connected with VID 10C4, 9999, it's index is 0. If three
	// devices are connected with VID 10C4, 9999 are connected, they would be referenced as
	// device 0, 1, and 2

	QString serialString;
	m_lastError = HID_DEVICE_NOT_FOUND;

	// Begin to look for the device if it is not opened
	HANDLE hHidDeviceHandle = openDevice(m_devicePath);

	// Check that the device is opened and the handle is valid
	if (hHidDeviceHandle == INVALID_HANDLE_VALUE){
		m_lastError = HID_DEVICE_NOT_FOUND;
		return serialString;
	}


	wchar_t serialBuf[512];
	// Obtain the serial number
	if (HidD_GetSerialNumberString(hHidDeviceHandle, serialBuf, 512))
	{
		serialString = QString::fromWCharArray(serialBuf);
		// Return success
		m_lastError = HID_DEVICE_SUCCESS;
		CloseHandle(hHidDeviceHandle);
		return serialString;
	}

	CloseHandle(hHidDeviceHandle);
	return serialString;
}



bool Win_QrfeHidDevice::setFeatureReport(QByteArray buffer)
{
	m_lastError = HID_DEVICE_SUCCESS;

	// Check to see that the device is opened
	if (!isOpen())
	{
		m_lastError = HID_DEVICE_NOT_OPENED;
		return false;
	}

	if (!HidD_SetFeature(m_Handle, buffer.data(), buffer.size()))
	{
		m_lastError = HID_DEVICE_TRANSFER_FAILED;
		return false;
	}

	return true;
}

bool Win_QrfeHidDevice::getFeatureReport(QByteArray &buffer, quint32 buffSize)
{
	m_lastError = HID_DEVICE_SUCCESS;

	// Check to see that the device is opened
	if (!isOpen())
	{
		m_lastError = HID_DEVICE_NOT_OPENED;
		return false;
	}

	buffer.clear();
	char* buff = new char[buffSize];
	memset(buff, 0, buffSize);
	buff[0] = buffer.at(0);
	if (!HidD_GetFeature(m_Handle, buff, buffSize))
	{
		delete buff;
		m_lastError = HID_DEVICE_TRANSFER_FAILED;
		return false;
	}
	buffer = QByteArray(buff, buffSize);
	delete buff;

	return true;
}

bool Win_QrfeHidDevice::setReportInterrupt(QByteArray buffer)
{
	m_lastError = HID_DEVICE_SUCCESS;

	if (buffer.size() > m_OutputReportBufferLength)
	{
		m_lastError = HID_DEVICE_INVALID_BUFFER_SIZE;
		return false;
	}

	// Check to see that the device is opened
	if (!isOpen())
	{
		m_lastError = HID_DEVICE_NOT_OPENED;
		return false;
	}

	quint32 bytesWritten = 0;
	OVERLAPPED o = {0};

	o.hEvent = CreateEvent(NULL, false, false, NULL);

	// Try to write the file
	if (!WriteFile(m_Handle, buffer.data(), buffer.size(), (LPDWORD)&bytesWritten, &o))
	{
		// If the write fails, see if it is because IO is pending
		if (GetLastError() == ERROR_IO_PENDING)
		{
			//If there is still data to be written, wait on the event for 3 seconds
			DWORD waitStatus = WaitForSingleObject(o.hEvent, m_setReportTimeout);

			// If the object is signaled, then get the overlapped result, the write succeeded
			// Otherwise determine if the error was a timeout, or another error
			if (waitStatus == WAIT_OBJECT_0)
			{
				GetOverlappedResult(m_Handle, &o, (LPDWORD)&bytesWritten, false);
			}
			else if (waitStatus == WAIT_TIMEOUT)
			{
				m_lastError = HID_DEVICE_TRANSFER_TIMEOUT;
				CancelIo(m_Handle);
				if (GetOverlappedResult(m_Handle, &o, (LPDWORD)&bytesWritten, false))
				{
					m_lastError = HID_DEVICE_SUCCESS;
				} else
				{
					return false;
				}
			}
			else
			{
				m_lastError = HID_DEVICE_TRANSFER_FAILED;
				CancelIo(m_Handle);
				return false;
			}
		}
		else
		{
			m_lastError = HID_DEVICE_TRANSFER_FAILED;
			return false;
		}
	}

	CloseHandle(o.hEvent);

	return true;
}

bool Win_QrfeHidDevice::getReportInterrupt(QByteArray &buffer, WORD numReports)
{
	m_lastError = HID_DEVICE_SUCCESS;
	quint32 bytesRead = 0;

	if (numReports > m_MaxReportRequest)
	{
		m_lastError = HID_DEVICE_INVALID_BUFFER_SIZE;
	}

	// Check to see that the device is opened
	if (!isOpen())
	{
		m_lastError = HID_DEVICE_NOT_OPENED;
		return false;
	}

	OVERLAPPED o = {0};

	o.hEvent = CreateEvent(NULL, false, false, NULL);

	// Clear out the report buffer, and set the head to the report ID
	char* buff = new char[m_InputReportBufferLength * numReports];
	memset(buff, 0, m_InputReportBufferLength * numReports);

	// Try to read input data
	if (!ReadFile(m_Handle, buff, m_InputReportBufferLength * numReports, (LPDWORD)&bytesRead, &o))
	{
		// If the read fails, see if it is because IO is pending
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// If there is still data to read, wait on the event object for 3 seconds
			DWORD waitStatus = WaitForSingleObject(o.hEvent, m_getReportTimeout);

			// If the object is signaled, then get the overlapped result, the read succeeded
			// Otherwise determine if the error was a timeout, or another error
			if (waitStatus == WAIT_OBJECT_0)
			{
				GetOverlappedResult(m_Handle, &o, (LPDWORD)&bytesRead, false);
			}
			else if (waitStatus == WAIT_TIMEOUT)
			{
				m_lastError = HID_DEVICE_TRANSFER_TIMEOUT;
				CancelIo(m_Handle);
				if (GetOverlappedResult(m_Handle, &o, (LPDWORD)&bytesRead, false))
				{
					m_lastError = HID_DEVICE_SUCCESS;
				} else
				{
					delete buff;
					return false;
				}
			}
			else
			{
				delete buff;
				m_lastError = HID_DEVICE_TRANSFER_FAILED;
				CancelIo(m_Handle);
				return false;
			}
		}
		else
		{
			delete buff;
			m_lastError = HID_DEVICE_TRANSFER_FAILED;
			return false;
		}
	}

	CloseHandle(o.hEvent);


	/* Extract here already the message to be compatible with serial readers */
	if ( bytesRead == 64 ) bytesRead = buff[1];
	if ( bytesRead > 64 ) bytesRead = 64;

	// If the read succeeded, then send the number of bytes read back
	buffer = QByteArray(buff, bytesRead);
	delete buff;

	return true;
}

bool Win_QrfeHidDevice::setReportControl(QByteArray buffer)
{
	m_lastError = HID_DEVICE_SUCCESS;

	// Check to see that the device is opened
	if (!isOpen())
	{
		m_lastError = HID_DEVICE_NOT_OPENED;
		return false;
	}

	// Call SetOutputReport to send this report buffer over the control pipe
	if (!HidD_SetOutputReport(m_Handle, buffer.data(), buffer.size()))
	{
		m_lastError = HID_DEVICE_TRANSFER_FAILED;
		return false;
	}

	return true;
}

bool Win_QrfeHidDevice::getReportControl(QByteArray &buffer)
{
	m_lastError = HID_DEVICE_SUCCESS;

	// Check to see that the device is opened
	if (!isOpen())
	{
		m_lastError = HID_DEVICE_NOT_OPENED;
		return false;
	}

	// Clear out the report buffer, and set the head to the report ID
	unsigned char reportID = buffer.at(0);
	char* buff = new char[m_OutputReportBufferLength];
	memset(buff, 0, m_OutputReportBufferLength);
	buff[0] = reportID;

	// Call GetInputReport to get the requested report buffer over the control pipe
	if (!HidD_GetInputReport(m_Handle, buff, m_OutputReportBufferLength))
	{
		delete buff;
		m_lastError = HID_DEVICE_TRANSFER_FAILED;
		return false;
	}

	buffer = QByteArray(buff, m_OutputReportBufferLength);
	delete buff;

	return true;
}

bool Win_QrfeHidDevice::flush()
{
	return HidD_FlushQueue(m_Handle);
}

HANDLE Win_QrfeHidDevice::openDevice(QString devicePath)
{
	// This function attempts to open a device. If it succeeds
	// then a handle is returned. If it fails, then INVALID_HANDLE_VALUE is returned

	wchar_t* ba = new wchar_t[devicePath.size() + 1];
	devicePath.toWCharArray(ba);
	ba[devicePath.size()] = 0;

	// Open the device
	HANDLE hHidDeviceHandle = CreateFile(
			(LPCWSTR) ba,
			GENERIC_READ | GENERIC_WRITE,
			NULL,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL);
	delete ba;

	return hHidDeviceHandle;
}

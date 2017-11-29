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
#include "../inc/QrfeHidBase.h"

HidReturnCode  QrfeHidBase::m_lastError = HID_DEVICE_SUCCESS;


QrfeHidBase::QrfeHidBase(QObject* parent)
	:QIODevice(parent)
{
	// Call a reset on the device data to initialize
	resetDeviceData();
}

QrfeHidBase::QrfeHidBase(QString devicePath, QObject* parent)
	:QIODevice(parent)
{
	m_devicePath = devicePath;

	// Call a reset on the device data to initialize
	resetDeviceData();
}

QrfeHidBase::~QrfeHidBase()
{
	//close();
}

QString QrfeHidBase::devicePath() const
{
	return m_devicePath;
}

void QrfeHidBase::setDevicePath(QString devicePath)
{
	m_devicePath = devicePath;
}

quint8 QrfeHidBase::lastError()
{
	return m_lastError;
}

QString QrfeHidBase::errorString () const
{
	switch(lastError()){
	case HID_DEVICE_SUCCESS: 				return "Success";
	case HID_DEVICE_NOT_FOUND: 				return "Device not found";
	case HID_DEVICE_NOT_OPENED: 			return "Device not opened";
	case HID_DEVICE_ALREADY_OPENED: 		return "Device already opened";
	case HID_DEVICE_TRANSFER_TIMEOUT: 		return "Transfer timeout";
	case HID_DEVICE_TRANSFER_FAILED: 		return "Transfer failed";
	case HID_DEVICE_CANNOT_GET_HID_INFO: 	return "Cannot get HID info";
	case HID_DEVICE_HANDLE_ERROR: 			return "Handle error";
	case HID_DEVICE_INVALID_BUFFER_SIZE: 	return "Invalid buffer size";
	case HID_DEVICE_SYSTEM_CODE: 			return "System code";
	case HID_DEVICE_UNKNOWN_ERROR: 			return "Unknown error";
	default: 								return "Unknown error";
	}
}

void QrfeHidBase::resetDeviceData()
{
	// Set device opened to false
	m_deviceOpened = false;

	m_getReportTimeout = 0;
	m_setReportTimeout = 0;

}

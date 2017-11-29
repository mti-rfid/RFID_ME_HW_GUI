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
 * Win_QrfeDeviceChangeDetector.cpp
 *
 *  Created on: 25.11.2008
 *      Author: stefan.detter
 */

#include "../inc/Win_QrfeDeviceChangeDetector.h"

#include <QCoreApplication>

#include <dbt.h>
#include <guiddef.h>
#include <initguid.h>
#include <usbiodef.h>
#include <hidclass.h>

Win_QrfeDeviceChangeDetector::Win_QrfeDeviceChangeDetector()
#ifdef QrfeDEVICEDETECTOR_DEBUG
: QrfeTraceModule("CWinDeviceChangeDetecter")
#endif
{
	m_notifyDeviceInterface_USB = INVALID_HANDLE_VALUE;
	m_notifyDeviceInterface_HID = INVALID_HANDLE_VALUE;
}

Win_QrfeDeviceChangeDetector::~Win_QrfeDeviceChangeDetector()
{

}

bool Win_QrfeDeviceChangeDetector::registerNotification()
{
	// Register device notification
	DEV_BROADCAST_DEVICEINTERFACE deviceFilter = { 0 };

	ZeroMemory(&deviceFilter, sizeof(deviceFilter));
	deviceFilter.dbcc_size = sizeof(deviceFilter);
	deviceFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	deviceFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;
	m_notifyDeviceInterface_USB = RegisterDeviceNotification(this->winId(),
			&deviceFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	if (m_notifyDeviceInterface_USB == 0)
	{
#ifdef QrfeDEVICEDETECTOR_DEBUG
		trc(1, "Could not register device notifictaion for usb devices.");
		trc(1, "The notify handle: " + QString::number((ulong)m_notifyDeviceInterface_USB));
		trc(1, "The last error: " + QString::number((ulong)GetLastError()));
#endif
		return false;
	}
#ifdef QrfeDEVICEDETECTOR_DEBUG
	trc(9, "Device notification for USB devices registered.");
#endif

	deviceFilter.dbcc_classguid = GUID_DEVINTERFACE_HID;
	m_notifyDeviceInterface_HID = RegisterDeviceNotification(this->winId(),
			&deviceFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	if (m_notifyDeviceInterface_HID == 0)
	{
#ifdef QrfeDEVICEDETECTOR_DEBUG
		trc(1, "Could not register device notifictaion for hid devices.");
		trc(1, "The notify handle: " + QString::number((ulong)m_notifyDeviceInterface_HID));
		trc(1, "The last error: " + QString::number((ulong)GetLastError()));
#endif
		return false;
	}
#ifdef QrfeDEVICEDETECTOR_DEBUG
	trc(9, "Device notification for HID devices registered.");
#endif

	return true;
}

bool Win_QrfeDeviceChangeDetector::unregisterNotification()
{
	// Unegister device notification
	if (NULL != m_notifyDeviceInterface_USB)
	{
		UnregisterDeviceNotification(m_notifyDeviceInterface_USB);
		m_notifyDeviceInterface_USB = INVALID_HANDLE_VALUE;
	}

	if (NULL != m_notifyDeviceInterface_HID)
	{
		UnregisterDeviceNotification(m_notifyDeviceInterface_HID);
		m_notifyDeviceInterface_HID = INVALID_HANDLE_VALUE;
	}

	return true;
}

bool Win_QrfeDeviceChangeDetector::winEvent(MSG * msg, long * /*result*/)
{
	static quint32 count = 0;

	if (msg->message == WM_DEVICECHANGE)
	{

#ifdef QrfeDEVICEDETECTOR_DEBUG
		trc(10, "Msg: "
				"message(" + QString::number(msg->message) + ") / "
				"wParam (" + QString::number((quint64)msg->wParam) + ") / "
				"lParam (" + QString::number((quint64)msg->lParam) + ") / "
				"handle (" + QString::number((quint64)msg->hwnd) + ") / "
				"time (" + QString::number(msg->time) + ") / "
		);
#endif
		switch (msg->wParam)
		{
		case DBT_CONFIGCHANGECANCELED:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "DBT_CONFIGCHANGECANCELED");
#endif
			break;
		case DBT_CONFIGCHANGED:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "DBT_CONFIGCHANGED");
#endif
			break;
		case DBT_CUSTOMEVENT:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "DBT_CUSTOMEVENT");
#endif
			break;
		case DBT_DEVICEARRIVAL:
		{
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(9, "DBT_DEVICEARRIVAL");
#endif
			DEV_BROADCAST_HDR* hdr = (DEV_BROADCAST_HDR*) msg->lParam;
			switch (hdr->dbch_devicetype)
			{
			case DBT_DEVTYP_DEVICEINTERFACE:
			{
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--DBT_DEVTYP_DEVICEINTERFACE");
#endif
				DEV_BROADCAST_DEVICEINTERFACE* inter =
						(DEV_BROADCAST_DEVICEINTERFACE*) hdr;

				QString name = QString::fromWCharArray(inter->dbcc_name).toLower();
				bool ok;
				quint16 vendorID =
						name.mid(name.indexOf("vid_") + 4, 4).toUInt(&ok, 16);
				if (!ok)
					return false;
				quint16 productID =
						name.mid(name.indexOf("pid_") + 4, 4).toUInt(&ok, 16);
				if (!ok)
					return false;

#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "---- " + name);
#endif

				if (GUID_DEVINTERFACE_USB_DEVICE == inter->dbcc_classguid)
				{
#ifdef QrfeDEVICEDETECTOR_DEBUG
					trc(9, "------- USB Device was detected...");
#endif
					emit usbDeviceAttached(name, vendorID, productID);
				}
				else if (GUID_DEVINTERFACE_HID == inter->dbcc_classguid)
				{
#ifdef QrfeDEVICEDETECTOR_DEBUG
					trc(9, "------- HID Device was detected...");
#endif
					emit hidDeviceAttached(name, vendorID, productID);
				}
				else
					return false;

#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--------- with the VendorID: " + name.mid(name.indexOf("vid_") + 4, 4));
				trc(9, "--------- and the ProductID: " + name.mid(name.indexOf("pid_") + 4, 4));
#endif

				break;
			}
			case DBT_DEVTYP_HANDLE:
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--DBT_DEVTYP_HANDLE");
#endif
				break;
			case DBT_DEVTYP_OEM:
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--DBT_DEVTYP_OEM");
#endif
				break;
			case DBT_DEVTYP_PORT:
			{
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--DBT_DEVTYP_PORT");
				DEV_BROADCAST_PORT* port = (DEV_BROADCAST_PORT*) hdr;
				trc(9, "---- " + QString::fromWCharArray(port->dbcp_name));
#endif
				break;
			}
			case DBT_DEVTYP_VOLUME:
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--DBT_DEVTYP_VOLUME");
#endif
				break;
			}
			break;
		}
		case DBT_DEVICEQUERYREMOVE:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "DBT_DEVICEQUERYREMOVE");
#endif
			break;
		case DBT_DEVICEQUERYREMOVEFAILED:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "DBT_DEVICEQUERYREMOVEFAILED");
#endif
			break;
		case DBT_DEVICEREMOVECOMPLETE:
		{
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(9, "DBT_DEVICEREMOVECOMPLETE");
#endif
			DEV_BROADCAST_HDR* hdr = (DEV_BROADCAST_HDR*) msg->lParam;
			switch (hdr->dbch_devicetype)
			{
			case DBT_DEVTYP_DEVICEINTERFACE:
			{
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--DBT_DEVTYP_DEVICEINTERFACE");
#endif
				DEV_BROADCAST_DEVICEINTERFACE* inter =
						(DEV_BROADCAST_DEVICEINTERFACE*) hdr;

				QString name = QString::fromWCharArray(inter->dbcc_name).toLower();
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "---- " + name);
#endif
				bool ok;
				quint16 vendorID = name.mid(name.indexOf("vid_", 0,
						Qt::CaseInsensitive) + 4, 4).toUInt(&ok, 16);
				if (!ok)
					return false;
				quint16 productID = name.mid(name.indexOf("pid_", 0,
						Qt::CaseInsensitive) + 4, 4).toUInt(&ok, 16);
				if (!ok)
					return false;

				if (GUID_DEVINTERFACE_USB_DEVICE == inter->dbcc_classguid)
				{
#ifdef QrfeDEVICEDETECTOR_DEBUG
					trc(9, "------- USB Device was removed...");
#endif
					emit usbDeviceRemoved(name, vendorID, productID);
				}
				else if (GUID_DEVINTERFACE_HID == inter->dbcc_classguid)
				{
#ifdef QrfeDEVICEDETECTOR_DEBUG
					trc(9, "------- HID Device was removed...");
#endif
					emit hidDeviceRemoved(name, vendorID, productID);
				}
				else
					return false;

#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--------- with the VendorID: " + name.mid(name.indexOf("vid_") + 4, 4));
				trc(9, "--------- and the ProductID: " + name.mid(name.indexOf("pid_") + 4, 4));
#endif

				break;
			}
			case DBT_DEVTYP_HANDLE:
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--DBT_DEVTYP_HANDLE");
#endif
				break;
			case DBT_DEVTYP_OEM:
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--DBT_DEVTYP_OEM");
#endif
				break;
			case DBT_DEVTYP_PORT:
			{
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--DBT_DEVTYP_PORT");
				DEV_BROADCAST_PORT* port = (DEV_BROADCAST_PORT*) hdr;
				trc(9, "---- " + QString::fromWCharArray(port->dbcp_name));
#endif
				break;
			}
			case DBT_DEVTYP_VOLUME:
#ifdef QrfeDEVICEDETECTOR_DEBUG
				trc(9, "--DBT_DEVTYP_VOLUME");
#endif
				break;
			}
			break;
		}
		case DBT_DEVICEREMOVEPENDING:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "DBT_DEVICEREMOVEPENDING");
#endif
			break;
		case DBT_DEVICETYPESPECIFIC:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "DBT_DEVICETYPESPECIFIC");
#endif
			break;
		case DBT_DEVNODES_CHANGED:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "DBT_DEVNODES_CHANGED");
#endif
			break;
		case DBT_QUERYCHANGECONFIG:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "DBT_QUERYCHANGECONFIG");
#endif
			break;
		case DBT_USERDEFINED:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "DBT_USERDEFINED");
#endif
			break;

		default:
#ifdef QrfeDEVICEDETECTOR_DEBUG
			trc(10, "default");
#endif
			break;
		}
	}

	return false;
}


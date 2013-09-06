// HIDDevice.h: interface for the CHIDDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HIDDEVICE_H__05F2CE26_6902_4D68_80D5_11F608F4415A__INCLUDED_)
#define AFX_HIDDEVICE_H__05F2CE26_6902_4D68_80D5_11F608F4415A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef HIDINTERFACEDLL_EXPORTS
#define HIDINTERFACE_API __declspec(dllexport)
#else
#define HIDINTERFACE_API __declspec(dllimport)
#endif

//////////////
// Definitions
//////////////

// Return Codes
#define HID_DEVICE_SUCCESS				0x00
#define HID_DEVICE_NOT_FOUND			0x01
#define HID_DEVICE_NOT_OPENED			0x02
#define HID_DEVICE_ALREADY_OPENED		0x03
#define	HID_DEVICE_TRANSFER_TIMEOUT		0x04
#define HID_DEVICE_TRANSFER_FAILED		0x05
#define HID_DEVICE_CANNOT_GET_HID_INFO	0x06
#define HID_DEVICE_HANDLE_ERROR			0x07
#define HID_DEVICE_INVALID_BUFFER_SIZE	0x08
#define HID_DEVICE_SYSTEM_CODE			0x09
#define HID_DEVICE_UNKNOWN_ERROR		0xFF

// Max number of USB Devices allowed
#define MAX_USB_DEVICES	64

// Max number of reports that can be requested at time
#define MAX_REPORT_REQUEST_XP	512
#define MAX_REPORT_REQUEST_2K	200

#define DEFAULT_REPORT_INPUT_BUFFERS	0

// String Types
#define HID_VID_STRING				0x01
#define HID_PID_STRING				0x02
#define HID_PATH_STRING				0x03
#define HID_SERIAL_STRING			0x04

// String Lengths
#define VID_LENGTH					5
#define PID_LENGTH					5
#define MAX_PATH_LENGTH				MAX_PATH
#define MAX_SERIAL_STRING_LENGTH	256



/////////////////////////////////
// Universal HID Access Functions
/////////////////////////////////

DWORD HIDINTERFACE_API GetNumHidDevices(WORD vid, WORD pid);
BYTE HIDINTERFACE_API GetHidString(DWORD deviceIndex, WORD vid, WORD pid, BYTE hidStringType, LPSTR deviceString, DWORD deviceStringLength);
VOID HIDINTERFACE_API GetHidGuid(LPGUID hidGuid);



//////////////////////////////
// HID Device Class Definition
//////////////////////////////

class HIDINTERFACE_API CHIDDevice
{
public:	
	BYTE Open(DWORD deviceIndex, WORD vid, WORD pid, WORD numInputBuffers = DEFAULT_REPORT_INPUT_BUFFERS);
	BOOL IsOpened();

	BYTE GetString(BYTE hidStringType, LPSTR deviceString, DWORD deviceStringLength);

	BYTE SetFeatureReport_Control(BYTE* buffer, DWORD bufferSize);
	BYTE GetFeatureReport_Control(BYTE* buffer, DWORD bufferSize);
	BYTE SetOutputReport_Interrupt(BYTE* buffer, DWORD bufferSize);
	BYTE GetInputReport_Interrupt(BYTE* buffer, DWORD bufferSize, WORD numReports, DWORD* bytesReturned);
	BYTE SetOutputReport_Control(BYTE* buffer, DWORD bufferSize);
	BYTE GetInputReport_Control(BYTE* buffer, DWORD bufferSize);

	WORD GetInputReportBufferLength();
	WORD GetOutputReportBufferLength();
	WORD GetFeatureReportBufferLength();
	DWORD GetMaxReportRequest();
	BOOL FlushBuffers();

	void GetTimeouts(UINT* getReportTimeout, UINT* setReportTimeout);
	void SetTimeouts(UINT getReportTimeout, UINT setReportTimeout);

	BYTE Close();

	CHIDDevice();
	virtual ~CHIDDevice();

private:
	void ResetDeviceData();

	HANDLE m_Handle;
	BOOL m_DeviceOpened;

	wchar_t m_devicePathString[MAX_PATH];

	UINT m_GetReportTimeout;
	UINT m_SetReportTimeout;

	WORD m_InputReportBufferLength;
	WORD m_OutputReportBufferLength;
	WORD m_FeatureReportBufferLength;

	DWORD m_MaxReportRequest;
};

#endif // !defined(AFX_HIDDEVICE_H__05F2CE26_6902_4D68_80D5_11F608F4415A__INCLUDED_)

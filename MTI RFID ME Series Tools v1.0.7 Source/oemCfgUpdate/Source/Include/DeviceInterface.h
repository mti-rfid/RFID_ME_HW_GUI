#ifndef _DEVICE_INTERFACE_H
#define _DEVICE_INTERFACE_H

#include "stdafx.h"
#include "DeviceInterface.h"
#include "TransDLL.h"

typedef UINT  (WINAPI *DLL_USB_OPEN)(UINT uiPID, UINT uiVID);
typedef UINT  (WINAPI *DLL_SERIAL_OPEN)(UINT uiComPort, DCB &r_Dcb);
typedef UINT  (WINAPI *DLL_CLEAR_BUFFER)();
typedef UINT  (WINAPI *DLL_WRITE)(UINT uiMode, UCHAR *cData, UINT iLength);
typedef UINT  (WINAPI *DLL_READ)(UINT uiMode, UCHAR *cData, UINT *iLength);
typedef UINT  (WINAPI *DLL_CLOSE)();
typedef char* (WINAPI *DLL_ASK_VERSION)();
typedef INT   (WINAPI *DLL_ASK_DEV_TYPE)();
typedef UINT  (WINAPI *DLL_ASK_REV_COUNT)();

	

class ClsDeviceInterface
{
public:
	ClsDeviceInterface();
	~ClsDeviceInterface();

	bool bIsLoad;
	
	bool API_LoadLib();

	//DLL
	UINT   API_USB_Open(UINT uiPID, UINT uiVID);
	UINT   API_Serial_Open(UINT uiComPort, DCB &r_Dcb);
	BOOL   API_AskVersion(CString &r_Ver);
	char*  API_AskDevType();
	BOOL   API_ClearBuffer();
	UINT   API_AskRevCount();
	BOOL   API_Write(UCHAR *cData, int iLength);
	UINT   API_Read(UCHAR *cData, int iLength);
	void   API_Close();

private:

	//DLL
	DLL_USB_OPEN	   dll_UsbOpen;
	DLL_SERIAL_OPEN	   dll_SerialOpen;
	DLL_ASK_VERSION	   dll_AskVersion;
	DLL_ASK_DEV_TYPE   dll_AskDevType;
	DLL_CLEAR_BUFFER   dll_ClearBuffer;
	DLL_ASK_REV_COUNT  dll_AskRevCount;
	DLL_WRITE		   dll_Write;
	DLL_READ		   dll_Read;
	DLL_CLOSE		   dll_Close;

	HINSTANCE			m_Handle;


};

#endif
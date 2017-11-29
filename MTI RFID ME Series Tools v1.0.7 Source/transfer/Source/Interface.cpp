#include "stdafx.h"
#include "Interface.h"



ClsDeviceBase *clsDevice = NULL;
CString DLL_VERSION;


UINT WINAPI API_USB_Open(UINT uiPID, UINT uiVID)
{
	if( NULL != clsDevice)
		return FAILURE;

	ClsUSBPort  *clsUSBPort = NULL;


	clsDevice  = new ClsUSBPort;
	clsUSBPort = dynamic_cast<ClsUSBPort *>(clsDevice);
	
	if( NULL == clsUSBPort ||  false == clsUSBPort->API_Open(uiPID, uiVID) )
	{
		API_Close();
		return FAILURE;
	}

	return OK;
}


UINT WINAPI API_Serial_Open(UINT uiComPort, DCB &r_Dcb)
{
	if( NULL != clsDevice)
		return FAILURE;

	ClsSerial *clsSerial = NULL;


	clsDevice = new ClsSerial;
	clsSerial = dynamic_cast<ClsSerial *>(clsDevice);

	if( NULL == clsSerial ||  false == clsSerial->API_Open(uiComPort, r_Dcb) )
	{
		API_Close();
		return FAILURE;
	}

	return OK;
}


UINT WINAPI API_Write(UINT uiMode, UCHAR *cData, UINT iLength)
{
	if( NULL == clsDevice )
		return INVALID_HANDLE;

	return clsDevice->API_Write(uiMode, cData, iLength);
}



UINT WINAPI API_Read(UINT uiMode, UCHAR *cData, UINT *iLength)
{

	if( NULL == clsDevice )
		return INVALID_HANDLE;

	return clsDevice->API_Read(uiMode, cData, iLength);
}


UINT WINAPI API_Close()
{
	clsUTILITY::DelObj(CLOSE_TYPE_POINT, clsDevice);

	if( NULL == clsDevice)
		return OK;
	else
		return FAILURE;
}



UINT WINAPI API_ClearBuffer()
{
	if( NULL == clsDevice )
		return FAILURE;

	if(clsDevice->API_ClearBuffer() == true)
		return OK;
	else
		return FAILURE;
}


UINT WINAPI API_SetOverlapTime(UINT r_uiWtOverlapTime, UINT r_uiRdOverlapTime)
{
	if( NULL == clsDevice )
		return FAILURE;

	clsDevice->API_SetOverlapTime(r_uiWtOverlapTime, r_uiRdOverlapTime);
	
	return OK;
}

//====not respones result=========
UINT WINAPI API_AskRevCount()
{
	if( NULL == clsDevice)
		return 0;
		
	return clsDevice->API_AskRevCount();
}

UINT WINAPI API_AskUsbWtCount()
{
	if( NULL == clsDevice)
		return 0;

	//RS232 doesn't support this API
	if( API_AskDevType() == DEV_TYPE_RS232 )
		return 0;

	ClsUSBPort  *clsUSBPort = NULL;
	clsUSBPort = dynamic_cast<ClsUSBPort *>(clsDevice);

	if(clsUSBPort == NULL)
		return 0;

	return clsUSBPort->API_AskUsbWtCount();
}


INT WINAPI API_AskDevType()
{
	if( NULL == clsDevice )
		return NULL;

	return clsDevice->API_AskDevType();
}


char * WINAPI API_AskVersion()
{
	if( DLL_VERSION.IsEmpty() == true)
		DLL_VERSION.Format("%d.%d.%d", VER_MAJOR, VER_MINOR, VER_RELEASE);

	return (char*)DLL_VERSION.GetString();	
}


char* WINAPI API_AskUSBName()
{
	if( NULL == clsDevice )
		return NULL;

	return clsDevice->API_AskUSBName();
}

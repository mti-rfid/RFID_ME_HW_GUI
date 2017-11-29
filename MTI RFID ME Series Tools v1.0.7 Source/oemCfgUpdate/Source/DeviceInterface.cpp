#include "stdafx.h"
#include "DeviceInterface.h"



ClsDeviceInterface::ClsDeviceInterface()
{
	dll_UsbOpen		= NULL;
	dll_SerialOpen	= NULL;
	dll_AskVersion  = NULL;
	dll_AskDevType	= NULL;
	dll_ClearBuffer	= NULL;
	dll_AskRevCount = NULL;
	dll_Write		= NULL;
	dll_Read		= NULL;
	dll_Close		= NULL;

	m_Handle = NULL;
	bIsLoad  = false;
}

ClsDeviceInterface::~ClsDeviceInterface()
{
	if( NULL != m_Handle)
	{
		FreeLibrary(m_Handle);
		m_Handle = NULL;
	}
}


bool ClsDeviceInterface::API_LoadLib()
{
	m_Handle = GetModuleHandle( _T("Transfer") );
	if (m_Handle == NULL) 
	{
		m_Handle = LoadLibrary( _T("Transfer.dll") );
		if(m_Handle==NULL)
		{
			return FALSE;
		}
	}



	dll_UsbOpen		= (DLL_USB_OPEN)		GetProcAddress(m_Handle, "API_USB_Open" );
	dll_SerialOpen	= (DLL_SERIAL_OPEN)		GetProcAddress(m_Handle, "API_Serial_Open" );	
	dll_AskVersion	= (DLL_ASK_VERSION)		GetProcAddress(m_Handle, "API_AskVersion" );
	dll_AskDevType	= (DLL_ASK_DEV_TYPE)	GetProcAddress(m_Handle, "API_AskDevType" );
	dll_ClearBuffer	= (DLL_CLEAR_BUFFER)	GetProcAddress(m_Handle, "API_ClearBuffer" );
	dll_AskRevCount	= (DLL_ASK_REV_COUNT)   GetProcAddress(m_Handle, "API_AskRevCount" );
	dll_Write		= (DLL_WRITE)			GetProcAddress(m_Handle, "API_Write" );
	dll_Read		= (DLL_READ)			GetProcAddress(m_Handle, "API_Read" );
	dll_Close		= (DLL_CLOSE)			GetProcAddress(m_Handle, "API_Close" );


	if( NULL == dll_UsbOpen		||
		NULL == dll_SerialOpen  ||
		NULL == dll_AskVersion	||
		NULL == dll_AskDevType	||
		NULL == dll_ClearBuffer ||
		NULL == dll_AskRevCount ||
		NULL == dll_Write		||
		NULL == dll_Read		||
		NULL == dll_Close			)		return false;


	bIsLoad = true;
	return true;
}


UINT ClsDeviceInterface::API_USB_Open(UINT uiPID, UINT uiVID)
{
	if( NULL == m_Handle)
		return false;

	return dll_UsbOpen(uiPID, uiVID);
}


UINT ClsDeviceInterface::API_Serial_Open(UINT uiComPort, DCB &r_Dcb)
{
	if( NULL == m_Handle)
		return false;

	return  dll_SerialOpen( uiComPort, r_Dcb);
}



BOOL ClsDeviceInterface::API_AskVersion(CString &r_Ver)
{
	if( NULL == m_Handle)
		return false;

	CString strTmp;

	strTmp = dll_AskVersion();
	

	if( true == strTmp.IsEmpty() )
		return false;

	r_Ver = strTmp;

	return true;
}



char* ClsDeviceInterface::API_AskDevType()
{
	if( NULL == m_Handle)
		return NULL;

	int iType = 0;

	iType =  dll_AskDevType();

	switch(iType)
	{
		case DEV_TYPE_RS232:
			return "RS232";

		case DEV_TYPE_USB:
			return "USB";

		default:	
			return NULL;
	}
}


BOOL ClsDeviceInterface::API_ClearBuffer()
{
	if( NULL == m_Handle)
		return false;

	return dll_ClearBuffer();
}


UINT ClsDeviceInterface::API_AskRevCount()
{
	if( NULL == m_Handle)
		return 0;

	return dll_AskRevCount();
}


BOOL ClsDeviceInterface::API_Write(UCHAR *cData, int iLength)
{
	if( NULL == m_Handle)
		return false;

	UINT uiMode; //uiMode=2 is set Report, and it's only for USB. (RS232 has no set Report)
	CString tempDevType;
	tempDevType = API_AskDevType();
	if(tempDevType == "USB")
		uiMode = 2;
	else
		uiMode = 0;	

	return dll_Write(uiMode ,cData, iLength);
}


UINT ClsDeviceInterface::API_Read(UCHAR *cData, int iLength)
{
	if( NULL == m_Handle)
		return 0;

	UINT uiCount = iLength;
	
	if( 1 == dll_Read(INTERRUPT_WITH_OVERLAP, cData, &uiCount) )
		return uiCount;
	else
		return 0;	
}


void ClsDeviceInterface::API_Close()
{
	if( NULL == m_Handle)
		return ;

	dll_Close();
}


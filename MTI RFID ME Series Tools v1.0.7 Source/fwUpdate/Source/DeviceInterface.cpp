#include "stdafx.h"
#include "DeviceInterface.h"
//#define   UNICODE 
//#define   _UNICODE 



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


BOOL ClsDeviceInterface::API_USB_Open(UINT uiPID, UINT uiVID)
{
	if( NULL == m_Handle)
		return false;
	if(dll_UsbOpen(uiPID, uiVID) == 1)
		return true;
	else
		return false;
}


BOOL ClsDeviceInterface::API_Serial_Open(CString cstrComPort, DCB &r_Dcb)
{
	if( NULL == m_Handle)
		return false;


	char charComPort[1];	
	charComPort[0] = (char)*(cstrComPort.GetString()+3);//get the COM number
	UINT iComPort;
	iComPort = (UINT)atof(charComPort);



#ifdef UNICODE

	bool bResult = false;
	//int  Length = cstrComPort.GetLength() + 1;
	//char *tmpPath = new char[Length];
	//ZeroMemory(tmpPath, sizeof(tmpPath));
	//
	//for(int i = 0; i < Length; i++)
	//	tmpPath[i] = *(cstrComPort.GetString() + i);

	_stscanf_s(cstrComPort.GetBuffer(),  _T("COM%d"), &iComPort);
	if(dll_SerialOpen( iComPort, r_Dcb) == 1)
		bResult =  true;
	else
		bResult = false;

	//delete tmpPath;

	return bResult;

#else
	if(dll_SerialOpen( iComPort, r_Dcb) == 1)
		return true;
	else 
		return false;
#endif

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

	
	//add by yo, for change type in Unicode, 2011/12/12
	//TCHAR *pUnicode;
	//int nIndex;
	//char *pAnsi;
	//for change type in Unicode
	
	int iType = 0;
	iType = dll_AskDevType();
	switch(iType)
	{
	
		case 1:
			return "RS232";
			//mod by yo, for change type in Unicode, 2011/12/12
		/*	pUnicode = _T("RS232");
			nIndex = WideCharToMultiByte
				(CP_ACP, 0, pUnicode, -1, NULL, 0, NULL, NULL);
			pAnsi = new char[nIndex +1];
			WideCharToMultiByte
				(CP_ACP, 0, pUnicode, -1, pAnsi, nIndex, NULL, NULL);
			return pAnsi;*/
			
			//return _T("RS232");

		case 2:
			return "USB";
	/*		pUnicode = _T("USB");
			nIndex = WideCharToMultiByte
				(CP_ACP, 0, pUnicode, -1, NULL, 0, NULL, NULL);
			pAnsi = new char[nIndex +1];
			WideCharToMultiByte
				(CP_ACP, 0, pUnicode, -1, pAnsi, nIndex, NULL, NULL);
			return pAnsi;*/
			//return _T("USB");
	
		default:
			return NULL;
	}

	//return dll_AskDevType();
}


BOOL ClsDeviceInterface::API_ClearBuffer()
{
	if( NULL == m_Handle)
		return false;
	if(dll_ClearBuffer() == 1)
		return true;
	else 
		return false;
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

	UINT uiMode;//uiMode=2 is set Report, and it's only for USB. (RS232 has no set Report)
	CString tempDevType;
	tempDevType = API_AskDevType();
	if(tempDevType == "USB")
		uiMode = 2;
	else
		uiMode = 0;	
	
	if(dll_Write(uiMode, cData, iLength) == 1)
		return true;
	else
		return false;
}


UINT ClsDeviceInterface::API_Read(UCHAR *cData, UINT iLength)
{
	if( NULL == m_Handle)
		return 0;

	//UINT *iLength = 0;
	dll_Read(0, cData, &iLength);
	return iLength;
//edit by Clark, the following is the new interface for API_Read() to get "iLength" information, 2011/08/16
//int iLength = 0
//dll_Read(cData, &iLength);
//return iLength;
}


void ClsDeviceInterface::API_Close()
{
	if( NULL == m_Handle)
		return ;

	dll_Close();
}


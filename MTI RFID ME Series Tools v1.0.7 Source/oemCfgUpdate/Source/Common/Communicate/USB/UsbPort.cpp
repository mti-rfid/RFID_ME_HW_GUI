#include "stdafx.h"
#include "USBPort.h"

ClsUSBPort::ClsUSBPort()
{
	m_uiPID = 0;
	m_uiVID = 0;
	m_bIsCatchHid = false;
	m_pstrcDetail = NULL;
	m_pPreparsedData = NULL;
	m_hdlRdHID = INVALID_HANDLE_VALUE;
	m_hdlTxHID = INVALID_HANDLE_VALUE;
	m_hdlHID   = INVALID_HANDLE_VALUE;
	m_hDeviceInfo = INVALID_HANDLE_VALUE;
	m_hdlRdEvent = INVALID_HANDLE_VALUE;
	m_hdlWtEvent = INVALID_HANDLE_VALUE;

	ZeroMemory( &m_HidCaps,   sizeof(HIDP_CAPS) );
	ZeroMemory( &m_OverlapRd, sizeof(OVERLAPPED) );
	ZeroMemory( &m_OverlapWt, sizeof(OVERLAPPED) );
	ZeroMemory( &m_guidHID, sizeof(GUID) );
}


ClsUSBPort::~ClsUSBPort()
{
	CloseUSB();
}


bool ClsUSBPort::API_Open(UINT uiPID, UINT uiVID)
{
#ifdef	TRY_DEBUG
	try{
#endif
		m_uiPID = uiPID;
		m_uiVID = uiVID;

		CloseUSB();
		m_hdlRdEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		m_hdlWtEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_OverlapRd.hEvent = m_hdlRdEvent;
		m_OverlapWt.hEvent = m_hdlWtEvent;

		if( FindHID() == true )
			return true;
		else
			return false;

#ifdef	TRY_DEBUG
	}
	catch(...)
	{
		DWORD dwError = GetLastError();
	}
#endif
}

void ClsUSBPort::API_Close()
{
#ifdef	TRY_DEBUG
	try{
#endif


	CloseUSB();


#ifdef	TRY_DEBUG
	}
	catch(...)
	{
		DWORD dwError = GetLastError();
	}
#endif
}


UINT ClsUSBPort::API_Read(UCHAR *cData, UINT iLength)
{	
#ifdef	TRY_DEBUG
	try{
#endif
	
	if(m_hdlHID == INVALID_HANDLE_VALUE || iLength > m_HidCaps.InputReportByteLength)
		return false;

	DWORD dwRead = 0;
	bool  result = false;

	UCHAR *ucData = new UCHAR[m_HidCaps.InputReportByteLength];
	ZeroMemory(ucData, m_HidCaps.InputReportByteLength);
	memcpy(ucData, cData, iLength);

	result =  ReadFile( m_hdlHID,
					    ucData,
						m_HidCaps.InputReportByteLength,
				   	   &dwRead,
					   &m_OverlapRd );

	memcpy(cData, ucData, iLength);

	if(ucData) delete ucData;

	return dwRead;

#ifdef	TRY_DEBUG
	}
	catch(...)
	{
		DWORD dwError = GetLastError();
	}
#endif
}


bool ClsUSBPort::API_Write(UCHAR *cData, UINT iLength)
{
#ifdef	TRY_DEBUG
	try{
#endif	

	if(m_hdlHID == INVALID_HANDLE_VALUE || iLength > m_HidCaps.OutputReportByteLength)	
		return false;

	UCHAR *ucData = new UCHAR[m_HidCaps.OutputReportByteLength];
	ZeroMemory(ucData, m_HidCaps.OutputReportByteLength);
	memcpy(ucData, cData, iLength);

	bool result = HidD_SetOutputReport(m_hdlHID,
									   ucData,
									   m_HidCaps.OutputReportByteLength);
	
	if(ucData) delete ucData;

	return result;


#ifdef	TRY_DEBUG
	}
	catch(...)
	{
		DWORD dwError = GetLastError();
	}
#endif


}


//=====================================================================================================
bool ClsUSBPort::FindHID(void)
{

#ifdef	TRY_DEBUG
	try{
#endif


	SP_DEVICE_INTERFACE_DATA			strcInterfaceData;
	HIDD_ATTRIBUTES						strcAttributes;
	INT									iHidIndex       = 0;
	DWORD								dwInterfaceSize = 0;
	bool								LastDevice      = false;
	bool								bResult         = false;

	ZeroMemory(&strcInterfaceData, sizeof(SP_DEVICE_INTERFACE_DATA));

	//Get the HID Guid
	HidD_GetHidGuid(&m_guidHID);

	// Create a HDEVINFO with all USB devices.
	m_hDeviceInfo = SetupDiGetClassDevs(&m_guidHID,
									    NULL,
									    NULL,
									    DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if(m_hDeviceInfo == INVALID_HANDLE_VALUE)
		return false;

	
	strcInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	// Enumerate through all devices in Set. To Ask whether there is a HID or not.
	for( iHidIndex = 0; SetupDiEnumDeviceInterfaces(m_hDeviceInfo,NULL,&m_guidHID,iHidIndex,&strcInterfaceData); iHidIndex++) 										
	{
		bResult = SetupDiEnumDeviceInterfaces(m_hDeviceInfo,
											  NULL,
											  &m_guidHID,
											  iHidIndex,
											  &strcInterfaceData);

		if(bResult == false)
			continue;

		//Get Detail strcture size "dwInterfaceSize"
		SetupDiGetDeviceInterfaceDetail(m_hDeviceInfo,
									    &strcInterfaceData,
									    NULL,
									    0,
									    &dwInterfaceSize,				
										NULL);
		
		//Allocate   memory   for   the   hDevInfo   structure
		//Because the path of usb may change, the memory was different eveyy time.
		m_pstrcDetail         = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(dwInterfaceSize);
		m_pstrcDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

		//call the function again. Get the path of HID
		bResult = SetupDiGetDeviceInterfaceDetail(m_hDeviceInfo,			
												  &strcInterfaceData,		
												  m_pstrcDetail,	   	    
												  dwInterfaceSize,	    	
												  NULL,			
												  NULL);
		if(bResult == false)
			continue;

		//Get the HID Handle Point 
		m_hdlHID =  CreateFile(m_pstrcDetail->DevicePath,
						 	   GENERIC_WRITE | GENERIC_READ,
							   FILE_SHARE_READ | FILE_SHARE_WRITE,  //0,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
							   NULL);

		ZeroMemory(&strcAttributes, sizeof(HIDD_ATTRIBUTES));
		strcAttributes.Size = sizeof(HIDD_ATTRIBUTES);
		bResult = HidD_GetAttributes(m_hdlHID, &strcAttributes);

		while(1)
		{
			if( !( (bResult == true )                    && 
				   (strcAttributes.VendorID  == m_uiVID) && 
				   (strcAttributes.ProductID == m_uiPID)     ) )
			{
				break;
			}

			GetTheTopLevel();

			//if( OpenEndpoint() == false )
			//	break;

			m_bIsCatchHid = TRUE;	
			break;
		}


		if(m_bIsCatchHid == false)
		{
			if(m_pstrcDetail != NULL)
			{
				free(m_pstrcDetail);
				m_pstrcDetail = NULL;
			}

			clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlHID);
			clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlRdHID);
			clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlTxHID);		
		}
	}//end for

	if(m_hDeviceInfo != INVALID_HANDLE_VALUE)
	{
		SetupDiDestroyDeviceInfoList(m_hDeviceInfo);
		m_hDeviceInfo = INVALID_HANDLE_VALUE;
	}

	if( m_bIsCatchHid == false)
	{
		CloseUSB();
		return false;
	}

	return true;


#ifdef	TRY_DEBUG
	}
	catch(...)
	{
		DWORD dwError = GetLastError();
	}
#endif

}

void ClsUSBPort::GetTheTopLevel()
{	
#ifdef	TRY_DEBUG
	try{
#endif

	ZeroMemory( &m_HidCaps, sizeof(HIDP_CAPS) );

	//If user-mode applications can call HidD_GetPreparsedData, 
	//Kernel-mode drivers can use an IOCTL_HID_GET_COLLECTION_DESCRIPTOR request.
	HidD_GetPreparsedData(m_hdlHID, &m_pPreparsedData);

	//know the HID setting
	HidP_GetCaps(m_pPreparsedData, &m_HidCaps);

#ifdef	TRY_DEBUG
	}
	catch(...)
	{
		DWORD dwError = GetLastError();
	}
#endif
}


bool ClsUSBPort::OpenEndpoint()
{

	TCHAR  DeviceName[512];
	ZeroMemory(DeviceName, sizeof(DeviceName));

	_stprintf(DeviceName, _T("%s\\%s"), m_pstrcDetail->DevicePath, _T("PIPE01") );

	//Get the HID Handle Point 
	m_hdlRdHID =  CreateFile(DeviceName,
					 	     GENERIC_READ,
						     FILE_SHARE_READ | FILE_SHARE_WRITE,
						     NULL,
						     OPEN_EXISTING,
						     FILE_FLAG_OVERLAPPED,
						     NULL);


	_stprintf(DeviceName, _T("%s\\%s"), m_pstrcDetail->DevicePath, _T("PIPE02") );

	//Get the HID Handle Point 
	m_hdlTxHID =  CreateFile(DeviceName,
					 	     GENERIC_WRITE,
						     FILE_SHARE_READ | FILE_SHARE_WRITE,
						     NULL,
						     OPEN_EXISTING,
						     FILE_FLAG_OVERLAPPED,
						     NULL);

	if(m_hdlTxHID == INVALID_HANDLE_VALUE  ||  m_hdlRdHID == INVALID_HANDLE_VALUE)
		return false;
	else
		return true;
}


void ClsUSBPort::CloseUSB(void)
{
#ifdef	TRY_DEBUG
	try{
#endif
		m_bIsCatchHid = false;

		//CancelIo(m_hdlHID);

		clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlRdEvent);
		clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlWtEvent);
		clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlHID);
		clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlRdHID);
		clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlTxHID);

		if(m_pstrcDetail != NULL)
		{
			free(m_pstrcDetail);
			m_pstrcDetail = NULL;
		}

		if(m_pPreparsedData != NULL)
		{
			HidD_FreePreparsedData(m_pPreparsedData);
			m_pPreparsedData = NULL;
		}

		if(m_hDeviceInfo != INVALID_HANDLE_VALUE)
		{
			SetupDiDestroyDeviceInfoList(m_hDeviceInfo);
			m_hDeviceInfo = INVALID_HANDLE_VALUE;
		}

#ifdef	TRY_DEBUG
	}
	catch(...)
	{
		DWORD dwError = GetLastError();
	}
#endif
}



UINT ClsUSBPort::API_AskRevCount()
{
	if(m_hdlHID == INVALID_HANDLE_VALUE)	return 0;

	return m_HidCaps.InputReportByteLength;
}


char* ClsUSBPort::API_AskDevType()
{
	return "USB";
}

bool ClsUSBPort::API_ClearBuffer()
{
	return false;
}
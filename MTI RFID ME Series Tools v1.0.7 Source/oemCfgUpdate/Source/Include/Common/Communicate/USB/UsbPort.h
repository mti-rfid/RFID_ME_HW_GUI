#ifndef USB_PORT_H
#define USB_PORT_H

extern "C"	//these libs use C code
{
#include "setupapi.h"
#include "hidsdi.h"
};

#include "DeviceBase.h"


// SetupDiGetInterfaceDeviceDetail所需要的輸出長度，定義足夠大
#define INTERFACE_DETAIL_SIZE  1024
#define USB_LENGTH 	           64

class ClsUSBPort : public virtual ClsDeviceBase
{
private:
	UINT m_uiPID;
	UINT m_uiVID;
	GUID m_guidHID;

public:
	ClsUSBPort();
	~ClsUSBPort();
	bool  API_Open(UINT uiPID, UINT uiVID);
	void  API_Close();
	UINT  API_Read(UCHAR *cData, UINT iLength = USB_LENGTH);
	bool  API_Write(UCHAR *cData, UINT iLength = USB_LENGTH);
	UINT  API_AskRevCount();
	char* API_AskDevType();
	bool  API_ClearBuffer();

private:
	HANDLE     m_hdlHID;
	HANDLE     m_hdlRdHID;
	HANDLE     m_hdlTxHID;
	HANDLE     m_hdlRdEvent;
	HANDLE     m_hdlWtEvent;
	OVERLAPPED m_OverlapRd;
	OVERLAPPED m_OverlapWt;
	bool	   m_bIsCatchHid;

	PSP_INTERFACE_DEVICE_DETAIL_DATA  m_pstrcDetail;
	PHIDP_PREPARSED_DATA			  m_pPreparsedData;
	HDEVINFO					      m_hDeviceInfo;
	HIDP_CAPS						  m_HidCaps;

	bool  FindHID(void);
	bool  OpenEndpoint(void);
	void  GetTheTopLevel(void);
	void  CloseUSB(void);
};


#endif
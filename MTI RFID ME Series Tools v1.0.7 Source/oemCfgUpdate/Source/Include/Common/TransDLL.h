#ifndef _TRANSFER_DLL
#define _TRANSFER_DLL

//#define  _DLL_EXPORT

#ifdef _DLL_EXPORT
#define DLLMyLib __declspec(dllexport)
#else
#define DLLMyLib __declspec(dllimport)
#endif


//Device Type
#define DEV_TYPE_RS232 1
#define DEV_TYPE_USB   2


enum TRANS_MODE
{
	INTERRUPT_WITH_OVERLAP,
	INTERRUPT_WITHOUT_OVERLAP,
	REPORT,
};


enum RESULT
{	
	FAILURE,
	OK,
	INVALID_HANDLE,
	INVALID_PARAMETER,
	USB_REPORT_COUNT_NOT_MATCH,
	BUFFER_OVERFLOW,	
};


extern "C"
{
	DLLMyLib UINT  WINAPI API_USB_Open(UINT uiPID, UINT uiVID);
	DLLMyLib UINT  WINAPI API_Serial_Open(UINT uiComPort, DCB &r_Dcb);
	DLLMyLib UINT  WINAPI API_ClearBuffer();	
	DLLMyLib UINT  WINAPI API_Write(UINT uiMode, UCHAR *cData, UINT iLength);
	DLLMyLib UINT  WINAPI API_Read(UINT uiMode, UCHAR *cData, UINT *iLength);
	DLLMyLib UINT  WINAPI API_Close();
	DLLMyLib UINT  WINAPI API_SetOverlapTime(UINT r_uiWtOverlapTime, UINT r_uiRdOverlapTime);

	//deesn't respones result
	DLLMyLib UINT  WINAPI API_AskRevCount();
	DLLMyLib UINT  WINAPI API_AskUsbWtCount();
	DLLMyLib char* WINAPI API_AskVersion();
	DLLMyLib INT   WINAPI API_AskDevType();
	DLLMyLib char* WINAPI API_AskUSBName();
}


#endif
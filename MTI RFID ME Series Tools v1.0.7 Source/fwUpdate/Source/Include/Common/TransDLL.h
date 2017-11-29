#ifndef _TRANSFER_DLL
#define _TRANSFER_DLL


#ifdef _DLL_EXPORT
#define DLLMyLib __declspec(dllexport)
#else
#define DLLMyLib __declspec(dllimport)
#endif


extern "C"
{
	DLLMyLib BOOL  WINAPI API_USB_Open(UINT uiPID, UINT uiVID);
	DLLMyLib BOOL  WINAPI API_Serial_Open(TCHAR *tcComPort, DCB &r_Dcb);
	DLLMyLib char* WINAPI API_AskVersion();
	DLLMyLib char* WINAPI API_AskDevType();
	DLLMyLib BOOL  WINAPI API_ClearBuffer();
	DLLMyLib UINT  WINAPI API_AskRevCount();
	DLLMyLib BOOL  WINAPI API_Write(UCHAR *cData, UINT iLength);
	DLLMyLib UINT  WINAPI API_Read(UCHAR *cData, UINT iLength);
	DLLMyLib void  WINAPI API_Close();
	
}


#endif
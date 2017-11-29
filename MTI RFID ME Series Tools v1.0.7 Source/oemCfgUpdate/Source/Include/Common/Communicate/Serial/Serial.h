#ifndef CLS_SERIAL
#define CLS_SERIAL

#include "DeviceBase.h"

#define WAIT_TIME 1000

class ClsSerial : public virtual ClsDeviceBase
{
private:
	HANDLE	    m_hdlCom;
	HANDLE      m_hdlRdEvent;
	HANDLE      m_hdlWtEvent;
	OVERLAPPED  m_OverlapRd;
	OVERLAPPED  m_OverlapWt;
	DCB		    m_DCB;
	COMSTAT		m_COMSTAT;


public:
	ClsSerial();
	~ClsSerial();

	//Interface
	bool  API_Open(TCHAR *tcComPort, DCB &r_Dcb);
	bool  API_Write(UCHAR *cData, UINT iLength);
	UINT  API_Read(UCHAR *cData, UINT iLength);
	void  API_Close();
	UINT  API_AskRevCount();
	char* API_AskDevType();
	bool  API_ClearBuffer();
};

#endif
#include "stdafx.h"
#include "Serial.h"


ClsSerial::ClsSerial()
{
	m_hdlCom	 = INVALID_HANDLE_VALUE;
	m_hdlRdEvent = INVALID_HANDLE_VALUE;
	m_hdlWtEvent = INVALID_HANDLE_VALUE;

	ZeroMemory( &m_COMSTAT,   sizeof(COMSTAT) );
	ZeroMemory( &m_OverlapRd, sizeof(OVERLAPPED) );
	ZeroMemory( &m_OverlapWt, sizeof(OVERLAPPED) );
}

ClsSerial::~ClsSerial()
{
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlCom);
}


bool ClsSerial::API_Open(TCHAR *tcComPort, DCB &r_Dcb)
{
	DWORD dwError = 0;

    m_hdlCom =CreateFile( tcComPort,
                          GENERIC_READ | GENERIC_WRITE,  
						  0, 
						  NULL, 
						  OPEN_EXISTING, 
						  FILE_FLAG_WRITE_THROUGH, 
						  NULL);

	m_hdlRdEvent	   = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hdlWtEvent	   = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_OverlapRd.hEvent = m_hdlRdEvent;
	m_OverlapWt.hEvent = m_hdlWtEvent;


    if ( m_hdlCom     == INVALID_HANDLE_VALUE ||
		 m_hdlRdEvent == INVALID_HANDLE_VALUE ||
		 m_hdlWtEvent == INVALID_HANDLE_VALUE	)
    {
        return false;
    }


    if (!ClearCommError(m_hdlCom, &dwError, &m_COMSTAT))
    {
        return false;
    }


    if (!PurgeComm(m_hdlCom, PURGE_RXCLEAR | PURGE_TXCLEAR))
    {
        return false;
    }


    if (!GetCommState(m_hdlCom, &m_DCB))
    {
        return false;
    }


    m_DCB.BaudRate = r_Dcb.BaudRate;
    m_DCB.ByteSize = r_Dcb.ByteSize;
    m_DCB.Parity   = r_Dcb.Parity;
    m_DCB.StopBits = r_Dcb.StopBits;


    if (!SetCommState(m_hdlCom, &m_DCB))
    {
        return false;
    }

	return true;

}


bool ClsSerial::API_Write(UCHAR *cData, UINT iLength)
{
	if( INVALID_HANDLE_VALUE == m_hdlCom || 0 == iLength )
		return false;

	DWORD dwWrite = 0;
	
	WriteFile(m_hdlCom, cData, iLength, &dwWrite, &m_OverlapWt);

	if( WAIT_OBJECT_0 == WaitForSingleObject(m_OverlapWt.hEvent, INFINITE) )	return true;
	else																		return false;

}



UINT ClsSerial::API_Read(UCHAR *cData, UINT iLength)
{
	if( INVALID_HANDLE_VALUE == m_hdlCom || 0 == iLength )
		return 0;

	DWORD dwRead = 0;
	
	ReadFile(m_hdlCom, cData, iLength, &dwRead, &m_OverlapRd);
	WaitForSingleObject(m_OverlapRd.hEvent, WAIT_TIME);

	return dwRead;
}


void ClsSerial::API_Close()
{
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlCom);
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlRdEvent);
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlWtEvent);
}



UINT ClsSerial::API_AskRevCount()
{
	DWORD dwError = 0;

	ClearCommError(m_hdlCom, &dwError, &m_COMSTAT);

	return m_COMSTAT.cbInQue;
}


char* ClsSerial::API_AskDevType()
{
	return "RS232";
}

bool ClsSerial::API_ClearBuffer()
{
	DWORD dwError = 0;

	if (!ClearCommError(m_hdlCom, &dwError, &m_COMSTAT))
		return false;
    
    if (!PurgeComm(m_hdlCom, PURGE_RXCLEAR | PURGE_TXCLEAR))
		return false;
    
	return true;
}
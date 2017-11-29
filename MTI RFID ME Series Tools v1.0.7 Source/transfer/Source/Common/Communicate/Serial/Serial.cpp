#include "stdafx.h"
#include "Serial.h"


#include <stdio.h>


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


bool ClsSerial::API_Open(UINT uiComPort, DCB &r_Dcb)
{
	DWORD dwError = 0;
	CString cstrPort;

    if( uiComPort >= 10)
		cstrPort.Format( _T("\\\\.\\COM%d"), uiComPort);
	else
		cstrPort.Format( _T("COM%d"), uiComPort);

    m_hdlCom =CreateFile( cstrPort,
                          GENERIC_READ | GENERIC_WRITE,  
						  //0,  
						  FILE_SHARE_READ | FILE_SHARE_WRITE,  
						  NULL, 
						  OPEN_EXISTING, 
						  FILE_FLAG_OVERLAPPED,
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


	//Set Configuration
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


	//Set Buffer
    if (!SetupComm(m_hdlCom, 1024, 1024))
    {
        return false;
    }


	//Set Timeout
	COMMTIMEOUTS time_out;
	time_out.ReadIntervalTimeout         = MAXDWORD;
	time_out.ReadTotalTimeoutConstant    = 500;
	time_out.ReadTotalTimeoutMultiplier  = 20;
	time_out.WriteTotalTimeoutConstant   = 500;
	time_out.WriteTotalTimeoutMultiplier = 20;

	if( !SetCommTimeouts(m_hdlCom,&time_out) )
    {
        return false;
    }
	
	return true;
}


RESULT ClsSerial::API_Write(UINT uiMode, UCHAR *cData, UINT iLength)
{
	if( m_hdlCom == INVALID_HANDLE_VALUE )
		return INVALID_HANDLE;

	if( 0 == iLength )	
		return INVALID_PARAMETER;

	if( INTERRUPT_WITH_OVERLAP    != uiMode  &&
		INTERRUPT_WITHOUT_OVERLAP != uiMode      )	
	{
		return INVALID_PARAMETER;
	}


	bool  bResult = false;
	DWORD dwWrite = 0;
	

	switch(uiMode)
	{
		case INTERRUPT_WITH_OVERLAP:
			WriteFile(m_hdlCom, cData, iLength, &dwWrite, &m_OverlapWt);

			if( WAIT_OBJECT_0 == WaitForSingleObject(m_OverlapWt.hEvent, uiWtOverlapTime) )
			{
				bResult = true;
			}
			else
			{
				CancelIo(m_hdlCom);
				bResult = false;
			}
			break;

		case INTERRUPT_WITHOUT_OVERLAP:
			bResult = !!WriteFile(m_hdlCom, cData, iLength, &dwWrite, NULL);
			break;

		default:
			return INVALID_PARAMETER;
	}

	if(bResult == true)	 return OK;
	else				 return FAILURE;
	
}



RESULT ClsSerial::API_Read(UINT uiMode, UCHAR *cData, UINT *iLength)
{
	UINT tmpLen = *iLength;

	//Initial parameter
	*iLength = 0;

	if( m_hdlCom == INVALID_HANDLE_VALUE )
		return INVALID_HANDLE;

	if( 0 == tmpLen )	
		return INVALID_PARAMETER;

	if( INTERRUPT_WITH_OVERLAP    != uiMode  &&
		INTERRUPT_WITHOUT_OVERLAP != uiMode      )	
	{
		return INVALID_PARAMETER;
	}

	bool  bResult = false;
	DWORD dwRead  = 0;

	switch(uiMode)
	{
		case INTERRUPT_WITH_OVERLAP:
			ReadFile(m_hdlCom, cData, tmpLen, &dwRead, &m_OverlapRd);

			if( WAIT_OBJECT_0 == WaitForSingleObject(m_OverlapRd.hEvent, uiRdOverlapTime) )
			{
				bResult = true;
			}
			else
			{
				CancelIo(m_hdlCom);
				bResult = false;
			}
			break;

		case INTERRUPT_WITHOUT_OVERLAP:
			bResult = !!ReadFile(m_hdlCom, cData, tmpLen, &dwRead, NULL);
			break;

		default:
			return INVALID_PARAMETER;
	}


	if(bResult == true)
	{
		*iLength = dwRead;
		return OK;
	}
	else
	{
		return FAILURE;
	}
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


INT ClsSerial::API_AskDevType()
{
	return DEV_TYPE_RS232;
}


bool ClsSerial::API_ClearBuffer()
{
	DWORD dwError = 0;

    if (!PurgeComm(m_hdlCom, PURGE_RXCLEAR | PURGE_TXCLEAR))
		return false;

	if (!ClearCommError(m_hdlCom, &dwError, &m_COMSTAT))
		return false;   

	return true;
}


char* ClsSerial::API_AskUSBName()
{
	return NULL;
}

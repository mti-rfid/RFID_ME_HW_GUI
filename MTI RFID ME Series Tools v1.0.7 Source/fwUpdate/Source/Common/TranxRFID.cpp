#include "stdafx.h"
#include "TranxRFID.h"
#include <time.h>


//#define _OPEN_SAS_COM_TRANS_DATA_LOG
#define _OPEN_SAS_DEBUG_LOG_TO_FILE

#ifdef _OPEN_SAS_DEBUG_LOG_TO_FILE
#include <stdio.h>

void gfLog( LPCTSTR pData )
{
	LPCTSTR g_pDebugFile = _T("DebugLog.txt") ;

	int iStrlen = _tcslen(pData);
	SYSTEMTIME Now;
	GetSystemTime(&Now);

	TCHAR sTemp[512];
	ZeroMemory(sTemp, sizeof(sTemp));
	_stprintf_s(sTemp, _T("%02d:02d:%02d:%03d "), Now.wHour,Now.wMinute,Now.wSecond,Now.wMilliseconds);
	_tcscat_s(sTemp,pData);

	int iLength = _tcslen(sTemp);
    HANDLE hFile = CreateFile( g_pDebugFile, GENERIC_WRITE, FILE_SHARE_READ,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) ;

    SetFilePointer( hFile, 0, NULL, FILE_END ) ;
    
	DWORD dwWritten ;
    WriteFile( hFile, sTemp, iLength, &dwWritten, NULL ) ;
    
	TCHAR sEnter[ 2 ] = { _T('\r'), _T('\n') } ;
    WriteFile( hFile, sEnter, 2, &dwWritten, NULL ) ;
    
	CloseHandle( hFile ) ;

}
#endif


clsTRANX_RFID::clsTRANX_RFID()
{
	m_ucRxStep       = 0;
    m_ucRxLength     = 0;
	m_ucRxCnt        = 0;
	m_ucRxCmd        = 0;
	m_ucRxStatus     = 0;
	m_ucTxCnt	     = 0;
	m_TimeCnt        = 0;
	m_usOemAddr      = 0;
	m_ucOemData      = 0;
	m_uiCurFileCnt	 = 0;
	m_uiTotalFileCnt = 0;
	m_bIsOpen	     = false;
	//m_clsUSBPort     = NULL;
	m_clsDeviceInterface = NULL;
	m_enumTxCmd[0]   = CMD_NOTHING;
	m_hdlImageFile   = INVALID_HANDLE_VALUE;

	//add by yo chen, 2011/09/06
	tempRxDataLen_2 = 128;
	//END

	ZeroMemory( m_ucTxBuf, sizeof(m_ucTxBuf) );
	ZeroMemory( m_ucRxBuf, sizeof(m_ucTxBuf) );

	//m_clsUSBPort = new cUSBPort;
	m_clsDeviceInterface = new ClsDeviceInterface;
}


clsTRANX_RFID::~clsTRANX_RFID()
{
	//clsUTILITY::DelObj(CLOSE_TYPE_POINT, m_clsUSBPort);
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);

}

//mod by yo chen, change API_StartComm ->
			//1. bool API_USB_Open( PRODUCT_ID , VENDOR_ID)
			//2. API_Serial_Open( strTmp , m_Dcb)
bool clsTRANX_RFID::API_USB_Open(/*UINT uiPID, UINT uiVID*/)
{
	ResetFlow();
	for(int i = 0; i < (sizeof(VENDOR_ID) / sizeof(UINT)); i++) {
		if(m_bIsOpen = !!m_clsDeviceInterface->API_USB_Open(PRODUCT_ID[i], VENDOR_ID[i]/*uiPID, uiVID*/))
			break;
	}
	
	//add by yo chen for ini. some parameter	
	curDevType = API_AskDevType();
	if(curDevType == "USB")
		updateDev = 0;
	else if(curDevType == "RS232")
		updateDev = 1;
	else
		updateDev = -1;

	return m_bIsOpen;
}

bool clsTRANX_RFID::API_Serial_Open(CString cstrComPort, DCB &r_Dcb)
{
	m_bIsOpen = !!m_clsDeviceInterface->API_Serial_Open(cstrComPort, r_Dcb);

	//add by yo chen for ini. some parameter	
	curDevType = API_AskDevType();
	if(curDevType == "USB")
		updateDev = 0;
	else if(curDevType == "RS232")
		updateDev = 1;
	else
		updateDev = -1;

	return m_bIsOpen;
}

void clsTRANX_RFID::API_CloseComm(void)
{
	ResetFlow();
	m_bIsOpen = false;
	
	if(m_clsDeviceInterface)    m_clsDeviceInterface->API_Close();
}


bool clsTRANX_RFID::API_LoadLib()
{
	return m_clsDeviceInterface->API_LoadLib();
}

bool clsTRANX_RFID::API_IsLoadLib()
{
	if(m_clsDeviceInterface)
		return m_clsDeviceInterface->bIsLoad;
	else 
		return false;
}

//add by yo chen
char* clsTRANX_RFID::API_AskDevType()
{
	return m_clsDeviceInterface->API_AskDevType();
}
bool clsTRANX_RFID::API_Write(UCHAR *cData, int iLength)
{
	return !!m_clsDeviceInterface->API_Write(cData, iLength);
}
UINT clsTRANX_RFID::API_AskRevCount()
{
	return m_clsDeviceInterface->API_AskRevCount();
}
UINT clsTRANX_RFID::API_Read(UCHAR *cData, UINT iLength)
{
	CString tempDevType;
	tempDevType = API_AskDevType();
	if( tempDevType == "USB" )
		return m_clsDeviceInterface->API_Read(cData, iLength);
	else
	{
		UINT res=0;
		res = m_clsDeviceInterface->API_Read(cData, iLength);
		m_ucRxStatus = cData[5];//CMD ID
		m_ucRxStatus = cData[6];//Length
		m_ucRxStatus = cData[7];//Status, not 0x00 @ GetModuleID_new()
		return res;
	}
}
void clsTRANX_RFID::API_Close()
{
	return m_clsDeviceInterface->API_Close();
}
//=================================PACKET=======================================//
bool clsTRANX_RFID::API_IsOpen(void)
{
	return m_bIsOpen;
}

bool clsTRANX_RFID::API_IsBusy(void)
{
	return m_enumTxCmd[0]==CMD_NOTHING?false:true;
}


bool clsTRANX_RFID::API_QueryOneCmd(UINT index)
{
	DevType = API_AskDevType();
	if(DevType == "USB")
	{
		if(m_enumTxCmd[0] != CMD_NOTHING || !m_bIsOpen)
			return false;
	}

	bool bResult = false;

	ZeroMemory(m_enumTxCmd, sizeof(m_enumTxCmd));
	ZeroMemory(m_ucTxBuf,   sizeof(m_ucTxBuf));


	m_enumTxCmd[0] = CMD_SEND_ONE_CMD_BY_USER;
	m_enumTxCmd[1] = CMD_TRANX_ALL_OK;


	switch(index)
	{	
		case CMD_VERSION_B:
			bResult = Tx_bVersion();
			break;

		case CMD_ENTER_UPDATE_FW_MODE_B:
			bResult = Tx_bEnterUpdateFwMode();
			break;

		case CMD_EXIT_UPDATE_FW_MODE_B:
			bResult = Tx_bExitUpdateFwMode();
			break;

		case CMD_BEGIN_UPDATE_B:
			bResult = Tx_bBeginUpdate();
			break;

		case CMD_WRITE_IMAGE_B:
			bResult = Tx_bWriteImage();
			break;

		case CMD_SET_OEM_CONFIG_B:
			bResult = Tx_bSetOemConfig();
			break;

		default:
			return false;
	}

	if(bResult == false)
	{
	
		ResetFlow();   
		return false;
	}

	return true;
}


bool clsTRANX_RFID::API_QueryOneCmd(CString cstrData)
{
	if( !m_bIsOpen                      || 
		m_enumTxCmd[0] != CMD_NOTHING   || 
		cstrData.IsEmpty() == true      ||
		cstrData.GetLength()>USB_LENGTH    )
	{
		return false;
	}

	ZeroMemory(m_enumTxCmd, sizeof(m_enumTxCmd));
	ZeroMemory(m_ucTxBuf,   sizeof(m_ucTxBuf));


	m_enumTxCmd[0] = CMD_SEND_ONE_CMD_BY_USER;
	m_enumTxCmd[1] = CMD_TRANX_ALL_OK;


	TCHAR *pch = NULL;
	TCHAR  tmpData[USB_LENGTH];
	TCHAR *pcontext = NULL;
	ZeroMemory(tmpData, sizeof(tmpData));
	_tcscpy_s( tmpData, cstrData.GetString() );

//Push data============================
	//Cmd
	pch = _tcstok_s( tmpData, _T(" "), &pcontext );
	if(pch == NULL)	  return false;
	
	_stscanf_s(pch,  _T(" %x"), &m_ucTxBuf[0]);

	pch = _tcstok_s(NULL, _T(" "), &pcontext );
	for(int i = 1; pch != NULL; i++)
	{
		_stscanf_s(pch, _T(" %x"), &m_ucTxBuf[i]);
		pch = _tcstok_s(NULL, _T(" "), &pcontext );
	}  
//=====================================

	if( WriteData(m_ucTxBuf) == false )	
	{
		ResetFlow();   
		return false;
	}

	return true;
}




bool clsTRANX_RFID::API_UpdateFw(HANDLE hdlImageFile, LARGE_INTEGER strcLargeInt, bool bAllProgress)
{
	//add by yo Chen, 2011/08/31
	updateDev = 0;

	if(m_enumTxCmd[0] != CMD_NOTHING || strcLargeInt.LowPart == 0 || !m_bIsOpen)
		return false;

	int Length = API_AskRevCount();
	//Calculation Packet count
	m_uiTotalFileCnt =  (strcLargeInt.LowPart / PER_PACKET_UPDATE_IMAGE) +		 
						((strcLargeInt.LowPart % PER_PACKET_UPDATE_IMAGE)?1:0 );
	m_hdlImageFile = hdlImageFile;
	m_uiCurFileCnt = 0;

	if(bAllProgress == true)
	{
		m_enumTxCmd[0] = CMD_VERSION_B;
		m_enumTxCmd[1] = CMD_BEGIN_UPDATE_B;
		m_enumTxCmd[2] = CMD_WRITE_IMAGE_B;
		m_enumTxCmd[3] = CMD_EXIT_UPDATE_FW_MODE_B;
		m_enumTxCmd[4] = CMD_TRANX_ALL_OK;
	}
	else
	{
		m_enumTxCmd[0] = CMD_WRITE_IMAGE_B;
		m_enumTxCmd[1] = CMD_TRANX_ALL_OK;
	}

	SendCmd();
	return true;
}


bool clsTRANX_RFID::API_bSetOemConfig(TBYTE ucAddr, TBYTE ucData)
{
	if(m_enumTxCmd[0] != CMD_NOTHING || !m_bIsOpen)
		return false;

	m_usOemAddr = ucAddr;
	m_ucOemData = ucData;

	m_enumTxCmd[0] = CMD_ENTER_UPDATE_FW_MODE_B;
	m_enumTxCmd[1] = CMD_SET_OEM_CONFIG_B;
	m_enumTxCmd[2] = CMD_EXIT_UPDATE_FW_MODE_B;
	m_enumTxCmd[3] = CMD_TRANX_ALL_OK;

	SendCmd();
	return true;
}

bool clsTRANX_RFID::API_bVersion()
{
	if(m_enumTxCmd[0] != CMD_NOTHING || !m_bIsOpen)
		return false;

	m_enumTxCmd[0] = CMD_VERSION_B;
	m_enumTxCmd[1] = CMD_TRANX_ALL_OK;

	SendCmd();
	return true;
}

CString clsTRANX_RFID::API_AskVersion(void)
{
	return  m_cstrVersion;
}



bool clsTRANX_RFID::API_SetRegion(ENUM_REGION enumType)
{
	if(m_enumTxCmd[0] != CMD_NOTHING || !m_bIsOpen)
		return false;

	m_enumTxCmd[0] = CMD_ENTER_UPDATE_FW_MODE_B;
	m_enumTxCmd[1] = CMD_TRANX_ALL_OK;

	SendCmd();
	return true;
}

TCHAR clsTRANX_RFID::API_AskCmdStatus(void)
{
	return m_ucRxStatus;
}

UINT clsTRANX_RFID::API_AskProgressBarStep(void)
{
	if(m_uiTotalFileCnt == 0 || m_uiCurFileCnt ==0)
		return 0;

	UINT uiPercent = (m_uiCurFileCnt*100)/m_uiTotalFileCnt;
	return uiPercent>100?100:uiPercent;
}

//==================CMD===============================
bool  clsTRANX_RFID::Tx_bVersion()
{
	ZeroMemory(m_ucTxBuf, sizeof(m_ucTxBuf));

	m_ucTxBuf[0] = CMD_VERSION_B;
	m_ucTxBuf[1] = 0x03;
	m_ucTxBuf[2] = 0x00;

#ifdef _DEBUG
	OutputDebugString( _T("Tx CMD_VERSION_B\n") );
#endif

	return WriteData(m_ucTxBuf);
}

bool clsTRANX_RFID::MacReadOemData(TBYTE OemAddr, TBYTE& OemValue)//add by rick 0410
{
	ZeroMemory(m_ucTxBuf, sizeof(m_ucTxBuf));

	m_ucTxBuf[0] = CMD_READ_IMAGE_OEM_A;
	m_ucTxBuf[1] = 0x04;
	m_ucTxBuf[2] = 0x00;
	m_ucTxBuf[3] = OemAddr;
	//m_ucTxBuf.append(OemAddr);

	return WriteData(m_ucTxBuf);

}

bool clsTRANX_RFID::Tx_bEnterUpdateFwMode()
{
	ZeroMemory(m_ucTxBuf, sizeof(m_ucTxBuf));

	m_ucTxBuf[0] = CMD_ENTER_UPDATE_FW_MODE_B;
	m_ucTxBuf[1] = 0x02;

#ifdef _DEBUG
	OutputDebugString(_T("Tx CMD_ENTER_UPDATE_FW_MODE_B\n") );
#endif

	return WriteData(m_ucTxBuf);		
}

bool clsTRANX_RFID::Tx_bExitUpdateFwMode()
{
	ZeroMemory(m_ucTxBuf, sizeof(m_ucTxBuf));

	m_ucTxBuf[0] = CMD_EXIT_UPDATE_FW_MODE_B;
	m_ucTxBuf[1] = 0x2;

#ifdef _DEBUG
	OutputDebugString( _T("Tx CMD_EXIT_UPDAYE_FW_MODE_B\n") );
#endif
	
	return WriteData(m_ucTxBuf);
}


bool clsTRANX_RFID::Tx_bBeginUpdate()
{
	ZeroMemory(m_ucTxBuf, sizeof(m_ucTxBuf));

	m_ucTxBuf[0] = CMD_BEGIN_UPDATE_B;
	m_ucTxBuf[1] = 0x02;

#ifdef _DEBUG
	OutputDebugString( _T("Tx CMD_BEGIN_UPDATE_B\n") );
#endif

	return WriteData(m_ucTxBuf);
}


bool clsTRANX_RFID::Tx_bWriteImage(void)
{
	if( (m_uiTotalFileCnt == 0)					|| 
		(m_uiCurFileCnt>m_uiTotalFileCnt)		||
		(m_hdlImageFile == INVALID_HANDLE_VALUE)   )
		return false;
	
	DWORD dwRead = 0;	
	
	//add by yo chen
	/*DevType = API_AskDevType();
	stop_write_image = false;
	if(m_uiCurFileCnt == 0x19e)
		int stop = 1;*/

	if(curDevType == "USB"){
		ZeroMemory(m_ucTxBuf, sizeof(m_ucTxBuf) );
#ifdef _UNICODE

		UCHAR tmpData[PER_PACKET_UPDATE_IMAGE];
		ZeroMemory(tmpData, sizeof(tmpData));

		ReadFile(m_hdlImageFile, tmpData, PER_PACKET_UPDATE_IMAGE, &dwRead, NULL);

		m_ucTxBuf[0] = CMD_WRITE_IMAGE_B;
		m_ucTxBuf[1] = PER_PACKET_UPDATE_IMAGE + 2;
	 
		for(int i=0; i<USB_LENGTH; i++)   m_ucTxBuf[i+2] = tmpData[i];

#else	
		m_ucTxBuf[0] = CMD_WRITE_IMAGE_B;
		m_ucTxBuf[1] = PER_PACKET_UPDATE_IMAGE + 2;
		USB_LENGTH;
		ReadFile(m_hdlImageFile, m_ucTxBuf+2, PER_PACKET_UPDATE_IMAGE, &dwRead, NULL);
		
		//if(m_ucTxBuf[3] == 0xff && m_ucTxBuf[4] == 0xff)
		//{
		//	//stop_write_image = true;
		//	complete_times++;
		//}
		//if(complete_times == 2)
		//	stop_write_image = false;
#endif
		if(dwRead == 0)
		{
			#ifdef _DEBUG
				OutputDebugString( _T("Tx CMD_WRITE_IMAGE_B Data is 0\n") );
			#endif

			return false;
		}
#ifdef _DEBUG
		OutputDebugString( _T("Tx CMD_WRITE_IMAGE_B\n") );
#endif
		return WriteData(m_ucTxBuf);
	}
	//END, if(curDevType == "USB") USB

//=====if(curDevType == "UART")=====//
	else 
	{
		ZeroMemory(m_ucTxBuf_UART, sizeof(m_ucTxBuf_UART) );
#ifdef _UNICODE

		UCHAR tmpData[PER_PACKET_UPDATE_IMAGE_UART];
		ZeroMemory(tmpData, sizeof(tmpData));
		
		ReadFile(m_hdlImageFile, tmpData, PER_PACKET_UPDATE_IMAGE_UART, &dwRead, NULL);

		m_ucTxBuf_UART[0] = CMD_WRITE_IMAGE_B;
		m_ucTxBuf_UART[1] = PER_PACKET_UPDATE_IMAGE_UART + 2;
	 
		for(int i=0; i<UART_LENGTH; i++)   m_ucTxBuf_UART[i+2] = tmpData[i];
		
		/*m_ucTxBuf_UART[0] = CMD_WRITE_IMAGE_B;
		m_ucTxBuf_UART[1] = PER_PACKET_UPDATE_IMAGE_UART + 2;

		ReadFile(m_hdlImageFile, m_ucTxBuf_UART+2, PER_PACKET_UPDATE_IMAGE_UART, &dwRead, NULL);*/
#else	
		m_ucTxBuf_UART[0] = CMD_WRITE_IMAGE_B;
		m_ucTxBuf_UART[1] = PER_PACKET_UPDATE_IMAGE_UART + 2;
		
		ReadFile(m_hdlImageFile, m_ucTxBuf_UART+2, PER_PACKET_UPDATE_IMAGE_UART, &dwRead, NULL);
		/*if(m_ucTxBuf_UART[3] == 0xff && m_ucTxBuf_UART[4] == 0xff)
			stop_write_image = false;*/
#endif
		if(dwRead == 0)
		{
			#ifdef _DEBUG
				OutputDebugString( _T("Tx CMD_WRITE_IMAGE_B Data is 0\n") );
			#endif

			return false;
		}
#ifdef _DEBUG
		OutputDebugString( _T("Tx CMD_WRITE_IMAGE_B\n") );
#endif
		return WriteData(m_ucTxBuf_UART);
	}
	//END, if(curDevType == "UART") UART
}


bool clsTRANX_RFID::Tx_bSetOemConfig(void)
{
	ZeroMemory(m_ucTxBuf, sizeof(m_ucTxBuf));

	m_ucTxBuf[0] = CMD_SET_OEM_CONFIG_B;
	m_ucTxBuf[1] = 0x05;
	memcpy(m_ucTxBuf+2, &m_usOemAddr,sizeof(USHORT));
	m_ucTxBuf[4] = m_ucOemData;


#ifdef _DEBUG
	OutputDebugString( _T("Tx CMD_SET_OEM_CONFIG_B\n") );
#endif

	return WriteData(m_ucTxBuf);
}

TBYTE clsTRANX_RFID::API_RegularGetOemData()
{
	TBYTE RxChar = 0;
	UCHAR	  *ucRxBuf = new UCHAR[USB_LENGTH];
	ENUM_CMD  enuRdCmd = CMD_NOTHING;
	ZeroMemory(ucRxBuf, sizeof(ucRxBuf));

	UINT length = API_AskRevCount();
	UINT uiRead = 0;	
	uiRead = API_Read(ucRxBuf, length); //length = 0x40
/*	if( 0 ==  uiRead )
		return 0;
	if(length <= 0)
		return 0;*/
	
	if(ucRxBuf[0]==0xa7)
	{
		for(UINT i=0 ; i< 4; i++)
		{
			if(m_ucRxCnt>= USB_LENGTH || m_ucRxCnt>=uiRead)		
				m_ucRxCnt = 0;
			if(i>=USB_LENGTH)
				break;

			RxChar = m_ucRxBuf[m_ucRxCnt] = ucRxBuf[i];
		}
	}
	return RxChar;

}

/*TBYTE clsTRANX_RFID::API_RegularGetOemSerialNumber()
{
	TBYTE RxChar = 0;
	UCHAR	  *ucRxBuf = new UCHAR[USB_LENGTH];
	ENUM_CMD  enuRdCmd = CMD_NOTHING;
	ZeroMemory(ucRxBuf, sizeof(ucRxBuf));

	UINT length = API_AskRevCount();
	UINT uiRead = 0;	
	uiRead = API_Read(ucRxBuf, length); //length = 0x40



}*/

//==============================================
//enuRdCmd:  if return Cmd, it means that receive Packet and handle.
//           if return CMD_NOTHING, it means idle or receiveing data
ENUM_CMD WINAPI clsTRANX_RFID::API_Regular(int iTime)
{
	TBYTE	  RxChar = 0;
	//UCHAR	  ucRxBuf[USB_LENGTH];
	UCHAR	  *ucRxBuf = new UCHAR[USB_LENGTH];
	ENUM_CMD  enuRdCmd = CMD_NOTHING;

	ZeroMemory(ucRxBuf, sizeof(ucRxBuf));

	if(m_enumTxCmd[0] == CMD_NOTHING || !m_bIsOpen)
	{
		return CMD_NOTHING;
	}

	if( m_TimeCnt >= 200)
	{
		ResetFlow();
		m_bIsOpen = false;
		return CMD_TIME_OUT;	
	}
	else if( (m_TimeCnt>=100) && !(m_TimeCnt % 100) )
	{
		m_TimeCnt+=iTime;
		WriteData(m_ucTxBuf);
		return CMD_NOTHING;
	}	
	else
		m_TimeCnt+=iTime;

	//add by yo chen
	UINT length = API_AskRevCount();
	UINT uiRead = 0;	
	uiRead = API_Read(ucRxBuf, length); //length = 0x40
	if( 0 ==  uiRead )
		return enuRdCmd;
	if(length <= 0)
		return enuRdCmd;

	//UINT uiRead = m_clsDeviceInterface->API_Read(ucRxBuf, 64);
	for(UINT i=0 ; i< uiRead; i++)
	{
		if(m_ucRxCnt>= USB_LENGTH || m_ucRxCnt>=uiRead)		
			m_ucRxCnt = 0;
		if(i>=USB_LENGTH)
			break;


		RxChar = m_ucRxBuf[m_ucRxCnt] = ucRxBuf[i];
		switch(m_ucRxStep)
		{
			case STEP_RESPOEES_CMD: 
				if(m_ucTxBuf[0]+1 == RxChar )
				{
					m_ucRxStep++;
					m_ucRxCnt = 1;	//Receive 1 byte	
					m_ucRxCmd = RxChar;
				}
				else
				{
					m_ucRxStep   = 0;
					m_ucRxCnt    = 0;										
				}
				m_ucRxStatus = 0;
				break;

			case STEP_LENGTH:
				m_ucRxLength = RxChar;
				m_ucRxCnt++;

				(m_ucRxLength<=USB_LENGTH)?(m_ucRxStep++):(m_ucRxStep = 0);
				break;

			case STEP_DATA:
				
				m_ucRxCnt++;

				//Check Data, avoid dead lock or buffer overfolw
				if(m_ucRxCnt>USB_LENGTH)
				{
					m_ucRxStep = 0;
					break;
				}

				//Check packet, some cmd don't have data.
				if( m_ucRxCnt == m_ucRxLength)  //Receive Data Done
				{
					//Set cmd response status 
					m_ucRxStatus = m_ucRxBuf[2];
					//Set the Packet Cmd to let Usbcontrol Dialog know what data was received.
					enuRdCmd = (ENUM_CMD)m_ucRxCmd;

#ifdef _OPEN_SAS_COM_TRANS_DATA_LOG
LogMsg(m_ucRxBuf, m_ucRxBuf[1], LOG_RX_MSG);
#endif
					CheckPacket();


					//The length of cmd between AP and BL Mode is different.
					//Avoid AP mode to send too much leng. 
					clsUTILITY::DelObj(CLOSE_TYPE_POINT, ucRxBuf);
					return enuRdCmd;
				}
				break;

			default:
				m_ucRxStep = 0;
		}//Out switch
	}//for

	clsUTILITY::DelObj(CLOSE_TYPE_POINT, ucRxBuf);
	return enuRdCmd;
}


void clsTRANX_RFID::CheckPacket(void)
{
	//Reset Step Data
	m_TimeCnt  = 0;
	m_ucRxCnt  = 0;
	m_ucRxStep = 0;

	//Analyze the result of Packet Cmd
	AnalyzePacket();

	SendCmd();
	ZeroMemory(m_ucRxBuf, sizeof(m_ucRxBuf));
}

void clsTRANX_RFID::AnalyzePacket(void)
{
	switch(m_ucRxCmd)
	{
		case CMD_VERSION__BR:
			if(m_ucRxStatus == STATUS_OK)
				m_cstrVersion.SetString((TCHAR *)m_ucRxBuf+3, 45);
			
			ChangeCmd(CHANGE_CMD_TO_NEXT);

#ifdef _DEBUG
	OutputDebugString( _T("RX CMD_VERSION__BR\n") );
#endif
			break;

		case CMD_ENTER_UPDATE_FW_MODE_BR:
			ChangeCmd(CHANGE_CMD_TO_NEXT);

#ifdef _DEBUG
	OutputDebugString( _T("RX CMD_ENTER_UPDATE_FW_MODE_BR\n") );
#endif
			break;	


		case CMD_EXIT_UPDATE_FW_MODE_BR:
			m_uiCurFileCnt   = 0;
			m_uiTotalFileCnt = 0;
			ChangeCmd(CHANGE_CMD_TO_NEXT);

#ifdef _DEBUG
	OutputDebugString( _T("RX CMD_EXIT_UPDAYE_FW_MODE_BR\n") );
#endif
			break;	


		case CMD_BEGIN_UPDATE_BR:
			ChangeCmd(CHANGE_CMD_TO_NEXT);

#ifdef _DEBUG
	OutputDebugString( _T("RX CMD_BEGIN_UPDATE_BR\n") );
#endif
			break;

		case CMD_WRITE_IMAGE_BR:
			HandleWriteImageResult();

#ifdef _DEBUG
	OutputDebugString( _T("RX CMD_WRITE_IMAGE_BR\n") );
#endif
			break;

		case CMD_SET_OEM_CONFIG_BR:
			//Reset tmp Data
			m_usOemAddr    = 0;
			m_ucOemData    = 0;
			ChangeCmd(CHANGE_CMD_TO_NEXT);

#ifdef _DEBUG
	OutputDebugString( _T("RX CMD_SET_OEM_CONFIG_BR\n") );
#endif
			break;

		default :
			ChangeCmd(CHANGE_CMD_TO_NEXT);
			m_ucRxStep = 0;
	}//switch
}



void clsTRANX_RFID::HandleWriteImageResult(void)
{
	m_uiCurFileCnt++;

	switch(m_ucRxStatus)
	{
		case STATUS_OK:
			//Transfer Completely
			if(m_uiCurFileCnt == m_uiTotalFileCnt)
			{
				m_hdlImageFile = INVALID_HANDLE_VALUE;
				ChangeCmd(CHANGE_CMD_TO_NEXT);
			}
			break;

		case STATUS_ERROR_OVER_RANGE:
			m_uiCurFileCnt   = 0;
			m_uiTotalFileCnt = 0;
			m_hdlImageFile   = INVALID_HANDLE_VALUE;
			ChangeCmd(CHANGE_CMD_TO_NEXT);
			break;

		case STATUS_ERROR_INVALID_DATA_LENGTH:
		case STATUS_ERROR_INVALID_PARAMETER:
		case STATUS_ERROR_WRONG_PROCEDURE:
		case STATUS_ERROR_SYS_MODULE_FAILURE:
			ChangeCmd(CHANGE_CMD_TO_NEXT);
			break;

		default:
			//if Error,skip "write image" step. 
			ResetFlow();

			break;
	}
}


void clsTRANX_RFID::SendCmd(void)
{
	switch(m_enumTxCmd[m_ucTxCnt])
	{
		case CMD_VERSION_B:
			Tx_bVersion();
			break;

		case CMD_ENTER_UPDATE_FW_MODE_B:
			Tx_bEnterUpdateFwMode();
			break;

		case CMD_EXIT_UPDATE_FW_MODE_B:
			Tx_bExitUpdateFwMode();
			//add 3seconds delay 
			waitingResponseDelay(3);
			break;

		case CMD_BEGIN_UPDATE_B:
			Tx_bBeginUpdate();
			break;

		case CMD_WRITE_IMAGE_B:
			Tx_bWriteImage();
			break;

		case CMD_SET_OEM_CONFIG_B:
			Tx_bSetOemConfig();
			break;

		case CMD_NOTHING:
		case CMD_TRANX_ALL_OK:
			m_enumTxCmd[0] = CMD_NOTHING;
			m_ucTxCnt = 0;
			break;
	}
}

void clsTRANX_RFID::ResetFlow()
{
	m_ucTxCnt        = 0;
	m_ucRxStep       = 0;
	m_ucRxCnt        = 0;
	m_ucRxStatus     = 0;
	m_TimeCnt        = 0;
	m_uiCurFileCnt   = 0;
	m_uiTotalFileCnt = 0;
	m_hdlImageFile   = INVALID_HANDLE_VALUE;
	m_cstrVersion.Empty();

	ZeroMemory(m_enumTxCmd, sizeof(m_enumTxCmd));
	ZeroMemory(m_ucRxBuf,   sizeof(m_ucRxBuf));
}

bool clsTRANX_RFID::WriteData(TBYTE *ucTxBuf)
{
#ifdef _OPEN_SAS_COM_TRANS_DATA_LOG
LogMsg(ucTxBuf, m_ucTxBuf[1], LOG_TX_MSG);
#endif

	/*CString curDevType;
	curDevType = API_AskDevType();*/


#ifdef _UNICODE
	//=========temp mark
	//int size = 0;
	//if(curDevType == "USB")
	//{
	//	size = USB_LENGTH;
	//}
	//else if(curDevType == "RS232")
	//{
	//	size = 128;
	//}
	//else
	//{
	//	return false;
	//}

	////UCHAR tmpData[size];
	//UCHAR *tmpData = new UCHAR[size];
	//ZeroMemory(tmpData, sizeof(tmpData));	
	//for(int i=0; i<size; i++)   tmpData[i] = *(ucTxBuf+i) & 0xFF;

	//bool bResult = m_clsDeviceInterface->API_Write(tmpData, size);
	//delete tmpData;

	//return bResult;
	////return m_clsDeviceInterface->API_Write(tmpData, size);
	//=========temp mark

	if(curDevType == "USB")
	{
		UCHAR *tmpData = new UCHAR[USB_LENGTH];
		ZeroMemory(tmpData, sizeof(tmpData));	
		for(int i=0; i<USB_LENGTH; i++)   			tmpData[i] = *(ucTxBuf+i) & 0xFF;

		bool bResult = !!m_clsDeviceInterface->API_Write(tmpData, USB_LENGTH);
		delete tmpData;

		return bResult;
	}

	else//if(curDevType == "UART")
	{	
		//===Write===
		//UCHAR tmp[128];
		//memset(tmp, 0, sizeof(tmp));
		//tmp[0] = 0x10;//CMD_MAC_GET_MODULE_ID_A;
		//tmp[1] = 0x03;
		//tmp[2] = 0x00;//uiID & 0xFF;
		UCHAR *tmpData = new UCHAR[UART_LENGTH];
		ZeroMemory(tmpData, sizeof(tmpData));	
		for(int i=0; i<ucTxBuf[1]; i++)   tmpData[i] = *(ucTxBuf+i) & 0xFF;
		
		//Declaration
		UCHAR	    TxBuf[256];
		//END, Declaration

		int length = 0;//ini length
		ZeroMemory(TxBuf, sizeof(TxBuf));//ini TxBuf

		TxBuf[0] = 'M';//TxBuf[0:3] = Header
		TxBuf[1] = 'T';
		TxBuf[2] = 'I';
		TxBuf[3] = 'C';
		TxBuf[4] = 0xff;//TxBuf[4] = Device ID
		memcpy(TxBuf+5, tmpData, tmpData[1]);

		length = TxBuf[6] + 5;//TxBuf[6] = CMD length(only CMD without header/deviceID/CRC). use it to set real packet length (not include checksum)

		USHORT tmpCRC   = ~CulCRC(TxBuf, length*8);
		TxBuf[length++] = tmpCRC >> 8;//the real length add 2 byte is the checksum
		TxBuf[length++] = tmpCRC & 0xFF;

		delete tmpData;

		return API_Write(TxBuf, length);
	}

#else
	if(curDevType == "USB")
	{
		return API_Write(m_ucTxBuf, 64);
	}
	else //if(curDevType == "RS232")
	{
		//===Write===
		//UCHAR tmp[128];
		//memset(tmp, 0, sizeof(tmp));
		//tmp[0] = 0x10;//CMD_MAC_GET_MODULE_ID_A;
		//tmp[1] = 0x03;
		//tmp[2] = 0x00;//uiID & 0xFF;
		
		//Declaration
		UCHAR	    TxBuf[256];
		//END, Declaration

		int length = 0;//ini length
		ZeroMemory(TxBuf, sizeof(TxBuf));//ini TxBuf

		TxBuf[0] = 'M';//TxBuf[0:3] = Header
		TxBuf[1] = 'T';
		TxBuf[2] = 'I';
		TxBuf[3] = 'C';
		TxBuf[4] = 0;//TxBuf[4] = Device ID
		memcpy(TxBuf+5, ucTxBuf, ucTxBuf[1]);

		length = TxBuf[6] + 5;//TxBuf[6] = CMD length(only CMD without header/deviceID/CRC). use it to set real packet length (not include checksum)

		USHORT tmpCRC   = ~CulCRC(TxBuf, length*8);
		TxBuf[length++] = tmpCRC >> 8;//the real length add 2 byte is the checksum
		TxBuf[length++] = tmpCRC & 0xFF;

		return API_Write(TxBuf, length);
	}

#endif

}

void clsTRANX_RFID::LogMsg(TBYTE *ucData, UINT ucLength, TBYTE uc_Flag)
{
	TBYTE  uc_Log[512];

    ZeroMemory(uc_Log, sizeof(uc_Log));
    clsUTILITY::BinToHex(ucData, uc_Log, ucLength);

	TCHAR sTemp[512];
    ZeroMemory(sTemp, sizeof(sTemp));

    if (uc_Flag == LOG_TX_MSG)
    {       
		 _stprintf_s(sTemp, _T("  TX => %s ") ,uc_Log);
		 gfLog(sTemp);

    }
    else
    {     
		_stprintf_s(sTemp, _T("  RX => %s ") ,uc_Log);
		gfLog(sTemp);
    }

}



//=====================================
void clsTRANX_RFID::ResetData()
{
	RxStatus  = 0;
	RxCmd	  = 0;
	RxChar	  = 0;	
	RxCnt     = 0;
	RxPktLen  = 0;
	RxDataLen = 0;
	uiTimeOut = 0;
	m_bUpdate = false;
	RxStep    = HEADER_01;
	ZeroMemory(RxBuf, sizeof(RxBuf));
	ZeroMemory(TxBuf, sizeof(TxBuf));
}


//add by yo chen, create a simple GetModuleID function, 2011/08/15
bool clsTRANX_RFID::GetModuleID_new()
{
#ifdef TRY_DEBUG
	OutputDebugString( _T("GetModuleID\n") );
#endif
	//=====(Exit [BL] mode)
	//UCHAR tmp2[128];
	//memset(tmp2, 0, sizeof(tmp2));
	//tmp2[0] = 0xD2;
	//tmp2[1] = 0x02;
	//if(WriteData(tmp2) == false)
	//	return false;
	//Sleep(4000);
	////=====set the para. for API_Read()
	//UCHAR tempRxBuf[128];
	//memset(tempRxBuf, 0 ,128);
	//TBYTE tempRxDataLen = 128;

	//API_Read(tempRxBuf, tempRxDataLen);
	//=====(End, Exit [BL] mode)


	//===Write===
	//UCHAR tmp[128];//it's for no set
	TBYTE tmp[128];//it's for Unicode
	memset(tmp, 0, sizeof(tmp));
	tmp[0] = 0x10;//CMD_MAC_GET_MODULE_ID_A;
	tmp[1] = 0x03;
	tmp[2] = 0x00;//uiID & 0xFF;
	//*********************************************try to use WriteData(tmp);	
	if(WriteData(tmp) == false)
		return false;

	////temp ignore!!!!!!!!!!!!!!!!!!!!
	////Declaration
	//UCHAR	    TxBuf[256];

	////END, Declaration
	//int length = 0;//ini length
	//ZeroMemory(TxBuf, sizeof(TxBuf));//ini TxBuf

	//TxBuf[0] = 'M';//TxBuf[0:3] = Header
	//TxBuf[1] = 'T';
	//TxBuf[2] = 'I';
	//TxBuf[3] = 'C';
	//TxBuf[4] = 0;//TxBuf[4] = Device ID
	//memcpy(TxBuf+5, tmp, tmp[1]);//memcpy(dest, src, copy length)

	//length = TxBuf[6] + 5;//TxBuf[6] = CMD length(only CMD without header/deviceID/CRC). use it to set real packet length (not include checksum)

	//USHORT tmpCRC   = ~CulCRC(TxBuf, length*8);
	//TxBuf[length++] = tmpCRC >> 8;//the real length add 2 byte is the checksum
	//TxBuf[length++] = tmpCRC & 0xFF;
	//if(API_Write(TxBuf, length) == false)
	//	return false;
	////temp ignore!!!!!!!!!!!!!!!!!!!!

	//===Read===
	//Declaration
	UINT	    Length = 0;
	UCHAR	    RxBuf[256];
	UCHAR	    RxChar = 0;//read data step by step
	TBYTE	    RxCnt = 0;//increase the RxBuf[] from header. "RxBuf[RxCnt]"
	TBYTE	    RxCmd = 0;//storage the real CMD
	TBYTE	    RxPktLen = 0;//all packet length
	TBYTE	    RxDataLen = 0;//CMD length
	TBYTE	    RxStep = 0;
	TBYTE	    RxStatus = -1;//storage the CMD[6] = status
	//storage the FW version
	
	Sleep(20);
	Length = API_AskRevCount();
	if(Length <= 0)	
		return false;
	/*UCHAR rData[48];
	memset(rData, 0 ,48);*/
	
	for(RxStep = 0; RxStep <=7; RxStep++)
	{	
		if(RxStep <= 6)
			API_Read(&RxChar, 1);
		if(RxChar == 'M')
		{			
			RxBuf[RxCnt] = RxChar;
			RxCnt++;				
		}
		else if(RxChar == 'T')
		{
			RxBuf[RxCnt] = RxChar;
			RxCnt++;
		}
		else if(RxChar == 'I')
		{
			RxBuf[RxCnt] = RxChar;
			RxCnt++;
		}
		else if(RxChar == 'R')
		{
			RxBuf[RxCnt] = RxChar;
			RxCnt++;
		}
		else if(RxChar == 0x00)//RxBuf[4] = Device ID	//it doens't judge the correction -> (RxChar == 0)
		{
			RxBuf[RxCnt] = RxChar;
			RxCnt++;
		}
		else if(RxChar == 0x11)//RxBuf[5] = CMD ID	//it doens't judge the correction -> (RxChar == 0x11)
		{
			RxBuf[RxCnt] = RxChar;
			RxCnt++;

			RxCmd = RxChar;
		}
		else if(RxChar == 0x30 && RxStep == 6)//RxBuf[6] = CMD length	//it doens't judge the correction
		{
			RxBuf[RxCnt] = RxChar;
			
			RxDataLen = RxChar;				
			RxPktLen  = RxDataLen + 7;
			if(RxDataLen !=0 )//***why use this if()
			{
				RxCnt++;
			}				
		}
		else if(RxStep == 7)//handle packet & check CRC	//it doens't judge the correction
		{			
			if( API_AskRevCount() <  RxDataLen )
				return false;
			//RxStep = 0;
			API_Read(RxBuf+RxCnt, RxDataLen);
			//CRC Check
			if( 0x1D0F == CulCRC(RxBuf, RxPktLen*8) )
			{
				RxStatus = RxBuf[7];

				UCHAR charFwVersion[128];
				memset(charFwVersion, 0 ,128);
				for(int i = 0; RxBuf[8+i] != 0x00; i++)
				{
					charFwVersion[i] = RxBuf[8+i];
				}
				fwVersion = charFwVersion;//draw to upper layer (CPageBssic.cpp)
				//m_EditVersion.SetWindowText(fwVersion);

				//result   = (ENUM_RESULT)RxCmd;
				memcpy(RxBuf, RxBuf + 5, RxDataLen);
			}
			else
				return false;
		}
		else
			return false;
	}
	
	if(RxStatus == 0)
		return true;
	else
		return false;
}

//add by rick, create a simple GetModuleName function from OEMRegister, 2014/04/17
bool clsTRANX_RFID::GetModuleName_new(TBYTE OemAddress, UCHAR *OemData)
{
	TBYTE tmp[128];//it's for Unicode
	memset(tmp, 0, sizeof(tmp));
	tmp[0] = 0xa6;//CMD_READ_IMAGE_OEM_A;
	tmp[1] = 0x04;
	tmp[2] = 0x00;
	tmp[3] = OemAddress;
	//*********************************************try to use WriteData(tmp);	
	if(WriteData(tmp) == false)
		return false;

	UINT	    Length = 0;
	UCHAR	    RxBuf[256];
	UCHAR	    RxChar = 0;//read data step by step
	TBYTE	    RxCnt = 0;//increase the RxBuf[] from header. "RxBuf[RxCnt]"
	TBYTE	    RxCmd = 0;//storage the real CMD
	TBYTE	    RxPktLen = 0;//all packet length
	TBYTE	    RxDataLen = 0;//CMD length
	TBYTE	    RxStep = 0;
	TBYTE	    RxStatus = -1;//storage the CMD[6] = status

	Sleep(20);
	Length = API_AskRevCount();
	if(Length <= 0)	
		return false;

	API_Read(RxBuf, 11);
	OemData[0]=RxBuf[8];
	RxStatus=RxBuf[7];

	/*for(RxStep = 0; RxStep <=7; RxStep++)
	{
		if(RxStep <= 6)
			//API_Read(&RxChar, 11);
			API_Read(RxBuf, 11);
		if(RxChar == 'M')
		{
			RxBuf[RxCnt] = RxChar;
			RxCnt++;
		}
		else if(RxChar == 'T')
		{
			RxBuf[RxCnt] = RxChar;
			RxCnt++;
		}
		else if(RxChar == 'I')
		{
			RxBuf[RxCnt] = RxChar;
			RxCnt++;
		}
		else if(RxChar == 'R')
		{			
			RxBuf[RxCnt] = RxChar;
			RxCnt++;				
		}
		else if(RxChar == 0x00)//RxBuf[4] = Device ID	//it doens't judge the correction -> (RxChar == 0)
		{
			RxBuf[RxCnt] = RxChar;
			RxCnt++;
		}
		else if(RxChar == 0xa7)//RxBuf[5] = CMD ID	//it doens't judge the correction -> (RxChar == 0x11)
		{
			RxBuf[RxCnt] = RxChar;
			RxCnt++;

			RxCmd = RxChar;
		}
		else if(RxChar == 0x30 && RxStep == 6)//RxBuf[6] = CMD length	//it doens't judge the correction
		{
			RxBuf[RxCnt] = RxChar;
			
			RxDataLen = RxChar;				
			RxPktLen  = RxDataLen + 7;
			if(RxDataLen !=0 )//***why use this if()
			{
				RxCnt++;
			}				
		}
		else if(RxStep == 7)
		{
			if( API_AskRevCount() <  RxDataLen )
				return false;
			API_Read(RxBuf+RxCnt, RxDataLen);

		}
		else
				return false;
	}*/
	

	if(RxStatus == 0)
		return true;
	else
		return false;
}




//add by yo chen, send update CMD in UART, 2011/08/23
bool clsTRANX_RFID::sendUARTCmd(UCHAR *buf)
{
//===Write===
	
	//END, Declaration
	int length = 0;//ini length
	ZeroMemory(TxBuf, sizeof(TxBuf));//ini TxBuf

	TxBuf[0] = 'M';//TxBuf[0:3] = Header
	TxBuf[1] = 'T';
	TxBuf[2] = 'I';
	TxBuf[3] = 'C';
	TxBuf[4] = 0;//TxBuf[4] = Device ID
	memcpy(TxBuf+5, buf, buf[1]);

	length = TxBuf[6] + 5;//TxBuf[6] = CMD length(only CMD without header/deviceID/CRC). use it to set real packet length (not include checksum)
	
	USHORT tmpCRC   = ~CulCRC(TxBuf, length*8);
	TxBuf[length++] = tmpCRC >> 8;//the real length add 2 byte is the checksum
	TxBuf[length++] = tmpCRC & 0xFF;
	if(API_Write(TxBuf, length) == false)
		return false;
	else
		return true;
}
//END by yo chen

bool clsTRANX_RFID::API_UpdateFw_UART_Init(HANDLE hdlImageFile, LARGE_INTEGER strcLargeInt)
{
	if(strcLargeInt.LowPart == 0)
		return false;
	updateDev = 1;

	m_uiTotalFileCnt =  (strcLargeInt.LowPart / PER_PACKET_UPDATE_IMAGE_UART) +		 
						((strcLargeInt.LowPart % PER_PACKET_UPDATE_IMAGE_UART)?1:0 );
	m_hdlImageFile = hdlImageFile;
	m_uiCurFileCnt = 0;	

	//SendCmd();
	return true;
}

bool clsTRANX_RFID::API_UpdateFw_UART_Write()
{
	/*while(m_uiCurFileCnt < m_uiTotalFileCnt)
	{*/
	if(false == Tx_bWriteImage())
		return false;
	else
	{
		/*UCHAR tempRxBuf[128];
		memset(tempRxBuf, 0 ,128);
		TBYTE tempRxDataLen = 128;
		TBYTE	m_ucRxStatus;*/

		//===Key Point!!! after write image, it must wait for a while then read
		//===or u have to write data by overlapped!!
		Sleep(30);
		memset(tempRxBuf_2, 0 ,128);
		API_Read(tempRxBuf_2, tempRxDataLen_2);//try to read data (to clear the buffer)			
		m_ucRxStatus = tempRxBuf_2[7];
		if(m_ucRxStatus == 0x00)
		{
			m_uiCurFileCnt++;
		}
		else
			return false;
	}
	return true;
		//if(stop_write_image == true)
		//	break;
	/*}*/
}

#define POLY 0x1021
USHORT clsTRANX_RFID::CulCRC(unsigned char *buf, unsigned short bit_length)
{
    unsigned short shift=0, data=0, val=0;
    int i;

	shift = 0xFFFF;

	for ( i = 0; i < bit_length; i++ ){
        if ((i % 8) == 0)
			data = (*buf++) << 8;

		val = shift ^ data;
		shift = shift << 1;
		data = data << 1;

		if (val & 0x8000)
			shift = shift ^ POLY;		
	}	

	return shift;
}	

void clsTRANX_RFID::waitingResponseDelay(int sec)//add by rick for delay 3 seconds 2014-03-28
{
	time_t start_time, cur_time;
	time(&start_time);
	do
	{
		time(&cur_time);
	}while((cur_time-start_time)<sec);
}
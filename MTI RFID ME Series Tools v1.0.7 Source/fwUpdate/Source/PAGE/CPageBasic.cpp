#include "stdafx.h"
#include "USBControl.h"
#include "CPageBasic.h"

// CPageBasic dialog

IMPLEMENT_DYNAMIC(CPageBasic, CDialog)

CPageBasic::CPageBasic(CWnd* pParent /*=NULL*/)
	: CDialog(CPageBasic::IDD, pParent)
{
	m_uiInterval     = 0;
	m_uiThreadID     = 0;
	m_hdlThread      = INVALID_HANDLE_VALUE;
	m_hdlImageFile   = INVALID_HANDLE_VALUE;
	m_clsTranxRFID   = NULL;
	m_bUpdateFlag    = false;
	m_bIsOpen	     = false;
	m_bIsBusy	     = false;
	m_bThdEnableFlag = false;
	m_bQryExitFwMode = true;
	m_clsTranxRFID   = new clsTRANX_RFID;
	m_strFilePath.Empty();
	m_cstrVersion.Empty();
	ZeroMemory(&m_liBefore		 , sizeof(LARGE_INTEGER));
	ZeroMemory(&m_liNow			 , sizeof(LARGE_INTEGER));
	ZeroMemory(&m_liFreq		 , sizeof(LARGE_INTEGER));
	ZeroMemory(&m_strcLargeInt   , sizeof(LARGE_INTEGER));
	InitializeCriticalSection(&m_CriticalSection);

	

}


CPageBasic::~CPageBasic()
{
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlThread);
	clsUTILITY::DelObj(CLOSE_TYPE_POINT,  m_clsTranxRFID);
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
	DeleteCriticalSection(&m_CriticalSection);
}

void CPageBasic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1_MEMO, m_Memo);
	DDX_Control(pDX, IDC_EDIT2_FW_VERSION, m_EditVersion);
	DDX_Control(pDX, IDC_EDIT2_ModelName, m_EditModelName);//add by rick 2014-04-14
	DDX_Control(pDX, IDC_EDIT3_SerialNumber, m_EditSerialNumber);//add by rick 2014-04-14
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressBar);
	DDX_Control(pDX, IDC_BUTTON2, m_BtnUpdate);
	DDX_Control(pDX, IDC_STATIC_LINK_STATE, m_textLinkState);
	DDX_Control(pDX, IDC_BUTTON1_Open, m_BtnStart);
	DDX_Control(pDX, IDC_COMBO1_slePort, m_cmbPort);
	DDX_Control(pDX, IDC_BUTTON1_BOWSER, m_BtnBrowser);
}


BEGIN_MESSAGE_MAP(CPageBasic, CDialog)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1_BOWSER, &CPageBasic::OnBnClickedButton1_Select)
	ON_BN_CLICKED(IDC_BUTTON2_UPDATE, &CPageBasic::OnBnClickedButton2_Update)
	//ON_CBN_SELCHANGE(IDC_COMBO1_slePort, &CPageBasic::OnCbnSelchangeCombo1sleport)
	ON_BN_CLICKED(IDC_BUTTON1_Open, &CPageBasic::OnBnClickedButton3_Open)
END_MESSAGE_MAP()


BOOL CPageBasic::OnInitDialog()
{
    CDialog::OnInitDialog();
	
	//add by yo chen
	//m_clsThread			 = NULL;
	//m_clsDeviceInterface = NULL;
	m_bStart			 = false;

	//m_clsDeviceInterface = new ClsDeviceInterface;
	//dcb
	m_Dcb.BaudRate = CBR_115200;
	m_Dcb.ByteSize = 8;
	m_Dcb.StopBits = ONESTOPBIT;
	m_Dcb.Parity = NOPARITY;
	//END yo chen

	//add by yo chen
	DetectSerial();
	m_cmbPort.AddString( _T("USB") );
	m_cmbPort.SetCurSel(0);
	//add by yo chen
	endThd = 0;
	endThd_UART = 0;
	isBootThd_UART = false;
	isUSBThdRun = false;
	isUARTRun = false;
	/*ResetData();*/

	//Set SW Version Caption
	GetDlgItem(IDC_EDIT1_SW_VERSION)->SetWindowText(cstrToolVer) ;

	//Set Model Name Caption	
/*	TBYTE ucAddress=0030, ucData=0;
	bool Oemres=m_clsTranxRFID->API_bSetOemConfig(ucAddress, ucData);
	//GetDlgItem(IDC_EDIT2)->ShowWindow();
	for(int i=0; i<15; i++)
	{
		Oemres=m_clsTranxRFID->API_bSetOemConfig(ucAddress, ucData);
		//if(Oemres==true)
		{
			ucData=ucData;
		}
		ucAddress=ucAddress+1;
	}
*/


	//Set Progress bar ¶i«×ªí
	m_ProgressBar.SetRange(0,100);	//Set Range
	m_ProgressBar.SetPos(0);		//Set Position
	m_ProgressBar.SetStep(1);		//if calls the function "SetIt", the bar will run 1 step


	//Link Status Static Text	
	//m_clsFont.CreatePointFont(120,_T("Arial"));
	m_clsFont.CreateFont(20, 0, 0, 0, FW_HEAVY,
                         FALSE, FALSE, FALSE, 0, 
						 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, 
						 _T("Arial")); 
	m_textLinkState.SetFont(&m_clsFont);
	m_Msg.LoadString(STR_ACT_DISCONNECTED);
	m_textLinkState.SetWindowText( m_Msg );
	AddMemo( m_Msg );


	//Set Check Time Frequency
	QueryPerformanceFrequency(&m_liFreq);
	m_liFreq.QuadPart/=1000;  //mini sec


	//Thread
	//mod by yo chen, 2011/07/07
	/*m_clsThread = AfxBeginThread( (AFX_THREADPROC)CheckThread, (LPVOID)this);
	if(m_clsThread == NULL)		return false;*/


    return TRUE;
}



// CPageBasic message handlers
void CPageBasic::AddMemo(CString cstr)
{
	m_Memo.SetWindowText(cstr);
}


//Button=============================================================
void CPageBasic::OnBnClickedButton1_Select()
{
	OPENFILENAME  Dialog;				        //Dialog struct
	TCHAR		  szFile[1024] = TEXT("\0");	//File Name buffer¡@¡@¡@¡@¡@¡@¡@¡@¡@

	//Initial Dialog
	ZeroMemory(&Dialog,  sizeof(OPENFILENAME) );
	Dialog.lStructSize = sizeof(OPENFILENAME);
	Dialog.hwndOwner   = m_hWnd;
	Dialog.lpstrFile   = szFile;

	Dialog.lpstrFile[0]    = _T('\0');
	Dialog.nMaxFile        = sizeof(szFile)/sizeof(TCHAR);
	Dialog.lpstrFilter     = _T("Bin\0*.bin\0All Files\0*.*\0");
	Dialog.nFilterIndex    = 1;
	Dialog.lpstrFileTitle  = NULL;
	Dialog.nMaxFileTitle   = 0;
	Dialog.lpstrInitialDir = NULL;
	Dialog.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	//Show dialog
	if( GetOpenFileName(&Dialog) )
	{
		m_strFilePath = szFile;

		//Show Path to edit box
		this->GetDlgItem(IDC_EDIT3_PATH)->SetWindowText(m_strFilePath);
		//MessageBox(m_strFilePath,"File Path",MB_OK);
	}
}



void CPageBasic::OnBnClickedButton2_Update()
{	
	CString tempDevType;
	tempDevType = m_clsTranxRFID->API_AskDevType();
	if( tempDevType == "USB" )
		Update();
	else if( tempDevType == "RS232" )
	{
		//if(true == UpdateUART())
		bootThd_UART();
		//AddMemo( _T("Update successfully") );
	}
	else
		AddMemo( _T("Error: No device") );
		
}


void CPageBasic::EnableBtn(bool bEnableFlag)
{
	GetDlgItem(IDC_BUTTON2_UPDATE)->EnableWindow(bEnableFlag);
}

void CPageBasic::EnableBtnOpen(bool bEnableFlagOpen)
{
	GetDlgItem(IDC_BUTTON1_Open)->EnableWindow(bEnableFlagOpen);
}

void CPageBasic::EnableBtnBrowse(bool bEnableFlagBrowse)
{
	GetDlgItem(IDC_BUTTON1_BOWSER)->EnableWindow(bEnableFlagBrowse);
}

bool  CPageBasic::Update(void)
{
	EnterCriticalSection(&m_CriticalSection);

	//If in 0xD6 Write Image status, can't close m_hdlImageFile handle. Return directly.
	if(m_bIsBusy == true)
	{
			m_Msg.LoadString(STR_ERR_BUSY);
		AddMemo( m_Msg );
		LeaveCriticalSection(&m_CriticalSection);
		return false;		
	}

	while(1)
	{
		m_ProgressBar.SetPos(0);

		if( m_bIsOpen == false )
		{
				m_Msg.LoadString(STR_ERR_NO_USB);	
			AddMemo( m_Msg );
			break ;
		}

		if(m_strFilePath.IsEmpty() == true)
		{
				m_Msg.LoadString(STR_ACT_SELECT_DIRECTORY);	
			AddMemo( m_Msg );
			break;
		}

		//==============Hanlde File=======================//
		//If already open file before, close it.
		clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);

		m_hdlImageFile = CreateFile(m_strFilePath, 
									GENERIC_READ,  
						    	    FILE_SHARE_READ, 
									NULL, 
									OPEN_EXISTING, 
									FILE_ATTRIBUTE_NORMAL, 
									0);

		if(m_hdlImageFile == INVALID_HANDLE_VALUE)
		{
				m_Msg.LoadString(STR_ERR_CAN_NOT_OPEN_IMAGE);	
			AddMemo( m_Msg );
			break;
		}


		//==============Hanlde File Size==================//
		
		ZeroMemory(&m_strcLargeInt, sizeof(LARGE_INTEGER));

		//Get File Size
		if(  !GetFileSizeEx(m_hdlImageFile , &m_strcLargeInt) )
		{				
			m_Msg.LoadString(STR_ERR_CAN_NOT_GET_IMAGE_SIZE);	
			AddMemo( m_Msg );
			break;
		}
		
		if(m_strcLargeInt.LowPart == 0)
		{
			m_Msg.LoadString(STR_ERR_FILE_SIZE_ZERO);	
			AddMemo( m_Msg );
			break;
		}


		//==============Enter Update Status==============//
		m_bUpdateFlag = true;
		m_bQryExitFwMode = true;
		EnableBtn(false);
		EnableBtnOpen(false);
		EnableBtnBrowse(false);

		//If in AP Mode, change to BL Mode. Wait next to start update flow.
		//When restart the device, get the version, and judge whether update or not.
		if( CheckMode(AP_MODE) == true )
		{
			m_clsTranxRFID->API_QueryOneCmd(CMD_ENTER_UPDATE_FW_MODE_B);
			AddMemo( _T("Please wait, restart the module. Change mode to BL Mode") );
			break ;
		}

		LeaveCriticalSection(&m_CriticalSection);
		return true;
	}

	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
	LeaveCriticalSection(&m_CriticalSection);
	return false;
}

//original Thread====================================
UINT WINAPI CPageBasic::CheckThread(LPVOID r_vpAppCls)
{	UINT uiAskVersionTime = 0;
	UINT uiInterval =0;
	CPageBasic *thisCls = (CPageBasic *)r_vpAppCls;

	
	bool ReadOemRes=false;
	TBYTE OemData, ReceivdData, ModelName[17]/*SerialNumber[17]*/;
	char ModelNameChar[17], SerialNumberChar[17];

	while(thisCls->endThd)
	{	
		if(thisCls->m_bThdEnableFlag == false)
		{
			thisCls->Reset();
			thisCls->m_bUpdateFlag = false;
			Sleep(200);
			continue;
		}

		thisCls->UpdateStatus();	
		Sleep(TIMER_INTERVAL);

		//Calcaulation the time		
		QueryPerformanceCounter(&thisCls->m_liNow);
		uiInterval = (UINT)((thisCls->m_liNow.QuadPart - thisCls->m_liBefore.QuadPart)/thisCls->m_liFreq.QuadPart);
		thisCls->m_liBefore = thisCls->m_liNow;

		//Check time. Avoid timer crash the communication
		thisCls->m_uiInterval += uiInterval;
		uiAskVersionTime      += uiInterval;
		if(thisCls->m_uiInterval >=TIMER_INTERVAL)		thisCls->m_uiInterval = 0;
		else											continue;


		//Check Device status. If USB is disconnect, try to link.
		if( thisCls->m_bIsOpen == false && thisCls->endThd != 0)
		{
				thisCls->m_Msg.LoadString(STR_ACT_DISCONNECTED);
			thisCls->m_textLinkState.SetWindowText(thisCls->m_Msg);
			thisCls->AddMemo( thisCls->m_Msg );

			
			if(thisCls->m_clsTranxRFID->API_USB_Open() == true)
			{
				QueryPerformanceCounter(&thisCls->m_liBefore);
				thisCls->m_uiInterval = 0;

				thisCls->m_Msg.LoadString(STR_ACT_CONNECTED);
				thisCls->m_textLinkState.SetWindowText( thisCls->m_Msg );
				thisCls->AddMemo( thisCls->m_Msg );

				TBYTE OemAddr=0x30;
				int i=0;
				do
				{
					ReadOemRes=thisCls->m_clsTranxRFID->MacReadOemData(OemAddr, OemData);
					ReceivdData=thisCls->m_clsTranxRFID->API_RegularGetOemData();
					//ModelName[i]=ReceivdData;
					ModelNameChar[i]=(char)ReceivdData;
					OemAddr=OemAddr+1;
					Sleep(20);
					i++;
				}while(i<16);

				/*if(ModelName[0]==0x00)
				{
					ZeroMemory(ModelNameChar, sizeof(ModelNameChar));
					for(int j=0; j<17; j++)
					{
						ModelNameChar[j]=ModelName[j+1];
					}
				}*/
				CString           m_cstrModelName(ModelNameChar);
				thisCls->m_EditModelName.SetWindowText(m_cstrModelName);
	
				OemAddr=0x50;
				i=0, OemData=0;
				do
				{
					ReadOemRes=thisCls->m_clsTranxRFID->MacReadOemData(OemAddr, OemData);
					ReceivdData=thisCls->m_clsTranxRFID->API_RegularGetOemData();
					//ModelName[i]=ReceivdData;
					SerialNumberChar[i]=(char)ReceivdData;
					OemAddr=OemAddr+1;
					Sleep(20);
					i++;
				}while(i<16);

				/*if(ModelName[0]==0x00)
				{
					ZeroMemory(ModelNameChar, sizeof(ModelNameChar));
					for(int j=0; j<17; j++)
					{
						ModelNameChar[j]=ModelName[j+1];
					}
				}*/

				CString m_cstrSerialNumber(SerialNumberChar);
				thisCls->m_EditSerialNumber.SetWindowText(m_cstrSerialNumber);

				//Show Version. It means that the USB is linked
				thisCls->m_clsTranxRFID->API_bVersion();

				


			}
			continue ;
		}


		while(1)
		{
			//Communication
			if(thisCls->m_bIsBusy == false )
			{
				//Ask version to check status
				if(uiAskVersionTime >= ASK_VERSION_INTERVAL )
				{
					uiAskVersionTime = 0;
					thisCls->m_clsTranxRFID->API_bVersion();
				}
				else
				{
					uiAskVersionTime += TIMER_INTERVAL;
				}

				break;
			}

			//Pooling===============================================
			ENUM_CMD enumRt = thisCls->m_clsTranxRFID->API_Regular(TIMER_INTERVAL);
			thisCls->AnalyzePacket( enumRt );
			
			while(1)
			{
				//if not in idle, don't do next steps
				if(thisCls->m_bIsBusy == true)
					break;

				//Check the device in AP mode. 
				//If require updating, don't return to AP mode.
				if(thisCls->m_bUpdateFlag == false)
				{
					if(thisCls->m_bQryExitFwMode == true )
					{
						if( thisCls->CheckMode(BL_MODE) == true )
						{
							thisCls->AddMemo( _T("Please wait, change mode to AP Mode.") );	
							thisCls->EnableBtn(false);
							thisCls->m_clsTranxRFID->API_QueryOneCmd(CMD_EXIT_UPDATE_FW_MODE_B);					
						}
					}
					else
					{
						if( thisCls->CheckMode(AP_MODE) == true )
							thisCls->m_bQryExitFwMode = true;
						else
						{
							thisCls->AddMemo( _T("Please update Firmware again.") );
							thisCls->EnableBtn(true);
							//thisCls->MessageBox( _T("Please update Firmware again."), _T("Warning"), MB_ICONEXCLAMATION );
							//break;
						}
					}

					break;
				}

				//When restart the device, get the version, and judge whether update or not.
				if( enumRt == CMD_VERSION__BR )
				{				
					if( thisCls->MessageBox( _T("Do you want to update?"), _T("Update"), MB_YESNO|MB_ICONQUESTION ) == IDYES )
					{
						if( thisCls->Update() == true) 
						{
							//Start to update
							if( !thisCls->m_clsTranxRFID->API_UpdateFw(thisCls->m_hdlImageFile, thisCls->m_strcLargeInt) )
							{
								thisCls->m_Msg.LoadString(STR_ERR_BUSY);
								thisCls->AddMemo( thisCls->m_Msg );
							}
							else
								thisCls->AddMemo( _T("Start to Update\n") );
						}
						
					}
					else
					{
						EnterCriticalSection(&thisCls->m_CriticalSection);
						thisCls->m_bUpdateFlag = false;
						thisCls->EnableBtn(true);
						LeaveCriticalSection(&thisCls->m_CriticalSection);

						if( thisCls->CheckMode(BL_MODE) == true )
						{
							thisCls->AddMemo( _T("Please wait, change mode to AP Mode.") );		
							thisCls->EnableBtn(false);
							thisCls->m_clsTranxRFID->API_QueryOneCmd(CMD_EXIT_UPDATE_FW_MODE_B);
						}
					}

					break;		
				}//if

				break;
			}//while i

			break;
		}//while m

		//add by rick 0410
		/*TBYTE OemAddr=0x30;
		int i=0;
		do
		{
			ReadOemRes=thisCls->m_clsTranxRFID->MacReadOemData(OemAddr, OemData);
			ReceivdData=thisCls->m_clsTranxRFID->API_RegularGetOemData();
			ModelName[i]=ReceivdData;
			ModelNameChar[i]=(char)ReceivdData;
			OemAddr=OemAddr+1;
			Sleep(20);
			i++;
		}while(i<16);

		if(ModelName[0]==0x00)
		{
			ZeroMemory(ModelNameChar, sizeof(ModelNameChar));
			for(int j=0; j<17; j++)
			{
				ModelNameChar[j]=ModelName[j+1];
			}
		}
		CString           m_cstrModelName(ModelNameChar);
		thisCls->m_EditModelName.SetWindowText(m_cstrModelName);

		OemAddr=0x50;
		i=0, OemData=0;
		do
		{
			ReadOemRes=thisCls->m_clsTranxRFID->MacReadOemData(OemAddr, OemData);
			ReceivdData=thisCls->m_clsTranxRFID->API_RegularGetOemData();
			ModelName[i]=ReceivdData;
			ModelNameChar[i]=(char)ReceivdData;
			OemAddr=OemAddr+1;
			Sleep(20);
			i++;
		}while(i<16);

		if(ModelName[0]==0x00)
		{
			ZeroMemory(ModelNameChar, sizeof(ModelNameChar));
			for(int j=0; j<17; j++)
			{
				ModelNameChar[j]=ModelName[j+1];
			}
		}

		CString m_cstrSerialNumber(ModelNameChar);
		thisCls->m_EditSerialNumber.SetWindowText(m_cstrSerialNumber);*/

		//CString           m_cstrModelName(ModelNameChar);
    
	}//while o

	return 0;
}


void CPageBasic::UpdateStatus(void)
{
	EnterCriticalSection(&m_CriticalSection);
	m_bIsOpen	   = m_clsTranxRFID->API_IsOpen();
	m_bIsBusy	   = m_clsTranxRFID->API_IsBusy();
	m_cstrVersion  = m_clsTranxRFID->API_AskVersion();
	LeaveCriticalSection(&m_CriticalSection);
}



void CPageBasic::Reset(void)
{
	//Reset bar
	m_Msg.LoadString(STR_ACT_UPDATE);
	m_BtnUpdate.SetWindowText( m_Msg );

	m_ProgressBar.SetPos(0);
 	EnableBtn(true);

	//Set msg
	m_Msg.LoadString(STR_ACT_DISCONNECTED);
	m_textLinkState.SetWindowText( m_Msg );
	m_Msg.LoadString(STR_ACT_DISCONNECTED);
	m_textLinkState.SetWindowText( m_Msg );
	m_EditVersion.SetWindowText( _T("") );
	m_EditModelName.SetWindowText(_T("") );//add by rick, 2014-04-14
	m_EditSerialNumber.SetWindowText(_T("") );//add by rick, 2014-04-16

	//clear flag
	m_bQryExitFwMode = true;

	//Close communication
	m_clsTranxRFID->API_CloseComm();
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
	UpdateStatus();	
}

//==========================================

void CPageBasic::AnalyzePacket(ENUM_CMD enumRtCmd)
{
	UpdateStatus();

	switch( enumRtCmd )
	{
		/*case 0x000000a7:{
			//CString cstrEditOemData;
			CString cstrEdit;
			Reset();
			break;}*/

		case CMD_TIME_OUT:	
			//AddMemo( _T("Error: Timeout ") );		
			Reset();
			break;

		case CMD_VERSION__BR:{
			CString cstrEdit;
			m_EditVersion.GetWindowText(cstrEdit);

			if(m_cstrVersion != cstrEdit)
				m_EditVersion.SetWindowText(m_cstrVersion); 
			break;}

		case CMD_ENTER_UPDATE_FW_MODE_BR:		
			if(m_clsTranxRFID->API_AskCmdStatus() == STATUS_OK)
			{
				m_clsTranxRFID->API_CloseComm();
			}
			else
			{	
				AddMemo( _T("\"0xD0 Enter Update FW Mode\" fail!!, ") + PrintError());
			}

				m_Msg.LoadString(STR_ACT_UPDATE);
			m_BtnUpdate.SetWindowText( m_Msg );
			break;

		case CMD_EXIT_UPDATE_FW_MODE_BR:		
			switch(m_clsTranxRFID->API_AskCmdStatus())
			{	
				case STATUS_OK:
					AddMemo( _T("Please wait, restart the module. Change to AP mode ") );
					m_clsTranxRFID->API_CloseComm();
					m_bUpdateFlag = false;
					EnableBtn(true);
					break;

				default:
					AddMemo( _T("\"0xD2 Exit Update FW Mode\" fail!!, ") + PrintError());
					m_bQryExitFwMode = false;
			}
			EnableBtn(true);
				m_Msg.LoadString(STR_ACT_UPDATE);
			m_BtnUpdate.SetWindowText( m_Msg );
			break;


		case CMD_BEGIN_UPDATE_BR:
			if(m_clsTranxRFID->API_AskCmdStatus() != STATUS_OK)
			{
				AddMemo( _T("\"0xD4 Begin Update\" fail!!, ") + PrintError());
					m_Msg.LoadString(STR_ACT_UPDATE);
				m_BtnUpdate.SetWindowText( m_Msg );
			}
			break;	


		case CMD_WRITE_IMAGE_BR:{
			TBYTE Status = m_clsTranxRFID->API_AskCmdStatus();

			switch(Status)
			{
				case STATUS_OK:
				case STATUS_ERROR_OVER_RANGE:{
					UINT uiPercent = m_clsTranxRFID->API_AskProgressBarStep();
			
					if((uiPercent == 100) ||  (Status == STATUS_ERROR_OVER_RANGE))//Write Image OK
					{
							m_Msg.LoadString(STR_ACT_UPDATE);
						m_BtnUpdate.SetWindowText( m_Msg );
						clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
						m_bUpdateFlag = false;
						MessageBox(_T("Update succefully"), _T("Update succefully"), MB_OK);
						EnableBtn(true);
						EnableBtnOpen(true);
						EnableBtnBrowse(true);
					}
					else//continue Wrote Image
					{
						//"Update Button" Set the Caption
						CString cstr,tmpStr;
						cstr.Format(_T("%d%%"),uiPercent);
						
						m_BtnUpdate.GetWindowText(tmpStr);
						
						if(tmpStr != cstr)
							m_BtnUpdate.SetWindowText(cstr);								
					}

					//Change Process bar
					if(m_ProgressBar.GetPos() != 100 && uiPercent<=100)
						m_ProgressBar.SetPos(uiPercent);

					break;}

				default:
						m_Msg.LoadString(STR_ACT_UPDATE);
					m_BtnUpdate.SetWindowText( m_Msg );
					AddMemo( _T("\"0xD6 Write Image\" fail!!, ") + PrintError());	
					clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
					m_bUpdateFlag = false;
					MessageBox(_T("Update unsuccefully"), _T("Update unsuccefully"), MB_OK);
					EnableBtn(true);
					EnableBtnOpen(true);
					EnableBtnBrowse(true);
				}//i switch
				break;
							
			break;}

		case CMD_SET_OEM_CONFIG_BR:
			;
			break;		

		case CMD_NOTHING:
		default:
			;
			break;
	}	

	UpdateStatus();
}


CString CPageBasic::PrintError()
{
	TBYTE   ucStatus = 0;     	
	CString strTmp;

	ucStatus = m_clsTranxRFID->API_AskCmdStatus();
	strTmp.Format( _T(" Error code is 0x%02x,  " ), ucStatus);

	switch(ucStatus)
	{
		case STATUS_ERROR_INVALID_DATA_LENGTH:
				m_Msg.LoadString(STR_ERR_CODE_INVALID_LENGTH);
			strTmp+= m_Msg;
			break;

		case STATUS_ERROR_INVALID_PARAMETER:
				m_Msg.LoadString(STR_ERR_CODE_INVALID_PARAMETER);
			strTmp+= m_Msg;
			break;

		case STATUS_ERROR_WRONG_PROCEDURE:
				m_Msg.LoadString(STR_ERR_CODE_WRONG_PROCEDURE);
			strTmp+= m_Msg;
			break;

		case STATUS_ERROR_OVER_RANGE:
				m_Msg.LoadString(STR_ERR_CODE_OVER_RANGE);
			strTmp+= m_Msg;
			break;

		case STATUS_ERROR_SYS_MODULE_FAILURE:
				m_Msg.LoadString(STR_ERR_CODE_SYSTEM_MODULE_FAILLURE);
			strTmp+= m_Msg;
			break;

		default:
				m_Msg.LoadString(STR_ERR_CODE_NO_DEFINE);
			strTmp+= m_Msg;
	}

	return strTmp;
}

bool CPageBasic::CheckMode(TCHAR *CheckMode)
{
	if( m_cstrVersion.IsEmpty()!= true && _tcsncmp(m_cstrVersion.GetString() ,CheckMode, 4)==0)
		return true;
	else
		return false;
}

HBRUSH CPageBasic::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

		// TODO:  Change any attributes of the DC here
		switch (pWnd->GetDlgCtrlID()) 
		{ 
			case IDC_STATIC_LINK_STATE:{
				//Set BK Color
				pDC->SetBkMode(TRANSPARENT);
				HBRUSH B = CreateSolidBrush(RGB(105,105,105));

				//Set String Color
				//Mod by yo chen
				CString strTmp;
				m_textLinkState.GetWindowText(strTmp);
				if(strTmp == "Connected")	pDC->SetTextColor(RGB( 0, 255, 0));
				//if(m_clsTranxRFID->API_IsOpen() == true)	pDC->SetTextColor(RGB( 0, 255, 0));
				/*if( m_clsTranxRFID->API_USB_Open(PRODUCT_ID , VENDOR_ID) == true )	pDC->SetTextColor(RGB( 0, 255, 0));					
				else if( m_clsTranxRFID->API_Serial_Open(strTmp, m_Dcb) == true )	pDC->SetTextColor(RGB( 0, 255, 0));*/
				else														pDC->SetTextColor(RGB(255, 0, 0));

				return (HBRUSH) B;
			}break;

			default:
			return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		}

	return hbr;
}



//====================================================
void CPageBasic::API_SetThdEnable(bool bEnableFlag)
{
	
	m_bThdEnableFlag = bEnableFlag;

	//add by yo chen, when change page, seting the thread, 2011/10/03
	//1. IF exit Basic Page (bEnableFlag == false)
	if(bEnableFlag == false)
	{
		if(isUARTRun == true)
			m_clsTranxRFID->API_CloseComm();//release the UART port
		else
		{
			endThd = 0;//terminate the USB thread
			m_clsTranxRFID->API_CloseComm();
		}
	}

	//2. IF open/back Basic Page (bEnableFlag == true)
	else if(bEnableFlag == true)
	{
		if(isUSBThdRun == true)//check if USB thread is running. if Yes, recovery it
			bootThd();//UART has no thread, so just recovery the USB thread
		else if(isUARTRun ==  true)
		{
			OpenUART();

			//add by yo chen, if in UART mode, automatically recovery the FW version, 2011/10/04
			m_Msg.LoadString(STR_ACT_CONNECTED);			
			CString tempSt1;
			int askVerTimeout = 0;
			//m_EditVersion.SetWindowText("");//it's for no set
			m_EditVersion.SetWindowText(_T(""));//it's for Unicode
			m_EditVersion.GetWindowText(tempSt1);
			while( tempSt1 == "" && askVerTimeout < 100 )
			{
				if(true == m_clsTranxRFID->GetModuleID_new())
				{
					m_cstrVersion = m_clsTranxRFID->fwVersion;
					m_EditVersion.SetWindowText(m_cstrVersion);//get FW version string
					m_EditVersion.GetWindowText(tempSt1);
				}
				askVerTimeout++;
			}

			if( ExitBLMode() == false )
				AddMemo( _T("Please update Firmware again.") );
			else
				AddMemo( m_Msg );
		}			
	}	
	//END, add by yo chen, when change page, seting the thread, 2011/10/03
}

//add by yo chen, boot checkThread() when click 'Open' for USB mode, 2011/08/04
bool  CPageBasic::bootThd(void)
{
	InterlockedIncrement(&endThd);
	m_clsThread = AfxBeginThread( (AFX_THREADPROC)CheckThread, (LPVOID)this);
	if(m_clsThread == NULL)
		return false;
	else
		return true;
}
bool  CPageBasic::bootThd_UART(void)
{	
	isBootThd_UART = true;
	InterlockedIncrement(&endThd_UART);
	m_clsThread = AfxBeginThread( (AFX_THREADPROC)CheckThread_UART_Update, (LPVOID)this);
	if(m_clsThread == NULL)
		return false;
	else
		return true;
}

void CPageBasic::OnBnClickedButton3_Open()
{
	
	// TODO: Add your control notification handler code here
	//Load Transfer.dll
	
	//CPageBasic *thisCls;// = (CPageBasic *)r_vpAppCls;
//==================mod by yo chen, 2011/08/04
	if( m_clsTranxRFID->API_IsLoadLib() == false)
	{
		if( false == m_clsTranxRFID->API_LoadLib() )
		{
			AddMemo( _T("Load Transfer.dll unsuccessfully") );
			return;
		}	
		else
		{
			m_cstrDllVer = m_clsTranxRFID->API_AskVersion(  );
		}
	}
//=====================
	
	// TODO: Add your control notification handler code here
	if( m_bStart == true)//when click [Stop] button to disconnect with device
	{	
		CString strDevice;	
		m_cmbPort.GetWindowText(strDevice);
		if(strDevice == "USB")
		{
			InterlockedDecrement(&endThd);
			isUSBThdRun = false;
			m_clsTranxRFID->API_CloseComm();
		}
		else
		{	
			if(isBootThd_UART)
				InterlockedDecrement(&endThd_UART);
			m_clsTranxRFID->API_CloseComm();
			isUARTRun = false;
		}

		m_BtnStart.SetWindowText( _T("Open") );
		m_cmbPort.EnableWindow(true);
		m_bStart = !m_bStart;
		Reset();		

		m_Msg.LoadString(STR_ACT_DISCONNECTED);
		m_textLinkState.SetWindowText( m_Msg );
		AddMemo( m_Msg );
	}
	else//when click [Open] button to connect w/ device
	{			
		CString strDevice;	
		m_cmbPort.GetWindowText(strDevice);//get the device name(USB or COM#) from the comboBox

		//try to Link
		//for open USB
		if( strDevice == _T("USB"))
		{
			m_BtnStart.SetWindowText( _T("Stop") );
			//AddMemo( _T("Open communication port successfully.") );			
			m_cmbPort.EnableWindow(false);
			m_bStart = !m_bStart;
			
			bootThd();//boot the checkThread()
			isUSBThdRun = true;
		}
		
		else//for open RS232
		{
			int     iComm = 0;
			CString strTmp;
			//TCHAR *strTmp;
			
			m_cmbPort.GetWindowText(strTmp);

			//try to open UART and send command
			if( false == m_clsTranxRFID->API_Serial_Open(strTmp, m_Dcb) )//**why not use strDevice directly
			{
				AddMemo( _T("Open communication port error.") );				
				m_clsTranxRFID->API_CloseComm(); //*** what's the difference b/t API_CloseComm & API_Close
				Reset();
				return;
			}

			else// sned command to check whether linked the device successfully(it's only for RS232)
			{
				//TBYTE OemAddr=0x30;
				//m_clsTranxRFID->GetModuleName_new(OemAddr);
				
				////Send Command to check
				if ( false == m_clsTranxRFID->GetModuleID_new() )
				{
					AddMemo( _T("Connected with the device unsuccessfully.") );
					m_clsTranxRFID->API_Close();		
				}		
				else
				{
					TBYTE OemAddr=0x30;	
					UCHAR OemData[1], charModelName[17], charSerialNumber[17];
					//for( OemAddr=0x30; OemAddr<0x40; OemAddr++)
					for( int i=0; i<16; i++ )
					{
						if( false != m_clsTranxRFID->GetModuleName_new(OemAddr, OemData) )
						{
							charModelName[i] = OemData[0];
							OemAddr++;
						}
					}					
					uartModelName = charModelName;
					m_EditModelName.SetWindowText(uartModelName);

					OemAddr=0x50;
					for(int i=0; i<16; i++)
					{
						if(false != m_clsTranxRFID->GetModuleName_new(OemAddr, OemData) )
						{
							charSerialNumber[i] = OemData[0];
							OemAddr++;
						}
					}
					uartSerialNumber = charSerialNumber;
					m_EditSerialNumber.SetWindowText(uartSerialNumber);

					
					//m_UartModelName = ModelName; 
					//m_UartModelName = m_clsTranxRFID->uartModelName;
					//m_EditModelName.SetWindowText(m_UartModelName);

					m_BtnStart.SetWindowText( _T("Stop") );
					AddMemo( _T("Open communication port successfully.") );		
					m_cstrVersion = m_clsTranxRFID->fwVersion;
					m_EditVersion.SetWindowText(m_cstrVersion);//get FW version string
					isUARTRun = true;
					


					m_cmbPort.EnableWindow(false);
					m_bStart = !m_bStart;

					m_Msg.LoadString(STR_ACT_CONNECTED);
					m_textLinkState.SetWindowText( m_Msg );	

					if( ExitBLMode() == false )
						AddMemo( _T("Please update Firmware again.") );	
					else
						AddMemo( m_Msg );					
				}
			}
		}
	}
}


//add by yo chen, for detect COM port and usb, 2011/05/20
bool CPageBasic::DetectSerial()
{
  //What will be the return value from this function (assume the worst)
	bool bSuccess = FALSE;

	HKEY hSERIALCOMM;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_QUERY_VALUE, &hSERIALCOMM) == ERROR_SUCCESS)
	{
		//Get the max value name and max value lengths
		DWORD dwMaxValueNameLen = 0;
		DWORD dwMaxValueLen = 0;
		DWORD dwQueryInfo = RegQueryInfoKey(hSERIALCOMM, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwMaxValueNameLen, &dwMaxValueLen, NULL, NULL);
		
		if (dwQueryInfo == ERROR_SUCCESS)
		{
			DWORD dwMaxValueNameSizeInChars = dwMaxValueNameLen + 1; //Include space for the NULL terminator
			DWORD dwMaxValueNameSizeInBytes = dwMaxValueNameSizeInChars * sizeof(TCHAR);
			DWORD dwMaxValueDataSizeInChars = dwMaxValueLen/sizeof(TCHAR) + 1; //Include space for the NULL terminator
			DWORD dwMaxValueDataSizeInBytes = dwMaxValueDataSizeInChars * sizeof(TCHAR);
		
			//Allocate some space for the value name and value data			
			  ATL::CHeapPtr<TCHAR> szValueName;
			  ATL::CHeapPtr<BYTE> byValue;

			  if (szValueName.Allocate(dwMaxValueNameSizeInChars) && byValue.Allocate(dwMaxValueDataSizeInBytes))
			  {
					bSuccess = TRUE;

					//Enumerate all the values underneath HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM
					DWORD dwIndex = 0;
					DWORD dwType;
					DWORD dwValueNameSize = dwMaxValueNameSizeInChars;
					DWORD dwDataSize = dwMaxValueDataSizeInBytes;
					memset(szValueName.m_pData, 0, dwMaxValueNameSizeInBytes);
					memset(byValue.m_pData, 0, dwMaxValueDataSizeInBytes);
					LONG nEnum = RegEnumValue(hSERIALCOMM, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, byValue, &dwDataSize);
			
					while (nEnum == ERROR_SUCCESS)
					{
						//If the value is of the correct type, then add it to the array
						if (dwType == REG_SZ)
						{
							TCHAR* szPort = reinterpret_cast<TCHAR*>(byValue.m_pData);

							//Add Comport string to ComboBox
							m_cmbPort.AddString(szPort);						 				
						}

						//Prepare for the next time around
						dwValueNameSize = dwMaxValueNameSizeInChars;
						dwDataSize = dwMaxValueDataSizeInBytes;
						memset(szValueName.m_pData, 0, dwMaxValueNameSizeInBytes);
						memset(byValue.m_pData, 0, dwMaxValueDataSizeInBytes);
						++dwIndex;
						nEnum = RegEnumValue(hSERIALCOMM, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, byValue, &dwDataSize);
					}
			  }
			  else
				SetLastError(ERROR_OUTOFMEMORY);
		}
			
		//Close the registry key now that we are finished with it    
		RegCloseKey(hSERIALCOMM);

		if (dwQueryInfo != ERROR_SUCCESS)
				SetLastError(dwQueryInfo);
	}
	
	return bSuccess;
}

UINT WINAPI CPageBasic::CheckThread_UART_Update(LPVOID r_vpAppCls)
{	
	CPageBasic *thisCls = (CPageBasic *)r_vpAppCls;
	while(thisCls->endThd_UART)
	{	
		////block this thread when change to the Advance Page
		//if(thisCls->m_bThdEnableFlag == false)
		//{
		//	thisCls->m_bUpdateFlag = false;
		//	Sleep(200);
		//	continue;
		//}
		//EnterCriticalSection(&thisCls->m_CriticalSection);

		/*while(1)
		{*/
			thisCls->m_ProgressBar.SetPos(0);

		//	if( m_bIsOpen == false )
		//	{
		//		m_Msg.LoadString(STR_ERR_NO_USB);	
		//		AddMemo( m_Msg );
		//		break ;
		//	}

		if(thisCls->m_strFilePath.IsEmpty() == true)
		{				
			thisCls->m_Msg.LoadString(STR_ACT_SELECT_DIRECTORY);	
			thisCls->AddMemo(thisCls-> m_Msg );
			break;
		}

		//==============Hanlde File=======================//
		//If already open file before, close it.
		clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, thisCls->m_hdlImageFile);

		thisCls->m_hdlImageFile = CreateFile(thisCls->m_strFilePath, 
									GENERIC_READ,  
					    			FILE_SHARE_READ, 
									NULL, 
									OPEN_EXISTING, 
									FILE_ATTRIBUTE_NORMAL, 
									0);

		if(thisCls->m_hdlImageFile == INVALID_HANDLE_VALUE)
		{
			thisCls->m_Msg.LoadString(STR_ERR_CAN_NOT_OPEN_IMAGE);	
			thisCls->AddMemo( thisCls->m_Msg );
			break;
		}


		//==============Hanlde File Size==================//
		
		ZeroMemory(&thisCls->m_strcLargeInt, sizeof(LARGE_INTEGER));

		//Get File Size
		if(  !GetFileSizeEx(thisCls->m_hdlImageFile , &thisCls->m_strcLargeInt) )
		{				
			thisCls->m_Msg.LoadString(STR_ERR_CAN_NOT_GET_IMAGE_SIZE);	
			thisCls->AddMemo(thisCls-> m_Msg );
			break;
		}
		
		if(thisCls->m_strcLargeInt.LowPart == 0)
		{
			thisCls->m_Msg.LoadString(STR_ERR_FILE_SIZE_ZERO);	
			thisCls->AddMemo( thisCls->m_Msg );
			break;
		}


	//	//==============Enter Update Status==============//
	//	m_bUpdateFlag = true;
	//	m_bQryExitFwMode = true;
	//	EnableBtn(false);

		//If in AP Mode, change to BL Mode. Wait next to start update flow.
		//When restart the device, get the version, and judge whether update or not.
		/*if( CheckMode(AP_MODE) == true )
		{*/
			//=====set the para. for API_Read()
		int Length = thisCls->m_clsTranxRFID->API_AskRevCount();
		UCHAR tempRxBuf[128];
		memset(tempRxBuf, 0 ,128);			
		TBYTE tempRxDataLen = 128;
		//=====

		if(false == thisCls->m_clsTranxRFID->API_QueryOneCmd(CMD_ENTER_UPDATE_FW_MODE_B))//step1. enter update mode 
			break;
		thisCls->AddMemo( _T("Please wait, restart the module. Change mode to BL Mode") );
		memset(tempRxBuf, 0 ,128);
		thisCls->m_clsTranxRFID->API_Read(tempRxBuf, tempRxDataLen);//receive the data for Enter_Update_Mode
		if(tempRxBuf[7] != 0x00)
			break;
		Sleep(500);
		//Get module ID when enter the BL mode

		if(false == thisCls->m_clsTranxRFID->GetModuleID_new())
			break;				
		
		thisCls->m_cstrVersion = thisCls->m_clsTranxRFID->fwVersion;
		thisCls->m_EditVersion.SetWindowText(thisCls->m_cstrVersion);//get FW version string

		if(false == thisCls->m_clsTranxRFID->API_QueryOneCmd(CMD_BEGIN_UPDATE_B))//step2. Begin_Update
			break;
		thisCls->AddMemo( _T("Being to update") );
		Sleep(3000);
		memset(tempRxBuf, 0 ,128);
		thisCls->m_clsTranxRFID->API_Read(tempRxBuf, tempRxDataLen);//receive the data for Begin_Update
		if(tempRxBuf[7] != 0x00)
			break;

		//======MODIFY
		if( thisCls->MessageBox( _T("Do you want to update?"), _T("Update"), MB_YESNO|MB_ICONQUESTION ) == IDYES )
		{//ask update or not when restart the module
			thisCls->m_BtnStart.EnableWindow(false);
			thisCls->m_BtnUpdate.EnableWindow(false);
			thisCls->m_BtnBrowser.EnableWindow(false);
			if( !thisCls->m_clsTranxRFID->API_UpdateFw_UART_Init(thisCls->m_hdlImageFile, thisCls->m_strcLargeInt) )//step3. Update_Wtire_Image
			{								
				thisCls->AddMemo( _T("Update FW failure") );
				break;
			}	
			UINT uiPercent = 0;
			thisCls->AddMemo( _T("Please wait, the module is updating...") );
			while(uiPercent < 100)
			{		
				//1. Tx_bWriteImage() & m_uiCurFileCnt++
				if(!thisCls->m_clsTranxRFID->API_UpdateFw_UART_Write())
					break;
				
				//2. Change Process bar
				uiPercent = thisCls->m_clsTranxRFID->API_AskProgressBarStep();
				if(thisCls->m_ProgressBar.GetPos() != 100 && uiPercent<=100)
					thisCls->m_ProgressBar.SetPos(uiPercent);
				
				//3. Set % status at the update button
				CString cstr,tmpStr;
				cstr.Format(_T("%d%%"),uiPercent);						
				thisCls->m_BtnUpdate.GetWindowText(tmpStr);						
				if(tmpStr != cstr)
					thisCls->m_BtnUpdate.SetWindowText(cstr);
			}					
		}
		//=====END MODIFY
		
		//reset All buttons
/*    thisCls->m_BtnStart.EnableWindow(true);
		thisCls->m_BtnUpdate.EnableWindow(true);
		thisCls->m_BtnBrowser.EnableWindow(true);
*/
		
		thisCls->m_Msg.LoadString(STR_ACT_UPDATE);
		thisCls->m_BtnUpdate.SetWindowText( thisCls->m_Msg );
		thisCls->m_ProgressBar.SetPos(0);

		//step4. Exit_Update_Mode
		if(thisCls->ExitBLMode() == false)
		{
			thisCls->AddMemo( _T("Please update Firmware again.") );
			clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, thisCls->m_hdlImageFile);
			thisCls->MessageBox(_T("Please update Firmware again"), _T("Update again"), MB_OK);
		
			thisCls->m_BtnStart.EnableWindow(true);
			thisCls->m_BtnUpdate.EnableWindow(true);
			thisCls->m_BtnBrowser.EnableWindow(true);

			//LeaveCriticalSection(&thisCls->m_CriticalSection);
			break;
		}
		else
		{
			thisCls->AddMemo( _T("Update succefully") );
			clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, thisCls->m_hdlImageFile);
			/*if(MessageBox( _T("Change software configure will abort current task.\nDo you change software configure?"), 
		           _T("Configure"), 
				   MB_YESNO|MB_ICONQUESTION ) == IDNO )*/
			/*MessageBox( _T("Do you want to update?"), _T("Update"), MB_YESNO|MB_ICONQUESTION ) == IDYES*/
			thisCls->MessageBox(_T("Update succefully"), _T("Update succefully"), MB_OK);

			thisCls->m_BtnStart.EnableWindow(true);
			thisCls->m_BtnUpdate.EnableWindow(true);
			thisCls->m_BtnBrowser.EnableWindow(true);

			//LeaveCriticalSection(&thisCls->m_CriticalSection);
			break;
		}
		//if(false == thisCls->m_clsTranxRFID->API_QueryOneCmd(CMD_EXIT_UPDATE_FW_MODE_B))
		//	break;		
		//		
		//thisCls->AddMemo( _T("Please wait, change mode to AP Mode.") );
		//Sleep(3000);//must wait more than 3 sec.
		//memset(tempRxBuf, 0 ,128);
		//thisCls->m_clsTranxRFID->API_Read(tempRxBuf, tempRxDataLen);//receive the data for Exit_Update_Mode
		//if(tempRxBuf[7] != 0x00)
		//{
		//	thisCls->ExitBLMode();
		//	break;
		//}
		//thisCls->AddMemo( _T("Update succefully") );
		//if(false == thisCls->m_clsTranxRFID->GetModuleID_new())//get Module ID when turn back to the [AP] mode
		//	break;				
		//thisCls->m_EditVersion.SetWindowText(thisCls->m_clsTranxRFID->fwVersion);//get FW version string	
		//
		//
		//clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, thisCls->m_hdlImageFile);
		//LeaveCriticalSection(&thisCls->m_CriticalSection);
		////return true;
		//break;
		///*}*/
		
		//return true;
		/*clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, thisCls->m_hdlImageFile);
		LeaveCriticalSection(&thisCls->m_CriticalSection);
		return false;*/
	}
	return 0;
}

bool CPageBasic::ExitBLMode()
{
	//Change to AP mode, 2011/10/03
	//add by yo chen, Check the device in AP mode,
	//If require updating, don't return to AP mode, 2011/10/03
	if( CheckMode(BL_MODE) == true )
	{
		AddMemo( _T("Please wait, change mode to AP Mode."));
		m_clsTranxRFID->API_QueryOneCmd(CMD_EXIT_UPDATE_FW_MODE_B);//send EXIT CMD
		UCHAR tempRxBuf[128];
		memset(tempRxBuf, 0 ,128);			
		TBYTE tempRxDataLen = 128;		
		Sleep(5000);
		m_clsTranxRFID->API_Read(tempRxBuf, tempRxDataLen);
		CString tempSt1;
		int askVerTimeout = 0;
		m_EditVersion.SetWindowText( _T("") );
		m_EditVersion.GetWindowText(tempSt1);
		while( tempSt1 == "" && askVerTimeout < 100 )//get version again to check if module can change to AP Mode
		{
			if(true == m_clsTranxRFID->GetModuleID_new())
			{
				m_cstrVersion = m_clsTranxRFID->fwVersion;
				m_EditVersion.SetWindowText(m_cstrVersion);//get FW version string
				m_EditVersion.GetWindowText(tempSt1);
			}
			askVerTimeout++;
			if(CheckMode(BL_MODE) != true && CheckMode(AP_MODE) != true)
				tempSt1 = "";
		}
		if( CheckMode(BL_MODE) == true )//if it's still in BL Mode													
			return false;	
		else
			return true;
	}					
	//END, Change to AP mode, 2011/10/03
	else
		return true;

}

//add by yo chen, reOpen UART when change pages, 2011/10/07
bool CPageBasic::OpenUART()
{
	int     iComm = 0;
	CString strTmp;
	//TCHAR *strTmp;

	m_cmbPort.GetWindowText(strTmp);

	//try to open UART and send command
	if( false == m_clsTranxRFID->API_Serial_Open(strTmp, m_Dcb) )//**why not use strDevice directly
	{
		AddMemo( _T("Open communication port error.") );				
		m_clsTranxRFID->API_CloseComm(); //*** what's the difference b/t API_CloseComm & API_Close
		Reset();
		return false;
	}

	else// sned command to check whether linked the device successfully(it's only for RS232)
	{
		////Send Command to check
		if ( false == m_clsTranxRFID->GetModuleID_new() )
		{
			AddMemo( _T("Connected with the device unsuccessfully.") );
			m_clsTranxRFID->API_Close();	
			return false;
		}		
		else
		{
			//m_BtnStart.SetWindowText( _T("Stop") );
			//AddMemo( _T("Open communication port successfully.") );		
			m_cstrVersion = m_clsTranxRFID->fwVersion;
			m_EditVersion.SetWindowText(m_cstrVersion);//get FW version string
			//isUARTRun = true;

			//m_cmbPort.EnableWindow(false);
			//m_bStart = !m_bStart;

			//m_Msg.LoadString(STR_ACT_CONNECTED);
			//m_textLinkState.SetWindowText( m_Msg );	

			//if( ExitBLMode() == false )
			//	AddMemo( _T("Please update Firmware again.") );	
			//else
			//	AddMemo( m_Msg );
			return true;
		}
	}
}
//END, add by yo chen, reOpen UART when change pages, 2011/10/07
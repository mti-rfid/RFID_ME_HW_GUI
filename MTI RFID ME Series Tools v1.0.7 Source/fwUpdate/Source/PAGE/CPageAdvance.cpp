#include "stdafx.h"
#include "USBControl.h"
#include "CPageAdvance.h"
#include "ListCmd.h"

// CPageAdvance dialog

IMPLEMENT_DYNAMIC(CPageAdvance, CDialog)

CPageAdvance::CPageAdvance(CWnd* pParent /*=NULL*/)
	: CDialog(CPageAdvance::IDD, pParent)
{
	m_uiInterval      = 0;
	m_uiThreadID      = 0;
	m_uiUserCmd       = 0;
	m_hdlThread       = INVALID_HANDLE_VALUE;
	m_hdlImageFile    = INVALID_HANDLE_VALUE;
	m_clsTranxRFID    = NULL;
	m_clsThread		  = NULL;
	m_bUpdateFlag     = false;
	m_bIsOpen	      = false;
	m_bIsBusy	      = false;
	m_bThdEnableFlag  = false;
	m_bShowVerToMumo  = false;
	m_clsTranxRFID    = new clsTRANX_RFID;
	m_enumUserCmdFlag = USER_SEND_NOTHING;
	m_cstrUserCmdData.Empty();
	m_strFilePath.Empty();
	m_cstrVersion.Empty();
	ZeroMemory(&m_liBefore		 , sizeof(LARGE_INTEGER));
	ZeroMemory(&m_liNow			 , sizeof(LARGE_INTEGER));
	ZeroMemory(&m_liFreq		 , sizeof(LARGE_INTEGER));
	ZeroMemory(&m_strcLargeInt   , sizeof(LARGE_INTEGER));
	InitializeCriticalSection(&m_CriticalSection);
}

CPageAdvance::~CPageAdvance()
{
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlThread);
	clsUTILITY::DelObj(CLOSE_TYPE_POINT,  m_clsTranxRFID);
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
	DeleteCriticalSection(&m_CriticalSection);
}

void CPageAdvance::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1_MEMO,         m_Memo);
	DDX_Control(pDX, IDC_EDIT2_FW_VERSION,   m_EditVersion);
	DDX_Control(pDX, IDC_STATIC_LINK_STATE,  m_textLinkState);;
	DDX_Control(pDX, IDC_COMBO1_CMD,         m_ComboxSendCmd);
	DDX_Control(pDX, IDC_BUTTON2_SEND,       m_BtnSend);
	DDX_Control(pDX, IDC_PROGRESS,			 m_ProgressBar);
	DDX_Control(pDX, IDC_COMBO1_slePort2, m_cmbPort);
	DDX_Control(pDX, IDC_BUTTON1_Open, m_BtnStart);
	DDX_Control(pDX, IDC_BUTTON1_BOWSER, m_BtnBrowser);
}


BEGIN_MESSAGE_MAP(CPageAdvance, CDialog)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1_BOWSER,    &CPageAdvance::OnBnClickedButton1_Select)
	ON_BN_CLICKED(IDC_BUTTON2_SEND,      &CPageAdvance::OnBnClickedButton2_SendCmd)
	ON_BN_CLICKED(IDC_BUTTON4_CLEAR,     &CPageAdvance::OnBnClickedButton4_ClearMemo)
	ON_BN_CLICKED(IDC_BUTTON1_Open, &CPageAdvance::OnBnClickedButton3_Open)
END_MESSAGE_MAP()


BOOL CPageAdvance::OnInitDialog()
{
    CDialog::OnInitDialog();

	//Set SW Version Caption
	GetDlgItem(IDC_EDIT1_SW_VERSION)->SetWindowText(cstrToolVer) ;

	//Cmd ComBox
	for(int index=0; index < CMD_COUNT; index++)
		m_ComboxSendCmd.AddString( *(strCmdName + index));
	m_ComboxSendCmd.SetCurSel(0);


	//Link Status Static Text	
	m_clsFont.CreateFont(20, 0, 0, 0, FW_HEAVY,
                         FALSE, FALSE, FALSE, 0, 
						 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, 
						 _T("Arial")); 
	m_textLinkState.SetFont(&m_clsFont);
		m_Msg.LoadString(STR_ACT_DISCONNECTED);
	m_textLinkState.SetWindowText( m_Msg );


	//Set Check Time Frequency
	QueryPerformanceFrequency(&m_liFreq);
	m_liFreq.QuadPart/=1000;  //mini sec

	//add by yo chen, 2011/09/09
	m_Dcb.BaudRate = CBR_115200;
	m_Dcb.ByteSize = 8;
	m_Dcb.StopBits = ONESTOPBIT;
	m_Dcb.Parity = NOPARITY;

	DetectSerial();
	m_cmbPort.AddString( _T("USB") );
	m_cmbPort.SetCurSel(0);

	m_bStart = false;
	isBootThd_UART = false;
	endThd_UART = 0;
	endThd = 0;

	stepEnter = false;
	stepBegin = false;
	stepWrite = false;
	stepWriteDone = false;
	stepExit = false;
	m_BtnSend.EnableWindow(false);
	isUSBThdRun = false;
	isUARTThdRun = false;

	//Thread
	//m_clsThread = AfxBeginThread( (AFX_THREADPROC)CheckThread, (LPVOID)this);	//mod by yo chen 2011/07/07
	//if(m_clsThread == NULL)		return false;

    return TRUE;
}





// CPageAdvance message handlers
void CPageAdvance::AddMemo(CString str)
{
	m_Memo.InsertString(0, str);
}




//=========================================
void CPageAdvance::OnBnClickedButton1_Select()
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
	}
}

//void CUSBControlDlg::OnBnClickedButton1_Select()
//{
//	char szFilter[]="bin|*.bin|All Files|*.*||";
//	char fileBuffer[4096];
//	CFileDialog *dlg = NULL;
//	memset(fileBuffer, 0, 4096);
//
//	dlg = new CFileDialog(true,"bin","*.bin",NULL,szFilter, this);
//	dlg->m_ofn.lpstrFile = fileBuffer;  //add "Name Buffer"
//	dlg->m_ofn.nMaxFile = 4096;
//
//	if(dlg->DoModal() == IDOK)
//	{
//	 m_strFilePath = dlg->GetPathName();
//	 MessageBox(m_strFilePath,"File Path",MB_OK);
//	}
//
//	if(dlg)  delete dlg;
//}

void CPageAdvance::OnBnClickedButton2_SendCmd()
{
	CString msg;
	//add by yo chen for identify the UART mode
	CString tempDevType;
	tempDevType = m_clsTranxRFID->API_AskDevType();
	//END, add by yo chen for identify the UART mode

	if(tempDevType == "USB")
	{
		if( m_bIsOpen == false )	
		{
			msg.LoadString(STR_ERR_NO_USB);
			AddMemo( msg );
			return ;
		}

		if(m_bIsBusy == true || m_enumUserCmdFlag != USER_SEND_NOTHING)
		{
			msg.LoadString(STR_ERR_BUSY);
			AddMemo( msg );
			return;		
		}
	}

	m_cstrUserCmdData.Empty();
	m_uiUserCmd = 0; 	

	//Get Combox Text
	CString tmpData;
	m_ComboxSendCmd.GetWindowText(tmpData);
	_stscanf_s(tmpData.GetString(), _T("%x "), &m_uiUserCmd);	

	//Check Mode
	switch(m_uiUserCmd)
	{
		case CMD_ENTER_UPDATE_FW_MODE_B:{
			if(CheckMode(BL_MODE) == true)
			{
				AddMemo( _T("Error: The device is in the BL mode. Don't need to change again") );
				return;
			}
			/*if(tempDevType == "RS232")
			{
				stepEnter = true;
			}*/
			AddMemo( _T("Please wait, restart the module. Change mode to BL Mode") );
			break;}
		
		case CMD_BEGIN_UPDATE_B:
			if( CheckMode(AP_MODE) == true )
			{
				AddMemo( _T("Error: Please send the command 0xD0 to change to the BL Mode") );
				return;
			}
			/*if(tempDevType == "RS232")
			{
				stepBegin = true;
			}*/

			break;

		case CMD_EXIT_UPDATE_FW_MODE_B:{
			if( CheckMode(AP_MODE) == true )
			{
				AddMemo( _T("Error: The device is in the AP mode. Don't need to change again") );
				return;
			}
			/*if(tempDevType == "RS232")
			{
				stepExit = true;
			}*/
			AddMemo( _T("Please wait, restart the module. Change to AP mode ") );
			break;}

		case CMD_WRITE_IMAGE_B:
			//===mod by yo chen, 2011/09/15			
			if( tempDevType == "USB" )
				Update();
			if(tempDevType == "RS232")
			{
				if( CheckMode(AP_MODE) == true )
				{
					AddMemo( _T("Error: Please send the command 0xD0 to change to the BL Mode") );
					return;
				}
				/*stepWrite = true;*/
			}
				//##need to be change!!!
				//bootThd_UART();				
			//===END, mod by yo chen, 2011/09/15
			//Update();
			//return;
		   break;

		case CMD_VERSION_B:
			//===mod by yo chen, 2011/09/16
			if( tempDevType == "RS232" )
			{
				m_clsTranxRFID->GetModuleID_new();
				AddMemo( _T("\"0x10 Get Module ID\" :" + m_clsTranxRFID->fwVersion) );
				m_cstrVersion = m_clsTranxRFID->fwVersion;
				m_EditVersion.SetWindowText(m_cstrVersion);//get FW version string
			}
			//===END, mod by yo chen, 2011/09/16
		   break;

		default:
			m_uiUserCmd = 0;
			m_cstrUserCmdData.Empty();
			AddMemo( _T("Don't support this command") );
			return;
	}

	m_enumUserCmdFlag = USER_SEND_SELECT;
	//EnableBtn(false);
}


//void CPageAdvance::OnBnClickedButton3_SendData()
//{
//	// TODO: Add your control notification handler code here
//	if( m_bIsOpen == false )	
//	{
//		AddMemo( _T("Error: No USB device\n") );
//		return ;
//	}
//
//	if(m_bIsBusy == true || m_enumUserCmdFlag != USER_SEND_NOTHING)
//	{
//		AddMemo( _T("Error: Communication is Busy") );
//		return;		
//	}
//
//	m_uiUserCmd = 0; 
//	m_cstrUserCmdData.Empty();
//
//	m_EditCmdData.GetWindowText(m_cstrUserCmdData);
//
//	if(m_cstrUserCmdData.IsEmpty() == true)
//	{
//		AddMemo( _T("Error: No Data") );
//		return;		
//	}
//
//	if(m_cstrUserCmdData.GetLength() > USB_LENGTH)
//	{
//		CString tmpStr;
//		tmpStr.Format( _T("Error: Data too long. Only Enter Data %03d bytes"),USB_LENGTH );
//
//		AddMemo( tmpStr );
//		return;		
//	}
//
//	m_enumUserCmdFlag = USER_SEDN_DATA;
//	EnableBtn(false);
//}


void CPageAdvance::OnBnClickedButton4_ClearMemo()
{
    m_Memo.ResetContent();
}



void CPageAdvance::EnableBtn(bool bEnableFlag)
{
	GetDlgItem(IDC_BUTTON2_SEND)->EnableWindow(bEnableFlag);
}

//============================================================
bool  CPageAdvance::Update(void)
{
	EnterCriticalSection(&m_CriticalSection);
	CString msg;

		
	//If in 0xD6 Write Image status, can't close m_hdlImageFile handle. Return directly.
	if(m_bIsBusy == true)
	{
			msg.LoadString(STR_ERR_BUSY);	
		AddMemo( msg );
		LeaveCriticalSection(&m_CriticalSection);
		return false;		
	}

	while(1)
	{
		m_ProgressBar.SetPos(0);

		if( m_bIsOpen == false )
		{
				msg.LoadString(STR_ERR_NO_USB);	
			AddMemo( msg );
			break ;
		}

		if(m_strFilePath.IsEmpty() == true)
		{
				m_Msg.LoadString(STR_ACT_SELECT_DIRECTORY);	
			AddMemo( m_Msg );
			break;
		}

		if( CheckMode(AP_MODE) == true )
		{
			AddMemo( _T("Error: Please send the command 0xD0 to change to the BL Mode") );
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
		EnableBtn(false);


		LeaveCriticalSection(&m_CriticalSection);
		return true;
	}

	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
	LeaveCriticalSection(&m_CriticalSection);
	return false;
}


//Thread====================================
UINT WINAPI CPageAdvance::CheckThread(LPVOID r_vpAppCls)
{
	UINT uiAskVersionTime = 0;
	UINT uiInterval =0;
	CPageAdvance *thisCls = (CPageAdvance *)r_vpAppCls;
	CString msg;

	while(thisCls->endThd)
	{
		if(thisCls->m_bThdEnableFlag == false)
		{
			thisCls->Reset();
			thisCls->m_Memo.ResetContent();
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
			thisCls->m_textLinkState.SetWindowText( thisCls->m_Msg );
		
			if(thisCls->m_clsTranxRFID->API_USB_Open() == true)
			{
				QueryPerformanceCounter(&thisCls->m_liBefore);
				thisCls->m_uiInterval = 0;
				msg.LoadString(STR_ACT_CONNECTED);
				thisCls->m_textLinkState.SetWindowText( msg );				

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

				///Check User Cmd
				if(thisCls->m_enumUserCmdFlag != USER_SEND_NOTHING)
				{	

					if( thisCls->m_enumUserCmdFlag == USER_SEND_SELECT )
					{
						if( thisCls->m_uiUserCmd == 0 )
						{
								thisCls->m_Msg.LoadString(STR_ERR_CMD_ZERO);
							thisCls->AddMemo( thisCls->m_Msg );
						}

						if( !thisCls->m_clsTranxRFID->API_QueryOneCmd(thisCls->m_uiUserCmd) )
						{
							thisCls->m_Msg.LoadString(STR_ERR_WT_FAIL);
							thisCls->AddMemo( thisCls->m_Msg );
						}

						if(thisCls->m_uiUserCmd == CMD_VERSION_B)
							thisCls->m_bShowVerToMumo = true;
					}
					else if(thisCls->m_enumUserCmdFlag == USER_SEDN_DATA)
					{
						if( thisCls->m_cstrUserCmdData.IsEmpty() == true )
						{
								thisCls->m_Msg.LoadString(STR_ERR_NO_DATA);
							thisCls->AddMemo( thisCls->m_Msg );
						}

						if( !thisCls->m_clsTranxRFID->API_QueryOneCmd(thisCls->m_cstrUserCmdData) )
						{
								thisCls->m_Msg.LoadString(STR_ERR_WT_FAIL);
							thisCls->AddMemo( thisCls->m_Msg );
						}				
					
					}
						
					EnterCriticalSection(&thisCls->m_CriticalSection);
					thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
					thisCls->EnableBtn(true);
					thisCls->m_uiUserCmd = 0;
					thisCls->m_cstrUserCmdData.Empty();
					LeaveCriticalSection(&thisCls->m_CriticalSection);
					break;

				}


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

			//When restart the device, get the version, and judge whether update or not.
			if( thisCls->m_bIsBusy == false        && 
				thisCls->m_bUpdateFlag == true           )
			{				
				if( thisCls->Update() == true) 
				{
					//Start to update
					if( !thisCls->m_clsTranxRFID->API_UpdateFw(thisCls->m_hdlImageFile, thisCls->m_strcLargeInt, false) )
					{
						msg.LoadString(STR_ERR_BUSY);	
						thisCls->AddMemo( msg );
					}
					else
						thisCls->AddMemo( _T("Start to Update\n") );
				}
			}

			break;
		}//while i
		
	}//while o

	return 0;
}


//

void CPageAdvance::UpdateStatus(void)
{
	EnterCriticalSection(&m_CriticalSection);
	m_bIsOpen	   = m_clsTranxRFID->API_IsOpen();
	m_bIsBusy	   = m_clsTranxRFID->API_IsBusy();
	m_cstrVersion  = m_clsTranxRFID->API_AskVersion();
	LeaveCriticalSection(&m_CriticalSection);
}


void CPageAdvance::Reset(void)
{
	//Reset bar
	m_BtnSend.SetWindowText( _T("Send") );	
	m_ProgressBar.SetPos(0);
	m_EditVersion.SetWindowText( _T("") );
	//EnableBtn(true);

	//Set msg
	m_Msg.LoadString(STR_ACT_DISCONNECTED);
	m_textLinkState.SetWindowText( m_Msg );

	//clear flag
	m_bUpdateFlag = false; 
	m_bShowVerToMumo = false;	
	m_enumUserCmdFlag = USER_SEND_NOTHING;

	//Close communication
	m_clsTranxRFID->API_CloseComm();
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
	UpdateStatus();	
}

//==========================================


void CPageAdvance::AnalyzePacket(ENUM_CMD enumRtCmd)
{
	UpdateStatus();

	switch( enumRtCmd )
	{
		case CMD_TIME_OUT:				
			m_Msg.LoadString(STR_ERR_TIMEOUT);
			AddMemo( m_Msg );
			Reset();
			break;

		case CMD_VERSION__BR:{
			CString cstrEdit;
			m_EditVersion.GetWindowText(cstrEdit);

			if(m_cstrVersion != cstrEdit)
				m_EditVersion.SetWindowText(m_cstrVersion); 

			if(m_bShowVerToMumo)
			{
				m_bShowVerToMumo = false;
				AddMemo( _T("\"0x10 Get Module ID\" :" + m_cstrVersion) );
			}

			break;}

		case CMD_ENTER_UPDATE_FW_MODE_BR:		
			if(m_clsTranxRFID->API_AskCmdStatus() == STATUS_OK)
			{
				AddMemo( _T("\"0xD0 Enter Update FW Mode\" Successfully!! ") );
				m_clsTranxRFID->API_CloseComm();
			}
			else
			{	
				AddMemo( _T("\"0xD0 Enter Update FW Mode\" fail!!, ") + PrintError());
			}
			
			m_BtnSend.SetWindowText( _T("Send") );
			break;

		case CMD_EXIT_UPDATE_FW_MODE_BR:		
			if(m_clsTranxRFID->API_AskCmdStatus() == STATUS_OK)
			{
				AddMemo( _T("\"0xD2 Exit Update FW Mode\" Successfully!! ") );		
				AddMemo( _T("Please wait, restart the module. Change to AP mode ") );
				m_clsTranxRFID->API_CloseComm();
				EnableBtn(true);
			}
			else
			{	
				AddMemo( _T("\"0xD2 Exit Update FW Mode\" fail!!, ") + PrintError());
			}
			m_BtnSend.SetWindowText( _T("Send") );
			break;


		case CMD_BEGIN_UPDATE_BR:
			if(m_clsTranxRFID->API_AskCmdStatus() == STATUS_OK)
			{
				AddMemo( _T("\"0xD4 Begin Update\" Successfully") );		
			}
			else
			{
				AddMemo( _T("\"0xD4 Begin Update\" fail!!, ") + PrintError());
				m_BtnSend.SetWindowText( _T("Send") );
			}
			break;	


		case CMD_WRITE_IMAGE_BR:{
			TBYTE Status = m_clsTranxRFID->API_AskCmdStatus();

			switch(Status)
			{
				case STATUS_OK:
				case STATUS_ERROR_OVER_RANGE:{
					UINT uiPercent = m_clsTranxRFID->API_AskProgressBarStep();
					
					if(uiPercent == 100 || Status == STATUS_ERROR_OVER_RANGE)//Write Image OK
					{
						AddMemo( _T("\"0xD6 Write Image \"Update Successfully") );	
						m_BtnSend.SetWindowText( _T("Send") );
						clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
						m_bUpdateFlag = false;
						EnableBtn(true);
					}
					else//continue Wrote Image
					{
						//"Update Button" Set the Caption
						CString cstr,tmpStr;
						cstr.Format(_T("%d%%"),uiPercent);
						
						m_BtnSend.GetWindowText(tmpStr);
						
						if(tmpStr != cstr)
							m_BtnSend.SetWindowText(cstr);								
					}

					//Change Process bar
					if(m_ProgressBar.GetPos() != 100 && uiPercent<=100)
						m_ProgressBar.SetPos(uiPercent);

					break;}

				case STATUS_ERROR_WRONG_PROCEDURE:
					AddMemo( _T("Please sned command 0xD4 \"Begin Update\"") );	
				default:
					m_BtnSend.SetWindowText( _T("Send") );
					AddMemo( _T("\"0xD6 Write Image\" fail!!, ") + PrintError());	
					clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
					m_bUpdateFlag = false;
					EnableBtn(true);
			}//i switch
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


CString CPageAdvance::PrintError()
{
	TBYTE   ucStatus = 0;     	
	CString strTmp;

	ucStatus = m_clsTranxRFID->API_AskCmdStatus();
	strTmp.Format( _T(" Error code is 0x%02x, " ), ucStatus);

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

bool CPageAdvance::CheckMode(TCHAR *CheckMode)
{
	if( m_cstrVersion.IsEmpty()!= true && _tcsncmp(m_cstrVersion.GetString() ,CheckMode, 4)==0)
		return true;
	else
		return false;
}

HBRUSH CPageAdvance::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
			//if( m_clsTranxRFID->API_IsOpen() == true)	pDC->SetTextColor(RGB( 0, 255, 0));
			//Mod by yo chen
			CString strTmp;
			m_textLinkState.GetWindowText(strTmp);
			if(strTmp == "Connected")	pDC->SetTextColor(RGB( 0, 255, 0));
			else						pDC->SetTextColor(RGB(255, 0, 0));

			return (HBRUSH) B;

		}break;

		default:
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}

	return hbr;
}




//====================================================

void CPageAdvance::API_SetThdEnable(bool bEnableFlag)
{					
	m_bThdEnableFlag = bEnableFlag;

	//add by yo chen, when change page, seting the thread, 2011/10/03
	//1. IF exit Advance Page (bEnableFlag == false)
	if(bEnableFlag == false)
	{
		if(isUARTThdRun == true)
		{
			endThd_UART = 0;//terminate the UART thread
			m_clsTranxRFID->API_CloseComm();
		}
		else
		{
			endThd = 0;//terminate the USB thread		
			m_clsTranxRFID->API_CloseComm();
		}
	}

	//2. IF open/back Advance Page (bEnableFlag == true)
	else if(bEnableFlag == true)
	{
		if(isUSBThdRun == true)
			bootThd();
		else if(isUARTThdRun == true)
		{
			OpenUART();
			bootThd_UART();

			//add by yo chen, recovery the FW version, 2011/10/04
			CString tempSt1;
			int askVerTimeout = 0;
			m_EditVersion.SetWindowText( _T("") );
			m_EditVersion.GetWindowText(tempSt1);		
			while( tempSt1 == "" && askVerTimeout < 100)
			{
				if(true == m_clsTranxRFID->GetModuleID_new())
				{
					m_cstrVersion = m_clsTranxRFID->fwVersion;
					m_EditVersion.SetWindowText(m_cstrVersion);//get FW version string
					m_EditVersion.GetWindowText(tempSt1);
				}
				askVerTimeout++;
			}
			//END, add by yo chen, recovery the FW version, 2011/10/04
		}
	}
	//END, add by yo chen, when change page, seting the thread, 2011/10/03
}

//add by yo chen, boot checkThread() when click 'Open' for USB mode, 2011/09/09
bool  CPageAdvance::bootThd(void)
{
	InterlockedIncrement(&endThd);
	m_clsThread = AfxBeginThread( (AFX_THREADPROC)CheckThread, (LPVOID)this);
	if(m_clsThread == NULL)
		return false;
	else
		return true;
}
bool  CPageAdvance::bootThd_UART(void)
{	
	isBootThd_UART = true;
	InterlockedIncrement(&endThd_UART);
	m_clsThread = AfxBeginThread( (AFX_THREADPROC)CheckThread_UART, (LPVOID)this);
	if(m_clsThread == NULL)
		return false;
	else
		return true;
}
//=====add by yo chen, 2011/09/09
bool CPageAdvance::DetectSerial()
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
void CPageAdvance::OnBnClickedButton3_Open()
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
			{
				InterlockedDecrement(&endThd_UART);
			}
			m_clsTranxRFID->API_CloseComm();
			isUARTThdRun = false;
		}

		m_BtnStart.SetWindowText( _T("Open") );
		m_cmbPort.EnableWindow(true);
		m_BtnSend.EnableWindow(false);
		//m_EditVersion.SetWindowText( _T("") );
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
			m_BtnSend.EnableWindow(true);
			m_bStart = !m_bStart;
			
			bootThd();//boot the checkThread()
			if(m_clsTranxRFID->API_IsOpen() == true)
				AddMemo( _T("Connected") );//show "Connect" message on memo field
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
				////Send Command to check
				if ( false == m_clsTranxRFID->GetModuleID_new() )
				{
					AddMemo( _T("Connected with the device unsuccessfully.") );
					m_clsTranxRFID->API_Close();		
				}		
				else
				{
					bootThd_UART();//boot thread here!!!
					isUARTThdRun = true;

					//reset all step flag when stop the thread
					stepWriteDone = false;
					stepEnter = false;
					stepBegin = false;
					stepWrite = false;
					stepExit = false;

					m_BtnStart.SetWindowText( _T("Stop") );
					m_BtnSend.EnableWindow(true);
					//AddMemo( _T("Open communication port successfully.") );			
					m_cstrVersion = m_clsTranxRFID->fwVersion;
					m_EditVersion.SetWindowText(m_cstrVersion);//get FW version string
					m_cmbPort.EnableWindow(false);
					m_bStart = !m_bStart;

					m_Msg.LoadString(STR_ACT_CONNECTED);
					m_textLinkState.SetWindowText( m_Msg );
					AddMemo( m_Msg );

					if(CheckMode(BL_MODE) == true)
					{
						AddMemo( _T("Please update firmware again or send 0xD2 command to exit [BL] mode") );
					}
				}
			}
		}
	}
}

//add by yo chen, 2011/09/15
UINT WINAPI CPageAdvance::CheckThread_UART(LPVOID r_vpAppCls)
{	
	CPageAdvance *thisCls = (CPageAdvance *)r_vpAppCls;

	//=====set the para. for API_Read()
	int Length = thisCls->m_clsTranxRFID->API_AskRevCount();
	UCHAR tempRxBuf[128];
	memset(tempRxBuf, 0 ,128);			
	TBYTE tempRxDataLen = 128;
	
	//befor boot the thread, reset all flag
	//thisCls->stepWriteDone = false;

	while(thisCls->endThd_UART)
	{	
		
		if(thisCls->m_bThdEnableFlag == false)
		{
			thisCls->m_bUpdateFlag = false;
			Sleep(200);
			continue;
		}

		//EnterCriticalSection(&thisCls->m_CriticalSection);

		
		//Check User Cmd
		if(thisCls->m_enumUserCmdFlag != USER_SEND_NOTHING)
		{	

			if( thisCls->m_enumUserCmdFlag == USER_SEND_SELECT )
			{
				if( thisCls->m_uiUserCmd == 0 )
				{
					thisCls->m_Msg.LoadString(STR_ERR_CMD_ZERO);
					thisCls->AddMemo( thisCls->m_Msg );
				}

				if(thisCls->m_uiUserCmd == CMD_WRITE_IMAGE_B)//1. Write CMD
				{
										
					//check if send the Write CMD repeatedly
					if(thisCls->stepWriteDone == true)
					{
						thisCls->AddMemo( _T("Please sned command 0xD4 \"Begin Update\" to update" ) );
						thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
						continue;
					}
					//check if send the Begin CMD
					//else if(thisCls->stepBegin == false)
					//{
					//	thisCls->AddMemo( _T("Please sned command 0xD4 \"Begin Update\"") );
					//	//thisCls->AddMemo( _T("\"0xD6 Write Image\" fail!!, ") + thisCls->PrintError());	
					//	thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
					//	continue;
					//}

					//only for writeImage CMD
					thisCls->m_ProgressBar.SetPos(0);
					if(thisCls->m_strFilePath.IsEmpty() == true)
					{				
						thisCls->m_Msg.LoadString(STR_ACT_SELECT_DIRECTORY);	
						thisCls->AddMemo(thisCls-> m_Msg );
						thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
						continue;//break;
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
						thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
						continue;//break;
					}

					//==============Hanlde File Size==================//					
					ZeroMemory(&thisCls->m_strcLargeInt, sizeof(LARGE_INTEGER));

					//Get File Size
					if(  !GetFileSizeEx(thisCls->m_hdlImageFile , &thisCls->m_strcLargeInt) )
					{				
						thisCls->m_Msg.LoadString(STR_ERR_CAN_NOT_GET_IMAGE_SIZE);	
						thisCls->AddMemo(thisCls-> m_Msg );
						thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
						continue;//break;
					}
					
					if(thisCls->m_strcLargeInt.LowPart == 0)
					{
						thisCls->m_Msg.LoadString(STR_ERR_FILE_SIZE_ZERO);	
						thisCls->AddMemo( thisCls->m_Msg );
						thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
						continue;//break;
					}
					if( thisCls->MessageBox( _T("Do you want to update?"), _T("Update"), MB_YESNO|MB_ICONQUESTION ) == IDYES )
					{//ask update or not when restart the module
						bool isUpdateFailed = false;
						thisCls->m_BtnStart.EnableWindow(false);
						thisCls->m_BtnSend.EnableWindow(false);
						thisCls->m_BtnBrowser.EnableWindow(false);
						if( !thisCls->m_clsTranxRFID->API_UpdateFw_UART_Init(thisCls->m_hdlImageFile, thisCls->m_strcLargeInt) )//step3. Update_Wtire_Image
						{								
							thisCls->AddMemo( _T("update FW failure") );
							//return false;
						}	
						UINT uiPercent = 0;
						thisCls->AddMemo( _T("Please wait, the module is updating...") );
						while(uiPercent < 100)// && thisCls->m_enumUserCmdFlag != USER_SEND_NOTHING)
						{		
							//1. Tx_bWriteImage() & m_uiCurFileCnt++
							if(!thisCls->m_clsTranxRFID->API_UpdateFw_UART_Write())
							{
								if(thisCls->m_clsTranxRFID->API_AskCmdStatus() != STATUS_OK)
								{
									thisCls->AddMemo( _T("\"0xD6 Write Image\" fail!!, ") + thisCls->PrintError());
									thisCls->AddMemo( _T("Please sned command 0xD4 \"Begin Update\"") );
									isUpdateFailed = true;
									thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
									break;
								}
								/*else
									thisCls->AddMemo( _T("Error"));
								thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
								continue;*/
							}
							
							
							//2. Change Process bar
							uiPercent = thisCls->m_clsTranxRFID->API_AskProgressBarStep();
							if(thisCls->m_ProgressBar.GetPos() != 100 && uiPercent<=100)
								thisCls->m_ProgressBar.SetPos(uiPercent);
							
							//3. Set % status at the update button
							CString cstr,tmpStr;
							cstr.Format(_T("%d%%"),uiPercent);						
							thisCls->m_BtnSend.GetWindowText(tmpStr);						
							if(tmpStr != cstr)
								thisCls->m_BtnSend.SetWindowText(cstr);							
						}
						
						

						//reset All buttons
						thisCls->m_BtnStart.EnableWindow(true);
						thisCls->m_BtnSend.EnableWindow(true);
						thisCls->m_BtnBrowser.EnableWindow(true);
						
						thisCls->m_BtnSend.SetWindowText( _T("Send") );
						thisCls->m_ProgressBar.SetPos(0);						

						if(isUpdateFailed == true)
							continue;
						thisCls->AddMemo( _T("\"0xD6 Write Image \"Update Successfully") );
						thisCls->stepBegin = false;//let ExitCMD can be run
						thisCls->stepWrite = false;
						thisCls->stepWriteDone = true;
					}
					//if click "No"
					else
					{
						thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
						continue;					
					}
											
					clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, thisCls->m_hdlImageFile);
				}
				else if (thisCls->m_uiUserCmd == CMD_VERSION_B)//2. Get version CMD
				{

				}
				else//3. Enter/Begin/Exit CMD
				{
					//check the CMD process is right or not
					/*bool wrongStep = false;
					switch(thisCls->m_uiUserCmd)
					{
						case CMD_ENTER_UPDATE_FW_MODE_BR:	
							break;
						case CMD_BEGIN_UPDATE_BR:
							
							break;
						case CMD_EXIT_UPDATE_FW_MODE_BR:
							break;
						default:
							;
							break;
					}
					if(wrongStep)
						continue;*/
					thisCls->stepWriteDone = false;//reset the flag
					thisCls->stepBegin = false;

					if( !thisCls->m_clsTranxRFID->API_QueryOneCmd(thisCls->m_uiUserCmd) )
					{
						thisCls->m_Msg.LoadString(STR_ERR_WT_FAIL);
						thisCls->AddMemo( thisCls->m_Msg );
					}	
					Sleep(1100);
					memset(tempRxBuf, 0 ,128);					
					thisCls->m_clsTranxRFID->API_Read(tempRxBuf, tempRxDataLen);//receive the data for Enter/Begin/Exit
					/*if(thisCls->m_uiUserCmd != CMD_ENTER_UPDATE_FW_MODE_B && thisCls->m_clsTranxRFID->API_AskCmdStatus() != STATUS_OK)
					{
						thisCls->AddMemo( _T("Error"));
						continue;
					}	*/				

					switch(thisCls->m_uiUserCmd)
					{
						case CMD_ENTER_UPDATE_FW_MODE_B:
							thisCls->AddMemo(_T("\"0xD0 Enter Update FW Mode\" Successfully!! "));
							break;
						case CMD_BEGIN_UPDATE_B:
							thisCls->AddMemo(_T("\"0xD4 Begin Update\" Successfully"));
							thisCls->stepExit = false;
							thisCls->stepWriteDone = false;
							thisCls->stepBegin = true;
							break;
						case CMD_EXIT_UPDATE_FW_MODE_B:
							if(thisCls->stepBegin == true || thisCls->m_clsTranxRFID->API_AskCmdStatus() != STATUS_OK)							
								thisCls->AddMemo( _T("\"0xD2 Exit Update FW Mode\" fail!!, ") + thisCls->PrintError());	
								//error status = 0xFF
							else// if(thisCls->CheckMode(AP_MODE) == true)
							{
								thisCls->AddMemo(_T("\"0xD2 Exit Update FW Mode\" Successfully!! "));
								thisCls->stepWriteDone = false;//reset stepWriteDone when exit BL mode

								//reset all step flag when exit BL mode
								thisCls->stepEnter = false;
								thisCls->stepBegin = false;
								thisCls->stepWrite = false;
								thisCls->stepExit = false;
							}
							
							break;
						default:
							;
							break;				
					}

					//get version (instead of Sleep())
					CString tempSt1;
					thisCls->m_EditVersion.SetWindowText( _T("") );
					thisCls->m_EditVersion.GetWindowText(tempSt1);
					while( tempSt1 == "")
					{
						if(true == thisCls->m_clsTranxRFID->GetModuleID_new())
						{
							thisCls->m_cstrVersion = thisCls->m_clsTranxRFID->fwVersion;
							thisCls->m_EditVersion.SetWindowText(thisCls->m_cstrVersion);//get FW version string
							thisCls->m_EditVersion.GetWindowText(tempSt1);
						}
					}
					//END, get version (instead of Sleep())	


				}				

				if(thisCls->m_uiUserCmd == CMD_VERSION_B)
					thisCls->m_bShowVerToMumo = true;
			}
			else if(thisCls->m_enumUserCmdFlag == USER_SEDN_DATA)
			{
				if( thisCls->m_cstrUserCmdData.IsEmpty() == true )
				{
					thisCls->m_Msg.LoadString(STR_ERR_NO_DATA);
					thisCls->AddMemo( thisCls->m_Msg );
				}

				if( !thisCls->m_clsTranxRFID->API_QueryOneCmd(thisCls->m_cstrUserCmdData) )
				{
					thisCls->m_Msg.LoadString(STR_ERR_WT_FAIL);
					thisCls->AddMemo( thisCls->m_Msg );
				}				
			
			}
				
			//EnterCriticalSection(&thisCls->m_CriticalSection);
			thisCls->m_enumUserCmdFlag = USER_SEND_NOTHING;
			thisCls->EnableBtn(true);
			thisCls->m_uiUserCmd = 0;
			thisCls->m_cstrUserCmdData.Empty();
			//LeaveCriticalSection(&thisCls->m_CriticalSection);
			//break;

		}
		//END, Check User Cmd

		

		//LeaveCriticalSection(&thisCls->m_CriticalSection);
		/*return false;*/
	}	
	return 0;
}
//END, add by yo chen, 2011/09/15

//bool CPageAdvance::ExitBLMode()
//{
//	//Change to AP mode, 2011/10/03
//	//add by yo chen, Check the device in AP mode,
//	//If require updating, don't return to AP mode, 2011/10/03
//	if( CheckMode(BL_MODE) == true )
//	{
//		AddMemo( _T("Please wait, change mode to AP Mode."));
//		m_clsTranxRFID->API_QueryOneCmd(CMD_EXIT_UPDATE_FW_MODE_B);//send EXIT CMD
//		UCHAR tempRxBuf[128];
//		memset(tempRxBuf, 0 ,128);			
//		TBYTE tempRxDataLen = 128;
//		m_clsTranxRFID->API_Read(tempRxBuf, tempRxDataLen);
//		CString tempSt1;
//		int askVerTimeout = 0;
//		m_EditVersion.SetWindowText( _T("") );
//		m_EditVersion.GetWindowText(tempSt1);
//		while( tempSt1 == "" && askVerTimeout < 100 )//get version again to check if module can change to AP Mode
//		{
//			if(true == m_clsTranxRFID->GetModuleID_new())
//			{
//				m_cstrVersion = m_clsTranxRFID->fwVersion;
//				m_EditVersion.SetWindowText(m_cstrVersion);//get FW version string
//				m_EditVersion.GetWindowText(tempSt1);
//			}
//			askVerTimeout++;
//		}
//		if( CheckMode(BL_MODE) == true )//if it's still in BL Mode													
//			return false;	
//		else
//			return true;
//	}					
//	//END, Change to AP mode, 2011/10/03
//	else
//		return true;
//
//}

//add by yo chen, reOpen UART when change pages, 2011/10/07
bool CPageAdvance::OpenUART()
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

// USBControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "USBControl.h"
#include "USBControlDlg.h"
#include "ListCmd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	CListBox *m_MemoStep =(CListBox*)GetDlgItem(IDC_LIST_STEP);

	int isHaveItem = m_MemoStep->GetTextLen(0);
	//if(cstrTmp.IsEmpty() == true)
	if(isHaveItem == 0 || isHaveItem==0xffffffff)
	{
		m_MemoStep->InsertString(0,"Update firmware steps");
		m_MemoStep->InsertString(1,"");
		m_MemoStep->InsertString(2,"I.    Use command 0xD0 to enter the BL mode");
		m_MemoStep->InsertString(3,"II.   Press the Bowser button to select the image that you want to update");
		m_MemoStep->InsertString(4,"III.  Press the Update button. Please wait until the bar shows 100%");
		m_MemoStep->InsertString(5,"IV.  Use command 0xD2 to exit the BL mode");
		m_MemoStep->InsertString(6,"");
		m_MemoStep->InsertString(7,"MTI update firmware tool");
	}
}



// CUSBControlDlg dialog




CUSBControlDlg::CUSBControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBControlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUSBControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_Memo);
	DDX_Control(pDX, IDC_EDIT1, m_EditVersion);
	DDX_Control(pDX, IDC_EDIT2, m_EditCmdData);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressBar);
	DDX_Control(pDX, IDC_COMBO1, m_ComboxSendCmd);
	DDX_Control(pDX, IDC_BUTTON2, m_BtnUpdate);
	DDX_Control(pDX, IDC_STATIC_1, m_textLinkStatus);
}

BEGIN_MESSAGE_MAP(CUSBControlDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CUSBControlDlg::OnBnClickedButton1_Select)
	ON_BN_CLICKED(IDC_BUTTON2, &CUSBControlDlg::OnBnClickedButton2_Update)
	ON_BN_CLICKED(IDC_BUTTON3, &CUSBControlDlg::OnBnClickedButton3_ClearMemo)
	ON_BN_CLICKED(IDC_BUTTON4, &CUSBControlDlg::OnBnClickedButton4_SendCmd)
END_MESSAGE_MAP()


// CUSBControlDlg message handlers

BOOL CUSBControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CoInitialize(NULL);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	Init();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUSBControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUSBControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUSBControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CUSBControlDlg::~CUSBControlDlg()
{
	clsUTILITY::DelObj(CLOSE_TYPE_POINT,  m_clsTranxRFID);
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlThread);
	CoUninitialize();
}

void CUSBControlDlg::Init()
{
	m_uiInterval   = 0;
	m_uiThreadID   = 0;
	m_hdlThread    = INVALID_HANDLE_VALUE;
	m_hdlImageFile = INVALID_HANDLE_VALUE;
	m_clsTranxRFID = NULL;
	m_bUpdateFlag  = false;
	m_clsTranxRFID = new clsTRANX_RFID;
	ZeroMemory(&m_liBefore , sizeof(LARGE_INTEGER));
	ZeroMemory(&m_liNow    , sizeof(LARGE_INTEGER));
	ZeroMemory(&m_liFreq   , sizeof(LARGE_INTEGER));


	//Set main dialog title
	this->SetWindowText(cToolVersion);

	//Set Progress bar 進度表
	m_ProgressBar.SetRange(0,100);	//Set Range
	m_ProgressBar.SetPos(0);		//Set Position
	m_ProgressBar.SetStep(1);		//if calls the function "SetIt", the bar will run 1 step


	//Cmd ComBox
	for(int index=0; index < CMD_COUNT; index++)
		m_ComboxSendCmd.InsertString(index, *(strCmdName + index));
	m_ComboxSendCmd.SetCurSel(0);


	//Link Status Static Text	
	m_clsFont.CreatePointFont(120,_T("Arial"));
	m_textLinkStatus.SetFont(&m_clsFont);
	m_textLinkStatus.SetWindowText( _T("Disconnected") );


	//Set Check Time Frequency
	QueryPerformanceFrequency(&m_liFreq);
	m_liFreq.QuadPart/=1000;  //mini sec


	//Thread
	m_hdlThread = (HANDLE) _beginthreadex(NULL, 0, CheckThread, this, CREATE_SUSPENDED, &m_uiThreadID);

	if(m_hdlThread == INVALID_HANDLE_VALUE)
		return ;	
	if (SetThreadPriority(m_hdlThread, THREAD_PRIORITY_TIME_CRITICAL) == 0)
        return ;

    ResumeThread(m_hdlThread);
}

void CUSBControlDlg::AddMemo(CString str)
{
	m_Memo.InsertString(0,str);
}


//=========================================
void CUSBControlDlg::OnBnClickedButton1_Select()
{
	OPENFILENAME  Dialog;				        //Dialog struct
	TCHAR		  szFile[1024] = TEXT("\0");	//File Name buffer　　　　　　　　　

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
//	dlg->m_ofn.lpstrFile = fileBuffer;  //設定文件名緩衝區，因為只預設200，以防萬一，多增加一個緩衝
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
void CUSBControlDlg::OnBnClickedButton2_Update()
{
	Update();
}

void CUSBControlDlg::OnBnClickedButton3_ClearMemo()
{
	m_Memo.ResetContent();
}


void CUSBControlDlg::OnBnClickedButton4_SendCmd()
{

	if( m_clsTranxRFID->API_IsOpen() == false )	
	{
		AddMemo( _T("Error: Non USB device\n") );
		return ;
	}

	if(m_clsTranxRFID->API_IsBusy() == true)
	{
		AddMemo( _T("Error: Communication is Busy") );
		return;		
	}


	CString cstrCmd;
	int iIndex = 0; 

	//Get Combox Text
	m_ComboxSendCmd.GetWindowText(cstrCmd);
	sscanf(cstrCmd.GetString(), "%x ", &iIndex);
	m_EditCmdData.GetWindowText(cstrCmd);

	
	//Check Mode
	switch(iIndex)
	{
	case CMD_ENTER_UPDATE_FW_MODE_B:{
		if(CheckMode(BL_MODE) == true)
		{
			AddMemo( _T("Error: The device is in the BL mode. Don't need to change again") );
			return;
		}
		break;}
	
	case CMD_BEGIN_UPDATE_B:
		if( CheckMode(AP_MODE) == true )
		{
			AddMemo( _T("Error: Please send the command 0xD0 to change to the BL Mode") );
			return;
		}
		break;

	case CMD_EXIT_UPDAYE_FW_MODE_B:{
		if( CheckMode(AP_MODE) == true )
		{
			AddMemo( _T("Error: The device is in the AP mode. Don't need to change again") );
			return;
		}
		break;}
	}

	if( !m_clsTranxRFID->API_QueryOneCmd(iIndex, cstrCmd) )
	{
		AddMemo( _T("Error: Write fail. Maybe data error or mode error") );
		return;	
	}
}

//============================================================
void  CUSBControlDlg::Update(void)
{
	//If in 0xD6 Write Image status, can't close m_hdlImageFile handle. Return directly.
	if(m_clsTranxRFID->API_IsBusy() == true)
	{
		AddMemo( _T("Error: Communication is Busy") );
		return;		
	}

	while(1)
	{
		m_ProgressBar.SetPos(0);

		if( m_clsTranxRFID->API_IsOpen() == false )
		{
			AddMemo( _T("Error: Non USB device\n") );
			break ;
		}

		if(m_strFilePath.IsEmpty() == true)
		{
			AddMemo( _T("Please select the directory") );
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
			AddMemo( _T("Error: Can't not open the Image") );
			break;
		}


		//==============Hanlde File Size==================//
		LARGE_INTEGER  strcLargeInt;	
		ZeroMemory(&strcLargeInt, sizeof(LARGE_INTEGER));

		//Get File Size
		if(  !GetFileSizeEx(m_hdlImageFile , &strcLargeInt) )
		{
			AddMemo( _T("Error: Can't not get the file size") );
			break;
		}
		
		if(strcLargeInt.LowPart == 0)
		{
			AddMemo( _T("Error: The size of File is ZERO") );
			break;
		}


		//==============Enter Update Status==============//
		m_bUpdateFlag = true;

		//If in AP Mode, change to BL Mode. Wait next to start update flow.
		//When restart the device, get the version, and judge whether update or not.
		if( CheckMode(AP_MODE) == true )
		{
			CString tmp;
			m_clsTranxRFID->API_QueryOneCmd(CMD_ENTER_UPDATE_FW_MODE_B, tmp);
			AddMemo( _T("Please wait, change mode to BL Mode") );
			break ;
		}

		//Start to update
		if( !m_clsTranxRFID->API_UpdateFw(m_hdlImageFile, strcLargeInt) )
		{
			AddMemo( _T("Error: Communication is Busy") );
			break ;
		}

		AddMemo( _T("Start to Update\n") );
		return;
	}

	clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
}


//Thread====================================
UINT WINAPI CUSBControlDlg::CheckThread(LPVOID r_vpAppCls)
{
	UINT uiAskVersionTime = 0;
	UINT uiInterval =0;
	CUSBControlDlg *thisCls = (CUSBControlDlg *)r_vpAppCls;

try
{
	while(1)
	{
		//Calcaulation the time		
		QueryPerformanceCounter(&thisCls->m_liNow);
		uiInterval = (UINT)((thisCls->m_liNow.QuadPart - thisCls->m_liBefore.QuadPart)/thisCls->m_liFreq.QuadPart);
		thisCls->m_liBefore = thisCls->m_liNow;

		//Check time. Avoid timer crash the communication
		thisCls->m_uiInterval += uiInterval;
		uiAskVersionTime      += uiInterval;
		if(thisCls->m_uiInterval >=TIMER_INTERVAL)		thisCls->m_uiInterval = 0;
		else
		{
			Sleep(TIMER_INTERVAL);
			continue;
		}


		//Check Device status. If USB is disconnect, try to link.
		if( thisCls->m_clsTranxRFID->API_IsOpen() == false )
		{
			thisCls->m_textLinkStatus.SetWindowText( _T("Disconnected") );
		
			if(thisCls->m_clsTranxRFID->API_StartComm() == true)
			{
				QueryPerformanceCounter(&thisCls->m_liBefore);
				thisCls->m_uiInterval = 0;
		
				thisCls->m_textLinkStatus.SetWindowText( _T("Connected") );

				//Show Version. It means that the USB is linked
				thisCls->m_clsTranxRFID->API_bVersion();
			}

			Sleep(TIMER_INTERVAL);
			continue ;
		}


		//Communication
		if(thisCls->m_clsTranxRFID->API_IsBusy() == false )
		{
			//Ask version to check status
			if(uiAskVersionTime >= 2000 )
			{
				uiAskVersionTime = 0;

				//if( cstrEdit.CompareNoCase(_T("Un Link")) == 0  )
				thisCls->m_clsTranxRFID->API_bVersion();
			}
			else
			{
				uiAskVersionTime += TIMER_INTERVAL;
			}

		}
		else	//Pooling
		{
			
			thisCls->AnalyzePacket( thisCls->m_clsTranxRFID->API_Regular(TIMER_INTERVAL) );
		}


		Sleep(TIMER_INTERVAL);
	}//while o

	return 0;

}
catch(...)
{
return 0 ;
}
}

//==========================================

void CUSBControlDlg::AnalyzePacket(ENUM_CMD enumRtCmd)
{
	switch( enumRtCmd )
	{
		case CMD_TIME_OUT:				
			m_BtnUpdate.SetWindowText( _T("Update") );
			m_ProgressBar.SetPos(0);

			AddMemo( _T("Error: Timeout ") );
			m_textLinkStatus.SetWindowText( _T("Disconnected") );
			m_EditVersion.SetWindowText( _T("") );

			m_clsTranxRFID->API_CloseComm();
			clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
			break;

		case CMD_VERSION__BR:{
			CString strVersion = m_clsTranxRFID->API_AskVersion();

			CString cstrEdit;
			m_EditVersion.GetWindowText(cstrEdit);

			if(cstrEdit != strVersion)
				m_EditVersion.SetWindowText(strVersion); 

			//When restart the device, get the version, and judge whether update or not.
			if(m_bUpdateFlag == true && m_clsTranxRFID->API_IsBusy() == false )
			{				
				if( MessageBox( "Do you still want to update??", "Update", MB_YESNO|MB_ICONQUESTION ) == IDYES )
					Update();
				else
				{
					m_bUpdateFlag = false;

					if( CheckMode(BL_MODE) == true )
					{
						AddMemo( _T("Please wait, change mode to AP Mode.") );
						AddMemo( _T("If can't return to AP Mode, please update image again by press \"Update button\".") );

						CString tmp;
						m_clsTranxRFID->API_QueryOneCmd(CMD_EXIT_UPDAYE_FW_MODE_B, tmp);
						break ;
					}
				}

			}
			break;}

		case CMD_ENTER_UPDATE_FW_MODE_BR:		
			if(m_clsTranxRFID->API_AskCmdStatus() == STATUS_OK)
			{
				AddMemo( _T("\"0xD0 Enter Update FW Mode\" Successfully!! ") );

				m_clsTranxRFID->API_CloseComm();
				m_textLinkStatus.SetWindowText( _T("Disconnected") );
				AddMemo( _T("Please wait, restart the module. Change to FW mode ") );
			}
			else
			{	
				AddMemo( _T("\"0xD0 Enter Update FW Mode\" fail!!, ") + PrintError());
			}
			
			m_BtnUpdate.SetWindowText( _T("Update") );
			break;

		case CMD_EXIT_UPDAYE_FW_MODE_BR:		
			if(m_clsTranxRFID->API_AskCmdStatus() == STATUS_OK)
			{
				AddMemo( _T("\"0xD2 Exit Update FW Mode\" Successfully!! ") );		
				AddMemo( _T("Please wait, restart the module. Change to AP mode ") );
				m_clsTranxRFID->API_CloseComm();
				m_bUpdateFlag = false;
				m_textLinkStatus.SetWindowText( _T("Disconnected") );
			}
			else
			{	
				AddMemo( _T("\"0xD2 Exit Update FW Mode\" fail!!, ") + PrintError());
			}
			m_BtnUpdate.SetWindowText( _T("Update") );
			break;


		case CMD_BEGIN_UPDATE_BR:
			if(m_clsTranxRFID->API_AskCmdStatus() == STATUS_OK)
			{
				AddMemo( _T("\"0xD4 Begin Update\" Successfully") );		
			}
			else
			{
				AddMemo( _T("\"0xD4 Begin Update\" fail!!, ") + PrintError());
				m_BtnUpdate.SetWindowText( _T("Update") );
			}
			break;	


		case CMD_WRITE_IMAGE_BR:{
			UCHAR Status = m_clsTranxRFID->API_AskCmdStatus();
			if( (Status == STATUS_OK) ||  (Status == STATUS_ERROR_OVER_RANGE) )
			{
				UINT uiPercent = m_clsTranxRFID->API_AskProgressBarStep();
				
				if(uiPercent == 100)//Write Image OK
				{
					AddMemo( _T("\"0xD6 Write Image \"Update Successfully") );	
					//AddMemo( _T("Please use command \"0xD2 Exit Update FW Mode\" to finish") );
					m_BtnUpdate.SetWindowText( _T("Update") );
					clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
					m_bUpdateFlag = false;
				}
				else//continue Wrote Image
				{
					//"Update Button" Set the Caption
					CString cstr;
					cstr.Format("%d%%",uiPercent);
					m_BtnUpdate.SetWindowText(cstr);								
				}

				//Change Process bar
				if(m_ProgressBar.GetPos() != 100 && uiPercent<=100)
					m_ProgressBar.SetPos(uiPercent);
			}
			else
			{
				m_BtnUpdate.SetWindowText( _T("Update") );
				AddMemo( _T("\"0xD6 Write Image\" fail!!, ") + PrintError());	
				clsUTILITY::DelObj(CLOSE_TYPE_HANDLE, m_hdlImageFile);
			}					
			break;}

		case CMD_SET_OEM_CONFIG_BR:
			;
			break;		

		case CMD_NOTHING:
		default:
			;
			break;
	}	
}


CString CUSBControlDlg::PrintError()
{
	UCHAR   ucStatus = 0;     	
	CString strTmp;

	ucStatus = m_clsTranxRFID->API_AskCmdStatus();
	strTmp.Format( " Error code is 0x%02x, ", ucStatus);

	switch(ucStatus)
	{
		case STATUS_ERROR_INVALID_DATA_LENGTH:
			strTmp+="\"Invalid data length\"";
			break;

		case STATUS_ERROR_INVALID_PARAMETER:
			strTmp+="\"Invalid parameter\"";
			break;

		case STATUS_ERROR_WRONG_PROCEDURE:
			strTmp+="\"Wrong procedure\"";
			break;

		case STATUS_ERROR_OVER_RANGE:
			strTmp+="\"Over range\"";
			break;

		case STATUS_ERROR_SYS_MODULE_FAILURE:
			strTmp+="\"System module faillure\"";
			break;

		default:
			strTmp+="\"Not define\"";
	}

	return strTmp;
}

bool CUSBControlDlg::CheckMode(char *CheckMode)
{
	CString strVersion = m_clsTranxRFID->API_AskVersion();

	if( strVersion.IsEmpty()!= true && strncmp(strVersion.GetString() ,CheckMode, 4)==0)
		return true;
	else
		return false;
}

HBRUSH CUSBControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

		// TODO:  Change any attributes of the DC here
		switch (pWnd->GetDlgCtrlID()) 
		{ 
			case IDC_STATIC_1:{

			pDC->SetBkMode(TRANSPARENT);
			//pDC->SetTextColor(RGB(255,255, 0));
			//pWnd->SetFont(cFont);

			//HBRUSH B = color?CreateSolidBrush(RGB(125,125,255)):CreateSolidBrush(RGB(56,64,33));
			//HBRUSH B = (!color?CreateSolidBrush(RGB(125,125,255)):CreateSolidBrush(RGB(238,44,44)));
			HBRUSH B;
			if(m_clsTranxRFID->API_IsOpen() == true)
			{
				B = CreateSolidBrush(RGB(125,125,255));
				//m_EditVersion.SetWindowText(_T (" Link"));
			}
			else
			{
				B = CreateSolidBrush(RGB(238,44,44));
				//m_EditVersion.SetWindowText(_T ("Un Link"));		
			}

			return (HBRUSH) B;
			}break;

			default:
			return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		}

	return hbr;
}

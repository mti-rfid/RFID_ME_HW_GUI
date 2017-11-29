#include "stdafx.h"
#include "CMenuCtrl.h"


// CMenuCtrl

IMPLEMENT_DYNAMIC(CMenuCtrl, CTabCtrl)
CMenuCtrl::CMenuCtrl()
{
}

CMenuCtrl::~CMenuCtrl()
{
}


BEGIN_MESSAGE_MAP(CMenuCtrl, CTabCtrl)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CMenuCtrl::API_Init(void)
{
    m_iTabSelected = 0;
    m_tabPages[PAGE_BASIC] = new CPageBasic;
    m_tabPages[PAGE_ADVANCE] = new CPageAdvance;
	((CPageBasic *)m_tabPages[PAGE_BASIC] )->API_SetThdEnable(true);

    m_tabPages[PAGE_BASIC]->Create(IDD_DIALOG1_BASIC, this);
    m_tabPages[PAGE_ADVANCE]->Create(IDD_DIALOG2_ADVANCE, this);

    m_tabPages[PAGE_BASIC]->ShowWindow(SW_SHOW);
    m_tabPages[PAGE_ADVANCE]->ShowWindow(SW_HIDE);
    SetRectangle();
}


void CMenuCtrl::SetRectangle(void)
{
	CRect TabRect,itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&TabRect);
	GetItemRect(0, &itemRect);

	//Paint Tab
	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=TabRect.right-itemRect.left-1;
	nYc=TabRect.bottom-nY-1;

	m_tabPages[PAGE_BASIC]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);

	for(int nCount=1; nCount < PAGE_COUNT; nCount++)
    {
		m_tabPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
	}


	//Paint Main
	GetParent()->GetClientRect(&m_strcMainRect);    
	GetParent()->SetWindowPos( &wndTop, 
								m_strcMainRect.left, 
								m_strcMainRect.top, 
		                        m_strcMainRect.right, 
								m_strcMainRect.bottom - SUB_LENGTH, 
								SWP_SHOWWINDOW);



	//CRect RectAdvance;
	//GetItemRect(0, &m_rectBasic);
	//GetItemRect(1, &RectAdvance);
	//this->GetDlgItem(IDD_DIALOG1_BASIC)->GetClientRect(this->GetParent&m_rectBasic);
	//this->GetDlgItem(IDD_DIALOG2_ADVANCE)->GetClientRect(&RectAdvance);

	//m_rectBasic.left   = RectAdvance.left   - m_rectBasic.left;
	//m_rectBasic.top    = RectAdvance.top    - m_rectBasic.top;
	//m_rectBasic.right  = RectAdvance.right  - m_rectBasic.right;
	//m_rectBasic.bottom = RectAdvance.bottom - m_rectBasic.bottom;
	//GetParent()->GetDlgItem(IDD_DIALOG1_BASIC)->GetClientRect(&m_rectBasic);
	//GetParent()->GetDlgItem(IDD_DIALOG2_ADVANCE)->GetClientRect(&RectAdvance);
}

void CMenuCtrl::SetRectangle(UINT uiDlgID)
{
	int nX = 0, nY = 0, nXc = 0, nYc = 0;

	switch(uiDlgID)
	{
		case PAGE_BASIC:
			nX=m_strcMainRect.left;
			nY=m_strcMainRect.top;
			nXc=m_strcMainRect.right;
			nYc=m_strcMainRect.bottom - SUB_LENGTH;
			break;

		case PAGE_ADVANCE:
			nX=m_strcMainRect.left;
			nY=m_strcMainRect.top;
			nXc=m_strcMainRect.right;
			nYc=m_strcMainRect.bottom ;
			break;
	}

	GetParent()->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_NOMOVE);
}

// udcMenuCtrl 
void CMenuCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(MessageBox( _T("Change software configure will abort current task.\nDo you change software configure?"), 
		           _T("Configure"), 
				   MB_YESNO|MB_ICONQUESTION ) == IDNO )
	{
		return;
	}

    CTabCtrl::OnLButtonDown(nFlags, point);
	int iNextTabSelected = 0;

	iNextTabSelected = GetCurFocus();

    if(m_iTabSelected != iNextTabSelected)
	{
		switch(iNextTabSelected)
		{
			case PAGE_BASIC:{
				CPageAdvance *clsPageAdvance = (CPageAdvance *)m_tabPages[m_iTabSelected];	
				CPageBasic   *clsPageBasic   = (CPageBasic *)m_tabPages[iNextTabSelected];		

				clsPageAdvance->API_SetThdEnable(FALSE);
				clsPageBasic->API_SetThdEnable(TRUE);	
				SetRectangle(PAGE_BASIC);
				break;}

			case PAGE_ADVANCE:{
				CPageBasic   *clsPageBasic    = (CPageBasic *)m_tabPages[m_iTabSelected];
				CPageAdvance *clsPageAdvance  = (CPageAdvance *)m_tabPages[iNextTabSelected];
	
				clsPageBasic->API_SetThdEnable(FALSE);			
				clsPageAdvance->API_SetThdEnable(TRUE);	
				SetRectangle(PAGE_ADVANCE);
				break;}
		}


		m_tabPages[m_iTabSelected]->ShowWindow(SW_HIDE);
		m_tabPages[iNextTabSelected]->ShowWindow(SW_SHOW);
		m_tabPages[iNextTabSelected]->SetFocus();

		m_iTabSelected = iNextTabSelected;
	}
}
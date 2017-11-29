#pragma once
#include "CPageBasic.h"
#include "CPageAdvance.h"
#include "USBControl.h"

//PAGE
#define PAGE_BASIC      0
#define PAGE_ADVANCE    1
#define PAGE_COUNT      2


//Main sub length
#define SUB_LENGTH	165

// udcMenuCtrl

class CMenuCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CMenuCtrl)

public:
	CMenuCtrl();
	virtual ~CMenuCtrl();
    void    API_Init(void);

protected:
	DECLARE_MESSAGE_MAP()

private:
	void *m_vpAppCls;
	CRect m_strcMainRect;

    void SetRectangle(void);
	void SetRectangle(UINT uiDlgID);

private:
    CDialog *m_tabPages[PAGE_COUNT];
    int		 m_iTabSelected;

public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};



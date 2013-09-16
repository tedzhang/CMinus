#pragma  once


#include <afxtempl.h>
#include "sizecbar.h"

class COutputBar : public CSizingControlBar
{

public:

public:
	CRichEditCtrl &GetRichEditCtrl() { return m_EditCtrl;}
protected:
    
	CRichEditCtrl m_EditCtrl;
	//{{AFX_MSG(CSizingTabCtrlBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//}}AFX_MSG

public:
	virtual BOOL Create(LPCTSTR lpszWindowName, CWnd* pParentWnd,
		CSize sizeDefault, BOOL bHasGripper, UINT nID,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);


	DECLARE_MESSAGE_MAP()
};


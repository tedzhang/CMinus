#include"stdafx.h"
#include "resource.h"
#include "OutputBar.h"

BEGIN_MESSAGE_MAP(COutputBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CSizingTabCtrlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL COutputBar::Create(LPCTSTR lpszWindowName, CWnd* pParentWnd,
					CSize sizeDefault, BOOL bHasGripper, UINT nID,
					DWORD dwStyle )
{
	if(!CSizingControlBar::Create(lpszWindowName,pParentWnd,sizeDefault,bHasGripper,nID,dwStyle))
		return FALSE;

	DWORD richeditstyle = WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL
		| ES_WANTRETURN | WS_TABSTOP | ES_READONLY;
	VERIFY(m_EditCtrl.CreateEx(WS_EX_CLIENTEDGE,richeditstyle,CRect(0,0,0,0),this,ID_BOTEDIT));

	HFONT hFont = (HFONT) (::GetStockObject(DEFAULT_GUI_FONT));
	CFont font;
	font.Attach(hFont);
	m_EditCtrl.SetFont(&font);
	return TRUE;

}




void COutputBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType,cx,cy);
	if(m_EditCtrl.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(&rc);
		m_EditCtrl.MoveWindow(&rc);

	}
	
}
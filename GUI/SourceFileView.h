#pragma once

#include <afxtempl.h>
#include "sizecbar.h"
// CSourceFileView view

class CSourceFileView : public CSizingControlBar
{
	DECLARE_DYNCREATE(CSourceFileView)
public:
	CTreeCtrl & GetTreeCtrl() { return m_wndTreeCtrl;}
	virtual ~CSourceFileView();
	CSourceFileView();  
	void InsertItem(const CString& srcFileName,HTREEITEM hParent= TVI_ROOT);

protected:
	CImageList m_imglist;
	CTreeCtrl m_wndTreeCtrl;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void OnTreeSelChange(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
public:
	BOOL Create(LPCTSTR lpszWindowName, CWnd* pParentWnd,
		CSize sizeDefault, BOOL bHasGripper,
		UINT nID,  DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);
   void OnSize(UINT nType, int cx, int cy);


};



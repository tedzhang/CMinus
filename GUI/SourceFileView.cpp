// SourceFileView.cpp : implementation file
//

#include "stdafx.h"
#include "GUI.h"
#include "SourceFileView.h"
#include ".\sourcefileview.h"
#include "GUIDoc.h"
#include "MainFrm.h"
#include "GUIView.h"
// CSourceFileView

IMPLEMENT_DYNCREATE(CSourceFileView, CSizingControlBar)

CSourceFileView::CSourceFileView()
{
	
}

CSourceFileView::~CSourceFileView()
{
	
	m_imglist.DeleteImageList();
}

BEGIN_MESSAGE_MAP(CSourceFileView, CSizingControlBar)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED,ID_LEFTTREE,OnTreeSelChange)
END_MESSAGE_MAP()


// CSourceFileView diagnostics

#ifdef _DEBUG
void CSourceFileView::AssertValid() const
{
	CSizingControlBar::AssertValid();
}

void CSourceFileView::Dump(CDumpContext& dc) const
{
	CSizingControlBar::Dump(dc);
}
#endif //_DEBUG


// CSourceFileView message handlers

BOOL CSourceFileView::Create(LPCTSTR lpszWindowName, CWnd* pParentWnd,
							CSize sizeDefault, BOOL bHasGripper,
							UINT nID, DWORD dwStyle)
{
	if(!CSizingControlBar::Create(lpszWindowName,pParentWnd,sizeDefault,bHasGripper,nID,dwStyle))
		return FALSE;

	DWORD dwStyletree = TVS_HASBUTTONS|TVS_LINESATROOT|TVS_HASLINES|TVS_LINESATROOT;
	if(!m_wndTreeCtrl.CreateEx(WS_EX_CLIENTEDGE,WS_VISIBLE|WS_CHILD|dwStyletree,CRect(0,0,0,0),this,ID_LEFTTREE))
		return FALSE;
	m_imglist.Create(16, 16, ILC_MASK|ILC_COLOR32, 0, 1);
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_SRCFILE);
    m_imglist.Add(hIcon);
	m_wndTreeCtrl.SetImageList(&m_imglist,TVSIL_NORMAL);

	return TRUE;
}

void CSourceFileView::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType,cx,cy);
	if(m_wndTreeCtrl.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(&rc);
		m_wndTreeCtrl.MoveWindow(&rc);

	}

}
void CSourceFileView::InsertItem(const CString& srcFileName,HTREEITEM hParent/* = TVI_ROOT */)
{
	
	TV_INSERTSTRUCT insertitem;
	insertitem.hParent =hParent;
	insertitem.item.mask =TVIF_IMAGE|TVIF_TEXT|TVIF_SELECTEDIMAGE;
	insertitem.item.hItem  = NULL;
	insertitem.item.pszText= (LPTSTR)(LPCTSTR)srcFileName;
	insertitem.item.iImage = 0;
	insertitem.item.iSelectedImage = 0;
	HTREEITEM hItem = m_wndTreeCtrl.InsertItem(&insertitem);
}

void CSourceFileView:: OnTreeSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pItem = (LPNMTREEVIEW)pNMHDR;
	HTREEITEM hitem = pItem->itemNew.hItem;
	CString strItemText = m_wndTreeCtrl.GetItemText(hitem);

	CMainFrame *pFrm = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	POSITION  pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate *pTemplate = AfxGetApp()->GetNextDocTemplate(pos);
    POSITION docpos1 = pTemplate->GetFirstDocPosition();
    
	BOOL bFound = FALSE;
	CDocument *pActiveDoc = NULL;
	while(docpos1)
	{
		CGUIDoc *pDoc = (CGUIDoc*)(pTemplate->GetNextDoc(docpos1));
		if(pDoc->GetPathName() == strItemText)
		{
			pActiveDoc = pDoc;
			bFound = true;
			break;
		}
	}
	
    if(!pActiveDoc)
	{
	/*	CDocument * pNewDoc = pTemplate->OpenDocumentFile(strItemText,TRUE);*/
		CDocument *pNewDoc =pTemplate->CreateNewDocument();
		pNewDoc->SetPathName(strItemText);
		CMDIChildWnd *pActiveChild = pFrm->MDIGetActive();
		CFrameWnd *pFrame =  pTemplate->CreateNewFrame(pNewDoc,pActiveChild);
		pTemplate->InitialUpdateFrame(pFrame,pNewDoc);
	}
	else
	{
		CMDIChildWnd *pActiveChild = pFrm->MDIGetActive(); 
		do 
		{
			CString strText;
			CGUIView *pView = (CGUIView*)(pActiveChild->GetActiveView());
			strText = pView->GetFilePathName();
		    if(strText== strItemText)
		    {
			   pActiveChild->MDIActivate();
			   break;
		    }
		   pActiveChild = (CMDIChildWnd*)pActiveChild->GetWindow(GW_HWNDNEXT);
		} while(pActiveChild);
	}
	//pFrm->RecalcLayout();
}


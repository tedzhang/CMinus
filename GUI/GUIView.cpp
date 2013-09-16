// GUIView.cpp : implementation of the CGUIView class
//

#include "stdafx.h"
#include "GUI.h"

#include "GUIDoc.h"
#include "GUIView.h"
#include ".\guiview.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <process.h>

// CGUIView

IMPLEMENT_DYNCREATE(CGUIView, CView)

BEGIN_MESSAGE_MAP(CGUIView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_COMPILE_LEXPARSE, OnCompileLexparse)
	ON_COMMAND(ID_COMPILE_SYNATXPARSE, OnCompileSynatxparse)
	ON_COMMAND(ID_COMPILE_SYMBOLTABLECONSTRUCTION, OnCompileSymboltableconstruction)
	ON_COMMAND(ID_COMPILE_GENERATEPCODE, OnCompileGeneratepcode)
	ON_COMMAND(ID_COMPILE_GENERATEASSEMBLYCODE, OnCompileGenerateassemblycode)
	ON_COMMAND(ID_COMPILE_GENERATEEXECUTABLEFILE, OnCompileGenerateexecutablefile)
	ON_COMMAND(ID_COMPILE_RUN, OnCompileRun)
END_MESSAGE_MAP()

// CGUIView construction/destruction

CGUIView::CGUIView()
{
	// TODO: add construction code here

}

CGUIView::~CGUIView()
{
}

BOOL CGUIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CGUIView drawing

void CGUIView::OnDraw(CDC* /*pDC*/)
{
	CGUIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CGUIView printing

BOOL CGUIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGUIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGUIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CGUIView diagnostics

#ifdef _DEBUG
void CGUIView::AssertValid() const
{
	CView::AssertValid();
}

void CGUIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGUIDoc* CGUIView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGUIDoc)));
	return (CGUIDoc*)m_pDocument;
}
#endif //_DEBUG

BOOL CGUIView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR *phDR;
	phDR = (NMHDR*)lParam;
	// does notification come from my scintilla window?
	if (phDR != NULL && phDR->hwndFrom == m_wndScintilla.m_hWnd)
	{
		SCNotification *pMsg = (SCNotification*)lParam;
		switch (phDR->code)
		{
		case SCN_STYLENEEDED:
			break;
		case SCN_CHARADDED:
			{
				CDocument *pDoc = GetDocument();
				pDoc->SetModifiedFlag(TRUE);
			}
			break;
		case SCN_SAVEPOINTREACHED:
			break;
			//       called when the document is changed - mark document modified
		case SCN_SAVEPOINTLEFT:
			{
				CDocument *pDoc = GetDocument();
				pDoc->SetModifiedFlag(TRUE);
			}
			break;
		case SCN_MODIFYATTEMPTRO:
			break;
		case SCN_KEY:
			break;
		case SCN_DOUBLECLICK:
			break;
			//       called when something changes and we want to show new indicator state or brace matching
		case SCN_UPDATEUI:
			{
				m_wndScintilla.UpdateUI();
			}
			break;
		case SCN_MODIFIED:
			break;
		case SCN_MACRORECORD:
			break;
			//       user clicked margin - try folding action
		case SCN_MARGINCLICK:
			{
				m_wndScintilla.DoDefaultFolding(pMsg->margin, pMsg->position);
			}
			break;
		case SCN_NEEDSHOWN:
			break;
		case SCN_PAINTED:
			break;
		case SCN_USERLISTSELECTION:
			break;
		case SCN_URIDROPPED:
			break;
		case SCN_DWELLSTART:
			break;
		case SCN_DWELLEND:
			break;
		}
		return TRUE; // we processed the message
	}
	return CWnd::OnNotify(wParam, lParam, pResult);
}
BOOL CGUIView::Save(LPCSTR szPath)
{
	return m_wndScintilla.SaveFile(szPath);
}

// CGUIView message handlers

int CGUIView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CString strTitle;
	strTitle.LoadString(IDS_SYNATXEDIT_CTRLWNDNAME);
	if (!m_wndScintilla.Create(strTitle, WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_SYNATXEDIT)) // hb - todo autogenerate id
	{   
		LPVOID lpMsgBuf;
		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,0,NULL);
		// Write to stderr
		TRACE (_T("%s\n"), (LPCTSTR)lpMsgBuf);
		// Free the buffer.
		LocalFree( lpMsgBuf );
		return -1;
	}
  
	return 0;
}

void CGUIView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_wndScintilla.GetSafeHwnd())
	{
		m_wndScintilla.MoveWindow(0, 0, cx, cy);
	}
}

void CGUIView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CDocument *pDoc = GetDocument();
	CString strTitle = pDoc->GetPathName();

	m_wndScintilla.Init();
	
	m_wndScintilla.SetDisplayLinenumbers(TRUE);
	m_wndScintilla.SetLexer(SCLEX_CPP);
	m_wndScintilla.SetDisplayFolding(TRUE);
	m_wndScintilla.SendMessage(SCI_SETKEYWORDS, 0, (long)_T("and and_eq asm auto bitand bitor bool break case catch char class compl const const_cast continue default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline int long mutable namespace new not not_eq operator or or_eq private protected public register reinterpret_cast return short signed sizeof static static_cast struct switch template this throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq"));
     m_wndScintilla.LoadFile(strTitle);

}

 unsigned _stdcall CGUIView:: WorkThread(LPVOID pvoid)
{
      CompilerThreadParam * pParam = (CompilerThreadParam*) pvoid;
	  CProxy proxy;
	  CString csOutput = proxy.Execute(pParam->filePath,pParam->opinion);

	  if(pParam->opinion != CProxy::kRUNEXE && csOutput.IsEmpty())
	  {
		  CString strResult;
		  strResult.LoadString(IDS_ERRINVOKETOOL);
		  pParam->pView->OutPutResult(strResult);
	  }
	  else
	    pParam->pView->OutPutResult(csOutput);
	  return 0;
}

void CGUIView:: OutPutResult(const CString& strText)
{
	CMainFrame * mainfrm = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CRichEditCtrl  &pEdit = mainfrm->GetOutPutWindow();
	
	if(!strText.IsEmpty())
	 pEdit.SetWindowText(strText);
}
CString CGUIView::GetFilePathName()
{
	CDocument *pDoc = GetDocument();
	CString strTitle = pDoc->GetPathName();
	if(strTitle.IsEmpty() || pDoc->IsModified())
	{
		if(!pDoc->DoFileSave())
			return CString();
	}
	strTitle = pDoc->GetPathName();
	return strTitle;
}
inline void CGUIView::BeginProcessing(CProxy::ExeOpinion opin)
{
	CString strTitle = GetFilePathName();
	if(strTitle.IsEmpty()) return;
	UINT threadid;
	m_Param.filePath = strTitle;
	m_Param.opinion = opin;
	m_Param.pView   = this;
	_beginthreadex(NULL,0,WorkThread,&m_Param,0,&threadid);

}

void CGUIView::OnCompileLexparse()
{
	// TODO: Add your command handler code here	
	BeginProcessing(CProxy::kLEX);
}


void CGUIView::OnCompileSynatxparse()
{
	BeginProcessing(CProxy::kSYNATX);
}

void CGUIView::OnCompileSymboltableconstruction()
{
	BeginProcessing(CProxy::kTYPECHECKSYMTBL);
}

void CGUIView::UpdateSrcView(const CString& strFileName)
{
	//Find the root node
	CString strCFileName = GetFilePathName();
	//find the .c or.C extension
	int pos = strCFileName.ReverseFind('.');
	strCFileName.Delete(pos,strFileName.GetLength()-pos);//".c"or ".C"
	strCFileName.Append(".c");

	//Find the root node of it
	CMainFrame * pfrm = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CTreeCtrl & tree = pfrm->GetSrcFileView().GetTreeCtrl();
	HTREEITEM hItem ;
	for (hItem = tree.GetChildItem(TVI_ROOT); hItem != NULL;
		hItem = tree.GetNextSiblingItem(hItem))
	{
		if(tree.GetItemText(hItem) == strCFileName)
			break;			
	}
	//find if it exists or not
	HTREEITEM hParent = hItem;
	for(hItem = tree.GetChildItem(hParent);hItem != NULL;
		hItem = tree.GetNextSiblingItem(hItem))
	{
		if(tree.GetItemText( hItem)== strFileName)
			return;
	}
	pfrm->GetSrcFileView().InsertItem(strFileName,hParent);
	tree.Expand(hItem,TVE_EXPAND);

}
void CGUIView::OnCompileGeneratepcode()
{
	BeginProcessing(CProxy::kGENPCODE);
	//Get the asm file name
	CString strFileName = GetFilePathName();
	if(strFileName.IsEmpty()) return;
	CString strExt;
	//find the .c or.C extension
	int pos = strFileName.ReverseFind('.');
	strFileName.Delete(pos,strFileName.GetLength()-pos);//".c"or ".C"
	strFileName.Append(".pa");
	UpdateSrcView(strFileName);
}

void CGUIView::OnCompileGenerateassemblycode()
{
	BeginProcessing(CProxy::kGENASSEMCODE);
	//Get the asm file name
	CString strFileName = GetFilePathName();
	if(strFileName.IsEmpty()) return;
	CString strExt;
	//find the .c or.C extension
	int pos = strFileName.ReverseFind('.');
	strFileName.Delete(pos,strFileName.GetLength()-pos);//".c"or ".C"
	strFileName.Append(".asm");
    UpdateSrcView(strFileName);


}

void CGUIView::OnCompileGenerateexecutablefile()
{
	// TODO: Add your command handler code here
	BeginProcessing(CProxy::kGENEXE);
}

void CGUIView::OnCompileRun()
{
	BeginProcessing(CProxy::kRUNEXE);
}

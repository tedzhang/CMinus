// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "GUI.h"

#include "MainFrm.h"
#include "SourceFileView.h"
#include "GeneratedFileView.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	//Place the 2 bars

	CString csLefWinName,csBotWinName;
	csLefWinName.LoadString(IDS_LEFTWINNAME);
	csBotWinName.LoadString(IDS_BOTTOMWINNAME);

	//
	VERIFY(m_wndLeftContainer.Create(csLefWinName,this,CSize(300,500),TRUE,ID_LEFTWIN));
	VERIFY(m_wndBottomContainer.Create(csBotWinName,this,CSize(750,150),TRUE,ID_BOTTOMWIN));

	//m_wndLeftContainer.SetSCBStyle()
	DWORD exStyle = CBRS_SIZE_DYNAMIC | CBRS_FLYBY|CBRS_TOOLTIPS|CBRS_GRIPPER;
	m_wndLeftContainer.SetBarStyle(m_wndLeftContainer.GetBarStyle()|exStyle|CBRS_LEFT);
	m_wndBottomContainer.SetBarStyle(m_wndBottomContainer.GetBarStyle()|exStyle|CBRS_BOTTOM);

	m_wndLeftContainer.EnableDocking(CBRS_ALIGN_ANY);
	m_wndBottomContainer.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndLeftContainer,AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndBottomContainer,AFX_IDW_DOCKBAR_BOTTOM);

	//Set the icon
	SetIcon(AfxGetApp()->LoadIcon(IDI_APPICON),TRUE);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::RecalcLayout(BOOL bNotify )
{
	CMDIFrameWnd::RecalcLayout(bNotify);
	//CMDIFrameWnd::RecalcLayout(bNotify);
}

// CMainFrame message handlers



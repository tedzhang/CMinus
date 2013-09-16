// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "SourceFileView.h" //For CSizingControlBar
#include "OutputBar.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	/*CTreeCtrl & GetTreeCtrl() { return m_wndTree;}*/
	CRichEditCtrl     & GetOutPutWindow() { return m_wndBottomContainer.GetRichEditCtrl();}
	CSourceFileView & GetSrcFileView(){ return m_wndLeftContainer;}
// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void RecalcLayout(BOOL bNotify = TRUE);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	CSourceFileView m_wndLeftContainer;
	COutputBar m_wndBottomContainer;
protected:
    int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

};



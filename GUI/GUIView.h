// GUIView.h : interface of the CGUIView class
//


#pragma once


#include "scintillawnd.h"
#include "Proxy.h"

class CGUIView : public CView
{
protected: // create from serialization only
	CGUIView();
	DECLARE_DYNCREATE(CGUIView)

// Attributes
public:
	CGUIDoc* GetDocument() const;

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL Save(LPCSTR szPath);
	virtual CScintillaWnd *GetEditControl(){return &m_wndScintilla;};
public:
	CScintillaWnd     m_wndScintilla;
	CString GetFilePathName();

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CGUIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	typedef struct 
	{
		CGUIView * pView;
		CString filePath;
		CProxy::ExeOpinion opinion;
	} CompilerThreadParam;

    void OutPutResult(const CString& strText);
   
	static unsigned _stdcall WorkThread(LPVOID pvoid);
	CompilerThreadParam m_Param;
	inline void BeginProcessing(CProxy::ExeOpinion opin);
	void UpdateSrcView(const CString& strFileName);
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg void OnCompileLexparse();
	afx_msg void OnCompileSynatxparse();
	afx_msg void OnCompileSymboltableconstruction();
	afx_msg void OnCompileGeneratepcode();
	afx_msg void OnCompileGenerateassemblycode();
	afx_msg void OnCompileGenerateexecutablefile();
	afx_msg void OnCompileRun();
private:

};

#ifndef _DEBUG  // debug version in GUIView.cpp
inline CGUIDoc* CGUIView::GetDocument() const
   { return reinterpret_cast<CGUIDoc*>(m_pDocument); }
#endif


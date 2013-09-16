// GUIDoc.cpp : implementation of the CGUIDoc class
//

#include "stdafx.h"
#include "GUI.h"

#include "GUIDoc.h"
#include "GUIView.h"
#include "SourceFileView.h"
#include "MainFrm.h"
#include ".\guidoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGUIDoc

IMPLEMENT_DYNCREATE(CGUIDoc, CDocument)

BEGIN_MESSAGE_MAP(CGUIDoc, CDocument)
END_MESSAGE_MAP()


// CGUIDoc construction/destruction

CGUIDoc::CGUIDoc()
{
	// TODO: add one-time construction code here

}

CGUIDoc::~CGUIDoc()
{
	for(int i = 0; i < m_Files.size();++i)
	{
		delete m_Files[i];
	}
	m_Files.clear();
}

void CGUIDoc::UpdateSourceFileView(LPCTSTR lpszPathName)
{
	CString temp(lpszPathName);
	if(temp .IsEmpty()) return;
	char ext = temp.GetAt(temp.GetLength()-1);
	if( ext != 'C' && ext != 'c') return;

	CMainFrame *pfrm = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if(!pfrm) return;
	pfrm->GetSrcFileView().InsertItem(lpszPathName);

	
}
BOOL CGUIDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	UpdateSourceFileView(GetPathName());

	

	return TRUE;
}




// CGUIDoc serialization

void CGUIDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	  
	}
	else
	{
		// TODO: add loading code here
		
	}
}


// CGUIDoc diagnostics

#ifdef _DEBUG
void CGUIDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGUIDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGUIDoc commands

BOOL CGUIDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	UpdateSourceFileView(lpszPathName);

	//POSITION pos = GetFirstViewPosition();
	//CGUIView *pFirstView = (CGUIView*)GetNextView( pos ); // assume only one view
	//if (pFirstView != NULL)
	//{
	//	pFirstView->m_wndScintilla.Init();
	//	m_wndScintilla.Init();
	//	pFirstView->m_wndScintilla.SetDisplayLinenumbers(TRUE)
	//	pFirstView->m_wndScintilla.LoadFile(lpszPathName);
	//	
	//}

	return TRUE;
}

BOOL CGUIDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
	POSITION pos = GetFirstViewPosition();
	CGUIView *pFirstView = (CGUIView*)GetNextView( pos ); // assume only one view
	if (pFirstView != NULL)
		return pFirstView->Save(lpszPathName);
   
	 return CDocument::OnSaveDocument(lpszPathName);
}

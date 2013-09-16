// GUIDoc.h : interface of the CGUIDoc class
//


#pragma once

#include <vector>

class CGUIDoc : public CDocument
{
protected: // create from serialization only
	CGUIDoc();
	DECLARE_DYNCREATE(CGUIDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CGUIDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
private:
	void UpdateSourceFileView(LPCTSTR lpszPathName);

protected:
	struct FileInfo
	{
		CString strFilePath;

	};
	std::vector<FileInfo*> m_Files; 
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};



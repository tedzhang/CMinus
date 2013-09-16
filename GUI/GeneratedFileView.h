#pragma once


// CGeneratedFileView view

class CGeneratedFileView : public CTreeView
{
	DECLARE_DYNCREATE(CGeneratedFileView)

protected:
	CGeneratedFileView();           // protected constructor used by dynamic creation
	virtual ~CGeneratedFileView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};



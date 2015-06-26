// mainDoc.h : interface of the CmainDoc class
//
#pragma once
#include <InputOnboard/ComponentEditContext.h>
#include <InputOnBoard/AircraftLayoutEditContext.h>


class CmainDoc : public CDocument
{
protected: // create from serialization only
	CmainDoc();
	DECLARE_DYNCREATE(CmainDoc)

// Attributes
public:

// Operations
public:
	CComponentEditContext mEditMeshContext;
	CAircraftLayoutEditContext mEditLayoutContext;

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CmainDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCloseDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};



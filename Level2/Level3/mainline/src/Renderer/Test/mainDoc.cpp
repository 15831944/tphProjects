// mainDoc.cpp : implementation of the CmainDoc class
//

#include "stdafx.h"
#include "main.h"

#include "mainDoc.h"
#include ".\maindoc.h"
#include <database/ARCportDatabaseConnection.h>

#include <InputOnBoard/AircraftComponentModelDatabase.h>
#include <InputOnBoard/ComponentModel.h>
#include <main\Onboard\OnboardViewMsg.h>
#include <common/UnitsManager.h>
#include <common/AirportDatabase.h>
#include <InputOnBoard/AircaftLayOut.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmainDoc

IMPLEMENT_DYNCREATE(CmainDoc, CDocument)

BEGIN_MESSAGE_MAP(CmainDoc, CDocument)
END_MESSAGE_MAP()

//CAircraftComponentModelDatabase mModelDB;

// CmainDoc construction/destruction
#include "../../Renderer/RenderEngine/3ds2mesh.h"

CmainDoc::CmainDoc()
{
	// TODO: add one-time construction code here
	mEditMeshContext.SetDocument(this);
	UNITSMANAGER->SetLengthUnits(UM_LEN_METERS);

	mEditMeshContext.SetCurrentComponent(new CComponentMeshModel(NULL));
	//test code
	//C3DS2Mesh::ImportModel("S:\\coord.3ds",*mEditMeshContext.GetCurrentComponent());
	
	mEditLayoutContext.SetEditLayOut(new CAircaftLayOut());
}

CmainDoc::~CmainDoc()
{
}

BOOL CmainDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	CString strInputPath = _T("C:\\");

	//get arcport.mdb path
	CString strArcportdbPath;
	strArcportdbPath.Format(_T("%s\\arcport.mdb"),strInputPath);

	CString strPartsDatabasePath(_T(""));
	strPartsDatabasePath.Format(_T("%s\\project.mdb"),strInputPath);
	DATABASECONNECTION.SetInputPath(strInputPath);
	DATABASECONNECTION.SetAccessDatabasePath(strArcportdbPath);



	return TRUE;
}




// CmainDoc serialization

void CmainDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CmainDoc diagnostics

#ifdef _DEBUG
void CmainDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CmainDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CmainDoc commands

void CmainDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateAllViews(NULL,VM_ONBOARD_COMPONENT_SAVE_MODEL);
	mEditMeshContext.SaveEditModel();
	delete mEditMeshContext.GetCurrentComponent();

	delete mEditLayoutContext.GetEditLayOut();
	CDocument::OnCloseDocument();
}

BOOL CmainDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDocument::OnSaveDocument(lpszPathName);
}

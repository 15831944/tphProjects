// ShapesManager.cpp: implementation of the CShapesManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShapesManager.h"
#include "Termplan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


struct TACType2Shape
{
	char	actype[4];
	int		idx;
};

static TACType2Shape ACTYPE2SHAPETABLE[] = {
	{ "310", 4 },
	{ "32S", 4 },
	{ "330", 5 },
	{ "340", 6 },
	{ "737", 2 },
	{ "747", 1 },
	{ "74F", 1 },
	{ "74M", 1 },
	{ "757", 0 },
	{ "75F", 0 },
	{ "767", 0 },
	{ "777", 3 },
	{ "AB3", 0 },
	{ "ABF", 0 },
	{ "ABX", 0 },
	{ "ABY", 0 },
	{ "CR2", 0 },
	{ "D10", 0 },
	{ "D1F", 0 },
	{ "M11", 0 },
	{ "M1F", 0 },
	{ "M90", 0 },
};

static int snDefaultACShape = 0;

int CShapesManager::GetACShapeIdxByACType(LPCTSTR _sACType) const
{
	int n = sizeof(ACTYPE2SHAPETABLE) / sizeof(TACType2Shape);
	for(int i=0; i<n; i++) {
		if( 0 == strcmp(_sACType, ACTYPE2SHAPETABLE[i].actype) )
			return ACTYPE2SHAPETABLE[i].idx;
	}
	return snDefaultACShape;
}



CShapesManager* CShapesManager::m_pInstance = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShapesManager::CShapesManager()
{
	LoadData();
	LoadShapePlugins();
}

CShapesManager::~CShapesManager()
{
	for(int i=0; i<static_cast<int>(m_vShapeList.size()); i++)
		delete m_vShapeList[i];
	m_vShapeList.clear();
	for(int i=0; i<static_cast<int>(m_vDefaultList.size()); i++)
		delete m_vDefaultList[i];
	m_vDefaultList.clear();
}

CShapesManager* CShapesManager::GetInstance()
{
	if(m_pInstance == NULL) {
		m_pInstance = new CShapesManager();
	}
	return m_pInstance;
}

void CShapesManager::DeleteInstance()
{
	delete m_pInstance;
	m_pInstance = NULL;
}


BOOL CShapesManager::LoadData()
{
	CFile* pFile = NULL;
	CString sFileName = ((CTermPlanApp*) AfxGetApp())->GetShapesDBPath() + "\\Shapes.dat";
	BOOL bRet = TRUE;
	m_vShapeList.clear();
	try
	{
		pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		char line[513];
		//skip a line
		ar.ReadString(line, 512);
		//read line 2
		ar.ReadString(line, 512);
		if(_stricmp(line, "Shapes Database") == 0)
		{
			//skip the column names
			ar.ReadString(line, 512);
			//read the values
			//read a line
			CString sPath = ((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\";
			while(*(ar.ReadString(line, 512)) != '\0')
			{
				CShape* pShape = new CShape();
				char* b = line;
				char* p = NULL;
				int c = 1;
				while((p = strchr(b, ',')) != NULL)
				{
					*p = '\0';
					switch(c)
					{
					case 1: //name
						pShape->Name(b);
						break;
					case 2: //shape file
						pShape->ShapeFileName(sPath+b);
						break;
					case 3: //image file
						pShape->ImageFileName(sPath+b);
						break;
					default:
						break;
					}
					b = ++p;
					c++;
				}
				if(b!=NULL) // the last column did not have a comma after it
				{
					switch(c)
					{
					case 1: //name
						pShape->Name(b);
						break;
					case 2: //shape file
						pShape->ShapeFileName(sPath+b);
						break;
					case 3: //image file
						pShape->ImageFileName(sPath+b);
						break;
					default:
						break;
					}
				}
				m_vShapeList.push_back(pShape);
			}
		}
		else {
			bRet = FALSE;
		}
		ar.Close();
		delete pFile;
	}
	catch(CException* e)
	{
		AfxMessageBox("Error reading Shapes DB file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}

	return bRet;
}

// return NULL, if not found
CShape* CShapesManager::FindShapeByName( CString _csName )
{
	int nCount = m_vShapeList.size();
	for( int i=0; i<nCount; i++ )
	{
		CShape* pShape = m_vShapeList[i];

		if( pShape->Name().CompareNoCase( _csName ) == 0 )
		{
			if (PathFileExists(pShape->ShapeFileName())==FALSE)			
				break;

			return pShape;
		}
	}	

	nCount = m_vDefaultList.size();
	for( int i=0; i<nCount; i++ )
	{
		CShape* pShape = m_vDefaultList[i];

		if( pShape->Name().CompareNoCase( _csName ) == 0 )
		{
			return pShape;
		}
	}

	CShape* pDefaultShape = new CShape();
	CString sPath = ((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\";
	pDefaultShape->Name(_csName);
	pDefaultShape->ImageFileName(sPath + "CUBE100.bmp");
	pDefaultShape->ShapeFileName(sPath + "CUBE100.dxf");
	m_vDefaultList.push_back(pDefaultShape);

	return pDefaultShape;
}

BOOL CShapesManager::IsShapeExist( CString _csName )
{
    int nCount = m_vShapeList.size();
    for( int i=0; i<nCount; i++ )
    {
        CShape* pShape = m_vShapeList[i];

        if( pShape->Name().CompareNoCase( _csName ) == 0 )
        {
            return TRUE;
        }
    }	
    return FALSE;
}

// return -1, if not found
int CShapesManager::FindShapeIndexByName( CString _csName )
{
	int nCount = static_cast<int>(m_vShapeSetsModules.size());
	for( int i = 0; i < nCount; i++ )
	{
		CString strShapeSetName = GetShapeSetName(i);
		if (strShapeSetName.CompareNoCase( _csName ) == 0 )
			return i;
	}

	return -1;
}

int CShapesManager::GetShapeSetCount() const
{
	return m_vShapeSetsModules.size();
}

const TCHAR* CShapesManager::GetShapeSetName(int _nSet) const
{
	return (*m_vShapeSetsModules[_nSet].pfGetShapeSetName)( _nSet - m_vShapeSetsModules[_nSet].nShapeSetIdxOffset );
}

bool CShapesManager::IsShapeSetStatic(int _nSet) const
{
	return 0 != ( ARC_SHAPESETTYPE_STATIC | (*m_vShapeSetsModules[_nSet].pfGetShapeSetType)( _nSet - m_vShapeSetsModules[_nSet].nShapeSetIdxOffset ) );
}

bool CShapesManager::IsShapeSetMobile(int _nSet) const
{
	return 0 != ( ARC_SHAPESETTYPE_MOBILE | (*m_vShapeSetsModules[_nSet].pfGetShapeSetType)( _nSet - m_vShapeSetsModules[_nSet].nShapeSetIdxOffset ) );
}

int CShapesManager::GetShapeCount(int _nSet) const
{
	return (*m_vShapeSetsModules[_nSet].pfGetShapeCount)( _nSet - m_vShapeSetsModules[_nSet].nShapeSetIdxOffset );
}

const TCHAR* CShapesManager::GetShapeName(int _nSet, int _nShape) const
{
	return (*m_vShapeSetsModules[_nSet].pfGetShapeName)( _nSet - m_vShapeSetsModules[_nSet].nShapeSetIdxOffset, _nShape );
}

//UINT CShapesManager::GetShapeImgResourceID(int _nSet, int _nShape) const
//{
//	return (*m_vShapeSetsModules[_nSet].pfGetShapeImgResourceID)( _nSet - m_vShapeSetsModules[_nSet].nShapeSetIdxOffset, _nShape );
//}
/*
HGLOBAL CShapesManager::GetShapeImgResource(int _nSet, int _nShape) const
{
	HMODULE hMod = m_vShapeSetsModules[_nSet].hModule;
	int nOffsetSet = _nSet - m_vShapeSetsModules[_nSet].nShapeSetIdxOffset;
	UINT nResID = (*m_vShapeSetsModules[_nSet].pfGetShapeImgResourceID)( nOffsetSet, _nShape );

	HRSRC hResInfo = ::FindResource( hMod, MAKEINTRESOURCE(nResID), RT_BITMAP );

	HGLOBAL hResData = ::LoadResource( hMod, hResInfo );
	return ::LockResource( hResData );
}
*/

HBITMAP CShapesManager::GetShapeImgBitmap(int _nSet, int _nShape) const
{
	HMODULE hMod = m_vShapeSetsModules[_nSet].hModule;
	int nOffsetSet = _nSet - m_vShapeSetsModules[_nSet].nShapeSetIdxOffset;
	UINT nResID = (*m_vShapeSetsModules[_nSet].pfGetShapeImgResourceID)( nOffsetSet, _nShape );

	return ::LoadBitmap( hMod, MAKEINTRESOURCE(nResID) );
}

HBITMAP CShapesManager::GetShapeImgSmallBitmap(int _nSet, int _nShape) const
{
	HMODULE hMod = m_vShapeSetsModules[_nSet].hModule;
	int nOffsetSet = _nSet - m_vShapeSetsModules[_nSet].nShapeSetIdxOffset;
	UINT nResID = (*m_vShapeSetsModules[_nSet].pfGetShapeImgSmallResourceID)( nOffsetSet, _nShape );

	return ::LoadBitmap( hMod, MAKEINTRESOURCE(nResID) );
}

GLuint CShapesManager::GenerateShapeDisplayList(int _nSet, int _nShape, COLORREF _col) const
{
	if (m_vShapeSetsModules.empty())
		return 0;

	HMODULE hMod = m_vShapeSetsModules[_nSet].hModule;
	int nOffsetSet = _nSet - m_vShapeSetsModules[_nSet].nShapeSetIdxOffset;

	if(m_vShapeSetsModules[_nSet].pfGenerateDisplayList) {
		return (*m_vShapeSetsModules[_nSet].pfGenerateDisplayList)( nOffsetSet, _nShape, _col );
	}
	else if(m_vShapeSetsModules[_nSet].pfRenderShape) {
		GLuint lid=glGenLists(1);
		glNewList(lid, GL_COMPILE);
		(*m_vShapeSetsModules[_nSet].pfRenderShape)( nOffsetSet, _nShape, _col );
		glEndList();

		return lid;
	}
	return 0;
}


bool CShapesManager::LoadShapePlugins()
{
	// plug-in path relocation, by Benny, 2009-07-15
	// to use newest plugs-in, build Plugins.sln first
#ifdef _DEBUG
	TCHAR cPluginPath[MAX_PATH] = {0} ;
	if(GetModuleFileName(AfxGetInstanceHandle(), cPluginPath, 1024))
	{
		PathRemoveFileSpec(cPluginPath) ;
	}
	CString sPluginFiles = cPluginPath;
	sPluginFiles += _T("\\Plugins\\Debug\\*.dll");
#else
	CString sPluginFiles = ((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\*.dll";
#endif


	m_vShapeSetsModules.clear();

	//iterate through dll files in DB shapes data dir
	CFileFind finder;
	BOOL bWorking = finder.FindFile(sPluginFiles);
	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		HMODULE hMod = ::LoadLibrary( finder.GetFilePath() );
		if( !hMod ) {
			CString sMsg;
			sMsg.Format( "WARNING: failed to load shape set plugin \'%s\'.", finder.GetFileName() );
			AfxMessageBox( sMsg, MB_OK|MB_ICONWARNING );
			continue;
		}

		TGetPluginType pfGetPluginType = (TGetPluginType) ::GetProcAddress( hMod, "GetPluginType" );
		if( !pfGetPluginType ) {
			CString sMsg;
			sMsg.Format( "WARNING: failed to bind function \'GetPluginType\' in shape set plugin \'%s\'.", finder.GetFileName() );
			AfxMessageBox( sMsg, MB_OK|MB_ICONWARNING );
			continue;
		}

		TGetPluginName pfGetPluginName = (TGetPluginName) ::GetProcAddress( hMod, "GetPluginName" );
		if( !pfGetPluginName ) {
			CString sMsg;
			sMsg.Format( "WARNING: failed to bind function \'GetPluginName\' in shape set plugin \'%s\'.", finder.GetFileName() );
			AfxMessageBox( sMsg, MB_OK|MB_ICONWARNING );
			continue;
		}

		// TRACE( "Found plugin %s\n", (*pfGetPluginName)() );

		if( ARC_PLUGINTYPE_SHAPESET != (*pfGetPluginType)() )
			continue; // plugin is not a shape set plugin

		
		TGetShapeSetCount pfGetShapeSetCount = (TGetShapeSetCount) ::GetProcAddress( hMod, "GetShapeSetCount" );
		TGetShapeSetName pfGetShapeSetName = (TGetShapeSetName) ::GetProcAddress( hMod, "GetShapeSetName" );
		TGetShapeSetType pfGetShapeSetType = (TGetShapeSetType) ::GetProcAddress( hMod, "GetShapeSetType" );
		TGetShapeCount pfGetShapeCount = (TGetShapeCount) ::GetProcAddress( hMod, "GetShapeCount" );
		TGetShapeName pfGetShapeName = (TGetShapeName) ::GetProcAddress( hMod, "GetShapeName" );
		TGetShapeImgResourceID pfGetShapeImgResourceID = (TGetShapeImgResourceID) ::GetProcAddress( hMod, "GetShapeImgResourceID" );
		TGetShapeImgSmallResourceID pfGetShapeImgSmallResourceID = (TGetShapeImgSmallResourceID) ::GetProcAddress( hMod, "GetShapeImgSmallResourceID" );
		TRenderShape pfRenderShape = (TRenderShape) ::GetProcAddress( hMod, "RenderShape" );
		TGenerateShapeDisplayList pfGenerateDisplayList = (TGenerateShapeDisplayList) ::GetProcAddress( hMod, "GenerateShapeDisplayList" );

		if( !pfGetShapeSetCount ||
			!pfGetShapeSetName ||
			!pfGetShapeSetType ||
			!pfGetShapeCount ||
			!pfGetShapeName ||
			!pfGetShapeImgResourceID ||
			!pfGetShapeImgSmallResourceID ||
			!(pfRenderShape || pfGenerateDisplayList) )
		{
			CString sMsg;
			sMsg.Format( "WARNING: failed to bind a required function in shape set plugin \'%s\'.", finder.GetFileName() );
			AfxMessageBox( sMsg, MB_OK|MB_ICONWARNING );
			continue;
		}

		int nShapeSetIdxOffset = m_vShapeSetsModules.size();
		int nShapeIdxOffset = 0;
		
		for( int _i=0; _i<(*pfGetShapeSetCount)(); _i++)  {

			nShapeIdxOffset = _i?(*pfGetShapeCount)( _i-1):0;

			_shape_set_module m = {
				hMod,
				nShapeSetIdxOffset,
				nShapeIdxOffset,
				pfGetPluginType,
				pfGetPluginName,
				pfGetShapeSetCount,
				pfGetShapeSetName,
				pfGetShapeSetType,
				pfGetShapeCount,
				pfGetShapeName,
				pfGetShapeImgResourceID,
				pfGetShapeImgSmallResourceID,
				pfRenderShape,
				pfGenerateDisplayList,
			};

			m_vShapeSetsModules.push_back( m );
		}

	}

	finder.Close();


	return true;
}

int CShapesManager::SetAndShapeToIdx(int _nSet, int _nShape) const
{
	// TRACE("Shape offset = %d\n", m_vShapeSetsModules[_nSet].nShapeIdxOffset);
	return m_vShapeSetsModules[_nSet].nShapeIdxOffset + _nShape;
}

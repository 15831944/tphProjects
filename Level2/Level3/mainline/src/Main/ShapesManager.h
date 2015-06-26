// ShapesManager.h: interface for the CShapesManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAPESMANAGER_H__82218FD1_97CF_4B34_ABEA_7CA7F08DC2F0__INCLUDED_)
#define AFX_SHAPESMANAGER_H__82218FD1_97CF_4B34_ABEA_7CA7F08DC2F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CommonData/Shape.h>
#include "Plugins\Common\ARCPluginsCommon.h"


#define SHAPESMANAGER CShapesManager::GetInstance()
#define DESTROY_SHAPESMANAGER CShapesManager::DeleteInstance();

class CShapesManager  
{
private:
	CShapesManager();
	virtual ~CShapesManager();
	CShape::CShapeList m_vShapeList;
	CShape::CShapeList m_vDefautShapeList;
public:
	static CShapesManager* GetInstance();
	static void DeleteInstance();

	CShape::CShapeList* GetShapeList() { return &m_vShapeList; }
	CShape::CShapeList* GetDefaultShapeList() { return &m_vDefautShapeList; }
	// return NULL, if not found
	CShape* FindShapeByName( CString _csName );
	// return -1, if not found
	int FindShapeIndexByName( CString _csName );
	// return default shape, if not found
	CShape* GetShapeByName(CString _csName);
    // 
    BOOL IsShapeExist(CString str);

	// return -1, if not found
	int FindShapeSetIndexByName( CString _csName );
	void AddShapeByName(CString _csName,CShape* shape);
	void RemoveShapeByName(CString _csName);

	//////////////////////////////////////////////////////////////////////////
public:	
	BOOL LoadData();
	int GetShapeSetCount() const;
	const TCHAR* GetShapeSetName(int _nSet) const;
	bool IsShapeSetStatic(int _nSet) const;
	bool IsShapeSetMobile(int _nSet) const;
	int GetShapeCount(int _nSet) const;
	const TCHAR* GetShapeName(int _nSet, int _nShape) const;
	HBITMAP GetShapeImgBitmap(int _nSet, int _nShape) const;
	HBITMAP GetShapeImgSmallBitmap(int _nSet, int _nShape) const;
	GLuint GenerateShapeDisplayList(int _nset, int _nShape, COLORREF _col) const;

	int SetAndShapeToIdx(int _nSet, int _nShape) const;

	int GetACShapeIdxByACType(LPCTSTR _sACType) const;

private:
	struct _shape_set_module {
		HMODULE hModule;

		int nShapeSetIdxOffset;
		int nShapeIdxOffset;

		TGetPluginType pfGetPluginType;
		TGetPluginName pfGetPluginName;

		TGetShapeSetCount pfGetShapeSetCount;
		TGetShapeSetName pfGetShapeSetName;
		TGetShapeSetType pfGetShapeSetType;
		TGetShapeCount pfGetShapeCount;
		TGetShapeName pfGetShapeName;
		TGetShapeImgResourceID pfGetShapeImgResourceID;
		TGetShapeImgSmallResourceID pfGetShapeImgSmallResourceID;
		TRenderShape pfRenderShape;
		TGenerateShapeDisplayList pfGenerateDisplayList;
	};

	std::vector< _shape_set_module > m_vShapeSetsModules;

	bool LoadShapePlugins();
	//////////////////////////////////////////////////////////////////////////
	


private:
	static CShapesManager* m_pInstance;

};

#endif // !defined(AFX_SHAPESMANAGER_H__82218FD1_97CF_4B34_ABEA_7CA7F08DC2F0__INCLUDED_)

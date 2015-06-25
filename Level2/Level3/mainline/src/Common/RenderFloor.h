#pragma once
// RenderFloor.h: interface for the CRenderFloor class.
//
//////////////////////////////////////////////////////////////////////
/********************************************************************
	created:	2010/03/03
	created:	3:3:2010   9:23
	filename:	RenderFloor.h
	author:		Benny
	
	purpose:	Providing data structure for CRenderFloor3D, base class of CFloor2
*********************************************************************/

#include "ARCVector.h"
#include "BaseFloor.h"

struct CPictureInfo
{
	CString sFileName;
	CString sPathName;
	ARCVector2 vSize;
	Path refLine;    //only two point path 

	ARCVector2 vOffset;
	double dScale;
	double dRotation;

	bool bShow;

	void ImportFile(ArctermFile& _file);
	void ExportFile(ArctermFile& _file);
};


class CRenderFloor : public CBaseFloor
{
public:
	CRenderFloor(int nLevel);
	CRenderFloor(int nLevel, const CString& sName);
	virtual ~CRenderFloor();	

	int Level() const { return m_nLevel; }
	void Level(int nLevel) { m_nLevel = nLevel; }
	const CString& FloorName() const { return m_sName; }
	void FloorName(const CString& sName) { m_sName = sName; }

	void SetMapFileName(const CString& sFileName);
	void SetMapPathName(const CString& sPathName);
	int linkToTerminalFloor() const {return m_indexlinkToTerminalFloor;}
	void linkToTerminalFloor(int nIndex){m_indexlinkToTerminalFloor = nIndex;}

	std::vector< std::vector<Point> > * GetRegionOutLinePtr(){ return &m_outlines; }

	DistanceUnit GetBoundRadius()const;

	//picture map functions
	void InvalidatePictureMap(){ m_bPicTextureValid = FALSE;  }
	void SetPictureFileName(const CString& filename);
	void SetPictureFilePath(const CString& filePath);
	CString GetPictureFileName(){ return m_picInfo.sFileName ; }
	CString GetPictureFilePath(){ return m_picInfo.sPathName; }

	static CString GetDefaultLevelName(int nLevel, CString prefix = "FLOOR");
protected:
	void InitDefault();

// 	BOOL LoadImage(CString sFileName);
	static CString FindExtension(const CString& name);
	static CString RemoveExtension(const CString& name);

public:
	CPictureInfo m_picInfo;

protected:
	int m_indexlinkToTerminalFloor;
	int m_nLevel;
	CString m_sName;
   
	BOOL m_bPicTextureValid;
	BOOL m_bPicLoaded;

	//
	std::vector< std::vector<Point> > m_outlines;


public:
	// for database access
	static void ReadMarkerLocationFromDB(int cur_Envir_type ,int m_nLevel,ARCVector2& marker) ;
	static void SaveMarkerLocationToDB(int cur_Envir_type,int m_nLevel, ARCVector2& marker) ;
	static void DeleteMarkerLocationFromDB(int cur_Envir_type ,int m_nLevel) ;

	static void ReadAlignLineFromDB(int sys_mode, int m_nLevel,CAlignLine& line) ;
	static void SaveAlignLineFromDB(int sys_mode,int m_nLevel, CAlignLine& line);
	static void DeleteAlignLineFromDB(int cur_Envir_type ,int m_nLevel,int index) ;

	static void SaveAlignLineStateToDB(int sys_mode, int m_nLevel,BOOL state) ;
	static BOOL ReadAlignLineStateFromDB(int sys_mode, int m_nLevel,BOOL& state) ;

	static void SaveMarketStateToDB(int sys_mode, int m_nLevel,BOOL state) ;
	static BOOL readMarketStateFromDB(int sys_mode, int m_nLevel,BOOL& state) ;
};

class CRenderFloorList : public std::vector<CRenderFloor*>
{

};

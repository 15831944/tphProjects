// Cameras.h: interface for the CCameras class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAS_H__F4E870EF_384B_46AB_AFD1_6593FC87EFF5__INCLUDED_)
#define AFX_CAMERAS_H__F4E870EF_384B_46AB_AFD1_6593FC87EFF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\common\dataset.h"
#include "3DCamera.h"
#include <vector>

struct C3DViewPaneInfo;
class C3DViewCameraDesc;
class C3DViewCameras;

struct CPaneInfo {
	int nNumRows;
	int nNumCols;
	int nHeightRow0;
	int nWidthCol0;

	void Convert(C3DViewPaneInfo& paneInfo) const;
};

struct CViewDesc {
	CString name;
	CPaneInfo paneinfo;
	std::vector<C3DCamera*> panecameras; //always 4 cameras of same type

	void Convert(C3DViewCameraDesc& camDesc) const;
};
class ARCBoundingSphere ;
class CCameras : public DataSet  
{
public:
	CCameras();
	virtual ~CCameras();
	
	CCameras(int szType);

	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file) const;

    virtual const char *getTitle(void) const { return "Cameras Data"; }
    virtual const char *getHeaders (void) const{  return "Name,Data,,,"; }

	virtual void initDefaultValues();

	//const C3DCameraList& CameraList() const { return m_vCameras; }
	//int AddCameraCopy(const C3DCamera& _cam, const CString& _sName);
	void InitDefault(const ARCBoundingSphere& bs);
	
	virtual void clear();

//data
public:
	//CPaneInfo m_PaneInfo;
	CViewDesc m_userView2D;
	CViewDesc m_userView3D;
	CViewDesc m_defaultView3D;
	CViewDesc m_defaultView2D;
	std::vector<CViewDesc> m_savedViews;
	
	//C3DCamera::PROJECTIONTYPE m_userViewType;
	//C3DCameraVector m_vBookmarkedCameras;
	//C3DCameraVector m_vPaneCameras;

	void Convert(C3DViewCameras& cams) const;
protected:
	
};

#endif // !defined(AFX_CAMERAS_H__F4E870EF_384B_46AB_AFD1_6593FC87EFF5__INCLUDED_)

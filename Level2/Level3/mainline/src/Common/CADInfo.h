#pragma once
#include <common/ARCColor.h>
#include <common/ARCVector.h>
#include <OpenDWGToolKit/CADFileLayer.h>
#define NO_MAP_STR _T("No map selected")

//cad info, or picture
class CCADInfo 
{
public:
	CString sFileName;
	CString sPathName;
	ARCVector2 vOffset;
	double dScale;
	double dRotation;
	CCADInfo();
};


//user input cad layer info
class CCADLayerInfo
{
public:
	CCADLayerInfo();
	ARCColor3 cColor;
	BOOL bIsOn;
	CString sName;	
};

typedef std::vector<CCADLayerInfo> LayerInfoList;

//all file info
class TiXmlElement;
class CCADFileDisplayInfo
{
public:
	int GetLayerInfoIdx(CString sName)const;
	int GetLayerCount()const;
	CCADLayerInfo& LayerAt(int idx){ return mLayersInfo.at(idx); }
	const CCADLayerInfo& LayerAt(int idx)const{ return mLayersInfo.at(idx); }
	void AddLayerInfo(const CCADLayerInfo & info){ return  mLayersInfo.push_back(info); }

	LayerInfoList mLayersInfo;		//cad file layers setting
	CCADInfo mCADInfo;              //cad display setting
	CCADFileContent::SharePtr mLoadedLayers; //loaded layers

	void SetMap(CString sMapName, CString sMapPath);
	void UpdateLayersInfo();

	BOOL bVisible;

	const ARCColor3& GetMonochromeColor() const { return m_cMonochromeColor; }
	void SetMonochromeColor(const ARCColor3& col);
	BOOL IsMonochrome() const { return m_bIsMonochrome; }
	void IsMonochrome(BOOL b);
	BOOL m_bIsMonochrome;
	ARCColor3 m_cMonochromeColor;
	void UseLayerInfoFiles(BOOL b) { m_bUseLayerInfoFiles = b; }
	BOOL UseLayerInfoFiles() const { return m_bUseLayerInfoFiles; }
	BOOL m_bUseLayerInfoFiles;


	static bool CompareList(const LayerInfoList& list1, const LayerInfoList& list2);


	CCADFileDisplayInfo();
public:
	virtual TiXmlElement * serializeTo(TiXmlElement *, int nVersion)const;
	virtual void serializeFrom(const TiXmlElement* pElm, int nVersion);
};
#pragma once

#include <Common/Camera.h>

//------------------------------------------------------------
// Store data of 3D view cameras, implement serialization
//------------------------------------------------------------

class AirportDatabaseConnection;

struct C3DViewPaneInfo
{
	C3DViewPaneInfo()
		: nLeftWidth(200)
		, nRightWidth(0)
		, nTopHeight(150)
		, nBottomHeight(0)
	{

	}

	enum
	{
		MinColNum = 1,
		MinRowNum = 1,
		MaxColNum = 2,
		MaxRowNum = 2,
	};

	int GetColCount() const
	{
		return nRightWidth ? MaxColNum : MinColNum;
	}

	int GetRowCount() const
	{
		return nBottomHeight ? MaxRowNum : MinRowNum;
	}

	bool HasSplitter() const
	{
		return GetColCount()>MinColNum || GetRowCount()>MinRowNum;
	}
	//------------------------------------------------------------
	// When nRightWidth is zero, no columns splitter
	// When nBottomHeight is zero, no rows splitter
	// if window resized, apply the ratio
	int nLeftWidth;
	int nRightWidth;
	int nTopHeight;
	int nBottomHeight;
	//------------------------------------------------------------
};

enum EnvironmentMode;
class C3DViewCameraDesc
{
public:
	C3DViewCameraDesc()
	{

	}

	static void InitCameraData(CCameraData& camData, CCameraData::PROJECTIONTYPE ept);
	void InitCameraData(CCameraData::PROJECTIONTYPE ept);

	static void InitCameraDataByMode(CCameraData& camData, CCameraData::PROJECTIONTYPE ept, EnvironmentMode eEnv);
	void InitCameraDataByMode(CCameraData::PROJECTIONTYPE ept, EnvironmentMode eEnv);

	typedef std::vector<C3DViewCameraDesc> List;

	CString GetName() const { return m_sName; }
	void SetName(CString sName) { m_sName = sName; }
	C3DViewPaneInfo& GetPaneInfo() { return m_paneInfo; }
	const C3DViewPaneInfo& GetPaneInfo() const { return m_paneInfo; }
	CCameraData& GetCameraData(int nRow, int nCol);
	const CCameraData& GetCameraData(int nRow, int nCol) const;

	CCameraData& GetDefaultCameraData();
	const CCameraData& GetDefaultCameraData() const;

protected:
	static CString GetCameraXMLTag(int nRow, int nCol);
private:
	CString m_sName;
	C3DViewPaneInfo m_paneInfo;
	CCameraData m_cameras[C3DViewPaneInfo::MaxRowNum][C3DViewPaneInfo::MaxColNum];

public: //save/load to/from xml file
	virtual TiXmlElement* serializeTo(TiXmlElement* pElm, int nVersion) const;
	virtual void serializeFrom(const TiXmlElement* pElm, int nVersion);

};

class C3DViewCameras
{
public:
	C3DViewCameras(void);
	~C3DViewCameras(void);

	void InitCameraData();

protected:
	static CString GetSavedCameraDescXMLTag(int nIndex);

public:
	bool m_b3DMode; // currently use 3D or 2D
	C3DViewCameraDesc       m_userCamera3D;
	C3DViewCameraDesc       m_userCamera2D;
	C3DViewCameraDesc::List m_savedCameras;

	const C3DViewCameraDesc& GetUserCamera() const;
	C3DViewCameraDesc& GetUserCamera();

	void AddSavedCameraDesc(CString strName);
	void AddSavedCameraDesc(const C3DViewCameraDesc& cam);
	void AddSavedCameraDesc(const CCameraData& cam, CString strName); // version for none splitter

public: //save/load to/from xml file
	int GetCurrentVersion() const;

	virtual TiXmlElement* serializeTo(TiXmlElement* pElm) const;
	virtual void serializeFrom(const TiXmlElement* pElm);

	friend std::istream& operator >> (std::istream& is, C3DViewCameras& cams);
	friend std::ostream& operator << (std::ostream& os, const C3DViewCameras& cams);

	std::string GetCamerasXMLStirng() const;
	void ParseCamerasFromXMLString(const std::string& str);

	enum ParentObjectType
	{
		enumAircraftLayout = 0, // use layout ID as Parent ID
		enumEnvironment, // use environment mode enumeration as Parent ID

		ParentObjectType_Count,
	};

	static const TCHAR* const m_sParentObjectTypeName[];

	bool ReadFromDatabase(ParentObjectType eType, int nParentID);
	bool SaveIntoDatabase(ParentObjectType eType, int nParentID);
	static bool DeleteFromDatabase(ParentObjectType eType, int nParentID);

	static int GetXMLDataID( C3DViewCameras::ParentObjectType eType, int nParentID );
	static AirportDatabaseConnection GetConnection();

protected:
	static bool DeleteDBRecord( ParentObjectType eType, int nParentID );
	static bool InsertDBRecord( ParentObjectType eType, int nParentID, int nDataID );
};




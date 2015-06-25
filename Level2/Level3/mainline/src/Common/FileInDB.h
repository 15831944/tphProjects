#pragma once
#include <common\Camera.h>
#include <common\Grid.h>
#include <common\Guid.h>

class AirportDatabaseConnection;
class CAircraftModelDatabase;
class CADORecordset;

class CFileInDB
{
public:
	CFileInDB();
	CFileInDB(int id, AirportDatabaseConnection* pConn );
	~CFileInDB();
	CString GetPath()const{ return m_sFilePath; }

	//int m_ID;
	//CString m_sFileName; //store file name 
	CString m_sFilePath; //temp file

	int Save(int id, AirportDatabaseConnection* pConn);
	void Read(int id,AirportDatabaseConnection* pConn );
	static void Delete(int id, AirportDatabaseConnection* pConn );
public:
	bool FileExists()const;
	void MakeSureFileValid(CString ext);

};


#define STR_THUMBFILE_X _T("ThumbX.bmp")
#define STR_THUMBFILE_Y _T("ThumbY.bmp")
#define STR_THUMBFILE_Z _T("ThumbZ.bmp")
#define STR_THUMBFILE_P _T("ThumbP.bmp")
#define STR_MODEL_FILE _T("model.xml")
#define STR_MODEL_NODE_FILE _T("scenenode.xml") //file store the 3dmodel configure

#define STR_CRTOR _T("creator")
#define STR_CRTIME _T("createTime")
#define STR_MDTIME _T("modifyTime")
#define STR_IDNAME _T("idname")
//#define STR_MESH_FILENAME _T("mesh_name")

#define STR_MOD_CAM _T("model_camera")
#define STR_MOD_GRID _T("model_grid")

#define STR_VERSION _T("formatversion")
#define STR_BEXTMODEL _T("use_external_model")


#include <common\elaptime.h>

class CBaseDBModel
{
public:
	CBaseDBModel();
	virtual ~CBaseDBModel();

	enum DBModelType
	{
		Component_Model,
		Aircraft_Model,
		Furnishing_Model
	};

	int m_nID;
	CString m_sModelName;	
	int m_nFileID;
	CString msTmpWorkDir; //temporary working dir; unique for this model;

	bool IsFileOpen()const;	
	bool MakeSureFileOpen();
	void CloseFile();
	bool OpenFile();
	void SaveFile();
	bool IsFileSaved()const{ return m_nFileID>=0; }


	virtual void SaveDataToDB()=0;
	virtual void SaveData()=0;
	virtual void DeleteData()=0;
	virtual void ReadData()=0;	

	virtual const CString GetFileExtnsion()const = 0;
protected:
	void ZipToFile(CString filePath);
	void ExtractFile(CString filePath);

	virtual void DBReadData(CADORecordset& recordset) = 0;
	virtual DBModelType GetDBModelType()const =0;

	virtual AirportDatabaseConnection* GetConnection()=0;
protected:
	virtual void DBSaveData(AirportDatabaseConnection* pConn) = 0;
	virtual void DBDeleteData(AirportDatabaseConnection* pConn) = 0;
};

class CModel :  public CBaseDBModel
{
public:
	CModel();
	virtual ~CModel();
	void SetCreator(CString strCreator) { m_strCreator = strCreator; }
	CString GetCreator()const {return m_strCreator;}
	
	void SetCreateTime(const CTime& tCreatorTime) {m_tCreateTime = tCreatorTime; }
	CTime GetCreateTime()const {return m_tCreateTime;}

	void SetLastModifiedTime(CTime tModifiedTime) {m_tModifiedTime = tModifiedTime;}
	CTime GetLastModifiedTime()const {return m_tModifiedTime;}

	CString GetMeshFileName()const;

	virtual void SetDimension(const ARCVector3& dim){ mDimension = dim; }
	//CString msMeshFileName; ///use for external mesh or self mesh
	bool mbExternalMesh; //is import from external mesh
	ARCVector3 mDimension;

	CTime  m_tCreateTime;
	CTime	 m_tModifiedTime;
	CString m_strCreator;
	CCameraData mActiveCam; //perspective view camera
	CGrid mGrid;//size of the scene


	CGuid mguid; //guid for this model

public:
	virtual void Clear();
	void ReadContent();
    void SaveContent();

	virtual const CString GetFileName()const = 0;
	virtual int GetCurrentVersion()const = 0;

public: //save/load to/from xml file
	virtual TiXmlElement * serializeTo(TiXmlElement *, int nVersion)const;
	virtual void serializeFrom(const TiXmlElement* pElm, int nVersion);

};
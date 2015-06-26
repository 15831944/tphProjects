#pragma once

#include <afx.h>
#include <vector>
#include <utility>
#include <string>
#import "..\..\lib\msado26.tlb" rename("EOF","adoEOF")

enum categoryType
{
	weightType,
	speedType,
	compositeType,
	wingspanType,
	unknownType
};

class CAACategory
{
public:
	CAACategory();
	CAACategory(int nIdxInTable, int nIdx, const CString& strName, const CString& strACCat);
	CAACategory(const CAACategory& _other);
	virtual ~CAACategory();
protected:
	int m_nIdxInTable;
	int m_nIdx;
	CString m_strName;
	CString m_strCat;
public:
	void SetValue(int nIdxInTable, int nIdx, const CString& strName, const CString& strACCat);
	void GetValue(int& nIdxInTable, int& nIdx, CString& strName, CString& strACCat) const;

	void SetIDInTable(int nIdx) { m_nIdxInTable = nIdx; }
	int GetIDInTable() const { return m_nIdxInTable; }

	void SetID(int nIdx) { m_nIdx = nIdx; }
	int GetID() const { return m_nIdx; }

	void SetName(const CString& strName) { m_strName = strName; }
	const CString& GetName() const { return m_strName; }

	void SetCat(const CString& strACCat) { m_strCat = strACCat;	}
	const CString& GetCat() const { return m_strCat; }

	const CAACategory& operator=(const CAACategory& _other)
	{
		m_nIdxInTable = _other.GetIDInTable();
		m_nIdx = _other.GetID();
		m_strName = _other.GetName();
		m_strCat = _other.GetCat();		
		return *this;
	}
};
typedef std::vector<CAACategory*> AACategoryVect;

class CAACategoryManager
{
public:
	CAACategoryManager(categoryType catType);
	virtual ~CAACategoryManager();
public:
	int GetCount() const { return (int)m_wcList.size(); }

	void ClearRecords();

	int ReadData();

	int AddRecord(ADODB::_RecordsetPtr& rs);
	int AddRecord(const CString& strName, const CString& strCat);
	int AddRecord(CAACategory& cat);

	int DeleteRecord(int nIndex);

	int ModifyRecordIdx(int nIndex, int nNew);
	CString ModifyRecordName(int nIndex, const CString& strName);
	CString ModifyRecordCat(int nIndex, const CString& strCat);

	void MoveRecord(int nIndex, int nNew);
	CAACategory* GetRecord(int nIndex);

	void SetProjectID(int nID) { m_nProjID = nID; }
	int GetProjectID() { return m_nProjID; }
protected:
	AACategoryVect m_wcList;
	int m_nProjID;
	categoryType m_catType; 
};


//----------------------------------
typedef std::vector<CString> CATVECT;
class CAACompositeCat
{
public:
	CAACompositeCat()
	{ }
	CAACompositeCat(int nIdxInTable, int nIdx, const CString& strName, const CATVECT& vACCat)
	{ m_nIdxInTable = nIdxInTable; m_nIdx = nIdx; m_strName = strName; m_vStrCat = vACCat; }
	CAACompositeCat(const CAACompositeCat& _other)
	{ m_nIdxInTable = _other.GetIDInTable(); m_nIdx = _other.GetID(); m_strName = _other.GetName(); m_vStrCat = _other.GetCats(); }
	virtual ~CAACompositeCat()
	{ }
public:

	void SetValue(int nIdxInTable, int nIdx, const CString& strName, const CATVECT& vACCat)
	{ m_nIdxInTable = nIdxInTable; m_nIdx = nIdx; m_strName = strName; m_vStrCat = vACCat; }
	void GetValue(int& nIdxInTable, int& nIdx, CString& strName, CATVECT& vACCat) const
	{ nIdxInTable = m_nIdxInTable; nIdx = m_nIdx; strName = m_strName; vACCat = m_vStrCat; }
	
	void SetIDInTable(int nIdx) { m_nIdxInTable = nIdx; }
	int GetIDInTable() const { return m_nIdxInTable; }

	void SetID(int nIdx) { m_nIdx = nIdx; }
	int GetID() const { return m_nIdx; }

	void SetName(const CString& strName) { m_strName = strName; }
	const CString& GetName() const { return m_strName; }

	void SetCat(const CString& strACCat, int nIndex) { m_vStrCat[nIndex] = strACCat; }
	const CString& GetCat(int nIndex) const { return m_vStrCat[nIndex]; }

	void SetCats(const CATVECT& vACCat) { m_vStrCat = vACCat; }
	const CATVECT& GetCats() const { return m_vStrCat; }

	const CAACompositeCat& operator=(const CAACompositeCat& _other)
	{
		m_nIdxInTable = _other.GetIDInTable();
		m_nIdx = _other.GetID();
		m_strName = _other.GetName();
		m_vStrCat = _other.GetCats();		
		return *this;
	}
protected:
	int m_nIdxInTable;
	int m_nIdx;
	CString m_strName;
	std::vector<CString> m_vStrCat;	
};
typedef std::vector<CAACompositeCat> AACompositeCatVect;

class CAACompCatManager
{
public:
	CAACompCatManager();
	virtual ~CAACompCatManager();
public:
	int GetCount() const { return (int)m_wcList.size(); }

	void ClearRecords();

	void ReadData();

	int AddRecord(ADODB::_RecordsetPtr& rs);
	int AddRecord(const CString& strName, const CATVECT& vACCat);
	int AddRecord(CAACompositeCat& cat);

	int DeleteRecord(int nIndex);

	int ModifyRecordIdx(int nIndex, int nNew);
	CString ModifyRecordName(int nIndex, const CString& strName);
	CString ModifyRecordCat(int nIndex, const CString& strCat, int nSubIndex);

	void MoveRecord(int nIndex, int nNew);

	const CAACompositeCat& GetRecord(int nIndex);
    CAACompositeCat* GetCompCatByID(int nID);
	int GetCompCatIDByName(const CString& strName);
	CAACompositeCat* GetCompCatByName(const CString& strName);

	const AACompositeCatVect& GetCompositeCatVect()
	{
		return m_wcList;
	}

	void SetProjectID(int nID) { m_nProjID = nID; }
	int GetProjectID() { return m_nProjID; }
protected:
	categoryType m_catType; 
	int m_nProjID;
	AACompositeCatVect m_wcList;
};



//-------------------------------------------------------

class CAirsideAircraftCat
{
public:
	CAirsideAircraftCat(void);
	virtual ~CAirsideAircraftCat(void);

	void ReadData();
public:
	CAACategoryManager* GetWeightCatManager() const { return m_pwcManager;	}
	CAACategoryManager* GetSpeedCatManager() const { return m_pscManager; }
	CAACompCatManager* GetCompCatManager() const { return m_pccManager; }
	CAACategoryManager* GetWingspanManager() const { return m_pwsManager; }

	void SetProjectID(int nID) 
	{
		m_nProjID = nID;
		m_pwcManager->SetProjectID(nID);
		m_pscManager->SetProjectID(nID);
		m_pccManager->SetProjectID(nID);
	}
	int GetProjectID() { return m_nProjID; }
protected:
	CAACategoryManager* m_pwcManager;
	CAACategoryManager* m_pscManager;
	CAACompCatManager* m_pccManager;
	CAACategoryManager* m_pwsManager;
protected:
	int m_nProjID;
};


// Floors.h: interface for the CFloors class.
//
#pragma once

#include "common\dataset.h"
#include "Common\path.h"
#include "common/IFloorDataSet.h"
#include <vector>

class CFloor2;

typedef std::vector<CFloor2*> CFloorList;



class CFloors : public DataSet  , public IFloorDataSet
{
public:
	CFloors();
	virtual ~CFloors();
	CFloors(int szType);

	int GetCount()const{ return (int)m_vFloors.size(); }
	CFloor2 * GetFloor2(int idx);
	const CFloor2* GetFloor2(int idx)const;
	
	virtual void LoadData(CString _projpath, int nProjId){ return loadDataSet(_projpath); }
	virtual void SaveData(CString _projpath, int nProjId){ return saveDataSet(_projpath,false); }

	virtual CRenderFloor * GetFloor(int idx);
	virtual const CRenderFloor* GetFloor(int idx)const;

	void AddFloor(CFloor2* pFloor); 
	//will add to list
	virtual CFloor2 * NewFloor();


    virtual void clear ();

	// exception: FileVersionTooNewError
	virtual void loadDataSet (const CString& _pProjPath);

	virtual void saveDataSet (const CString& _pProjPath, bool _bUndo) const;

	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file) const;

    virtual const char *getTitle(void) const { return "Floors Data"; }
    virtual const char *getHeaders (void) const{  return "Name,Points,,,"; }

	void writeLayerInfo (const CString& _pProjPath) const;
	void readLayerInfo (const CString& _pProjPath);

	void writeVRInfo(const CString& _pProjPath) const;
	void readVRInfo(const CString& _pProjPath);

	void readObsoleteOld(ArctermFile& p_file);
	void readObsolete23(ArctermFile& p_file);
	void readObsolete24(ArctermFile& p_file);
	void readObsolete25(ArctermFile& p_file);
	void readObsolete26(ArctermFile& p_file);  //add guid 
	void readObsolete27(ArctermFile& p_file );
//////////////////////////////////////////////////////////////////////////
public:
	// give logic altitude, get real altitude
	double getRealAltitude( double _dLogicAltitude ) const;

	double getLevelAltitude(int _ifloor)const;
	double getLeavlRealAttitude(int _ifloor)const;

	// give real altitude, get logic altitude
	double getLogicAltitude( double _dRealAltitude ) const;

	//give logic altitude path, give real attitude path
	Path getRealAttitudePath(const Path& path);

	//Just it is if can't changed.
	bool IsReadOnly() { return m_bReadOnly; }
	void SetReadOnly( bool bReadOnly ) { m_bReadOnly = bReadOnly; }

	CFloor2* GetActiveFloor(); 
	int GetActiveFloorLevel();
	CFloor2* ActiveFloor(int nlevel);

	void RemoveFloor(int nLevel, bool nodelete= true);

	double getVisibleAltitude(double visiblefloorAltitude)const;
	double getVisibleFloorAltitude(double visibleAlitude)const;
private:
	bool m_bReadOnly;
public:
	CFloorList m_vFloors;

};


class CLandsideFloors : public CFloors
{
public:
	CLandsideFloors();

	virtual CFloor2* NewFloor();
	virtual void loadDataSet (const CString& _pProjPath);
};

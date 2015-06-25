
//Author : cjchen
//DATE : 4/20/2008
//class : CControllerInterventionData
//provide data for Controller Intervention Dialog 
#ifndef _CONTROLLER_INTERVERION_H
#define _CONTROLLER_INTERVERION_H
#include "SectorData.h"
#include <vector>
#include "../InputAirside/ALTObject.h"
struct Item 
{
	int id ;
	CString name ;
};
class CControllerInterventionData
{
public:
typedef  CSectorData SECTOR_TYPE ;
typedef  SECTOR_TYPE* P_SECTOR_TYPE ;
typedef  std::vector<SECTOR_TYPE*> SECTOR_VECTOR ; 
typedef  SECTOR_VECTOR::iterator ITERATOR_SECTOR ;
typedef  SECTOR_VECTOR::const_iterator ITERATOR_SECTOR_CONST;
private :
   SECTOR_VECTOR  sector;
   SECTOR_VECTOR  del_sector ;
   int m_nProjID ;
public:
    CControllerInterventionData(int projID);
	CControllerInterventionData() ; 
	~CControllerInterventionData();
    void AddSector( P_SECTOR_TYPE P_data);
	void DelSector( P_SECTOR_TYPE P_data);
	const SECTOR_VECTOR& getSectors();
	CSectorData* createNewSector();
	void DelSectorToDB() ;
	void setProjID(int projID ) ;
public:
	BOOL saveDataToDB();
private:
	BOOL getDataFromDB();
public:
	 static const char* Priority_item[4] ;
	static int  getPriorityitemSize() ;
private:
	 static int  n_priorityitemSize ;
public:
	 static const char* HOlD_Priority[2] ;
	static int  getHoldPrioritysize() ;
private:
	 static int n_holdpriority ;
public:
	typedef std::vector<Item>  FIX_VECTOR ;
	typedef FIX_VECTOR::iterator FIX_VECTOR_ITERATOR ;
	typedef FIX_VECTOR::const_iterator FIX_VECTOR_ITERATOR_CONST ;
    FIX_VECTOR getFixName() ;
};
#endif
#pragma once
#include "parameters.h"
#include <vector>
#include "../common/elaptime.h"


class CXTPPropertyGridItem;

class CAirsideDistanceTravelParam :
	public CParameters
{
public:
	CAirsideDistanceTravelParam();
	virtual ~CAirsideDistanceTravelParam();

public:


	int GetUnit(){ return m_enumUnit;}
	void SetUnit(int unit){ m_enumUnit = unit;}


	//members
protected:	
	int m_enumUnit;

	//Threshold
	long m_lThreshold;


	//operation 

public:
	//initialize the parameters from DB 
	virtual void LoadParameter();
	//create table and use default parameters
	//virtual void InitParameter(CXTPPropertyGrid* pGrid);
	//update the parameters to DB 
	virtual void UpdateParameter();
	//create table ,called by Initparameter
	virtual bool CreatePrameterTable();

protected:
public:
	BOOL ExportFile(ArctermFile& _file) ;
	BOOL ImportFile(ArctermFile& _file) ;
};

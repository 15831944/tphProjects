#pragma once
#include "parameters.h"

class CAirsideDurationParam :
	public CParameters
{
public:
	CAirsideDurationParam(void);
	~CAirsideDurationParam(void);

	//members
public:

public:
	//initialize the parameters from DB 
	virtual void LoadParameter();
	//create table and use default parameters
	virtual void InitParameter(CXTPPropertyGrid* pGrid);
	//update the parameters to DB 
	virtual void UpdateParameter();
	//create table ,called by Initparameter
	virtual bool CreatePrameterTable();

};

#pragma once

class CProjectViewSetting ;
class CProjectViewSetting
{
public:
		CProjectViewSetting(void);
private:
	int  n_EnvironmentMode ;
	int n_simulation ;
	static CProjectViewSetting*  setting ;
	int id ;
	BOOL empty ;
public:

   int getEnvironmentMode() ;
   int getSimulation() ;
   void setEnvironmentMode(int index) ;
   void setSimulation(int index);
   BOOL saveDataToDB() ;
   BOOL IsEmpty() ;
   void Empty() ;
	~CProjectViewSetting(void) ;
public:
	void getDataFromDB() ;
};

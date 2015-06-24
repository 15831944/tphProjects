#pragma once

#include "..\common\DataSet.h"
#include "..\common\LatLong.h"

class ArctermFile;

class CAirportInfo:public DataSet
{
public:
	CAirportInfo(void);
	~CAirportInfo(void);

public:
//	virtual void loadDataSet (const CString& _pProjPath);
	bool loadDataSet(const CString& _pProjPath,int mode, int airport);
	CString m_csAirportName;

	virtual void readData(ArctermFile& p_file);

	virtual void writeData (ArctermFile& p_file,CString strName) const;
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Airport information" ;};
	virtual const char *getHeaders (void) const {return "Airport Name,longtitude,latitude,x,y..." ;};
	virtual void clear (void);
	void saveDataSet(const CString& _pProjPath,int airport,CString strName,bool _bUndo); 
	BOOL CreateFile(CFile& file,LPCTSTR lpszFileName,UINT nOpenFlags,int nCreationDisposition=CREATE_ALWAYS);
	BOOL CreateDirectory(LPCTSTR lpszDirectory);
	 virtual void initDefaultValues ();

	void GetPosition(double &dx,double& dy);
	void SetPosition(double dx, double dy);
	void GetElevation(double& elevation);
	void SetElevation(double elevation);

	CLongitude *m_longtitude;
	CLatitude *m_latitude;
	//control the ARP(Hide/Show itself)
//	BOOL m_bHideControl;
	
	double m_dx;
	double m_dy;

	double m_elevation;
};

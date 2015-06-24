#pragma once

#include "glShape.h"
#include "FlightShape.h"
class CglShapeResource :public Referenced
{
public:
	CglShapeResource();
	CglShapeResource(const CString& foldpath);
	~CglShapeResource(void);

	void SetResourcePath(const CString& foldpath);

	CglShape * GetShape(const CString& strID);

	CglShape * NewShapeAndReside(const CString& filename,const CString& strID);

	CglShape * NewShape(const CString& filename);

	FlightShape * NewFlightShape(const CString& filename);
	FlightShape * NewFlightShapeAndReside(const CString& filename, const CString& strID);


	BOOL Load(bool flightShape= false);

	void removeShape(const CString& strID);

	typedef std::map<CString, ref_ptr<CglShape> > ShapeMap;

	ShapeMap & getShapeMap(){ return m_vResideShapes; }

	void GenDisplayList();
private: 
	ShapeMap m_vResideShapes;

	CString m_strpath;


};

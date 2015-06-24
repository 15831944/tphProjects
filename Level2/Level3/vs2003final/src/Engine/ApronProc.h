#ifndef __APRONPROC_H
#define __APRONPROC_H
#pragma once
#include "airfieldprocessor.h"
#include "../Main/Processor2.h"
#include <CommonData/Textures.h>
class ApronProc :
	public AirfieldProcessor
{
protected:
	
public:
	ApronProc(void);
	virtual ~ApronProc(void);
	// processor type
	virtual int getProcessorType (void) const { return ApronProcessor; }
	// processor name
	virtual const char *getProcessorName (void) const { return "Apron"; }
	// processor i/o
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;
	virtual void initServiceLocation (int pathCount, const Point *pointList){
		if (pathCount < 3)
		throw new StringError ("Processor must have not less than 2 service m_location");//must >=2
		m_location.init (pathCount, pointList);
	}
	void DrawToScene(double dAlt,const std::vector<GLuint> texlist);
	void DrawSelectArea(double dAlt);

};
#endif
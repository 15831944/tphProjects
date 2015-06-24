#pragma once
#include "airfieldprocessor.h"

class ArctermFile;

class ContourProc :public AirfieldProcessor
{
public:
	ContourProc(void);
	~ContourProc(void);

	//return the proc type
	virtual int getProcessorType(void) const { return ContourProcessor ;};
	//return the name of the proc
	virtual const char *getProcessorName(void) const { return "CONTOUR" ;};
	
	//set the height of the contour
	void setHeight(double height)	{ m_lHeight = height ;};
	
	//get height
	double getHeight() {	return m_lHeight ; };
	
	//write the height to file
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	virtual void initServiceLocation (int pathCount, const Point *pointList);

	// Returns service m_location.
    virtual Point GetServiceLocation() const;

private:
	double m_lHeight;




};

#pragma once
#include "process.h"
#include "../common/ARCVector.h"
#include "../Common/line.h"
#include "../Inputs/probab.h"
#include <CommonData/ExtraProcProp.h>
#include <Common/ProbabilityDistribution.h>

class ENGINE_TRANSFER BillboardProc : public Processor, public BillboardProp
{
public:
	BillboardProc(void);
	~BillboardProc(void);

	virtual ExtraProcProp* GetExtraProp() { return this; }

	//operation
public:
	//copy constructor.
	BillboardProc (const BillboardProc& aProc);;
	BillboardProc& operator = ( const BillboardProc& rhs);

	//Returns processor type.
	virtual int getProcessorType (void) const { return BillboardProcessor; };
	//Returns processor name.
	virtual const char *getProcessorName (void) const { return "Billboard"; };
    
	void initServiceLocation (int pathCount, const Point *pointList);
	//processor i/o
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

public:
	bool InitWholeExposurePath();//Path& path);
	/************************************************************************
	FUNCTION: test the pax wether has a  intersection with the billboard,
			if has,return the intersection point and the angle 
	Parameter :startPoint,endpoint the pax walk through the line that made of the two points
	RETURN	:true ,has a intersection ;others ,not			
	/************************************************************************/
	bool GetIntersectionWithLine(const Point& startPoint,const Point& endPoint,Point& StopPoint,Point& directionPoint,double& distanceToBillboard, double& dAngel);
//	void InitExpsurePath(int pathCount, const Point *pointList){ m_pathExpsure.init (pathCount, pointList); }
	Path* GetExpsurePath() { return outConstraint(); }
	int  GetExpsurePathPonitCount() { return outConstraint()->getCount(); }
	double getHeight(){return m_dHeight;}
	void setHeight(double dHeight){m_dHeight = dHeight;}

	double getThickness(){return m_dThickness;}
	void setThickness(double dThickness){m_dThickness = dThickness;}

	//double getExposureAreaRadius(){return m_dExposureAreaRadius;}
	//void setExposureAreaRadius(double dExposureAreaRadius){m_dExposureAreaRadius = dExposureAreaRadius;}

	int getType(){return m_nType;}
	void setType(int nType){m_nType = nType;}

	CString getText(){return m_strText;}
	void setText(CString strText){m_strText = strText;onTextModify();}

	CString getBitmapPath(){return m_strBitmapPath;}
	void setBitmapPath(CString strBitmapPath){m_strBitmapPath = strBitmapPath;onBmpModify();}
	
	//CString getTextOrPath(){return m_strTextOrPath;}
	//void setTextOrPath(CString strText){m_strTextOrPath = strText;}
	
	//double getCoefficient(){return m_dCoefficient;}
	//void setCoefficient(double dCoefficient){m_dCoefficient = dCoefficient;}
	
	//angle
	//int getAngle(){return m_nAngle;}
	//void setAngle(int nAngle){m_nAngle = nAngle;}

	bool useBitmap(){ return m_nType == 1; }
	bool useText(){ return !useBitmap();	}
	//attribute
	int getBmpModStamp(){ return m_ibmpModStamp; }
	void onBmpModify(){ ++m_ibmpModStamp; }
	int getTextModStamp() { return m_iTextModeStamp; }
	void onTextModify() { ++m_iTextModeStamp; }
	
	int m_ibmpModStamp;
	int m_iTextModeStamp;
private:

	//
	GammaDistribution m_gammaDistribution;

//
public:
	virtual void UpdateMinMax();
};

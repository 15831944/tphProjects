#ifndef __HODLPROCESSOR_DEF
#define __HOLDPROCESSOR_DEF

#pragma once
#include "airfieldprocessor.h"


class FixProc;
class HoldProc :
	public AirfieldProcessor
{
public:
	HoldProc(void);
	virtual ~HoldProc(void);
	//processor type 
	virtual int getProcessorType()const{ return HoldProcessor; } 
	//processor name 
	virtual const char * getProcessorName(void)const { return "Hold"; }
	//processor io
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile)const;


	void setFixName(const CString& fixname){ m_strFixname = fixname; }
	void setInBoundDegree(int inboundDegree){ m_inBoundDegree = inboundDegree; }

	void setOutBoundLegNm(double outboundnm){ m_outBoundLegNm = outboundnm; }
	void setOutBoundLegMin(double outboundmin){ m_outBoundLegMin = outboundmin; }
		
	void setRightTurn(bool brightTurn){ m_brightTurn = brightTurn; }
	void setMaxAltitude(double maxaltfeet){  m_maxAlt = maxaltfeet; }
	void setMinAltitude(double minaltfeet){  m_minAlt = minaltfeet; }

	CString getFixName()const{ return m_strFixname; }
	int getInBoundDegree()const { return m_inBoundDegree; }
	double getOutBoundLegNm()const { return m_outBoundLegNm; }
	double getOutBoundLegMin()const { return m_outBoundLegMin; }
	
	bool isRightTurn()const { return m_brightTurn; }
	double getMaxAltitude()const { return m_maxAlt; }
	double getMinAltitude()const { return m_minAlt; }

	FixProc * getFix(){return m_pfix;}
	const FixProc*getFix()const{return m_pfix;}
	void setFix(FixProc* pfix){ m_pfix = pfix; }

	void RefreshRelateFix( ProcessorList* proclist); 

	virtual void RefreshRelatedData( InputTerminal* _pInTerm );

	Path getPath()const;
private:
	 CString m_strFixname;
	 int m_inBoundDegree;
	 double m_outBoundLegNm;
	 double m_outBoundLegMin;
	 bool m_brightTurn;
	 double m_maxAlt;
	 double m_minAlt;

	 FixProc * m_pfix;

	
};


#endif
// RunwayProc.h: interface for the RunwayProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUNWAYPROC_H__3CB8D540_2A69_429A_9212_ABC07A5AA57D__INCLUDED_)
#define AFX_RUNWAYPROC_H__3CB8D540_2A69_429A_9212_ABC07A5AA57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AirfieldProcessor.h"
#include "../Main/RunwayTextures.h"

class RunwayProc : public AirfieldProcessor, public BaseRunwayProcessor 
{
public:
	RunwayProc();
	virtual ~RunwayProc();
	//processor type
	virtual int getProcessorType (void) const { return RunwayProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return "RUNWAY"; }
	//processor i/o
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	virtual void initServiceLocation (int pathCount, const Point *pointList);
	virtual void autoSnap(ProcessorList *procList);
	//Threshold 1, 2
	void SetThreshold1(const DistanceUnit& _dThresh1){ m_dThreshold1 = _dThresh1; };
	void SetThreshold2(const DistanceUnit& _dThresh2){ m_dThreshold2 = _dThresh2; };
	DistanceUnit GetThreshold1() const { return m_dThreshold1; };
	DistanceUnit GetThreshold2() const { return m_dThreshold2; };
	//Width
	void SetWidth(const DistanceUnit& _dWidth){ m_dWidth = _dWidth; }
	DistanceUnit GetWidth() const { return m_dWidth; };
	//Marking 1, 2
	void SetMarking1(const std::string& _sMarking){ m_sMarking1 = _sMarking; };
	void SetMarking2(const std::string& _sMarking){ m_sMarking2 = _sMarking; };
	void SetMarking1(LPCTSTR _psMarking){ m_sMarking1 = _psMarking; };
	void SetMarking2(LPCTSTR _psMarking){ m_sMarking2 = _psMarking; };
	virtual const std::string& GetMarking1() const { return m_sMarking1; };
	virtual const std::string& GetMarking2() const { return m_sMarking2; };
	virtual std::string GetMarking1()  { return m_sMarking1; };
	virtual std::string GetMarking2()  { return m_sMarking2; };


	//helpers for 3D rendering
	int GetBlockCount() const;										//returns the number of blocks
	int GetBlockTextureID(int _idx) const;							//returns a texture id for the block
	const std::vector<ARCVector2>& GetBlockGeometry(int _idx) const;		//returns a vector of 4 2d vertices
	const std::vector<ARCVector2>& GetBlockTexCoords(int _idx) const;	//returns a vector of 4 2d tex coords

	void GenerateBlocks();	//generates the blocks based on the runway data

	virtual Processor* CreateNewProc();
	virtual bool CopyOtherData(Processor* _pDestProc);

private:
	DistanceUnit m_dWidth;
	DistanceUnit m_dThreshold1;
	DistanceUnit m_dThreshold2;
	std::string m_sMarking1;
	std::string m_sMarking2;

	std::vector< RunwayTextures::BlockType > m_vBlocks;
};

#endif // !defined(AFX_RUNWAYPROC_H__3CB8D540_2A69_429A_9212_ABC07A5AA57D__INCLUDED_)

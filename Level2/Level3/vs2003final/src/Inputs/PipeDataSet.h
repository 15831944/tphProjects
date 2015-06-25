// PipeDataSet.h: interface for the CPipeDataSet class.
#pragma once
#include "..\common\dataset.h"
#include "Pipe.h"

typedef std::vector<CPipe> PIPEVECTOR;

class CPipeGraphMgr;
class CGraphVertexList;
class InputTerminal;

class PointToPipeInfo 
{
public:
	int m_nIndex;
	DistanceUnit m_dDisToPipe;
	CPipe* m_pPipe;
	bool m_bChecked;

	PointToPipeInfo()
	{
		m_nIndex = -1;
		m_dDisToPipe = 0.0;
		m_pPipe = NULL;
		m_bChecked = false;
	}

	bool operator < ( const PointToPipeInfo& _info ) const
	{
		return m_dDisToPipe < _info.m_dDisToPipe;
	}

	bool operator > ( const PointToPipeInfo& _info ) const
	{
		return m_dDisToPipe > _info.m_dDisToPipe;
	}		
};


class CPipeDataSet : public DataSet  
{
public:
	CPipeDataSet();
	virtual ~CPipeDataSet();

	virtual void readObsoleteData ( ArctermFile& p_file );
	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;
	virtual const char *getTitle (void) const { return "Pipe data file" ;};
	virtual const char *getHeaders (void) const {return "Pipe data " ;};
	virtual void clear (void);

	void AddPipe( CPipe& _newPipe );
	int GetPipeCount()const { return m_vPipes.size();	};
	CPipe* GetPipeAt( int _iIndex );
	std::vector<CPipe>* GetPipeVector() { return &m_vPipes; }
	const CPipe& operator [] (int _iIndex ) const;
	void DeletePipeAt(  int _iIndex  );

	void DetachMovingSideWalk( const ProcessorID& _movingSideProcID );
	void ReplaceMovingSideWalk( const ProcessorID& _oldProcID, const ProcessorID& _newProcID );

	void CalculateAllIntermediatData();
	void CalculateAllTimePointOnSideWalk();

	// Give a start point, end point, find the pipe list should flow.
	std::vector<int> AutomaicGetPipe( Point _ptStart, Point _ptEnd );
	void AutomaicGetPipeFlowCoveredRegion( Point& _ptStart, Point& _ptEnd ,POLLYGONVECTOR& _regions, Point& _ptFlowStart, Point& _ptFlowEnd );
	void GetPipeListFlowCoveredRegion( Point& _ptStart, Point& _ptEnd ,std::vector<int>& _vPipeIdx,POLLYGONVECTOR& _regions, Point& _ptFlowStart, Point& _ptFlowEnd );
	void clearAllEntryPointList();

	int GetPointListForLog( CGraphVertexList& _shortestPath, int _nPercent, PTONSIDEWALK&  _pointList );

	CPipeGraphMgr* m_pPipeMgr;

	CPipe* FindPipeByGuid(const CGuid& guid, const GUID& cls_guid);
	void SetLandsideInput(InputLandside* pLandsideInput);

	bool getUsedProInfo(const ProcessorID& _id,InputTerminal *_pTerm,std::vector<CString>& _strVec);
private:
	PIPEVECTOR m_vPipes;
	InputLandside* m_pInputLandside;
	void GetPipeListCoveredRegion( Point& _ptStart, Point& _ptEnd ,std::vector<int>& _vPipeIdx,POLLYGONVECTOR& _regions,Point& _ptFlowStart, Point& _ptFlowEnd );
	bool GetPipeList( PointToPipeInfo* _pStartInfo, PointToPipeInfo* _pEndInfo, DistanceUnit& _dTotal, 	std::vector<int>& _resList );
	void preDealThePath( CGraphVertexList& _shortestPath, std::vector<CMobPipeToPipeXPoint>& mobPointList );
};

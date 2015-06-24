// TempProcInfo.h: interface for the CTempProcInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPPROCINFO_H__EF9D8F8B_DCAD_46D4_93A1_229B0A457244__INCLUDED_)
#define AFX_TEMPPROCINFO_H__EF9D8F8B_DCAD_46D4_93A1_229B0A457244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "common\ARCVector.h"
#include "engine\IntegratedStation.h"
#include "engine\TrainTrafficGraph.h"
#include "Engine\TaxiwayTrafficGraph.h"


class CTempProcInfo  
{
private:
	//service location data
	std::vector<ARCVector3> m_vServiceLocation;

	//In constrain data
	std::vector<ARCVector3> m_vInConstraint;

	//out constrain data
	std::vector<ARCVector3> m_vOutConstraint;

	//queue path 
	std::vector<ARCVector3> m_vQueieList;

	//baggage barier
	std::vector<ARCVector3> m_vBaggageBarrier;

	//baggage moving path
	std::vector<ARCVector3> m_vBaggageMovingPath;

	//Retail store area pollygon
	std::vector<ARCVector3> m_vStoreAreaPath;

	//processor index
	int m_iProcIndex;
	
	IntegratedStation* m_pIntegratedStation;

	BOOL m_bIsCommonProcessor;

	int m_iProcType;

	//all railway and stations
	TrainTrafficGraph* m_pTempTrafficeGraph;
	

	//Taxiways Traffic
	TaxiwayTrafficGraph* m_pTempTaxiwayTrafficGraph;

	//integrated station's prebording area
	//HoldingArea* m_pPreboardingArea;
public:
	CTempProcInfo();
	
	virtual ~CTempProcInfo();
	
	//set current processor index
	void SetProcIndex (int _iIndex) { m_iProcIndex = _iIndex;};

	//set processor's service location data
	void SetTempServicLocation(const std::vector<ARCVector3>& _newServiceLocation);

	//set processor's In constrain  data
	void SetTempInConstraint(const std::vector<ARCVector3>& _newInConstrain);

	//set processor's out constrain  data
    void SetTempOutConstrain(const std::vector<ARCVector3>& _newOutConstrain);

	//set processor's queue data
	void SetTempQueue(const std::vector<ARCVector3>& _newQueue);

	void SetTempBaggageBarrier( const std::vector<ARCVector3>& _newPath){ m_vBaggageBarrier = _newPath;	};
	const std::vector<ARCVector3>& GetTempBaggageBarrier(){ return m_vBaggageBarrier;	};
	
	void SetTempBaggageMovingPath( const std::vector<ARCVector3>& _newPath){ m_vBaggageMovingPath = _newPath; }
	const std::vector<ARCVector3>& GetTempBaggageMovingPath(){ return m_vBaggageMovingPath;	};

	void SetTempStoreArea(const std::vector<ARCVector3>& _newPath){m_vStoreAreaPath = _newPath;}
	const std::vector<ARCVector3>& GetTempStoreArea()const {return m_vStoreAreaPath;}
	// set Integrated Station's struct
//	void SetIntegratedStationStruct(CAR & _carVector);
    
	void SetProcessorGroup(BOOL _isCommonProcessor) { m_bIsCommonProcessor = _isCommonProcessor;	}
	BOOL GetProcessorGroup()const { return m_bIsCommonProcessor;}
    
	//get processor's index
	int GetProcIndex () const { return m_iProcIndex;};
	
	
	//get processor's service location
	const std::vector<ARCVector3>& GetTempServicLocation() const { return  m_vServiceLocation;};

	//get processor's in constrain
	const std::vector<ARCVector3>& GetTempInConstraint() const { return  m_vInConstraint;};

	//get processor's out constrain
	const std::vector<ARCVector3>& GetTempOutConstraint() const { return  m_vOutConstraint;};

	//get processor's queue
	const std::vector<ARCVector3>& GetTempQueue() const { return  m_vQueieList;};

	//get integrated Station
	//const IntegratedStation & GetIntegratedStation() const { return m_integratedStation;}
	IntegratedStation* GetIntegratedStation()  { return m_pIntegratedStation;};
	//void  SetIntegratedStation( IntegratedStation* _pStation)  {  m_pIntegratedStation=_pStation;};
	void  SetIntegratedStation( IntegratedStation* _pStation)  {  m_pIntegratedStation=_pStation;};

	const TrainTrafficGraph* GetTrafficGraph() const { return m_pTempTrafficeGraph;}
	void SetTrafficGraph(TrainTrafficGraph* _pTrainTrafficGraph)  {  m_pTempTrafficeGraph=_pTrainTrafficGraph;}

	const TaxiwayTrafficGraph* GetTaxiwayTrafficGraph() const { return m_pTempTaxiwayTrafficGraph;}
	void SetTaxiwayTrafficGraph(TaxiwayTrafficGraph* _pTaxiwayTrafficGraph)  {  m_pTempTaxiwayTrafficGraph=_pTaxiwayTrafficGraph;}

	//void SetPreboardingArea(HoldingArea* _pPrebordingArea) { m_pPreboardingArea=_pPrebordingArea;}
	//HoldingArea* GetPreboardingArea() const { return m_pPreboardingArea;	};
	int GetProcType()const{ return m_iProcType; }
	void SetProcType(int proctype) { m_iProcType = proctype; }
	
	// cleal all temporal data
	void ClearAll();

};

#endif // !defined(AFX_TEMPPROCINFO_H__EF9D8F8B_DCAD_46D4_93A1_229B0A457244__INCLUDED_)

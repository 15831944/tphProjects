// AllCarSchedule.h: interface for the CAllCarSchedule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALLCARSCHEDULE_H__5C46BE64_40A6_4AF2_9259_F29CD45A8E90__INCLUDED_)
#define AFX_ALLCARSCHEDULE_H__5C46BE64_40A6_4AF2_9259_F29CD45A8E90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SingleCarSchedule.h"
#include "..\engine\TrainTrafficGraph.h"
#include "..\common\dataset.h"
typedef std::vector<CSingleCarSchedule*> ALLCARSCHEDULE;
class CAllCarSchedule  :public DataSet 
{
private:
	ALLCARSCHEDULE m_allCarSchedule;
	ProcessorList* m_pSystemProcessorlist;
	CRailWayData* m_pSystemRailWay;
	
public:
	CAllCarSchedule();
	CAllCarSchedule(const CAllCarSchedule& _anotherSchedule);
	CAllCarSchedule& operator = (const CAllCarSchedule& _anotherSchedule);

	virtual ~CAllCarSchedule();

public:
	
	virtual void readData (ArctermFile& p_file);	
	virtual void readObsoleteData ( ArctermFile& p_file );
	virtual void writeData (ArctermFile& p_file) const;
	virtual const char *getTitle (void) const { return "cars schedule File" ;};
	virtual const char *getHeaders (void) const {return "all cars schedule info " ;};

	virtual void clear (void);
	bool IsCarAlreadyExist(CString _sCarName);
	void AddNewCarSchedule( CSingleCarSchedule* _newCarSchedule){ m_allCarSchedule.push_back(_newCarSchedule); };
	bool DeleteCarSchedule( int _scheduleIndex );

	//check if all car flow is valid
	bool IsAllCarFlowValid(TrainTrafficGraph* _pTrafficGraph) const;
	void SetSystemProcessorList( ProcessorList* _pProcList) { m_pSystemProcessorlist=_pProcList;	};

	void SetSystemRailWay( CRailWayData* _pRailWaySys); 

	const ALLCARSCHEDULE* GetAllCarSchedule() const { return &m_allCarSchedule;	}
	void ClearCarSchedule();
	bool SelectBestSchedule( IntegratedStation* _pCurrentStation, IntegratedStation* _pDesStation, ElapsedTime _currentTime, std::vector<CViehcleMovePath>& _bestMovePath );
	void GetStationTimeTable( IntegratedStation* _pCurrentStation, TimeTableVector& _timeTable);

	//we have defined the schedule according to railwaysystem,
	//but if the railway system is changed, that is the pointer of
	// RailWayInfo is changed, so we should adjust CSingleCarSchedule's data member
	// RAILWAY m_allRailWay, to adapt that change
	// if railway has been deleted , we should delete the single schedule which use that railway
	
	void AdjustRailWay( TrainTrafficGraph* _pTraffic  );

	// delete all schedule which use this station
	// update all schedule which do not use this staiton
	void StationIsDeleted( int _iDeletedStationIdx );

	// test if there is at least one sechedule can take people from source station to dest station
	bool IfHaveScheduleBetweenStations( IntegratedStation* _pSourceStation, IntegratedStation* _pDesStation )const;
};

#endif // !defined(AFX_ALLCARSCHEDULE_H__5C46BE64_40A6_4AF2_9259_F29CD45A8E90__INCLUDED_)

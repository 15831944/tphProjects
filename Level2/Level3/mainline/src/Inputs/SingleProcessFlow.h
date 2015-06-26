#pragma once
#include "singlepaxtypeflow.h"
#include "ProcessorDestinationList.h"
class CSingleProcessFlow : public CSinglePaxTypeFlow
{
public:
	CSingleProcessFlow( InputTerminal* _pTerm );
	CSingleProcessFlow( const CSinglePaxTypeFlow& _anotherPaxFlow );
	CSingleProcessFlow(void);
	virtual ~CSingleProcessFlow(void);
public:
    CProcessorDestinationList* GetStartPair() const;
    void SetStartPair(CProcessorDestinationList* pStartPair);
    // add _secondProc to _insertProcID's destination list, add _insertProcID to _firstProc's destination list and remove _secondProc from _firstProc's destination list
    virtual void InsertBetwwen(const ProcessorID & _firstProcID, const ProcessorID & _secondProcID, const CFlowDestination& _insertProcID);
    //add  _sourceProcID's dest proc into _insertProcID's dest list , and delete _sourceProcID's all dest list ,then add _insertProcID into _sourceProcID's dest list
    virtual void InsertProceoosorAfter(const ProcessorID& _sourceProcID, const ProcessorID& _insertProcID);
    // add all destination processors of _oldDestProcId to _soruceProcID and remove all dest proc from _oldDestProcId , then remove _oldDestProcID
	void TakeOverDestProc( const  ProcessorID& _sourceProcID, const ProcessorID& _oldDestProcID , bool _bEraseOldProc = false );
	virtual void ClearIsolateNode();
	//check sub flow percentage valid
	bool IfSubFlowValid(const CString& sProcessName)const;
private:
    // find processor destination list by processor id, return NULL if not found.
    CProcessorDestinationList* FindProcDestinationListByProcID(const ProcessorID& procID);
private:
	CProcessorDestinationList* m_pStartPair;
};

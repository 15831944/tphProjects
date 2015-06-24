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
	// add all destnition proccessors of _oldDestProcId to _soruceProcID and remove all destproc from _oldDestProcId , then remove _oldDestProcID
	void TakeOverDestProc( const  ProcessorID& _sourceProcID, const ProcessorID& _oldDestProcID , bool _bEraseOldProc = false );
	virtual void ClearIsolateNode();

	//check sub flow percentag valid
	bool IfSubFlowValid(const CString& sProcessName)const;
private:
	CProcessorDestinationList* m_pStartPair;
};

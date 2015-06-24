#pragma once

#include "../Inputs/InputFlowSync.h"
#include "SimFlowSyncItem.h"
#include "../Common/ARCException.h"
class Passenger;
class CSimFlowPaxItem;

class ARCSyncUnavailableError : public ARCFlowError
{
public:
	ARCSyncUnavailableError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg = "", const CString& _strTime = "" );
	virtual ~ARCSyncUnavailableError();

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

class CSimFlowSync
{
public:
	CSimFlowSync();
	~CSimFlowSync();

	bool ProcessPassengerSync(const ProcessorID& procID, const ElapsedTime& curTime);
	bool ProcessVisitorSync(Person* pVisitor,const ProcessorID& procID, const ElapsedTime& curTime);

	//--------------------------------------------------------------------------------------------
	//Summary:
	//		when mobile element encounter error, need make waiter available
	void MakePassengerSyncAvailable(const ElapsedTime& curTime);
	void MakeVisitorSyncAvailable(Person* pVisitor, const ElapsedTime& curTime);

	void LoadData(Passenger* pPassenger, CInputFlowSync* pInputFlowSync);

private:
	CSimFlowPaxItem* CreateInstance(Passenger* pPassenger,CInputFlowPaxSyncItem* pPaxSyncItem);
	void noticefyLeave(Person* pPerson,const ElapsedTime& curTime);

	void ClearData();
	void DeleteItem(CSimFlowPaxItem* pItem);
private:
	Passenger* m_pPax;
	std::vector<CSimFlowPaxItem*> m_vectSimFlowSyncItem;
};
//--------------------------------------------------------------------------------
//				class:				CSimFlowMobItem
//				author:				sky.wen
//				date:				11/03/2011
//				purpose:			passenger sync with non passenger
//-----------------------------------------------------------------------------------
#pragma once
#include "..\CommonData\procid.h"

class Person;
class PaxVisitor;
class ElapsedTime;

//----------------------------------------------------------------------------------
//Summary:
//		every mobile element have the state that whether can go or wait
//---------------------------------------------------------------------------------
class MobileSyncState
{
public:
	MobileSyncState(Person* pMobileElement)
		:m_pMobileElement(pMobileElement)
		,m_bReady(false)
	{

	}
	~MobileSyncState()
	{

	}

	Person* m_pMobileElement;
	bool	m_bReady;
};

//-----------------------------------------------------------------------------------
//Summary:
//		non passenger and passenger have the same base class. passenger flow has the 
//		other properties like CSimFlowPaxItem
//-----------------------------------------------------------------------------------
class CSimFlowMobItem
{
public:
	CSimFlowMobItem();
	virtual ~CSimFlowMobItem();

	void SetMobileState(MobileSyncState* pMobileElementState);
	MobileSyncState* GetMobileState()const;

	void SetSyncProcID(const ProcessorID& syncProcID);
	const ProcessorID& GetSyncProcID()const;
private:
	MobileSyncState* m_pMobileSyncState;
	ProcessorID		m_syncProcID;
};

//------------------------------------------------------------------------------------
//Summary:
//		pax sync with non passengers 
//-----------------------------------------------------------------------------------
class CSimFlowPaxItem
{
public:
	CSimFlowPaxItem(void);
	~CSimFlowPaxItem(void);

	int GetCount()const;
	void AddItem(CSimFlowMobItem* pNonPaxSyncItem);
	void DeleteItem(CSimFlowMobItem* pNonPaxSyncItem);
	CSimFlowMobItem* GetItem(int idx)const;

	bool NeedSync()const;
	bool GetCanGoFurther(const ElapsedTime& curTime );
	CSimFlowMobItem* GetSyncVisitor(Person* pVisitor,const ProcessorID& procID);
	bool IsExsit(Person* pPerson)const;

	void SetSyncProcID(const ProcessorID& syncProcID);
	const ProcessorID& GetSyncProcID()const;

	void SetState(bool bState);
	bool GetState()const;

	bool available()const;

	void notifyNonPaxLeave(const ElapsedTime& curTime);

private:
	void ClearData();
private:
	std::vector<CSimFlowMobItem*> m_vNonPaxSyncItem;
	ProcessorID		m_syncProcID;
	bool	m_bReady;
};

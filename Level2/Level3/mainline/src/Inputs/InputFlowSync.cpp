#include "StdAfx.h"
#include "InputFlowSync.h"
#include "in_term.h"
#include <algorithm>

CInputFlowSyncItem::CInputFlowSyncItem()
{

}

CInputFlowSyncItem::~CInputFlowSyncItem()
{

}

void CInputFlowSyncItem::SetMobElementConstraint( const CMobileElemConstraint& constraint )
{
	m_paxType = constraint;
}

const CMobileElemConstraint& CInputFlowSyncItem::GetMobElementConstraint() const
{
	return m_paxType;
}	

void CInputFlowSyncItem::SetProcID( const ProcessorID& procID )
{
	m_syncPaxProcID = procID;
}

const ProcessorID& CInputFlowSyncItem::GetProcID() const
{
	return m_syncPaxProcID;
}

bool CInputFlowSyncItem::readData( ArctermFile& p_file,InputTerminal* pTerm )
{
	char buf[2560];
	if (p_file.getField(buf,2559) == 0)
		return false;

	m_paxType.SetInputTerminal(pTerm);
	m_paxType.readConstraintWithVersion(buf);

	m_syncPaxProcID.SetStrDict(pTerm->inStrDict);
	if(!m_syncPaxProcID.readProcessorID(p_file))
		return false;
	return true;
}

bool CInputFlowSyncItem::writeData( ArctermFile& p_file ) const
{
	m_paxType.writeConstraint(p_file);
	m_syncPaxProcID.writeProcessorID(p_file);
	return true;
}

CInputFlowPaxSyncItem::CInputFlowPaxSyncItem()
{

}

CInputFlowPaxSyncItem::~CInputFlowPaxSyncItem()
{
	ClearData();
}

int CInputFlowPaxSyncItem::GetCount() const
{
	return (int)m_vNonPaxSyncItem.size();
}

void CInputFlowPaxSyncItem::AddItem( CInputFlowSyncItem* pNonPaxSyncItem )
{
	ASSERT(pNonPaxSyncItem);
	if (pNonPaxSyncItem)
	{
		m_vNonPaxSyncItem.push_back(pNonPaxSyncItem);
	}
}

void CInputFlowPaxSyncItem::DeleteItem( CInputFlowSyncItem* pNonPaxSyncItem )
{
	ASSERT(pNonPaxSyncItem);
	if (pNonPaxSyncItem == NULL)
		return;
	
	std::vector<CInputFlowSyncItem*>::iterator iter = std::find(m_vNonPaxSyncItem.begin(),m_vNonPaxSyncItem.end(),pNonPaxSyncItem);
	if (iter != m_vNonPaxSyncItem.end())
	{
		delete pNonPaxSyncItem;
		m_vNonPaxSyncItem.erase(iter);
	}
}

CInputFlowSyncItem* CInputFlowPaxSyncItem::GetItem(int idx) const
{
	if (idx >= 0 && idx < GetCount())
	{
		return m_vNonPaxSyncItem.at(idx);
	}

	return NULL;
}

bool CInputFlowPaxSyncItem::readData( ArctermFile& p_file,InputTerminal* pTerm )
{
	CInputFlowSyncItem::readData(p_file,pTerm);

	char buf[255];
	p_file.getField(buf,255);
	m_strSyncName = buf;

	int nCount = 0;
	p_file.getInteger(nCount);
	for (int i = 0; i < nCount; i++)
	{
		CInputFlowSyncItem* pNonPaxSyncItem = new CInputFlowSyncItem();
		if(!pNonPaxSyncItem->readData(p_file,pTerm))
		{
			return false;
		}
		m_vNonPaxSyncItem.push_back(pNonPaxSyncItem);
	}
	return true;
}

bool CInputFlowPaxSyncItem::writeData( ArctermFile& p_file ) const
{
	CInputFlowSyncItem::writeData(p_file);

	p_file.writeField(m_strSyncName);

	int nCount = GetCount();
	p_file.writeInt(nCount);
	for (int i = 0; i < nCount; i++)
	{
		CInputFlowSyncItem* pNonPaxSyncItem = m_vNonPaxSyncItem.at(i);
		pNonPaxSyncItem->writeData(p_file);
	}
	return true;
}

void CInputFlowPaxSyncItem::ClearData()
{
	for (size_t i = 0; i < m_vNonPaxSyncItem.size(); i++)
	{
		delete m_vNonPaxSyncItem[i];
	}
	m_vNonPaxSyncItem.clear();
}

void CInputFlowPaxSyncItem::GetFittestNonSyncItem( const CMobileElemConstraint& nonPaxtype,std::vector<CInputFlowSyncItem*>& nonSyncList ) const
{
	CInputFlowSyncItem* pBestFlowSyncItem = NULL;
	for (size_t i = 0; i < m_vNonPaxSyncItem.size(); i++)
	{
		CInputFlowSyncItem* pItem = m_vNonPaxSyncItem.at(i);
		const CMobileElemConstraint& paxType = pItem->GetMobElementConstraint();
		if (paxType.fits(nonPaxtype))
		{
			//first fit
			if (pBestFlowSyncItem == NULL)
			{
				nonSyncList.push_back(pItem);
				pBestFlowSyncItem = pItem;
				continue;
			}

			if (pBestFlowSyncItem->GetMobElementConstraint() == paxType)
			{
				nonSyncList.push_back(pItem);
				continue;
			}
					
			if (pBestFlowSyncItem->GetMobElementConstraint().fits(paxType))
			{
				nonSyncList.clear();
				nonSyncList.push_back(pItem);
				pBestFlowSyncItem = pItem;;
			}
		}
	}
}

void CInputFlowPaxSyncItem::SetSyncName( const CString& sSyncName )
{
	m_strSyncName = sSyncName;
}

const CString& CInputFlowPaxSyncItem::GetSyncName() const
{
	return m_strSyncName;
}

CInputFlowSyncItem* CInputFlowPaxSyncItem::GetNonPaxSyncItem( const CMobileElemConstraint& paxType,const ProcessorID& procID )const
{
	for (unsigned i = 0; i < m_vNonPaxSyncItem.size(); i++)
	{
		CInputFlowSyncItem* pItem = m_vNonPaxSyncItem.at(i);
		if (pItem->GetMobElementConstraint() == paxType && pItem->GetProcID() == procID)
		{
			return pItem;
		}
	}
	return NULL;
}

CInputFlowSync::CInputFlowSync()
: DataSet(FlowSyncFile,2.3f)
{
}

CInputFlowSync::~CInputFlowSync()
{
	ClearData();
}

void CInputFlowSync::readData(ArctermFile& p_file)
{
	p_file.getLine();

	int nCount = 0;
	p_file.getInteger(nCount);
	p_file.getLine();
	for (int i = 0; i < nCount; i++)
	{
		CInputFlowPaxSyncItem* pFlowSyncPaxItem = new CInputFlowPaxSyncItem();
		if(!pFlowSyncPaxItem->readData(p_file,GetInputTerminal()))
		{
			delete pFlowSyncPaxItem;
			pFlowSyncPaxItem = NULL;
			return;
		}
		m_vInputFlowSyncItemList.push_back(pFlowSyncPaxItem);
		p_file.getLine();
	}
}

void CInputFlowSync::readObsoleteData( ArctermFile& p_file )
{

}

void CInputFlowSync::writeData(ArctermFile& p_file) const
{	
	int nCount = GetCount();
	p_file.writeInt(nCount);
	p_file.writeLine();
	for (int i = 0; i < nCount; i++)
	{
		CInputFlowSyncItem* pFlowSyncPaxItem = m_vInputFlowSyncItemList.at(i);
		pFlowSyncPaxItem->writeData(p_file);
		p_file.writeLine();
	}
}

void CInputFlowSync::clear()
{
	ClearData();
}

const char* CInputFlowSync::getTitle() const
{
	return "Flow synchronization point info";
}

const char* CInputFlowSync::getHeaders() const
{
	return "PaxType, SyncPaxProcID, NonPaxType, SyncNonPaxProcID";
}

int CInputFlowSync::GetCount() const
{
	return (int)m_vInputFlowSyncItemList.size();
}

void CInputFlowSync::AddItem( CInputFlowPaxSyncItem* pNonPaxSyncItem )
{
	ASSERT(pNonPaxSyncItem);
	if (pNonPaxSyncItem)
	{
		m_vInputFlowSyncItemList.push_back(pNonPaxSyncItem);
	}
}

void CInputFlowSync::DeleteItem( CInputFlowPaxSyncItem* pNonPaxSyncItem )
{
	ASSERT(pNonPaxSyncItem);
	if (pNonPaxSyncItem == NULL)
		return;

	InputFlowSyncItemList::iterator iter = std::find(m_vInputFlowSyncItemList.begin(),m_vInputFlowSyncItemList.end(),pNonPaxSyncItem);
	if (iter != m_vInputFlowSyncItemList.end())
	{
		delete pNonPaxSyncItem;
		m_vInputFlowSyncItemList.erase(iter);
	}
}

CInputFlowPaxSyncItem* CInputFlowSync::GetItem(int idx) const
{
	if (idx >= 0 && idx < GetCount())
	{
		return m_vInputFlowSyncItemList.at(idx);
	}

	return NULL;
}

void CInputFlowSync::ClearData()
{
	for (size_t i = 0; i < m_vInputFlowSyncItemList.size(); i++)
	{
		delete m_vInputFlowSyncItemList[i];
	}
	m_vInputFlowSyncItemList.clear();
}

bool CInputFlowSync::GetFlowPaxSync( const CMobileElemConstraint& paxType,InputFlowSyncItemList& syncList) const
{
	std::vector<ProcessorID> vIDs;
	for (unsigned i = 0; i < m_vInputFlowSyncItemList.size(); i++)
	{
		CInputFlowPaxSyncItem* pFlowSync = m_vInputFlowSyncItemList.at(i);
		if (vIDs.empty())
		{
			vIDs.push_back(pFlowSync->GetProcID());
		}

		if (std::find(vIDs.begin(),vIDs.end(),pFlowSync->GetProcID()) == vIDs.end())
		{
			vIDs.push_back(pFlowSync->GetProcID());
		}
	}

	for (int ii = 0; ii < static_cast<int>(vIDs.size()); ii++)
	{
		CInputFlowPaxSyncItem* pFlowSync = GetFlowPaxSync(paxType,vIDs.at(ii));
		if (pFlowSync)
		{
			syncList.push_back(pFlowSync);
		}
	}

	if (syncList.empty())
		return false;

	return true;
}

CInputFlowPaxSyncItem* CInputFlowSync::GetFlowPaxSync( const CMobileElemConstraint& paxType,const ProcessorID& procID ) const
{
	CInputFlowPaxSyncItem* pBestFlowSync = NULL;
	for (unsigned i = 0; i < m_vInputFlowSyncItemList.size(); i++)
	{
		CInputFlowPaxSyncItem* pFlowSync = m_vInputFlowSyncItemList.at(i);
		if (pFlowSync->GetMobElementConstraint().fits(paxType) && procID == pFlowSync->GetProcID())
		{
			if (pBestFlowSync == NULL)
			{
				pBestFlowSync = pFlowSync;
				continue;
			}

			if(pBestFlowSync->GetMobElementConstraint().fits(pFlowSync->GetMobElementConstraint()))
			{
				pBestFlowSync = pFlowSync;
			}
		}
	}
	return pBestFlowSync;
}

CInputFlowPaxSyncItem* CInputFlowSync::GetFlowParentNonPaxSync( const CMobileElemConstraint& nonpaxType,const ProcessorID& procID ) const
{
	for (unsigned i = 0; i < m_vInputFlowSyncItemList.size(); i++)
	{
		CInputFlowPaxSyncItem* pFlowSync = m_vInputFlowSyncItemList.at(i);
		CInputFlowSyncItem* pNonFlowSyncItem = pFlowSync->GetNonPaxSyncItem(nonpaxType,procID);
		if (pNonFlowSyncItem)
		{
			return pFlowSync;
		}
	}
	return NULL;
}

CInputFlowSyncItem* CInputFlowSync::GetFlowNonPaxSync( const CMobileElemConstraint& nonpaxType,const ProcessorID& procID ) const
{
	for (unsigned i = 0; i < m_vInputFlowSyncItemList.size(); i++)
	{
		CInputFlowPaxSyncItem* pFlowSync = m_vInputFlowSyncItemList.at(i);
		CInputFlowSyncItem* pNonFlowSyncItem = pFlowSync->GetNonPaxSyncItem(nonpaxType,procID);
		if (pNonFlowSyncItem)
		{
			return pNonFlowSyncItem;
		}
	}
	return NULL;
}
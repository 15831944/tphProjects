#include "StdAfx.h"
#include ".\retailelement.h"
#include "RetailReport.h"
#include "..\Results\RetailLogEntry.h"
#include "..\Common\states.h"

RetailPaxElement::RetailPaxElement(void)
{
}

RetailPaxElement::~RetailPaxElement(void)
{
}

bool RetailPaxElement::CalculateQueueTime(int nStart, int nEnd,ElapsedTime& eEnterQueueTime,ElapsedTime& eQueueTime)
{
	PaxEvent track1, track2;
	track2.init (getEvent(nStart));
	int trackCount = getCount();

	bool bWait = false;
	for (int i = nStart + 1; i <= nEnd; i++)
	{
		track1 = track2;
		track2.init (getEvent(i));

		if (track1.getState() == WaitInQueue && m_pStoreProcElement->CheckOut(track1.getProc()))
		{
			if (!bWait)
			{
				eEnterQueueTime = track1.getTime();
				bWait = true;
			}

			while (track2.getState() != LeaveQueue && (i < trackCount-1) &&
				track2.getState() != JumpQueue && track2.getProc() == track1.getProc())
			{
				i++;
				track2.init (getEvent(i));
			}

			ElapsedTime time1 = track1.getTime();
			ElapsedTime time2 = track2.getTime();
			ElapsedTime mytotalTime = time2 - time1;
			eQueueTime += mytotalTime;
		}
	}

	return bWait;
}

ElapsedTime RetailPaxElement::CalculateServiceTime( int nStart,int nEnd )
{
	ElapsedTime totalTime;
	PaxEvent track1, track2,track3;
	track2.init (getEvent(0));
	int trackCount = getCount();

	ElapsedTime etInterval;
	for (int i = nStart + 1; i <= nEnd; i++)
	{
		track1 = track2;
		track2.init (getEvent(i));
		if (i != trackCount - 1)
		{
			track3.init(getEvent(i+1));
		}

		if (track1.getState() == ArriveAtServer &&
			m_pStoreProcElement->CheckOut(track1.getProc()))
		{
			if (i != trackCount - 1)
			{
				if (track3.getState() == LeaveServer)
				{
					track2 = track3;
					i++;
				}
			}

			etInterval=track2.getTime() - track1.getTime();
			totalTime += etInterval;
		}
	}
	
	return totalTime;
}
//-------------------------------------------------------------------------
//Summary:
//		record passenger process at shop
//-------------------------------------------------------------------------
void RetailPaxElement::BuildRetailShop(const ElapsedTime& p_start, const ElapsedTime& p_end )
{
	loadEvents();
	int nStartPos = -1;
	int nEndPos = -1;
	if (PassObject (p_start, p_end,nStartPos,nEndPos))
	{
		PaxEvent startTrack;
		startTrack.init (getEvent(nStartPos));
		m_paxData.m_lEnterStore = startTrack.getTime();

		PaxEvent endTrack;
		endTrack.init(getEvent(nEndPos));
		m_paxData.m_lLeaveStore = endTrack.getTime();

		ElapsedTime eEnterQueueTime;
		ElapsedTime eQueueTime;
		if (CalculateQueueTime(nStartPos,nEndPos,eEnterQueueTime,eQueueTime))
		{
			m_paxData.m_lEnterCheckOutQueueTime = eEnterQueueTime;
			m_paxData.m_lCheckOutQueueTime = eQueueTime;
		}
		
		m_paxData.m_lCheckOutServiceTime = CalculateServiceTime(nStartPos,nEndPos);
	}
}

bool RetailPaxElement::PassObject( const ElapsedTime& p_start, const ElapsedTime& p_end,int& nStartPos,int& nEndPos )
{
	PaxEvent track;
	int trackCount = getCount();
	bool bExistStart = false;
	bool bExistEnd = false;
	for (int i = 0; i < trackCount; i++)
	{
		track.init (getEvent(i));

		if (m_pStoreProcElement->ValidProcessor(track.getProc()))
		{
			if (!bExistStart &&track.getState() == ArriveAtServer&&track.getTime() >= p_start)
			{
				nStartPos = i;
				bExistStart = true;
			}

			if (bExistStart &&track.getState() == LeaveServer&&track.getTime() <= p_end)
			{
				nEndPos = i;
				bExistEnd = true;
			}
		}
	}
	
	if (bExistEnd && bExistStart)
	{
		return true;
	}

	return false;
}

void RetailPaxElement::InitStoreElement( StoreProcessorElement* pStoreElement )
{
	m_pStoreProcElement = pStoreElement;
}

void StoreProcessorElement::Clear()
{
	int nCount = (int)m_shopDataList.size();
	for (int i = 0; i < nCount; i++)
	{
		delete m_shopDataList[i];
	}
	m_shopDataList.clear();
}

bool StoreProcessorElement::CalculateRetailShopData()
{
	//clear old data
	Clear();

	int nCount = (int)m_vlogEntry.size();
	for (int i = 0; i < nCount; i++)
	{
		CRetailLogEntry logEntry = m_vlogEntry.at(i);
		int trackCount = logEntry.getCount();
		for (int i = 0; i < trackCount; i++)
		{
			const RetailEventStruct& eventS = logEntry.getEvent(i);
			RetailShopData* pRetailShopData = GetData(eventS.paxID);
			if (pRetailShopData)
			{
				pRetailShopData->m_bInventoryOk = eventS.InventoryOk;
				if (eventS.state != RETAIL_CHECKOUT)
				{
					pRetailShopData->m_dSales += eventS.price * eventS.itemCount;
					pRetailShopData->m_dAptRevenue += pRetailShopData->m_dSales*eventS.airportCut;
					pRetailShopData->m_lPotentiallyBought += eventS.itemCount;
				}
			}
			else
			{
				pRetailShopData = new RetailShopData();
				pRetailShopData->m_lPaxID = eventS.paxID;
				pRetailShopData->m_dSales = eventS.price * eventS.itemCount;
				pRetailShopData->m_dAptRevenue = pRetailShopData->m_dSales*eventS.airportCut;
				pRetailShopData->m_lPotentiallyBought = eventS.itemCount;
				pRetailShopData->m_bInventoryOk = eventS.InventoryOk;
				pRetailShopData->m_bCheckOut = false;
				pRetailShopData->m_bByPass = false;
				
				m_shopDataList.push_back(pRetailShopData);
			}

			if (eventS.state == RETAIL_CHECKOUT || eventS.state == RETAIL_CANCEL)
			{
				CString strProductName;
				if(m_pRetailReport->GetProductName(eventS.itemTypeID,strProductName))
				{
					CString strName;
					if (pRetailShopData->m_strProductNameList.IsEmpty())
					{
						strName.Format(_T("%s: %d"),strProductName,eventS.itemCount);
						pRetailShopData->m_strProductNameList = strName;
					}
					else
					{
						strName.Format(_T(" %s: %d"),strProductName,eventS.itemCount);
						pRetailShopData->m_strProductNameList += strName;
					}
				}
			}
			//check passenger whether check out
			if (eventS.state == RETAIL_CHECKOUT)
			{
				pRetailShopData->m_bCheckOut = true;
			}

			//check passenger did not go store to shopping
			if (eventS.state == RETAIL_BUY || eventS.state == RETAIL_SOLDOUT)
			{
				pRetailShopData->m_bByPass = true;
			}
		}
	}

	CalculatePaxRetailData();
	return true;
}

void StoreProcessorElement::CalculatePaxRetailData()
{
	if (m_shopDataList.empty())
		return;
	
	m_vRetailPaxElement.clear();
	int nCount = (int)m_shopDataList.size();
	for (int i = 0; i < nCount; i++)
	{
		const RetailShopData* shopData = m_shopDataList.at(i);
		RetailPaxElement element;
		if (m_pRetailReport && m_pRetailReport->GetRetailElement(shopData->m_lPaxID,element))
		{
			element.InitStoreElement(this);
			element.BuildRetailShop(m_pRetailReport->GetStartTime(),m_pRetailReport->GetEndTime());
			m_vRetailPaxElement.push_back(element);
		}
	}
}

void StoreProcessorElement::CalculateRetailShopSummaryData(const ElapsedTime& tStart, const ElapsedTime& tEnd,CStatisticalTools<double>& tempTools,double& dApt)
{
	Clear();

	int nCount = (int)m_vlogEntry.size();
	for (int i = 0; i < nCount; i++)
	{
		CRetailLogEntry logEntry = m_vlogEntry.at(i);
		int trackCount = logEntry.getCount();
		for (int i = 0; i < trackCount; i++)
		{
			const RetailEventStruct& eventS = logEntry.getEvent(i);
			RetailShopData* pRetailShopData = GetData(eventS.paxID);
			if (pRetailShopData)
			{
				pRetailShopData->m_bInventoryOk = eventS.InventoryOk;
				if (eventS.state != RETAIL_CHECKOUT)
				{
					pRetailShopData->m_dSales += eventS.price * eventS.itemCount;
					pRetailShopData->m_dAptRevenue += pRetailShopData->m_dSales*eventS.airportCut;
					pRetailShopData->m_lPotentiallyBought += eventS.itemCount;
				}
			}
			else
			{
				pRetailShopData = new RetailShopData();
				pRetailShopData->m_lPaxID = eventS.paxID;
				pRetailShopData->m_dSales = eventS.price * eventS.itemCount;
				pRetailShopData->m_dAptRevenue = pRetailShopData->m_dSales*eventS.airportCut;
				pRetailShopData->m_lPotentiallyBought = eventS.itemCount;
				pRetailShopData->m_bInventoryOk = eventS.InventoryOk;
				pRetailShopData->m_bCheckOut = false;
				pRetailShopData->m_bByPass = false;
				m_shopDataList.push_back(pRetailShopData);
			}

			//check passenger whether check out
			if (eventS.state == RETAIL_CHECKOUT)
			{
				pRetailShopData->m_bCheckOut = true;
			}
		
			//check passenger did not go store to shopping
			if (eventS.state == RETAIL_BUY || eventS.state == RETAIL_SOLDOUT)
			{
				pRetailShopData->m_bByPass = true;
			}
		}
	}

	CalculatePaxRetailData();

	int paxCount = (int)m_shopDataList.size();
	for (int i = 0; i < paxCount; i++)
	{
		RetailShopData* shopData = m_shopDataList.at(i);
		
		RetailPaxElement retailElement;
		if(!GetRetailPaxElement(shopData->m_lPaxID,retailElement))
			continue;
	
		const RetailPaxData& paxData = retailElement.GetRetailPaxData();
		if (paxData.m_lEnterCheckOutQueueTime < tStart || paxData.m_lEnterCheckOutQueueTime > tEnd)
			continue;
		
		if (!shopData->m_bCheckOut)
			continue;

		tempTools.AddNewData(shopData->m_dSales);

		dApt += shopData->m_dSales;
	}
	tempTools.SortData();
}

bool StoreProcessorElement::CheckOut( long ldx ) const
{
	long lCount = (long)m_vlogEntry.size();
	for (long i = 0; i < lCount; i++)
	{
		const CRetailLogEntry& logEntry = m_vlogEntry.at(i);
		if (logEntry.GetProcIndex() == ldx)
		{
			return logEntry.GetSubType() == 2 ? true : false;
		}
	}
	return false;
}

bool StoreProcessorElement::ValidProcessor( long ldx ) const
{
	long lCount = (long)m_vlogEntry.size();
	for (long i = 0; i < lCount; i++)
	{
		const CRetailLogEntry& logEntry = m_vlogEntry.at(i);
		if (logEntry.GetIndex() == ldx)
		{
			return true;
		}
	}
	return false;
}

void StoreProcessorElement::writeEntry( ArctermFile& p_file )
{
	long lCount = (long)m_shopDataList.size();
	for (long i = 0; i < lCount; i++)
	{
		const RetailShopData* shopData = m_shopDataList.at(i);
		RetailPaxElement paxElement;
		if (GetRetailPaxElement(shopData->m_lPaxID,paxElement))
		{
			const RetailPaxData& paxData = paxElement.GetRetailPaxData();

			p_file.writeInt(shopData->m_lPaxID);//1
			p_file.writeInt(shopData->m_bByPass ? 1: 0);//2
			p_file.writeField(m_procID.GetIDString());//3
			p_file.writeTime(paxData.m_lEnterStore,TRUE);//4
			p_file.writeTime(paxData.m_lLeaveStore,TRUE);//5
			p_file.writeInt(shopData->m_lPotentiallyBought);//6
			p_file.writeInt(shopData->m_bCheckOut ? 1: 0);//7
			p_file.writeTime(paxData.m_lEnterCheckOutQueueTime,TRUE);//8
			p_file.writeTime(paxData.m_lCheckOutQueueTime,TRUE);//9
			p_file.writeTime(paxData.m_lCheckOutServiceTime,TRUE);//10
			p_file.writeFloat(static_cast<float>(shopData->m_dSales));//11
			p_file.writeFloat(static_cast<float>(shopData->m_dAptRevenue));//12
		//	p_file.writeInt(shopData->m_bInventoryOk ? 1: 0);//13
			char str[MAX_PAX_TYPE_LEN];
			paxElement.printFullType (str);
			p_file.writeField (str);//14

			p_file.writeField(shopData->m_strProductNameList);//15
			p_file.writeLine();
		}
	}
}

bool StoreProcessorElement::GetRetailPaxElement( long lPaxID,RetailPaxElement& paxElement ) const
{
	long nCount = (long)m_vRetailPaxElement.size();
	for (long i = 0; i < nCount; i++)
	{
		const RetailPaxElement& element = m_vRetailPaxElement.at(i);
		if (element.getID() == lPaxID)
		{
			paxElement = element;
			return true;
		}
	}
	return false;
}

double StoreProcessorElement::GetTotalSales() const
{
	double dTotal = 0.0;
	long lCount = (long)m_shopDataList.size();
	for (long i = 0; i < lCount; i++)
	{
		const RetailShopData* shopData = m_shopDataList.at(i);
		dTotal += shopData->m_dSales;
	}
	return dTotal;
}

RetailShopData* StoreProcessorElement::GetData( long nPaxID )
{
	long lCount = (long)m_shopDataList.size();
	for (long i = 0; i < lCount; i++)
	{
		RetailShopData* shopData = m_shopDataList.at(i);
		if (shopData->m_lPaxID == nPaxID)
		{
			return  shopData;
		}
	}
	return NULL;
}

StoreProcessorElement::~StoreProcessorElement()
{
	Clear();
}

void StoreProcessorElement::InitStoreElement( const ProcessorID& procID,CRetailReport* pRetailReport )
{
	m_procID = procID;
	m_pRetailReport = pRetailReport;
}

void StoreProcessorElement::AddLogEntry( const CRetailLogEntry& logEntry )
{
	m_vlogEntry.push_back(logEntry);
}

StoreProcessorElement::StoreProcessorElement()
{

}
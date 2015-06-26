#include "StdAfx.h"
#include ".\lsresourcequeueresult.h"
#include "LandsideBaseParam.h"
#include "Landside/LandsideLayoutObject.h"
#include "Landside/InputLandside.h"

LSResourceQueueResult::LSResourceQueueResult(void)
{
}

LSResourceQueueResult::~LSResourceQueueResult(void)
{
}

void LSResourceQueueResult::GenerateResult( CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde )
{
	m_mapResQueue.clear();

	LandsideResourceQueueLog resQueueLog; 
	CString strDelayDescPath = (*pFunc)(LSResourceQueueLog);
	resQueueLog.LoadData(strDelayDescPath);

	//check start end object

	int nDelayCount = resQueueLog.GetItemCount();
	for (int nItem = 0; nItem < nDelayCount; ++ nItem)
	{
		LandsideResourceQueueItem* pResQueueItem = resQueueLog.GetItem(nItem);

		ASSERT(pResQueueItem != NULL);
		if(pResQueueItem == NULL)
			continue;

		LandsideFacilityLayoutObject* pRes = pLandisde->getObjectList().getObjectByID(pResQueueItem->m_nResourceID);
		if (pRes == NULL)
			continue;

		if(!pParameter->FilterObject(pRes->getName()))
			continue;

		CHierachyName carName;
		carName.fromString( pResQueueItem->m_strVehicleType);
		if (!pParameter->FilterName(carName))
			continue;
		
		QueueItem qItem;

		qItem.m_nResourceID = pResQueueItem->m_nResourceID;
		qItem.m_strResName = pResQueueItem->m_strResName;


		qItem.m_eTime = pResQueueItem->m_eTime;
		qItem.m_strVehicleType = pResQueueItem->m_strVehicleType;
		qItem.m_nVehicleID = pResQueueItem->m_nVehicleID;
		qItem.m_enumOperation = pResQueueItem->m_enumOperation;


		AddItem(qItem);
	}

	//time range
	ElapsedTime eStartTime = pParameter->getStartTime();
	ElapsedTime eEndTime = pParameter->getEndTime();
	ElapsedTime eInterval = pParameter->getInterval();

	ASSERT(eInterval.getPrecisely() > 0);
	if (eInterval <= ElapsedTime(0L))
		eInterval.setPrecisely(10*60*100L);//10mins

	std::vector<DetailTimeItem> vSampleItems;
	ElapsedTime eCurTime = eStartTime;
	while (eCurTime < eEndTime)
	{
		DetailTimeItem dtItem;
		dtItem.m_eTime = eCurTime;

		vSampleItems.push_back(dtItem);

		eCurTime+= eInterval;
	}

	std::map<int, ResQueue>::iterator iter =  m_mapResQueue.begin();
	for (; iter != m_mapResQueue.end(); ++iter)
	{
		ResQueue& resQ = (*iter).second;
		size_t nLogCount = resQ.m_vItems.size();


		for (int i = 0; i < pParameter->GetVehicleTypeNameCount(); i++)
		{
			CHierachyName name = pParameter->GetVehicleTypeName(i);

			std::vector<DetailTimeItem> vThisResItems = vSampleItems;

			int nResLen = 0;

			for (size_t nn = 0; nn < nLogCount; ++nn)
			{

				QueueItem& qItem = resQ.m_vItems[nn];

				//define vehicle type
				CHierachyName vehicleName;
				vehicleName.fromString( qItem.m_strVehicleType);
				if (vehicleName.fitIn(name))
				{
					if(qItem.m_enumOperation == LandsideResourceQueueItem::QO_Entry)
						nResLen += 1;
					if(qItem.m_enumOperation == LandsideResourceQueueItem::QO_Exit)
						nResLen -= 1;

					if(nResLen < 0)
					{
						ASSERT(0);
						nResLen = 0;
					}

					CString strTime = qItem.m_eTime.printTime();

					//update the queue length
					int nSegCount = static_cast<int>(vThisResItems.size());
					for (int nSeg = 0; nSeg < nSegCount; ++ nSeg)
					{
						if (nSeg == 0)
						{
							if (qItem.m_eTime <= vThisResItems[nSeg].m_eTime)
							{
								for (int iStart = nSeg; iStart < nSegCount; ++iStart)
								{
									vThisResItems[iStart].m_nLen = nResLen;
								}
								break;
							}
							continue;
						}

						if (qItem.m_eTime > vThisResItems[nSeg -1].m_eTime &&
							qItem.m_eTime <= vThisResItems[nSeg].m_eTime)
						{
							for (int iStart = nSeg; iStart < nSegCount; ++iStart)
							{
								vThisResItems[iStart].m_nLen = nResLen;
							}
							break;
						}
					}
				}
			}

			DetailItem resDetailItem;
			resDetailItem.m_nObjID = resQ.m_nResourceID;
			resDetailItem.m_strObjName = resQ.m_strResName;
			resDetailItem.m_strVehicleType = name.toString();
			resDetailItem.m_vTimeItems = vThisResItems;
			//push to result
			m_vResDetail.push_back(resDetailItem);

		}
	}

}

void LSResourceQueueResult::RefreshReport( LandsideBaseParam * parameter )
{


}

void LSResourceQueueResult::AddItem( QueueItem& qItem )
{
	if(m_mapResQueue.find(qItem.m_nResourceID) != m_mapResQueue.end())
	{
		m_mapResQueue[qItem.m_nResourceID].m_vItems.push_back(qItem);
	}
	else
	{
		ResQueue resItem;
		resItem.m_nResourceID = qItem.m_nResourceID;
		resItem.m_strResName = qItem.m_strResName;

		resItem.m_vItems.push_back(qItem);
		m_mapResQueue[qItem.m_nResourceID] = resItem;

	}
	
}




























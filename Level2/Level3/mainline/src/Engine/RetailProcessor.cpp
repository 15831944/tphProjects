#include "StdAfx.h"
#include ".\retailprocessor.h"
#include "proclist.h"
#include "..\Inputs\flight.h"
#include "..\Inputs\schedule.h"
#include "person.h"
#include "ARCportEngine.h"
#include "SimEngineConfig.h"
#include "..\Inputs\miscproc.h"
#include "..\Main\ARCPort.h"
#include "..\Economic\EconomicManager.h"
#include "..\Results\RetailLog.h"
#include "..\Results\RetailLogEntry.h"
#include "..\Inputs\ProductStockingList.h"
#include "person.h"
#include "..\Common\ProbabilityDistribution.h"
#include "..\Economic\ProcEconomicDataElement.h"

RetailProcessor::RetailProcessor(void)
:m_pRetailMisData(NULL)
,m_pRetailFeeData(NULL)
,m_emType(Retial_Shop_Section)
{
}

RetailProcessor::~RetailProcessor(void)
{
}

int RetailProcessor::readSpecialField( ArctermFile& procFile )
{
	LineProcessor::readSpecialField(procFile);

	int emType = 0;
	procFile.getInteger(emType);
	m_emType = (RetaiType)emType;

	int nCount = 0;
	procFile.getInteger(nCount);

	Point storeArea[MAX_POINTS];
	for (int i = 0; i < nCount; i++)
	{
		Point aPoint;
		procFile.getPoint(aPoint);
		storeArea[i] = aPoint;
	}

	initStoreArea(nCount,storeArea);

	return 1;
}

int RetailProcessor::writeSpecialField( ArctermFile& procFile ) const
{
	LineProcessor::writeSpecialField(procFile);
	
	procFile.writeInt(int(m_emType));

	int nCount = m_storeArea.getCount();
	procFile.writeInt(nCount);

	for (int i = 0; i < nCount; i++)
	{
		procFile.writePoint(m_storeArea.getPoint(i));
	}
	return 1;
}

void RetailProcessor::initStoreArea( int pathCount, const Point *pointList )
{
	if (pathCount == 0)
		return;

	if (pathCount < 3)
		throw new StringError ("Store Areas must have at least 3 point");

	m_storeArea.init(pathCount,pointList);

	double dArea = m_storeArea.calculateArea();
	if (dArea == 0.0)
	{
		char str[80];
		strcpy (str, "Retail Area ");
		name.printID (str + strlen (str));
		strcat (str, " has no space");
		throw new StringError (str);
	}
}

//---------------------------------------------------------------------
//Summary:
//		check processor is valid passenger
//---------------------------------------------------------------------
bool RetailProcessor::isPaxSkipVacant( Person* pPerson, const ElapsedTime& _curTime )
{
	if (m_pRetailMisData == NULL)
		return false;
	
	int depFlightIndex = pPerson->getLogEntry().getDepFlight();
	if (depFlightIndex < 0)
	{
		if (m_pRetailMisData->PaxQueueLenghtVacant(pPerson->getType(),getQueueLength()))
		{
			SkipProcess(pPerson,_curTime);
			return true;
		}
		return false;	
	}

	assert( m_pTerm );
	Flight *flight = m_pEngine->m_simFlightSchedule.getItem (depFlightIndex);
	if (flight)
	{
		if (m_pEngine->IsAirsideSel() && !flight->RetrieveAirsideDepartureTime())
			return false;

		ElapsedTime eDeltaTime;

		if(simEngineConfig()->isSimAirsideMode())
		{
			if (flight->RetrieveAirsideDepartureTime())
			{
				eDeltaTime = flight->GetRealDepTime() - _curTime;

				if (m_pRetailMisData->PaxSkipVacant(pPerson->getType(),getQueueLength(),eDeltaTime))
				{
					SkipProcess(pPerson,_curTime);
					return true;
				}
			}
		}
		else
		{
			eDeltaTime = flight->getDepTime() - _curTime;
			if (m_pRetailMisData->PaxSkipVacant(pPerson->getType(),getQueueLength(),eDeltaTime))
			{
				SkipProcess(pPerson,_curTime);
				return true;
			}
		}
	}	

	return false;	
}

//---------------------------------------------------------------------
//Summary:
//		check processor is valid passenger
//---------------------------------------------------------------------
bool RetailProcessor::isQueueLengthVacant( Person* pPerson) const
{
	if (m_pRetailMisData == NULL)
		return true;

	
	return m_pRetailMisData->PaxQueueLenghtVacant(pPerson->getType(),getQueueLength());
}

void RetailProcessor::initSpecificMisc(const MiscData *miscData )
{
	if (miscData && miscData->getType() == RetailProc)
	{
		m_pRetailMisData = (MiscRetailProcData*)miscData;
		m_pRetailMisData->SortSkipCondition();
	}

}

//-------------------------------------------------------------------
//Summary:
//		init retail processor log entry
//-------------------------------------------------------------------
void RetailProcessor::SetLogEntryOfRetail( const CRetailLogEntry& logEntry )
{
	m_retailLogEntry = logEntry;
}

void RetailProcessor::beginService( Person *aPerson, ElapsedTime curTime )
{
	Processor::beginService(aPerson,curTime);

	RetailEventStruct retailEvent;
	retailEvent.time = curTime;
	retailEvent.state = LOBYTE(LOWORD(RETAIL_BEGINSERVICE));
	retailEvent.paxID = aPerson->getID();
	retailEvent.price = 0l;
	retailEvent.itemTypeID = -1l;
	retailEvent.itemCount = 0l;
	retailEvent.airportCut = 0l;

	m_retailLogEntry.addEvent(retailEvent);

	//passenger has shopping properties
	if(aPerson == NULL || aPerson->getType().GetTypeIndex() != 0)
		return;

	Passenger* pPax = (Passenger*)aPerson;

	switch (m_emType)
	{
	case Retial_Shop_Section:
		ShoppingProcess(aPerson,curTime);
		break;
	case Retial_Check_Out:
		pPax->CheckOutShopping(this,curTime);
		break;
	case Retial_By_Pass:
		ByPassProcess(aPerson,curTime);
		break;
	default:
		break;
	}
}

void RetailProcessor::ShoppingProcess( Person *aPerson, ElapsedTime curTime )
{
	if (m_pRetailFeeData == NULL)
		return;
	
	std::vector<RetailFeeItem>& vData = m_pRetailFeeData->m_vList;
	for (size_t i = 0; i < vData.size(); i++)
	{
		const RetailFeeItem& feeData = vData.at(i);
		if (feeData.m_pPaxType && feeData.m_pPaxType->fits(aPerson->getType()))
		{
			ShoppingItemProcess(aPerson,curTime,feeData);
		}
	}
}

void RetailProcessor::ShoppingItemProcess( Person *aPerson, ElapsedTime curTime,const RetailFeeItem& feeData)
{
	//passenger has shopping properties
	if(aPerson == NULL || aPerson->getType().GetTypeIndex() != 0)
		return;

	Passenger* pPax = (Passenger*)aPerson;
	//check passenger have distribution to buy
	double dRate = random(100)/100.0;
	if (feeData.m_dProbBuy < dRate)
		return;
	
	std::map<int,int>::iterator iter = m_productMap.find(feeData.m_nProductID);
	int nCountValue = static_cast<int>(feeData.m_pUnitsDist->getRandomValue());
	RetailEventStruct retailEvent;
	retailEvent.time = curTime;
	retailEvent.paxID = aPerson->getID();
	retailEvent.airportCut = feeData.m_dAirportCut;
	retailEvent.itemTypeID = feeData.m_nProductID;
	retailEvent.price = feeData.m_dPrice;

	if (iter != m_productMap.end())
	{
		if ((*iter).second < nCountValue)
		{
			retailEvent.state = LOBYTE(LOWORD(RETAIL_SOLDOUT));
			retailEvent.price = feeData.m_dPrice;
			retailEvent.itemCount = (*iter).second;
			(*iter).second = 0;

			pPax->AddEntry(this,feeData.m_nProductID,retailEvent.itemCount);
		}
		else
		{
			retailEvent.state = LOBYTE(LOWORD(RETAIL_BUY));
			retailEvent.itemCount = nCountValue;
			(*iter).second -= nCountValue;

			pPax->AddEntry(this,feeData.m_nProductID,retailEvent.itemCount);
		}
		m_retailLogEntry.addEvent(retailEvent);
	}
}


void RetailProcessor::SkipProcess( Person *aPerson, ElapsedTime curTime )
{
	RetailEventStruct retailEvent;
	retailEvent.time = curTime;
	retailEvent.state = LOBYTE(LOWORD(RETAIL_SKIP));
	retailEvent.paxID = aPerson->getID();
	retailEvent.price = 0.0;
	retailEvent.itemTypeID = -1l;
	retailEvent.itemCount = 0l;
	retailEvent.airportCut = 0.0;

	m_retailLogEntry.addEvent(retailEvent);

	if(aPerson == NULL || aPerson->getType().GetTypeIndex() != 0)
		return;

	Passenger* pPax = (Passenger*)aPerson;

	pPax->CancelShopping(this,curTime);
}

void RetailProcessor::ByPassProcess( Person* aPerson,ElapsedTime curTime )
{
	RetailEventStruct retailEvent;
	retailEvent.time = curTime;
	retailEvent.state = LOBYTE(LOWORD(RETAIL_BYPASS));
	retailEvent.paxID = aPerson->getID();
	retailEvent.price = 0.0;
	retailEvent.itemTypeID = -1l;
	retailEvent.itemCount = 0l;
	retailEvent.airportCut = 0.0;

	m_retailLogEntry.addEvent(retailEvent);

	if(aPerson == NULL || aPerson->getType().GetTypeIndex() != 0)
		return;

	Passenger* pPax = (Passenger*)aPerson;

	pPax->CancelShopping(this,curTime);
}
//-------------------------------------------------------------------
//Summary:
//		init passenger shopping at shop processor
//-------------------------------------------------------------------
void RetailProcessor::InitShoppingRule()
{
	if(GetRetailType() != Retial_Shop_Section)
		return;

	//init passenger shopping rule
	if (m_pEngine == NULL)
		return;
	
	CARCPort* pArcport = m_pEngine->m_pARCport;
	if (pArcport == NULL)
		return;
	
	CEconomicManager& m_econManager = pArcport->getEconomicManager();
	CProcEconDatabase2* pRetailEconDatabase = m_econManager.m_pRetailPercRevenue;
	if (pRetailEconDatabase == NULL)
		return;
			
	CProcEconomicDataElement* pEconDataElement = (CProcEconomicDataElement*) pRetailEconDatabase->getEntryPoint(*getID());
	if (pEconDataElement == NULL)
		return;
	
	m_pRetailFeeData = (CRetailFeeData*)pEconDataElement->getData();
	if (m_pRetailFeeData)
	{
		m_pRetailFeeData->SortRetailFeeData();
	}
}

void RetailProcessor::InitShopStocking()
{
	if(GetRetailType() != Retial_Shop_Section)
		return;
	m_pTerm->m_pProductStockingList->RetrieveShopSectionProduct(m_productMap,*getID());
}

void RetailProcessor::FlushLog()
{
	if(!m_pTerm)return;

	long trackCount = m_retailLogEntry.getCurrentCount();
	RetailEventStruct *log = NULL;
	m_retailLogEntry.getLog (log);
	// write first member of group
	m_retailLogEntry.setEventList (log, trackCount);
	delete [] log;
	log = NULL;
	m_pTerm->m_pRetailLog->updateItem (m_retailLogEntry, m_retailLogEntry.GetIndex());
	m_retailLogEntry.clearLog();
}

bool RetailProcessor::GetRetailFeeData( int nProductID,RetailFeeItem& feeData ) const
{
	std::vector<RetailFeeItem>& vData = m_pRetailFeeData->m_vList;
	for (size_t i = 0; i < vData.size(); i++)
	{
		feeData = vData.at(i);
		if (feeData.m_nProductID == nProductID)
		{
			return true;
		}
	}
	return false;
}

void RetailProcessor::WriteLogCheckOutShopping( Person* aPerson,int nType,int nCount,RetailProcessor* pRetailProc, const ElapsedTime& curTime )
{
	if (pRetailProc == NULL)
		return;
	
	CRetailLogEntry& logEntry = pRetailProc->GetRetailLogEntry();
	RetailFeeItem feeItem;
	if(GetRetailFeeData(nType,feeItem))
	{
		//write check out entry log
		RetailEventStruct retailEvent;
		retailEvent.time = curTime;
		retailEvent.state = LOBYTE(LOWORD(RETAIL_CHECKOUT));
		retailEvent.paxID = aPerson->getID();
		retailEvent.price = feeItem.m_dPrice;
		retailEvent.itemTypeID =nType;
		retailEvent.itemCount = nCount;
		retailEvent.airportCut = feeItem.m_dAirportCut;

		logEntry.addEvent(retailEvent);
	}
}

void RetailProcessor::WriteLogCancelShopping( Person* aPerson,int nType,int nCount,RetailProcessor* pRetailProc, const ElapsedTime& curTime )
{
	if (pRetailProc == NULL)
		return;

	std::map<int,int>::iterator iter = m_productMap.begin();
	
	CRetailLogEntry& logEntry = pRetailProc->GetRetailLogEntry();
	iter = m_productMap.find(nType);
	if (iter != m_productMap.end())
	{
		RetailFeeItem feeItem;
		if(GetRetailFeeData(nType,feeItem))
		{
			(*iter).second += nCount;
			RetailEventStruct retailEvent;
			retailEvent.time = curTime;
			retailEvent.state = LOBYTE(LOWORD(RETAIL_CANCEL));
			retailEvent.paxID = aPerson->getID();
			retailEvent.price = feeItem.m_dPrice;
			retailEvent.itemTypeID = nType;
			retailEvent.itemCount = nCount;
			retailEvent.airportCut = feeItem.m_dAirportCut;

			logEntry.addEvent(retailEvent);
		}
	}
	
}

CRetailLogEntry& RetailProcessor::GetRetailLogEntry()
{
	return m_retailLogEntry;
}
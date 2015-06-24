#include "stdafx.h"

#include "AirsideNodeList.h"

#include"common\line.h"
#include"Engine\StandProc.h"
#include"Engine\RunwayProc.h"
#include"Engine\TaxiwayProc.h"

 
CAirsideNodeList::CAirsideNodeList()
{
	m_bDirty = true;
}
std::vector<CAirsideNode>& CAirsideNodeList::GetNodeList(ProcessorList* pProcList)
{
	ASSERT(pProcList != NULL);
	if(m_bDirty == true)
	{
		GenerateNodeList(pProcList);
		m_bDirty = false;
	}
	return m_vNodeList;
}
void CAirsideNodeList::GenerateNodeList(ProcessorList* pProcList)
{
	m_vNodeList.clear();

	//Genetate Intersections first;
	GenerateIntersections(pProcList);

	//Service Location of Runway & Taxiway
	int nCount = pProcList->getProcessorCount();

	for(int i=0; i<nCount; i++)
	{
		Processor *pProcessor = pProcList->getProcessor(i);
		int TYPE = pProcessor->getProcessorType();
		if( TYPE == RunwayProcessor )
		{
			Path *pPath = pProcessor->serviceLocationPath();;

			int nCount = pPath->getCount();
			ASSERT(nCount == 2);
			
			CString sLable;
			sLable.Format("%s",((RunwayProc*)pProcessor)->GetMarking1().c_str());		
			CAirsideNode m_airSideNode1(sLable,pPath->getPoint(0));
			m_vNodeList.push_back(m_airSideNode1);
			sLable.Empty();
			sLable.Format("%s",((RunwayProc*)pProcessor)->GetMarking2().c_str());
			CAirsideNode m_airSideNode2(sLable,pPath->getPoint(1));
			m_vNodeList.push_back(m_airSideNode2);
		}
		else if ( TYPE == TaxiwayProcessor)
		{
			Path *pPath = pProcessor->serviceLocationPath();
			int nCount = pPath->getCount();

			for (int i=0; i<nCount; i++)
			{
				CString sLable;
				sLable.Format("%s-%d",((TaxiwayProc*)pProcessor)->GetMarking().c_str(),i);
				CAirsideNode m_airsideNode(sLable, pPath->getPoint(i));

				m_vNodeList.push_back(m_airsideNode);
			}

		}
		else
			;
	}
}

void CAirsideNodeList::GenerateIntersections(ProcessorList* pProcList)
{
	int nCount = pProcList->getProcessorCount();

	for(int i=0; i<nCount; i++)
	{
		for(int j=i-1; j<nCount; j++)
		{
			if(j<0||i>=j)
				continue;
			Processor *pProc1 = pProcList->getProcessor(i);
			Processor *pProc2 = pProcList->getProcessor(j);
			//enum ProcessorClassList TYPE1,TYPE2;
			int TYPE1, TYPE2;
			TYPE1 = pProc1->getProcessorType();
			TYPE2 = pProc2->getProcessorType();
			/*			if(TYPE1==RunwayProcessor && TYPE2==RunwayProcessor)
			RunwayInterRunway(pProc1,pProc2);
			else */if(TYPE1==TaxiwayProcessor && TYPE2==TaxiwayProcessor)
			TaxiwayInterTaxiway(pProc1,pProc2);
			else if(TYPE1==RunwayProcessor && TYPE2==TaxiwayProcessor)
				RunwayInterTaxiway(pProc1,pProc2);
			else if(TYPE1==TaxiwayProcessor && TYPE2==RunwayProcessor)
				RunwayInterTaxiway(pProc1,pProc2, true);
			else if(TYPE1==StandProcessor && TYPE2==TaxiwayProcessor)
				StandInterTaxiway(pProc1,pProc2);
			else if(TYPE1==TaxiwayProcessor && TYPE2==StandProcessor)
				StandInterTaxiway(pProc1,pProc2, true);
			else
				;
		}
		//
	}
}


/*
//Impossibility
void CAirsideNodeList::RunwayInterRunway(Processor *p1, Processor *p2)
{

}
*/

void CAirsideNodeList::RunwayInterTaxiway(Processor *p1, Processor *p2, bool bExchange)
{
	if(bExchange)
	{
		Processor *tmpProces = p1;
		p1 = p2;
		p2 = tmpProces;
	}
	//p1:Runway,p2:Taxiway

	Path *pPath1 = p1->serviceLocationPath();
	Path *pPath2 = p2->serviceLocationPath();

	ASSERT(pPath1->getCount() == 2);

	Line lnRunway(pPath1->getPoint(0),pPath1->getPoint(1));
	int nCount  = pPath2->getCount();

	for(int i=0; i<nCount-1; i++)
	{
		Line  curLine(pPath2->getPoint(i), pPath2->getPoint(i+1));
		if(lnRunway.intersects(curLine))
		{
			Point interPt = lnRunway.intersection(curLine);
			//Distance...
			CString sLable;
			if (pPath1->getPoint(0) == lnRunway.nearest(interPt))
				sLable.Format("%s&%s%d",((RunwayProc*)p1)->GetMarking1().c_str(),((TaxiwayProc*)p2)->GetMarking().c_str(),i);
			else
				sLable.Format("%s&%s%d",((RunwayProc*)p1)->GetMarking2().c_str(),((TaxiwayProc*)p2)->GetMarking().c_str(),i);
			CAirsideNode m_airSideNode(sLable, interPt);
			m_vNodeList.push_back(m_airSideNode);
		}
	}
}

void CAirsideNodeList::StandInterTaxiway(Processor *p1, Processor *p2, bool bExchange)
{
	if(bExchange)
	{
		Processor *tmpProces = p1;
		p1 = p2;
		p2 = tmpProces;
	}
	//p1:Stand,P2:taxiway
	StandProc *pStandProc = (StandProc*)p1;
	if(pStandProc->IsUsePushBack())
	{
		Path *pPath1 =pStandProc->getPushBackWay() ;
		Path *pPath2 =p2->serviceLocationPath() ;

		ASSERT(pPath1->getCount() ==2 );

		Line lnStand(pPath1->getPoint(0),pPath1->getPoint(1));
		int nCount  = pPath2->getCount();

		for(int i=0; i<nCount-1; i++)
		{
			Line  curLine(pPath2->getPoint(i), pPath2->getPoint(i+1));
			if(lnStand.intersects(curLine))
			{
				Point interPt = lnStand.intersection(curLine);
				CString sLable;
				sLable.Format("%s&%s",(StandProc*)p1->getProcessorName(),((TaxiwayProc*)p2)->GetMarking().c_str());
				CAirsideNode m_airSideNode(sLable, interPt);
				m_vNodeList.push_back(m_airSideNode);
			}
		}
	}
}

void CAirsideNodeList::TaxiwayInterTaxiway(Processor *p1, Processor *p2)
{
	Path *pPath1 = p1->serviceLocationPath();
	Path *pPath2 = p2->serviceLocationPath();
	int nCount1 = pPath1->getCount();
	int nCount2 = pPath2->getCount();

	for(int i=0; i<nCount1-1; i++)
	{
		Line L1(pPath1->getPoint(i),pPath1->getPoint(i+1));
		for(int j=0; j<nCount2-1; j++)
		{
			Line L2(pPath2->getPoint(j),pPath2->getPoint(j+1));
			if(L1.intersects(L2))
			{
				Point interPt = L1.intersection(L2);
				CString sLable;
				sLable.Format("%s&%s",((TaxiwayProc*)p1)->GetMarking().c_str(),((TaxiwayProc*)p2)->GetMarking().c_str());
				CAirsideNode m_airSideNode(sLable, interPt);
				m_vNodeList.push_back(m_airSideNode);
			}
		}
	}
}


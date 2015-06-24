#include "StdAfx.h"
#include ".\retailreport.h"
#include "..\Engine\proclist.h"
#include "..\Results\RetailLog.h"
#include "StatisticalTools.h"
#include "..\engine\terminal.h"
#include "..\Inputs\ProductStockingList.h"
CRetailReport::CRetailReport(Terminal* _pTerm, const CString& _csProjPath)
:CBaseReport(_pTerm, _csProjPath)
{
}

CRetailReport::~CRetailReport(void)
{
}


void CRetailReport::GenerateDetailed( ArctermFile& p_file )
{
	CRetailLogEntry retailEntry;
	retailEntry.SetOutputTerminal((OutputTerminal*)m_pTerm);
	retailEntry.SetEventLog(m_pTerm->m_pRetailEventLog);

	long nCount = m_pTerm->m_pRetailLog->getCount();
	for (long i = 0; i < nCount; i++)
	{
		m_pTerm->m_pRetailLog->getItem(retailEntry,i);
		InitRetailData(retailEntry);
	}
	
	GenerateResult();
	writeEntry(p_file);
}

void CRetailReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );

	m_usesInterval=1;
	BuildProcessorMap();
}

void CRetailReport::BuildProcessorMap()
{
	m_processorMap.clear();
	int nCount = (int)m_procIDlist.size();
	for (int i = 0; i < nCount; i++)
	{
		ProcessorID procID = *(m_procIDlist.getID(i));

		ProcessorMap procMap;
		procMap.m_storeElement.InitStoreElement(procID,this);

		GroupIndex groupIndex = m_pTerm->procList->getGroupIndex(procID);
		for (int j = groupIndex.start; j <= groupIndex.end; j++)
		{
			Processor* pProcessor = m_pTerm->procList->getProcessor(j);
			if (pProcessor && pProcessor->getProcessorType() == RetailProc)
			{
				procMap.m_processorMap.push_back(j);
			}
		}

		m_processorMap.push_back(procMap);
	}
}

bool CRetailReport::GetRetailElement( long nPaxID,RetailPaxElement& retailElement )
{
	retailElement.initReport(this);
	retailElement.SetOutputTerminal( m_pTerm );
	retailElement.SetEventLog( m_pTerm->m_pMobEventLog );
	if(m_pTerm->paxLog->GetItemByID(retailElement,nPaxID))
	{
		retailElement.clearLog();
		if( retailElement.alive( m_startTime, m_endTime ) )
			if( retailElement.fits( m_multiConst ) )
				return true;
	}
	return false;
}

void CRetailReport::GenerateResult( )
{
	//generate result data
	int nCount = (int)m_processorMap.size();
	for (int i = 0; i < nCount; i++)
	{
		ProcessorMap& procMap = m_processorMap.at(i);
		procMap.m_storeElement.CalculateRetailShopData();
	}
}

void CRetailReport::GenerateSummary(ArctermFile& p_file )
{
	p_file.writeField ("Interval,Store...");
	p_file.writeLine();

	if (m_interval.getPrecisely() == 0)
		return;
	
	CRetailLogEntry retailEntry;
	retailEntry.SetOutputTerminal((OutputTerminal*)m_pTerm);
	retailEntry.SetEventLog(m_pTerm->m_pRetailEventLog);

	long nRetailCount = m_pTerm->m_pRetailLog->getCount();
	for (long nRet = 0; nRet < nRetailCount; nRet++)
	{
		m_pTerm->m_pRetailLog->getItem(retailEntry,nRet);
		InitRetailData(retailEntry);
	}

	int nCount = (int)m_processorMap.size();
	ElapsedTime totalTime;
	int intervalSize = 0;
	totalTime = m_endTime - m_startTime;
	intervalSize = (int)(totalTime / m_interval) + 1;

	CProgressBar bar( "Generating Retail Activity Report", 100, nCount*intervalSize, TRUE );
	try
	{
		for (long i = 0; i < nCount; i++)
		{
			ProcessorMap& procMap = m_processorMap.at(i);
			ElapsedTime eTime = m_startTime;
				
			for (long j = 0 ; j < intervalSize ;j++)
			{
				bar.StepIt();


				ElapsedTime startTime = eTime + m_interval * j;
				ElapsedTime endTime = startTime + m_interval;
				CStatisticalTools<double> tempTools;
				double dApt = 0.0;
				procMap.m_storeElement.CalculateRetailShopSummaryData(startTime,endTime,tempTools,dApt);
				p_file.writeInt(j);//0
				p_file.writeField (procMap.m_storeElement.GetIDString().GetString());//1
				p_file.writeFloat (static_cast<float>(tempTools.GetMin()), TRUE);//2
				p_file.writeFloat (static_cast<float>(tempTools.GetAvarage()), TRUE);//3
				p_file.writeFloat (static_cast<float>(tempTools.GetMax()), TRUE);//4

				p_file.writeFloat( static_cast<float>(tempTools.GetPercentile( 25 )),  TRUE );//Q1 //5
				p_file.writeFloat( static_cast<float>(tempTools.GetPercentile( 50 )), TRUE );//Q2 //6
				p_file.writeFloat( static_cast<float>(tempTools.GetPercentile( 75 )), TRUE );//Q3 //7

				p_file.writeFloat( static_cast<float>(tempTools.GetPercentile( 1 )), TRUE );//P1 //8
				p_file.writeFloat( static_cast<float>(tempTools.GetPercentile( 5 )), TRUE );//P5 //9
				p_file.writeFloat( static_cast<float>(tempTools.GetPercentile( 10 )), TRUE );//P10 //10

				p_file.writeFloat( static_cast<float>(tempTools.GetPercentile( 90 )), TRUE );//P90 //11
				p_file.writeFloat( static_cast<float>(tempTools.GetPercentile( 95 )), TRUE );//P95 //12
				p_file.writeFloat( static_cast<float>(tempTools.GetPercentile( 99 )), TRUE );//P99 //13

				p_file.writeFloat( static_cast<float>(tempTools.GetSigma()), TRUE );//Sigma //14
				p_file.writeFloat(static_cast<float>(dApt), TRUE); //15

				p_file.writeLine();
			}
			
		}
	}
	catch(ARCInvalidTimeError* timeError)
	{
		char msg[256];
		timeError->getMessage(msg);
		CString strMsg(msg);
		AfxMessageBox(msg);
		delete timeError;
	}
	catch(...)
	{
		throw;
	}
}

void CRetailReport::InitRetailData( const CRetailLogEntry& retailEntry )
{
	int nCount = (int)m_processorMap.size();
	for (int i = 0; i < nCount; i++)
	{
		ProcessorMap& procMap = m_processorMap.at(i);
		if (std::find(procMap.m_processorMap.begin(),procMap.m_processorMap.end(),retailEntry.GetProcIndex()) != procMap.m_processorMap.end())
		{
			procMap.m_storeElement.AddLogEntry(retailEntry);	
		}
	}
}

void CRetailReport::writeEntry( ArctermFile& p_file )
{
	p_file.writeField ("Passenger ID,Shop,Store...");
	p_file.writeLine();
	int nCount = (int)m_processorMap.size();
	for (int i = 0; i < nCount; i++)
	{
		ProcessorMap& procMap = m_processorMap.at(i);
		procMap.m_storeElement.writeEntry(p_file);
	}
}

bool CRetailReport::GetProductName( int nProductID,CString& sProductName ) const
{
	ASSERT(m_pTerm);
	return m_pTerm->m_pProductStockingList->GetProductName(nProductID,sProductName);

}
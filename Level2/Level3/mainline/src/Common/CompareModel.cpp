#include "StdAfx.h"
#include ".\comparemodel.h"
#include "Common/termfile.h"
#include "Common/exeption.h"
#include "Common/SimAndReportManager.h"
#include "MultiRunReport.h"
#include "Main/MultiRunsReportDataLoader.h"
#include "fileman.h"
#include "Reports/ReportParameter.h"
#include "reports/MutiRunReportAgent.h"
#include "ProgressBar.h"
#include "../engine/terminal.h"
#include "HeapObj.h"
CCompareModel::CCompareModel(void)
:m_bIsValid(true)
{
}

CCompareModel::~CCompareModel(void)
{
}
const CString& CCompareModel::GetModelName() const
{
	return m_strModelName;
}
void CCompareModel::SetUniqueName(const CString& strName)
{
	m_strUniqueName = strName;

}
const CString& CCompareModel::GetUniqueName() const
{
	return m_strUniqueName;
}
void CCompareModel::SetModelName(const CString& strName)
{
	m_strModelName = strName;
}

const CString& CCompareModel::GetModelLocation() const
{
	return m_strModelLocation;
}

void CCompareModel::SetModelLocation(const CString& strLocation)
{
	m_strModelLocation = strLocation;
}
const CString& CCompareModel::GetLocalPath() const
{
	return m_strLocalPath;
}

void CCompareModel::SetLocalPath(const CString& strLocalPath)
{
	m_strLocalPath = strLocalPath;
}
const CString& CCompareModel::GetDataBasePath() const
{
	return m_strDatabasePath;
}

void CCompareModel::SetDataBasePath(const CString& strDataBasePath)
{
	m_strDatabasePath = strDataBasePath;
}

const CString& CCompareModel::GetLastModifyTime() const
{
	return m_lastModifiedTime;
}

void CCompareModel::SetLastModifyTime(const CString& strLastTime)
{
	m_lastModifiedTime = strLastTime;
}
void CCompareModel::AddSimResult(const CString& strSimResult)
{
	m_vSimResult.push_back(strSimResult);
}
int CCompareModel::GetSimResultCount() const
{
	return static_cast<int>(m_vSimResult.size());
}
CString CCompareModel::GetSimResult(int nIndex) const
{
	ASSERT(nIndex >=0 && nIndex < GetSimResultCount());
	return m_vSimResult[nIndex];

}
void CCompareModel::ClearSimResult()
{
	m_vSimResult.clear();
}
void CCompareModel::readData(ArctermFile& p_file)
{
		char buf[256];
	// read the Model name;
	memset(buf, 0, sizeof(buf) / sizeof(char));
	if (p_file.getField(buf, 255) != 0)
		SetModelName(buf);

	int nSimResultCount;
	p_file.getInteger(nSimResultCount);
	for (int i =0; i < nSimResultCount; i++)
	{
		p_file.getField(buf,255);
		AddSimResult(buf);
	}

	// read the unique name;
	memset(buf, 0, sizeof(buf) / sizeof(char));
	if (p_file.getField(buf, 255) != 0)
		SetUniqueName(buf);

	// read the model location
	memset(buf,0,sizeof(buf) / sizeof(char));
	if (p_file.getField(buf, 255) != 0)
		SetModelLocation(buf);

	//read the local path
	memset(buf, 0, sizeof(buf) / sizeof(char));
	if (p_file.getField(buf, 255) != 0)
		SetLocalPath(buf);

	//read the database path
	memset(buf, 0, sizeof(buf) / sizeof(char));
	if (p_file.getField(buf, 255) != 0)
		SetDataBasePath(buf);

	//read the last modify time
	memset(buf, 0, sizeof(buf) / sizeof(char));
	if (p_file.getField(buf, 255) != 0)
		SetLastModifyTime(buf);
}
void CCompareModel::writeData(ArctermFile& p_file) const
{
	p_file.writeField(GetModelName());//write the model name

	int nSimResultCount = GetSimResultCount();
	p_file.writeInt(nSimResultCount);
	for (int j = 0; j < nSimResultCount; ++j)
	{
		p_file.writeField(GetSimResult(j));
	}

	p_file.writeField(GetUniqueName());//write the unique name
	p_file.writeField(GetModelLocation());//write the network path
	p_file.writeField(GetLocalPath());//write the local path
	p_file.writeField(GetDataBasePath());//write the database path
	p_file.writeField(GetLastModifyTime());//write the last modify time
	p_file.writeLine();
}
void CCompareModel::GenerateReport(ENUM_REPORT_TYPE enumRepType,int nDetailOrSummary,void *pReportResult,CReportParameter *pReportParam,const CString& strProjPath, Terminal *pTerminal, int nFloorCount)
{
	CString strModelPath = GetModelLocation();
	if(!FileManager::IsDirectory(strModelPath))
	{
		CString strMessage;
		strMessage.Format(_T("Project %s doesn't exists."),GetModelName());
		MessageBox(NULL,strMessage,_T("Multiply Run Report"),MB_OK);
		m_bIsValid = false;
		return;
	}

	//check the model exists
	
	CSimAndReportManager* pReportManager = pTerminal->GetSimReportManager();//new CSimAndReportManager();
	//pReportManager->loadDataSet(GetModelLocation());
	int nSimResultCountExists = pReportManager->getSubSimResultCout();
	if (nDetailOrSummary == 0)
	{
		switch(enumRepType)
		{
		case ENUM_DISTANCE_REP:

			{
				MultiRunsReport::Detail::PaxDataList* paxDataList = (MultiRunsReport::Detail::PaxDataList *)pReportResult;
				
				CProgressBar bar( "Generating multiple run report", 100, GetSimResultCount(), TRUE ,2);

				int nRangeCount = 0;
				for (int i = 0; i < GetSimResultCount(); ++i)
				{
					int nCurSimResult = atoi(m_vSimResult[i].Mid(3,m_vSimResult[i].GetLength()));
					if (nCurSimResult >= nSimResultCountExists)
					{
						m_vSimResult.erase(m_vSimResult.begin() + i,m_vSimResult.end());
						break;
					}
					
					pReportManager->SetCurrentSimResult(nCurSimResult);
					pReportManager->SetCurrentReportType(enumRepType);


					CMutiRunReportAgent temp;
					temp.Init(strProjPath,pTerminal);
					temp.AddReportWhatToGen( pReportParam->GetReportCategory(), pReportParam ,nFloorCount);								
					temp.GenerateAll();

					CString strReportFileName = pReportManager->GetCurrentReportFileName(GetModelLocation());
					long interval = 0 ;
					pReportParam->GetInterval(interval) ;
					MultiRunsReport::Detail::DistTravelledDataLoader loader(strReportFileName,interval);
					loader.LoadData();
					MultiRunsReport::Detail::DistTravelledDataLoader::DataList& dataList = loader.GetData();
					paxDataList->push_back(dataList);
					
					bar.StepIt();

				}

			}

			break;
		case ENUM_QUEUETIME_REP:
			{

				MultiRunsReport::Detail::TimeDataList *timeDataList = (MultiRunsReport::Detail::TimeDataList *)pReportResult;

				CProgressBar bar( "Generating multiple run report", 100, GetSimResultCount(), TRUE ,2);
				int nRangeCount = 0;
				for (int i = 0; i < GetSimResultCount(); ++i)
				{
					int nCurSimResult = atoi(m_vSimResult[i].Mid(3,m_vSimResult[i].GetLength()));
					if (nCurSimResult >= nSimResultCountExists)
					{
						m_vSimResult.erase(m_vSimResult.begin() + i,m_vSimResult.end());
						break;
					}

					pReportManager->SetCurrentSimResult(nCurSimResult);
					pReportManager->SetCurrentReportType(enumRepType);


					CMutiRunReportAgent temp;
					temp.Init(strProjPath,pTerminal);
					temp.AddReportWhatToGen( pReportParam->GetReportCategory(), pReportParam ,nFloorCount);								
					temp.GenerateAll();

                    long interval ;
					pReportParam->GetInterval(interval) ;
					CString strReportFileName = pReportManager->GetCurrentReportFileName(GetModelLocation());
					MultiRunsReport::Detail::TimeInTerminalDataLoader loader(strReportFileName,interval);
					loader.LoadData();
					MultiRunsReport::Detail::TimeInTerminalDataLoader::DataList& dataList = loader.GetData();
					timeDataList->push_back(dataList);

					bar.StepIt();
				}


			}
			break;
		case ENUM_SERVICETIME_REP:
			{

				MultiRunsReport::Detail::TimeDataList *timeDataList = (MultiRunsReport::Detail::TimeDataList *)pReportResult;

				CProgressBar bar( "Generating multiple run report", 100, GetSimResultCount(), TRUE ,2);
				int nRangeCount = 0;
				for (int i = 0; i < GetSimResultCount(); ++i)
				{
					int nCurSimResult = atoi(m_vSimResult[i].Mid(3,m_vSimResult[i].GetLength()));
					if (nCurSimResult >= nSimResultCountExists)
					{
						m_vSimResult.erase(m_vSimResult.begin() + i,m_vSimResult.end());
						break;
					}
					pReportManager->SetCurrentSimResult(nCurSimResult);
					pReportManager->SetCurrentReportType(enumRepType);

					CMutiRunReportAgent temp;
					temp.Init(strProjPath,pTerminal);
					temp.AddReportWhatToGen( pReportParam->GetReportCategory(), pReportParam ,nFloorCount);								
					temp.GenerateAll();

					long interval ;
					pReportParam->GetInterval(interval) ;
					CString strReportFileName = pReportManager->GetCurrentReportFileName(GetModelLocation());
					MultiRunsReport::Detail::TimeInTerminalDataLoader loader(strReportFileName,interval);
					loader.LoadData();
					MultiRunsReport::Detail::TimeInTerminalDataLoader::DataList& dataList = loader.GetData();
					timeDataList->push_back(dataList);

					bar.StepIt();
				}


			}
			break;
		case ENUM_DURATION_REP:
			{

				MultiRunsReport::Detail::TimeDataList* timeDataList = (MultiRunsReport::Detail::TimeDataList*)pReportResult;
				CProgressBar bar( "Generating multiple run report", 100, GetSimResultCount(), TRUE ,2);
				int nRangeCount = 0;
				for (int i = 0; i < GetSimResultCount(); ++i)
				{
					int nCurSimResult = atoi(m_vSimResult[i].Mid(3,m_vSimResult[i].GetLength()));
					if (nCurSimResult >= nSimResultCountExists)
						{
						m_vSimResult.erase(m_vSimResult.begin() + i,m_vSimResult.end());
						break;
					}
					pReportManager->SetCurrentSimResult(nCurSimResult);
					pReportManager->SetCurrentReportType(enumRepType);

					CMutiRunReportAgent temp;
					temp.Init(strProjPath,pTerminal);
					temp.AddReportWhatToGen( pReportParam->GetReportCategory(), pReportParam ,nFloorCount);								
					temp.GenerateAll();

					long interval ;
					pReportParam->GetInterval(interval) ;

					CString strReportFileName = pReportManager->GetCurrentReportFileName(GetModelLocation());
					MultiRunsReport::Detail::TimeInTerminalDataLoader loader(strReportFileName,interval);
					loader.LoadData();
					MultiRunsReport::Detail::TimeInTerminalDataLoader::DataList& dataList = loader.GetData();
					timeDataList->push_back(dataList);

					bar.StepIt();
				}
			}

			break;
		case ENUM_ACTIVEBKDOWN_REP:
			break;
		case ENUM_PAXCOUNT_REP:
			{
				MultiRunsReport::Detail::OccupnacyDataList* timeDataList = (MultiRunsReport::Detail::OccupnacyDataList*)pReportResult;
				CProgressBar bar( "Generating multiple run report", 100, GetSimResultCount(), TRUE ,2);
				int nRangeCount = 0;
				for (int i = 0; i < GetSimResultCount(); ++i)
				{		
					int nCurSimResult = atoi(m_vSimResult[i].Mid(3,m_vSimResult[i].GetLength()));
					if (nCurSimResult >= nSimResultCountExists)
					{
						m_vSimResult.erase(m_vSimResult.begin() + i,m_vSimResult.end());
						break;
					}

					pReportManager->SetCurrentSimResult(nCurSimResult);
					pReportManager->SetCurrentReportType(enumRepType);

					CMutiRunReportAgent temp;
					temp.Init(strProjPath,pTerminal);
					temp.AddReportWhatToGen( pReportParam->GetReportCategory(), pReportParam ,nFloorCount);								
					temp.GenerateAll();

					long interval ;
					pReportParam->GetInterval(interval) ;
					CString strReportFileName = pReportManager->GetCurrentReportFileName(GetModelLocation());
					MultiRunsReport::Detail::OccupancyDataLoader loader(strReportFileName,interval);
					loader.LoadData();
					MultiRunsReport::Detail::OccupancyDataLoader::DataList & dataList = loader.GetData();
					timeDataList->push_back(dataList);

					bar.StepIt();
				}
			}
			break;
		case ENUM_PAXDENS_REP:
			{
				MultiRunsReport::Detail::OccupnacyDataList *timeDataList = (MultiRunsReport::Detail::OccupnacyDataList *)pReportResult;
				CProgressBar bar( "Generating multiple run report", 100, GetSimResultCount(), TRUE ,2);
				int nRangeCount = 0;
				for (int i = 0; i < GetSimResultCount(); ++i)
				{
					int nCurSimResult = atoi(m_vSimResult[i].Mid(3,m_vSimResult[i].GetLength()));
					if (nCurSimResult >= nSimResultCountExists)
					{
						m_vSimResult.erase(m_vSimResult.begin() + i,m_vSimResult.end());
						break;
					}
					pReportManager->SetCurrentSimResult(nCurSimResult);
					pReportManager->SetCurrentReportType(enumRepType);

					CMutiRunReportAgent temp;
					temp.Init(strProjPath,pTerminal);
					temp.AddReportWhatToGen( pReportParam->GetReportCategory(), pReportParam ,nFloorCount);								
					temp.GenerateAll();

					long interval ;
					pReportParam->GetInterval(interval) ;
					CString strReportFileName = pReportManager->GetCurrentReportFileName(GetModelLocation());
					MultiRunsReport::Detail::CPaxDenistyDataLoader loader(strReportFileName,interval);
					loader.LoadData();
					MultiRunsReport::Detail::CPaxDenistyDataLoader::DataList& dataList = loader.GetData();
					timeDataList->push_back(dataList);

					bar.StepIt();
				}
			}
			break;
		default:
			break;
		}
	}
	else
	{
		switch(enumRepType)
		{
		case ENUM_QUEUETIME_REP:
			{
				MultiRunsReport::Summary::SummaryQTimeList * pSummaryQTimeList = (MultiRunsReport::Summary::SummaryQTimeList *)pReportResult;

				CProgressBar bar( "Generating multiple run report", 100, GetSimResultCount(), TRUE ,2);

				int nRangeCount = 0;
				for (int i = 0; i < GetSimResultCount(); ++i)
				{
					int nCurSimResult = atoi(m_vSimResult[i].Mid(3,m_vSimResult[i].GetLength()));
					if (nCurSimResult >= nSimResultCountExists)
					{
						m_vSimResult.erase(m_vSimResult.begin() + i,m_vSimResult.end());
						break;
					}

					pReportManager->SetCurrentSimResult(nCurSimResult);
					pReportManager->SetCurrentReportType(enumRepType);

					CMutiRunReportAgent temp;
					temp.Init(strProjPath,pTerminal);
					temp.AddReportWhatToGen( pReportParam->GetReportCategory(), pReportParam ,nFloorCount);								
					temp.GenerateAll();


					CString strReportFileName = pReportManager->GetCurrentReportFileName(GetModelLocation());
					MultiRunsReport::Summary::SummaryQTimeLoader loader(strReportFileName);
					loader.LoadData();
					MultiRunsReport::Summary::SummaryQTimeLoader::DataList& data = loader.GetData();
					pSummaryQTimeList->push_back(data);

					bar.StepIt();
				}
			}
			break;

		case ENUM_AVGQUEUELENGTH_REP:
			{
				MultiRunsReport::Summary::SummaryCriticalQueueList * pSummaryCriticalTimeList = (MultiRunsReport::Summary::SummaryCriticalQueueList *)pReportResult;

				CProgressBar bar( "Generating multiple run report", 100, GetSimResultCount(), TRUE ,2);

				int nRangeCount = 0;
				for (int i = 0; i < GetSimResultCount(); ++i)
				{
					int nCurSimResult = atoi(m_vSimResult[i].Mid(3,m_vSimResult[i].GetLength()));
					if (nCurSimResult >= nSimResultCountExists)
					{
						m_vSimResult.erase(m_vSimResult.begin() + i,m_vSimResult.end());
						break;
					}

					pReportManager->SetCurrentSimResult(nCurSimResult);
					pReportManager->SetCurrentReportType(enumRepType);

					CMutiRunReportAgent temp;
					temp.Init(strProjPath,pTerminal);
					temp.AddReportWhatToGen( pReportParam->GetReportCategory(), pReportParam ,nFloorCount);								
					temp.GenerateAll();

					CString strReportFileName = pReportManager->GetCurrentReportFileName(GetModelLocation());
					MultiRunsReport::Summary::SummaryCriticalTimeLoader loader(strReportFileName);
					loader.LoadData();
					MultiRunsReport::Summary::SummaryCriticalTimeLoader::DataList & data = loader.GetData();
					pSummaryCriticalTimeList->push_back(data);
					bar.StepIt();
				}
			}
			break;

		case ENUM_SERVICETIME_REP:
			{
				MultiRunsReport::Summary::SummaryTimeServiceList * pSummaryQTimeList = (MultiRunsReport::Summary::SummaryTimeServiceList *)pReportResult;

				CProgressBar bar( "Generating multiple run report", 100, GetSimResultCount(), TRUE ,2);

				int nRangeCount = 0;
				for (int i = 0; i < GetSimResultCount(); ++i)
				{
					int nCurSimResult = atoi(m_vSimResult[i].Mid(3,m_vSimResult[i].GetLength()));
					if (nCurSimResult >= nSimResultCountExists)
					{
						m_vSimResult.erase(m_vSimResult.begin() + i,m_vSimResult.end());
						break;
					}

					pReportManager->SetCurrentSimResult(nCurSimResult);
					pReportManager->SetCurrentReportType(enumRepType);
					CMutiRunReportAgent temp;
					temp.Init(strProjPath,pTerminal);
					temp.AddReportWhatToGen( pReportParam->GetReportCategory(), pReportParam ,nFloorCount);								
					temp.GenerateAll();

					CString strReportFileName = pReportManager->GetCurrentReportFileName(GetModelLocation());
					MultiRunsReport::Summary::SummaryTimeServiceLoader loader(strReportFileName);
					loader.LoadData();
					MultiRunsReport::Summary::SummaryTimeServiceLoader::DataList& data = loader.GetData();
					pSummaryQTimeList->push_back(data);
				}

				bar.StepIt();
			}

		default:
			break;
		}
	}

}

//////////////////////////////////////////////////////////////////////////
//CCompareModelList
CCompareModelList::CCompareModelList()
:m_fVersion(2.2f)
{

}
CCompareModelList::~CCompareModelList()
{

}
int CCompareModelList::GetCount()
{
	return static_cast<int>(m_vCompareModel.size());
}
void CCompareModelList::AddModel(const CCompareModel& compareModel)
{
	m_vCompareModel.push_back(compareModel);
}
void CCompareModelList::DelModel(int nIndex)
{
	if (nIndex < GetCount())
	{
		m_vCompareModel.erase(m_vCompareModel.begin()+nIndex);
		return;
	}

	ASSERT(FALSE);
}

void CCompareModelList::Clear()
{
	m_vCompareModel.clear();
}
void CCompareModelList::ReadData(const CString& strFile)
{
	ArctermFile file;//HEAPOBJ(ArctermFile, file, ArctermFile());
	try { file.openFile (strFile, READ); }
	catch (FileOpenError *exception)
	{
		delete exception;
		//initDefaultValues();
		SaveData(strFile);
		return;
	}

	float fVersionInFile = file.getVersion();

	if( fVersionInFile < m_fVersion || fVersionInFile == 21 )
	{
		//readObsoleteData( file );
		//file.closeIn();
		//saveDataSet(_pProjPath, false);
	}
	else if( fVersionInFile > m_fVersion )
	{
		// should stop read the file.
		//file.closeIn();
		//throw new FileVersionTooNewError( filename );		
	}
	else
	{
		readData (file);
		file.closeIn();
	}

}
void CCompareModelList::readData(ArctermFile& p_file)
{


	m_vCompareModel.clear();


	while (p_file.getLine() == 1)
	{
		CCompareModel model;
		model.readData(p_file);

	

		m_vCompareModel.push_back(model);
	}
}
void CCompareModelList::writeData(ArctermFile& p_file) const
{
	for (unsigned i = 0; i < m_vCompareModel.size(); i++)
	{
		m_vCompareModel[i].writeData(p_file);

	}

	if (m_vCompareModel.empty())
		p_file.writeLine();
}
void CCompareModelList::SaveData(const CString& strFile)
{
	ArctermFile file;//HEAPOBJ(ArctermFile, file,ArctermFile());
	file.openFile (strFile, WRITE, m_fVersion);

	file.writeField (getTitle());
	file.writeLine();

	file.writeField (getHeaders());
	file.writeLine();

	writeData (file);

	file.endFile();

}
CCompareModel& CCompareModelList::GetModel(int nIndex)
{
	ASSERT(nIndex >=0 &&nIndex < GetCount());
	return m_vCompareModel.at(nIndex);

}
void CCompareModelList::SetModelSelectedSimResult(int nIndex,std::vector<CString>& vSimResilt)
{
	if (nIndex >=0 &&nIndex < GetCount())
	{
		 m_vCompareModel.at(nIndex).ClearSimResult();
		 for (int i =0; i < static_cast<int>(vSimResilt.size());++i)
		 {
			 m_vCompareModel.at(nIndex).AddSimResult(vSimResilt[i]);
		 }
		
		 return ;
	}

	ASSERT(FALSE);
}
void CCompareModelList::DelModel(const CString& strModelName)
{
	int nCount = GetCount();
	for (int i =0; i < nCount; ++ i )
	{
		if (m_vCompareModel[i].GetModelName() == strModelName)
		{
			m_vCompareModel.erase(m_vCompareModel.begin()+i);
		}
	}

}
void CCompareModelList::GenerateReport(ENUM_REPORT_TYPE enumRepType,int nDetailOrSummary,void *pReportResult,CReportParameter *pReportParam,const CString& strProjPath, Terminal *pTerminal, int nFloorCount)
{
	int nCount = GetCount();
	for (int i =0; i < nCount; ++ i )
	{
		m_vCompareModel[i].GenerateReport(enumRepType,nDetailOrSummary,pReportResult,pReportParam,strProjPath,pTerminal,nFloorCount);
	}

}









#include "StdAfx.h"
#include ".\aodbbasedataitem.h"
#include "AODBConvertParameter.h"
#include "IN_TERM.H"
#include "../Engine/proclist.h"
#include "AODBFieldMapFile.h"
#include "Engine\Gate.h"


using namespace AODB;

BaseDataItem::BaseDataItem(void)
{
	m_bSuccess = false;
}

BaseDataItem::~BaseDataItem(void)
{
}

CString AODB::BaseDataItem::GetString( CSVFile& csvFile ) const
{
	char chField[1024] = {0};
	csvFile.getField(chField,1024);

	return CString(chField);
}

bool AODB::BaseDataItem::IsSuccess() const
{
	return m_bSuccess;
}

void AODB::BaseDataItem::Success(bool bSucess)
{
	m_bSuccess = bSucess;
}
//////////////////////////////////////////////////////////////////////////
//AODB::MapField
AODB::MapField::MapField()
{
	m_bValid = false;
}

AODB::MapField::~MapField()
{

}

void AODB::MapField::GetValue( CSVFile& csvFile )
{
	csvFile.getField(m_strOrg.GetBuffer(1024),1024);
	m_strOrg.ReleaseBuffer();
}

bool AODB::MapField::IsValid() const
{
	return m_bValid;
}

void AODB::MapField::SetValue(const CString& strValue, ConvertParameter& pParameter )
{
	m_strOrg = strValue;
}

CString AODB::MapField::GetMapValue() const
{
	return m_strMap;
}

CString AODB::MapField::GetErroDesc()
{
	return m_strMessage;
}

void AODB::MapField::setValid( bool bValid )
{
	m_bValid = bValid;
}

CString AODB::MapField::GetOriginal()const
{
	return m_strOrg;
}

bool AODB::MapField::IsEmpty() const
{
	return m_strOrg.IsEmpty();
}

//////////////////////////////////////////////////////////////////////////
//AODB::ProcMapField
AODB::ProcMapField::ProcMapField()
{

}

AODB::ProcMapField::~ProcMapField()
{

}

CString AODB::ProcMapField::GetMapValue()const
{
	if (m_procID.isBlank())
		return CString(_T(""));
	
	return m_procID.GetIDString();
}

//void AODB::ProcMapField::Convert( ConvertParameter& pParameter )
//{
	//setValid(false);
	//m_procID.SetStrDict(pParameter.pTerminal->inStrDict);

	//if(!m_strOrg.IsEmpty()) //NULL
	//{
	//	//get map object
	//	CString strObject = pParameter.pMapFile->GetMapValue(m_strOrg);
	//	if(strObject.IsEmpty())
	//	{
	//		m_strMessage.Format(_T("Cannot find mapped processor for Code(%s)"), m_strOrg);
	//	}
	//	else
	//	{
	//		if(m_procID.isValidID(strObject.GetBuffer()))
	//		{
	//			m_procID.setID(strObject.GetBuffer());

	//			setValid(true);
	//			if( !m_procID.isBlank() )
	//			{
	//				GroupIndex groupIdx = pParameter.pTerminal->procList->getGroupIndex( m_procID );	

	//				if( groupIdx.start < 0 )
	//				{
	//					setValid(false);
	//					m_strMessage.Format(_T("Processor %s( %s) does not exist."),strObject, m_strOrg);
	//				}
	//			}
	//		}
	//		else
	//		{
	//			m_strMessage.Format(_T("Processor name%s(%s) is not valid"),strObject, m_strOrg);
	//		}
	//	}
	//}
//}


//////////////////////////////////////////////////////////////////////////
//AODB::GateMapField
AODB::GateMapField::GateMapField()
{

}

AODB::GateMapField::~GateMapField()
{

}

bool AODB::GateMapField::IsValid()const
{
	if (IsEmpty())
	{
		return true;
	}

	return MapField::IsValid();
}

//void AODB::GateMapField::Convert( ConvertParameter& pParameter )
//{
	//ProcMapField::Convert(pParameter);

	//if(m_bValid)//check is GATE processor or not
	//{//process name is valid
	//	if( !m_procID.isBlank() )
	//	{
	//		GroupIndex groupIdx = pParameter.pTerminal->procList->getGroupIndex( m_procID );	

	//		if( groupIdx.start < 0 )
	//		{
	//			setValid(false);
	//			m_strMessage.Format(_T("Gate processor(%s) does not exist in the current model."), m_strOrg);
	//		}
	//		else
	//		{
	//			if(pParameter.pTerminal->procList->getProcessor( groupIdx.start )->getProcessorType() != GateProc)
	//			{
	//				setValid(false);
	//				m_strMessage.Format(_T("Processor(%s) is not a Gate processor."), m_strOrg);

	//			}
	//		}
	//	}
	//	else//empty id is valid
	//	{

	//	}
	//}
//}


//////////////////////////////////////////////////////////////////////////
//AODB::BaggageDeviceMapField
AODB::BaggageDeviceMapField::BaggageDeviceMapField()
{

}

AODB::BaggageDeviceMapField::~BaggageDeviceMapField()
{

}

bool AODB::BaggageDeviceMapField::IsValid()const
{
	if (IsEmpty())
	{
		return true;
	}

	return MapField::IsValid();
}

void AODB::BaggageDeviceMapField::Convert( ConvertParameter& pParameter )
{
	//ProcMapField::Convert(pParameter);


	//if(m_bValid)//check is GATE processor or not
	//{//process name is valid
	//	if( !m_procID.isBlank() )
	//	{
	//		GroupIndex groupIdx = pParameter.pTerminal->procList->getGroupIndex( m_procID );	

	//		if( groupIdx.start < 0 )
	//		{
	//			setValid(false);
	//			m_strMessage.Format(_T("Baggage processor(%s) does not exist."), m_strOrg);
	//		}
	//		else
	//		{
	//			if(pParameter.pTerminal->procList->getProcessor( groupIdx.start )->getProcessorType() != BaggageProc)
	//			{
	//				setValid(false);
	//				m_strMessage.Format(_T("Processor(%s) is not a Baggage processor."), m_strOrg);

	//			}
	//		}
	//	}
	//	else//empty id is valid
	//	{

	//	}
	//}

	setValid(false);
	m_procID.SetStrDict(pParameter.pTerminal->inStrDict);

	if(!m_strOrg.IsEmpty()) //NULL
	{
		//get map object
		std::vector<CString> vProcs = pParameter.pMapFile->GetMapValue(m_strOrg);
		if(vProcs.size() == 0)
		{
			m_strMessage.Format(_T("Cannot find mapped processor for Code(%s)"), m_strOrg);
			return;
		}
		std::vector<CString>::iterator iter =  vProcs.begin();
		for (; iter != vProcs.end(); ++ iter)
		{
			CString strObject = *iter;
			if(m_procID.isValidID(strObject.GetBuffer()))
			{
				m_procID.setID(strObject.GetBuffer());
				if(m_procID.isBlank())
					continue;


				GroupIndex groupIdx = pParameter.pTerminal->procList->getGroupIndex( m_procID );	

				if( groupIdx.start < 0 )
				{
					m_strMessage.Format(_T("Processor %s( %s) does not exist."),strObject, m_strOrg);
					continue;
				}
				Processor *pProc = pParameter.pTerminal->procList->getProcessor(groupIdx.start);
				if(pProc == NULL)
					continue;

				//proc type
				if(pProc->getProcessorType() != BaggageProc)
				{
					m_strMessage.Format(_T("Processor(%s) is not a Gate processor."), m_strOrg);
					continue;
				}

				setValid(true);
				return;//get the value

			}
			else
			{
				m_strMessage.Format(_T("Processor name%s(%s) is not valid"),strObject, m_strOrg);
			}
		}

	}


}


//////////////////////////////////////////////////////////////////////////
//AODB::StandMapField
AODB::StandMapField::StandMapField()
{

}

AODB::StandMapField::~StandMapField()
{

}

void AODB::StandMapField::Convert( ConvertParameter& pParameter )
{
	setValid(false);
	if(m_strOrg.IsEmpty())//empty is good
	{
		setValid(true);
		return;

	}
	std::vector<CString> vObject = pParameter.pMapFile->GetMapValue(m_strOrg);
	if(vObject.size() == 0)
	{
		m_strMessage.Format(_T("Cannot find mapped Stand for Code(%s)"), m_strOrg);
		return;
	}
	std::vector<CString>::iterator iter =  vObject.begin();
	for (; iter != vObject.end(); ++ iter)
	{
		CString strObject = *iter;

		ALTObjectID tempStandID( strObject );
		if( IsAValidStand( tempStandID,pParameter ) )
		{
			setValid(true);

			char strID[255];
			tempStandID.printID(strID);
			m_standID = ALTObjectID(strID);
			return;
		}
		else
		{
			m_strMessage.Format(_T("Stand %s(%s) does not exist."), strObject, m_strOrg);
		}

	}

}

CString AODB::StandMapField::GetMapValue()const
{
	if(m_standID.IsBlank())
		return CString(_T(""));

	return m_standID.GetIDString();
}

bool AODB::StandMapField::IsAValidStand( const ALTObjectID& _objID, ConvertParameter& pParameter  ) const
{
	if(_objID.IsBlank())
	{
		return true;
	}

	for(int i=0;i< (int)pParameter.m_vStandList.size();i++)
	{
		if(pParameter.m_vStandList.at(i).idFits(_objID))
			return true;
	}
	return false;
}



//////////////////////////////////////////////////////////////////////////
//AODB::DateMapField

AODB::DateMapField::DateMapField()
{
	m_nDayIndex = -1;// 0 is not valid, start with 1 ....
}

AODB::DateMapField::~DateMapField()
{

}
void AODB::DateMapField::GetValue( CSVFile& csvFile )
{
	MapField::GetValue(csvFile);

	setValid(true);
	//convert to date
	m_strOrg.Trim();
	m_oleDate.ParseDateTime(m_strOrg);
	if(m_oleDate.GetStatus() != COleDateTime::valid)
	{
		m_strMessage.Format(_T("%s is not a valid date."), m_strOrg);
		setValid(false);
	}
}

COleDateTime AODB::DateMapField::getDate() const
{
	return m_oleDate;
}

void AODB::DateMapField::SetValue(const CString& strValue, ConvertParameter& pParameter )
{
	AODB::MapField::SetValue(strValue,pParameter);
	setValid(true);
	//convert to date
	m_strOrg.Trim();
	m_oleDate.ParseDateTime(m_strOrg);
	if(m_oleDate.GetStatus() != COleDateTime::valid)
	{
		m_strMessage.Format(_T("%s is not a valid date."), m_strOrg);
		setValid(false);
	}
}

void AODB::DateMapField::Convert( ConvertParameter& pParameter )
{
	if(IsValid())//date time is valid
	{
		COleDateTimeSpan timeSpan = m_oleDate - pParameter.oleStartDate;
		m_nDayIndex = (int)timeSpan.GetDays();
		if(m_nDayIndex < 0)
            setValid(false);
	}

}

CString AODB::DateMapField::GetMapValue() const
{
	ASSERT(m_nDayIndex >= 0);
	CString strDay;
	strDay.Format(_T("Day %d"),m_nDayIndex + 1);
	return strDay;
}




//////////////////////////////////////////////////////////////////////////
//AODB::TimeMapField
AODB::TimeMapField::TimeMapField()
{

}

AODB::TimeMapField::~TimeMapField()
{

}

void AODB::TimeMapField::GetValue( CSVFile& csvFile )
{
	MapField::GetValue(csvFile);//read 

	setValid(true);

	m_strOrg.Trim();
	if(!m_eTime.SetTime(m_strOrg.Left(12).GetBuffer()))
	{
		setValid(false);
		m_strMessage.Format(_T("Time(%s) cannot be parsed."), m_strOrg);
	}


}

void AODB::TimeMapField::Convert( ConvertParameter& pParameter )
{
	setValid(true);

	m_strOrg.Trim();
	if(!m_eTime.SetTime(m_strOrg.Left(12).GetBuffer()))
	{
		setValid(false);
		m_strMessage.Format(_T("Time(%s) cannot be parsed."), m_strOrg);
	}
}

CString AODB::TimeMapField::GetMapValue() const
{
	return m_eTime.printTime();
}



AODB::DateTimeMapField::DateTimeMapField()
{
	m_nDayIndex = 0;
}

AODB::DateTimeMapField::~DateTimeMapField()
{

}

void AODB::DateTimeMapField::GetValue( CSVFile& csvFile )
{
	MapField::GetValue(csvFile);

	//convert to date
	m_strOrg.Trim();
	m_oleDateTime.ParseDateTime(m_strOrg);
}

void AODB::DateTimeMapField::SetValue(const CString& strValue,ConvertParameter& pParameter)
{
	MapField::SetValue(strValue,pParameter);

	m_strOrg.Trim();
	m_oleDateTime.ParseDateTime(m_strOrg);
}

COleDateTime AODB::DateTimeMapField::getDateTime() const
{
	return m_oleDateTime;
}

void AODB::DateTimeMapField::Convert( ConvertParameter& pParameter )
{
	if(m_oleDateTime.GetStatus() != COleDateTime::valid)
	{
		setValid(false);
		m_strMessage.Format(_T("Date time(%s) cannot be parsed."),m_strOrg);
		return;
	}

	COleDateTimeSpan timeSpan = m_oleDateTime - pParameter.oleStartDate;
	//valid time
	m_nDayIndex = timeSpan.GetDays() ;
	//ASSERT(m_nDayIndex >= 0);
	m_eTime.set(m_oleDateTime.GetHour(),m_oleDateTime.GetMinute(), m_oleDateTime.GetSecond());

	setValid(true);
}

CString AODB::DateTimeMapField::GetMapValue() const
{
	CString strTime;
	strTime.Format(_T("Day %d %s"),m_nDayIndex + 1, m_eTime.printTime());
	return strTime;
}

ElapsedTime AODB::DateTimeMapField::getElapsedTime() const
{
	ElapsedTime eTime;
	eTime.SetDay(m_nDayIndex + 1);
	return eTime + m_eTime;
	
}





















void AODB::ArrGateMapField::Convert( ConvertParameter& pParameter )
{
	setValid(false);
	m_procID.SetStrDict(pParameter.pTerminal->inStrDict);

	if(!m_strOrg.IsEmpty()) //NULL
	{
		//get map object
		std::vector<CString> vProcs = pParameter.pMapFile->GetMapValue(m_strOrg);
		if(vProcs.size() == 0)
		{
			m_strMessage.Format(_T("Cannot find mapped processor for Code(%s)"), m_strOrg);
			return;
		}
		std::vector<CString>::iterator iter =  vProcs.begin();
		for (; iter != vProcs.end(); ++ iter)
		{
			CString strObject = *iter;
			if(m_procID.isValidID(strObject.GetBuffer()))
			{
				m_procID.setID(strObject.GetBuffer());
				if(m_procID.isBlank())
					continue;


				GroupIndex groupIdx = pParameter.pTerminal->procList->getGroupIndex( m_procID );	

				if( groupIdx.start < 0 )
				{
					m_strMessage.Format(_T("Processor %s( %s) does not exist."),strObject, m_strOrg);
					continue;
				}
				Processor *pProc = pParameter.pTerminal->procList->getProcessor(groupIdx.start);
				if(pProc == NULL)
					continue;

				//proc type
				if(pProc->getProcessorType() != GateProc)
				{
					m_strMessage.Format(_T("Processor(%s) is not a Gate processor."), m_strOrg);
					continue;
				}

				//check arrival or departure
				GateProcessor *pGate = (GateProcessor *)pProc;
				if(pGate == NULL)
				{
					m_strMessage.Format(_T("Processor(%s) is not a Gate processor."), m_strOrg);
					continue;
				}

				if(pGate->getGateType() != ArrGate)
				{
					m_strMessage.Format(_T("Processor(%s) is not an Arrival Gate processor."), m_strOrg);
					continue;

				}
				setValid(true);
				return;//get the value

			}
			else
			{
				m_strMessage.Format(_T("Processor name%s(%s) is not valid"),strObject, m_strOrg);
			}
		}

	}
}

void AODB::DepGateMapField::Convert( ConvertParameter& pParameter )
{
	setValid(false);
	m_procID.SetStrDict(pParameter.pTerminal->inStrDict);

	if(!m_strOrg.IsEmpty()) //NULL
	{
		//get map object
		std::vector<CString> vProcs = pParameter.pMapFile->GetMapValue(m_strOrg);
		if(vProcs.size() == 0)
		{
			m_strMessage.Format(_T("Cannot find mapped processor for Code(%s)"), m_strOrg);
			return;
		}
		std::vector<CString>::iterator iter =  vProcs.begin();
		for (; iter != vProcs.end(); ++ iter)
		{
			CString strObject = *iter;
			if(m_procID.isValidID(strObject.GetBuffer()))
			{
				m_procID.setID(strObject.GetBuffer());
				if(m_procID.isBlank())
					continue;


				GroupIndex groupIdx = pParameter.pTerminal->procList->getGroupIndex( m_procID );	

				if( groupIdx.start < 0 )
				{
					m_strMessage.Format(_T("Processor %s( %s) does not exist."),strObject, m_strOrg);
					continue;
				}
				Processor *pProc = pParameter.pTerminal->procList->getProcessor(groupIdx.start);
				if(pProc == NULL)
					continue;

				//proc type
				if(pProc->getProcessorType() != GateProc)
				{
					m_strMessage.Format(_T("Processor(%s) is not a Gate processor."), m_strOrg);
					continue;
				}

				//check arrival or departure
				GateProcessor *pGate = (GateProcessor *)pProc;
				if(pGate == NULL)
				{
					m_strMessage.Format(_T("Processor(%s) is not a Gate processor."), m_strOrg);
					continue;
				}

				if(pGate->getGateType() != DepGate)
				{
					m_strMessage.Format(_T("Processor(%s) is not a Departure Gate processor."), m_strOrg);
					continue;
				}
				setValid(true);
				return;//get the value

			}
			else
			{
				m_strMessage.Format(_T("Processor name%s(%s) is not valid"),strObject, m_strOrg);
			}
		}

	}
}

void AODB::RosterProcMapField::Convert( ConvertParameter& pParameter )
{
	//setValid(false);
	//ProcessorID procID;
	//procID.SetStrDict(pParameter.pTerminal->inStrDict);

	//if(!m_strOrg.IsEmpty()) //NULL
	//{
	//	//get map object
	//	std::vector<CString> vProcs = pParameter.pMapFile->GetMapValue(m_strOrg);
	//	if(vProcs.size() == 0)
	//	{
	//		m_strMessage.Format(_T("Cannot find mapped processor for Code(%s)"), m_strOrg);
	//		return;
	//	}

	//	///for each mapped value
	//	std::vector<CString>::iterator iter =  vProcs.begin();
	//	std::vector<ProcessorID > vAvailableProc;

	//	for (; iter != vProcs.end(); ++ iter)
	//	{
	//		CString strObject = *iter;
	//		if(procID.isValidID(strObject.GetBuffer()))
	//		{
	//			procID.setID(strObject.GetBuffer());
	//			if(procID.isBlank())
	//				continue;


	//			GroupIndex groupIdx = pParameter.pTerminal->procList->getGroupIndex( procID );	

	//			if( groupIdx.start < 0 )
	//			{
	//				m_strMessage.Format(_T("Processor %s( %s) does not exist."),strObject, m_strOrg);
	//				continue;
	//			}
	//			Processor *pProc = pParameter.pTerminal->procList->getProcessor(groupIdx.start);
	//			if(pProc == NULL)
	//			{
	//				m_strMessage.Format(_T("Processor %s( %s) does not exist."),strObject, m_strOrg);
	//				continue;
	//			}

	//			//proc type
	//			if(pProc->getProcessorType() == GateProc)
	//			{
	//				vAvailableProc.push_back(procID);

	//				m_strMessage.Format(_T("Processor(%s) is not a  processor."), m_strOrg);
	//				continue;
	//			}

	//			m_strMap = procID.GetIDString();
	//			setValid(true);//non-gate processor is the first choice, for the gate processor usually have no roster definition
	//			return;
	//		}
	//	}

	//	if(vAvailableProc.size() == 0)
	//	{
	//		m_strMessage.Format(_T("Cannot find availble mapped processor for Code(%s)"), m_strOrg);
	//	}
	//	else //gate processor
	//	{
	//		//m_procID = vAvailableProc[0];
	//		m_strMap = vAvailableProc[0].GetIDString();
	//		setValid(true);
	//		return;
	//	}
	//

	//	//check if stand
	//	iter =  vProcs.begin();
	//	for (; iter != vProcs.end(); ++ iter)
	//	{
	//		CString strObject = *iter;
	//	
	//		
	//		ALTObjectID tempStandID( strObject );
	//		if( IsAValidStand( tempStandID,pParameter ) )
	//		{
	//			setValid(true);
	//			char strID[255];
	//			tempStandID.printID(strID);
	//			m_strMap = tempStandID.GetIDString();

	//			return;
	//		}
	//	}
	//}

	setValid(false);
	//convert the origin text directly
	std::vector<CString> vTexts;
	vTexts.push_back(m_strOrg);
	 CString strProcName;
	std::vector<ProcessorID > vAvailableProc;
	std::vector<ALTObjectID> vStand;

	if(Convert(pParameter,vTexts,strProcName,vAvailableProc ,vStand , m_strMessage))
	{
		setValid(true);
		m_strMap = strProcName;
		return;
	}

	//check the mapped text
	
	vTexts.clear();
	vTexts = pParameter.pMapFile->GetMapValue(m_strOrg);
	
	if(Convert(pParameter,vTexts,strProcName,vAvailableProc ,vStand , m_strMessage))
	{
		setValid(true);
		m_strMap = strProcName;
		return;
	}

	//check gate
	if(vAvailableProc.size() == 0 && vStand.size() == 0)
	{
		m_strMessage.Format(_T("Cannot find availble mapped processor for Code(%s)"), m_strOrg);
		return;
	}
	if(vAvailableProc.size() > 0) //gate processor
	{
		m_strMap = vAvailableProc[0].GetIDString();
		setValid(true);
		return;
	}

	//check stand
	if(vStand.size() > 0) //gate processor
	{
		m_strMap = vStand[0].GetIDString();
		setValid(true);
	}
}

bool AODB::RosterProcMapField::Convert(ConvertParameter& pParameter, std::vector<CString> vText,
									   CString& strProcName ,
									   std::vector<ProcessorID >& vAvailableProc, 
									   std::vector<ALTObjectID>& vStand, CString& strErrorMsg ) const
{
	ProcessorID procID;
	procID.SetStrDict(pParameter.pTerminal->inStrDict);

	if(vText.size() > 0) //NULL
	{
		//get map object
		std::vector<CString> vProcs = vText;

		///for each mapped value
		std::vector<CString>::iterator iter =  vProcs.begin();
		for (; iter != vProcs.end(); ++ iter)
		{
			CString strObject = *iter;
			if(procID.isValidID(strObject.GetBuffer()))
			{
				procID.setID(strObject.GetBuffer());
				if(procID.isBlank())
					continue;


				GroupIndex groupIdx = pParameter.pTerminal->procList->getGroupIndex( procID );	

				if( groupIdx.start < 0 )
				{
					strErrorMsg.Format(_T("Processor %s( %s) does not exist."),strObject, m_strOrg);
					continue;
				}
				Processor *pProc = pParameter.pTerminal->procList->getProcessor(groupIdx.start);
				if(pProc == NULL)
				{
					strErrorMsg.Format(_T("Processor %s( %s) does not exist."),strObject, m_strOrg);
					continue;
				}

				//proc type
				if(pProc->getProcessorType() == GateProc)
				{
					vAvailableProc.push_back(procID);
					//strErrorMsg.Format(_T("Processor(%s) is not a  processor."), m_strOrg);
					continue;
				}
				//non-gate processor is the first choice, for the gate processor usually have no roster definition
				strProcName = procID.GetIDString();
				
				return true;
			}
		}


		//check if stand
		iter =  vProcs.begin();
		for (; iter != vProcs.end(); ++ iter)
		{
			CString strObject = *iter;

			ALTObjectID tempStandID( strObject );
			if( IsAValidStand( tempStandID,pParameter ) )
			{
				char strID[255];
				tempStandID.printID(strID);
				vStand.push_back(tempStandID);
			}
		}
	}
	return false;

}
bool AODB::RosterProcMapField::IsAValidStand( const ALTObjectID& _objID, ConvertParameter& pParameter  ) const
{
	if(_objID.IsBlank())
	{
		return true;
	}

	for(int i=0;i< (int)pParameter.m_vStandList.size();i++)
	{
		if(pParameter.m_vStandList.at(i).idFits(_objID))
			return true;
	}
	return false;
}


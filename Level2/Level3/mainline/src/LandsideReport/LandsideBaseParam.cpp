#include "StdAfx.h"
#include ".\landsidebaseparam.h"
#include "../Common/fileman.h"
#include "../Common/exeption.h"
#include "../Common/Termfile.h"
#include "Landside\VehicleLandSideDefine.h"
#include "../Landside/InputLandside.h"
#include <algorithm>


LandsideBaseParam::LandsideBaseParam(void)
:m_startTime(0L)
,m_endTime(86400L)
,m_lInterval(0L)
,m_nProjID(-1)
,m_reportType(LandsideReport_Detail)
,m_pCommonData(NULL)
,m_lIntervalDistance(0l)
{
}

LandsideBaseParam::~LandsideBaseParam(void)
{
}
void LandsideBaseParam::SetProjID(int nProiID)
{
	m_nProjID = nProiID ;

}
int LandsideBaseParam::GetProjID()
{
	return m_nProjID ; 
}
CString LandsideBaseParam::GetParameterString()
{
	CString strString;
	strString += m_startTime.PrintDateTime();
	strString += ";";
	strString += m_endTime.PrintDateTime();
	strString += ";";




	strString.TrimRight(";");

	//vehicle Type
	CString strVehicleType;
	for (int i = 0; i < GetVehicleTypeNameCount(); i++)
	{
		if (GetVehicleTypeName(i).toString().IsEmpty())
			continue;
		
		if (strVehicleType.IsEmpty())
		{
			strVehicleType = _T(";") +GetVehicleTypeName(i).toString(); 
		}
		else 
		{
			strVehicleType += _T(",") + GetVehicleTypeName(i).toString();
		}
	}
	strString += strVehicleType;
	//from object
	CString strFromObject;
	for (int j = 0; j < GetFromObjectCount(); j++)
	{
		if (GetFromObject(j).GetIDString().IsEmpty())
			continue;
		
		if (strFromObject.IsEmpty())
		{
			strFromObject = _T(";") + GetFromObject(j).GetIDString();
		}
		else 
		{
			strFromObject += _T(",") + GetFromObject(j).GetIDString();
		}
	}

	strString += strFromObject;
	//To object
	CString strToObject;
	for (int k = 0; k < GetToObjectCount(); k++)
	{
		if (GetToObject(k).GetIDString().IsEmpty())
			continue;
		
		if (strToObject.IsEmpty())
		{
			strToObject = _T(";") + GetToObject(k).GetIDString();
		}
		else 
		{
			strToObject += _T(",") + GetToObject(k).GetIDString();
		}
	}
	strString += strToObject;

	strString.TrimRight(";");
	return strString;


}
void LandsideBaseParam::SetReportFileDir( const CString& strReportPath )
{
	m_strReportPath = strReportPath;
}


CString LandsideBaseParam::GetReportParamPath()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\%s"),m_strReportPath,GetReportParamName());
	return strFilePath;
}

void LandsideBaseParam::LoadParameterFile()
{
	CString strFilrName = GetReportParamPath();

	ArctermFile file;
	try 
	{
		file.openFile (strFilrName, READ);
	}
	catch (FileOpenError *exception)
	{
		delete exception;
		return;
	}
	file.getLine();
	ReadParameter(file);

	file.closeIn();
}

void LandsideBaseParam::SaveParameterFile()
{
	CString strFilrName = GetReportParamPath();

	int nIndex = strFilrName.ReverseFind('\\');
	if(nIndex <= 0)
		return ;


	CString strFolder = strFilrName.Left(nIndex);

	FileManager::MakePath(strFolder);

	ArctermFile arcFile;
	arcFile.openFile(strFilrName,WRITE);
	arcFile.writeField (_T("Report Parameter"));
	arcFile.writeLine();

	arcFile.writeField ("start time, end time, ...");
	arcFile.writeLine();




	WriteParameter (arcFile);

	arcFile.endFile();
}
//write base informations, start time, end time , interval ....
void LandsideBaseParam::WriteParameter( ArctermFile& _file )
{
	_file.writeInt(m_startTime.asSeconds()) ;
	_file.writeInt(m_endTime.asSeconds()) ;
	_file.writeInt(m_lInterval) ;
	_file.writeInt(m_nProjID) ;
	_file.writeInt((int)m_reportType) ;
	_file.writeInt(m_lIntervalDistance);
	_file.writeLine();

	//vehicle type
	int nVehicleTypeCount = GetVehicleTypeCount();
	_file.writeInt(nVehicleTypeCount);
	for (int nType = 0; nType < nVehicleTypeCount; ++ nType)
	{
		_file.writeInt(GetVehicleType(nType));
	}
	_file.writeLine();

	//From Object
	int nFromCount = (int)m_vFromObject.size();
	_file.writeInt(nFromCount);
	for (int nFrom = 0; nFrom < nFromCount; ++ nFrom)
	{
		m_vFromObject[nFrom].writeALTObjectID(_file);
	}
	_file.writeLine();

	//To Object
	int nToCount = (int)m_vToObjects.size();
	_file.writeInt(nToCount);
	for (int nTo = 0; nTo < nToCount; ++ nTo)
	{
		m_vToObjects[nTo].writeALTObjectID(_file);
	}
	_file.writeLine();

	//Filter Object
	int nFilterCount = (int)m_vObjectsFilter.size();
	_file.writeInt(nFilterCount);
	for (int nFilter = 0; nFilter < nFilterCount; ++ nFilter)
	{
		m_vObjectsFilter[nFilter].writeALTObjectID(_file);
	}
	_file.writeLine();

	//vehicle type name
	int nVehicleTypeNameCount = (int)m_vVehicleTypeName.size();
	_file.writeInt(nVehicleTypeNameCount);
	for (int nVehicleName = 0; nVehicleName < nVehicleTypeNameCount; ++nVehicleName)
	{
		CString strName = m_vVehicleTypeName[nVehicleName].toString();
		_file.writeField(strName);
	}
	_file.writeLine();
}

void LandsideBaseParam::ReadParameter( ArctermFile& _file )
{
	ASSERT(m_pCommonData != NULL);

	if(m_pCommonData == NULL)
		return;


	long time ;
	_file.getInteger(time) ;
	m_startTime = ElapsedTime(time) ;
	_file.getInteger(time) ;
	m_endTime = ElapsedTime(time) ;

	_file.getInteger(m_lInterval) ;
	_file.getInteger(m_nProjID) ;
	int type ;
	_file.getInteger(type) ;
	m_reportType = (LandsideReport_Detail_Summary)type ;

	_file.getInteger(m_lIntervalDistance);

	_file.getLine() ;

	ClearVehicleType();
	int nTypeCount= 0;
	_file.getInteger(nTypeCount);
	for (int nType = 0; nType < nTypeCount; ++nType)
	{
		int vehicleType = -1;
		_file.getInteger(vehicleType);
		AddVehicleType(vehicleType);
	}
	_file.getLine();
	
	if(_file.isBlank())
		return;


	//from Object

	int nFromCount= 0;
	_file.getInteger(nFromCount);
	for (int nFrom = 0; nFrom < nFromCount; ++nFrom)
	{
		ALTObjectID altObj;

		altObj.readALTObjectID(_file);
		m_vFromObject.push_back(altObj);
	}
	_file.getLine();

	//to object
	int nToCount= 0;
	_file.getInteger(nToCount);
	for (int nTo = 0; nTo < nToCount; ++nTo)
	{

		ALTObjectID altObj;

		altObj.readALTObjectID(_file);
		m_vToObjects.push_back(altObj);
	}
	_file.getLine();

	if(_file.isBlank())
		return;
	//filter object
	int nFilterCount= 0;
	_file.getInteger(nFilterCount);
	m_vObjectsFilter.clear();
	for (int nFilter = 0; nFilter < nFilterCount; ++nFilter)
	{

		ALTObjectID altObj;

		altObj.readALTObjectID(_file);
		m_vObjectsFilter.push_back(altObj);
	}
	_file.getLine();

	int nVehicleNameCount = 0;
	_file.getInteger(nVehicleNameCount);
	m_vVehicleTypeName.clear();
	for (int nVehicleName = 0; nVehicleName < nVehicleNameCount; ++nVehicleName)
	{
		CString strName;
		_file.getField(strName.GetBuffer(1024),1024);
		strName.ReleaseBuffer();
		CHierachyName vehicleName;
		vehicleName.fromString(strName);
		m_vVehicleTypeName.push_back(vehicleName);
	}
	_file.getLine();
}

ProjectCommon * LandsideBaseParam::GetCommonData() const
{
	return m_pCommonData;
}

void LandsideBaseParam::SetCommonData( ProjectCommon * pCommon )
{
	m_pCommonData = pCommon;
}

void LandsideBaseParam::setReportType( LandsideReport_Detail_Summary rpType )
{
	m_reportType = rpType;
}

LandsideReport_Detail_Summary LandsideBaseParam::getReportType()
{
	return m_reportType;
}

void LandsideBaseParam::LoadParameterFromFile( ArctermFile& _file )
{
	_file.getLine();
	ReadParameter(_file);
}



CString LandsideBaseParam::GetVehicleTypeName(InputLandside *pLandside, int nType )
{
	ASSERT(pLandside != NULL);
	if(pLandside == NULL)
		return "The type cannot be recognized";
	if(nType == LANDSIDE_ALLVEHICLETYPES)
		return "All Types";

	//ASSERT(FALSE);
	return "All Types";

//	return pLandside->getOperationVehicleList().GetVehicleTypeNameByID(nType);

}

bool LandsideBaseParam::HasVehicleTypeDefined()
{
	return GetVehicleTypeNameCount()>0;
}

int LandsideBaseParam::GetVehicleTypeCount() const
{
	return static_cast<int>(m_vVehicleType.size());
}

int LandsideBaseParam::GetVehicleType( int nIndex ) const
{
	ASSERT(nIndex >= 0&& nIndex < GetVehicleTypeCount());
	if(nIndex < 0 || GetVehicleTypeCount()< nIndex)
		return -1;
	return m_vVehicleType[nIndex];
}

void LandsideBaseParam::ClearVehicleType()
{
	m_vVehicleType.clear();
}

bool LandsideBaseParam::AddVehicleType( int nType )
{
	if(std::find(m_vVehicleType.begin(),m_vVehicleType.end(), nType) == m_vVehicleType.end())
		m_vVehicleType.push_back(nType);
	else
		return false;

	return true;

}

bool LandsideBaseParam::ContainVehicleType( int nType ) const
{
	int nVehicleTypeCount = GetVehicleTypeCount();
	//all types
	if(nVehicleTypeCount == 0)
		return true;

	for (int nItem = 0; nItem < nVehicleTypeCount; ++ nItem)
	{
		int nVehicleType = GetVehicleType(nItem);
		if( nVehicleType== LANDSIDE_ALLVEHICLETYPES || nVehicleType == nType)
			return true;
	}

	return false;
}

int LandsideBaseParam::GetFromObjectCount() const
{
	return static_cast<int>(m_vFromObject.size());
}

ALTObjectID LandsideBaseParam::GetFromObject( int nIndex ) const
{
	ASSERT(nIndex >= 0&& nIndex < GetFromObjectCount());
	if(nIndex < 0 || GetFromObjectCount()< nIndex)
		return ALTObjectID();

	return m_vFromObject[nIndex];
}

void LandsideBaseParam::ClearFromObject()
{
	m_vFromObject.clear();
}

bool LandsideBaseParam::AddFromObject( ALTObjectID nObject )
{
	if(std::find(m_vFromObject.begin(),m_vFromObject.end(), nObject) == m_vFromObject.end())
		m_vFromObject.push_back(nObject);
	else
		return false;

	return true;
}

bool LandsideBaseParam::ContainFromObject( ALTObjectID nObject ) const
{
	int nFromCount = GetFromObjectCount();
	if(nFromCount == 0)
		return true;

	for (int nItem = 0; nItem < nFromCount; ++ nItem)
	{
		ALTObjectID fromObject = GetFromObject(nItem);
		if( nObject.idFits(fromObject))
			return true;
	}
	return false;
}

int LandsideBaseParam::GetToObjectCount() const
{
	return static_cast<int>(m_vToObjects.size());
}

ALTObjectID LandsideBaseParam::GetToObject( int nIndex ) const
{
	ASSERT(nIndex >= 0&& nIndex < GetToObjectCount());
	if(nIndex < 0 || GetToObjectCount()< nIndex)
		return ALTObjectID();
	return m_vToObjects[nIndex];
}

void LandsideBaseParam::ClearToObject()
{
	m_vToObjects.clear();
}

bool LandsideBaseParam::AddToObject( ALTObjectID nObject )
{
	if(std::find(m_vToObjects.begin(),m_vToObjects.end(), nObject) == m_vToObjects.end())
		m_vToObjects.push_back(nObject);
	else
		return false;

	return true;
}

bool LandsideBaseParam::ContainToObject( ALTObjectID nObject ) const
{
	int nToCount = GetToObjectCount();
	if(nToCount == 0)//none is all
		return true;

	for (int nItem = 0; nItem < nToCount; ++ nItem)
	{
		ALTObjectID toObjectt = GetToObject(nItem);
		if( nObject.idFits(toObjectt))
			return true;
	}

	return false;
}

void LandsideBaseParam::AddFilterObject( const ALTObjectID& altObj )
{
	m_vObjectsFilter.push_back(altObj);
}

int LandsideBaseParam::GetFilterObjectCount() const
{
	return static_cast<int>(m_vObjectsFilter.size());
}

ALTObjectID LandsideBaseParam::GetFilterObject( int nIndex ) const
{
	ASSERT(nIndex >= 0&& nIndex < GetFilterObjectCount());
	if(nIndex < 0 || GetFilterObjectCount()< nIndex)
		return ALTObjectID();
	return m_vObjectsFilter[nIndex];
}

void LandsideBaseParam::ClearFilterObject()
{
	m_vObjectsFilter.clear();
}

bool LandsideBaseParam::FilterObject( const ALTObjectID& altObj ) const
{
	int nCount = static_cast<int>(m_vObjectsFilter.size());
	if(nCount == 0)//none is all
		return true;

	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		ALTObjectID toObjectt = GetFilterObject(nItem);
		if( altObj.idFits(toObjectt))
			return true;
	}

	return false;
}

void LandsideBaseParam::AddVehicleTypeName( const CHierachyName& vehicleName )
{
	m_vVehicleTypeName.push_back(vehicleName);
}

int LandsideBaseParam::GetVehicleTypeNameCount() const
{
	return static_cast<int>(m_vVehicleTypeName.size());
}

bool LandsideBaseParam::FilterName( const CHierachyName& vehicleName ) const
{
	int nCount = static_cast<int>(m_vVehicleTypeName.size());
	if(nCount == 0)//none is all
		return true;

	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CHierachyName name = GetVehicleTypeName(nItem);
		if( vehicleName.fitIn(name))
			return true;
	}

	return false;
}

CHierachyName LandsideBaseParam::GetVehicleTypeName( int idx ) const
{
	ASSERT(idx >= 0&& idx < GetVehicleTypeNameCount());
	if(idx < 0 || GetVehicleTypeNameCount()< idx)
		return CHierachyName();
	return m_vVehicleTypeName[idx];
}

void LandsideBaseParam::ClearVehicleName()
{
	m_vVehicleTypeName.clear();
}

CHierachyName LandsideBaseParam::GetFilterName( const CHierachyName& vehicleName ) const
{
	int nCount = static_cast<int>(m_vVehicleTypeName.size());
	if(nCount == 0)//none is all
		return CHierachyName("All Types");

	CHierachyName fitVehicle;
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CHierachyName name = GetVehicleTypeName(nItem);
		if(vehicleName.fitIn(name))
		{
			if( fitVehicle.isEmpty())
			{
				fitVehicle = name;
			}
			else if (name.fitIn(fitVehicle))
			{
				fitVehicle = name;
			}
		}
	
	}

	//doesn't find any fittest item
	if (fitVehicle.isEmpty())
	{
		return CHierachyName("All Types");
	}
	
	//find the fittest item
	return fitVehicle;
}







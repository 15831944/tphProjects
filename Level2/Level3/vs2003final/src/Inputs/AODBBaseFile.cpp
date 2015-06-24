#include "StdAfx.h"
#include ".\aodbbasefile.h"
#include "AODBException.h"
#include "../Common/csv_file.h"
#include "../Common/exeption.h"
#include "AODBConvertParameter.h"



using namespace AODB;

BaseFile::BaseFile(void)
{
}

BaseFile::~BaseFile(void)
{
	Clear();
}


CString BaseFile::getFileName() const
{
	return m_strFileName;
}

void BaseFile::setFileName( CString strFileName )
{
	m_strFileName = strFileName;
}

void BaseFile::Read()
{
	//check the filename is valid or not
	m_strFileName.Trim();
	ASSERT(m_strFileName.GetLength());


	CSVFile file;
	try 
	{ 
		file.init(m_strFileName, READ); 
	}
	catch (FileOpenError *exception)
	{
		//exception->getMessage();
		delete exception;

		CString strDesc;
		strDesc.Format(_T("Cannot read file - %s"),m_strFileName);
		throw AODBException(AODBException::ET_FATEL,strDesc);
		return;
	}

	ReadData (file);
	file.closeIn();

}

void BaseFile::Save()
{
	if(m_strFileName.IsEmpty())
		return;

	CSVFile file;
	try 
	{ 	
		file.init (m_strFileName, WRITE);
	}
	catch (FileOpenError *exception)
	{
		//exception->getMessage();
		delete exception;

		CString strDesc;
		strDesc.Format(_T("Cannot save file - %s"),m_strFileName);
		throw AODBException(AODBException::ET_FATEL,strDesc);
		return;
	}


	WriteData (file);

	file.closeOut();
}

void BaseFile::ReadData( CSVFile& csvFile )
{
	ASSERT(0);
}

void BaseFile::WriteData( CSVFile& csvFile )
{
	ASSERT(0);
}

int AODB::BaseFile::GetCount() const
{
	return static_cast<int>(m_vDataItems.size());
}

void AODB::BaseFile::AddData( BaseDataItem *pItem )
{
	m_vDataItems.push_back(pItem);
}

bool AODB::BaseFile::DeleteData(BaseDataItem *pItem)
{
	std::vector<BaseDataItem *>::iterator iter = std::find(m_vDataItems.begin(),m_vDataItems.end(),pItem);
	if (iter != m_vDataItems.end())
	{
		m_vDataItems.erase(iter);
		return true;
	}
	return false;
}

BaseDataItem * AODB::BaseFile::GetData( int nIndex )const
{
	ASSERT(nIndex >= 0 && nIndex < GetCount());
	if(nIndex >= 0 && nIndex < GetCount())
		return m_vDataItems.at(nIndex);

	return NULL;
}

void AODB::BaseFile::Clear()
{
	std::vector<BaseDataItem *>::iterator iter = m_vDataItems.begin();
	for(; iter != m_vDataItems.end(); ++iter)
	{
		delete *iter;
	}
	m_vDataItems.clear();
}

bool AODB::BaseFile::IsReady() const
{
	std::vector<BaseDataItem *>::const_iterator iter = m_vDataItems.begin();
	for(; iter != m_vDataItems.end(); ++iter)
	{
		if(*iter && (*iter)->IsSuccess())
		{
			//convert successfully
		}
		else

			return false;
	}

	return true;
}

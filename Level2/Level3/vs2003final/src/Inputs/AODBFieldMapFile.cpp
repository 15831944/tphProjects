#include "StdAfx.h"
#include ".\aodbfieldmapfile.h"

using namespace AODB;


//////////////////////////////////////////////////////////////////////////
//FieldMapItem
AODB::FieldMapItem::FieldMapItem()
{
	m_strCode = _T("CODE");
	m_strObject = _T("PROCESS");
}

AODB::FieldMapItem::~FieldMapItem()
{

}

void AODB::FieldMapItem::ReadData( CSVFile& csvFile )
{
	m_strCode = GetString(csvFile);
	m_strObject = GetString(csvFile);
	//m_strType = GetString(csvFile);

}

void AODB::FieldMapItem::WriteData( CSVFile& csvFile )
{
	csvFile.writeField(m_strCode);
	csvFile.writeField(m_strObject);
	//csvFile.writeField(m_strType);

}






//////////////////////////////////////////////////////////////////////////
//FieldMapFile
FieldMapFile::FieldMapFile(void)
{
}

FieldMapFile::~FieldMapFile(void)
{
}



FieldMapFile::FieldMapFile(const FieldMapFile& mapFile)
{
	Clear();

	m_strFileName = mapFile.m_strFileName;
	for (int i = 0; i < mapFile.GetCount(); i++)
	{
		FieldMapItem* pItem = new FieldMapItem(*(mapFile.GetItem(i)));
		m_vDataItems.push_back(pItem);
	}
}

FieldMapFile& FieldMapFile::operator = (const FieldMapFile& mapFile)
 {
 	Clear();
 
	m_strFileName = mapFile.m_strFileName;
 
 	for (int i = 0; i < mapFile.GetCount(); i++)
 	{
 		FieldMapItem* pItem = new FieldMapItem(*(mapFile.GetItem(i)));
 		m_vDataItems.push_back(pItem);
 	}
 
 	return *this;
 }

void AODB::FieldMapFile::ReadData( CSVFile& csvFile )
{
	//m_mapCodeValue.clear();

	
	while(csvFile.getLine())
	{
		FieldMapItem *pItem =new FieldMapItem;
		pItem->ReadData(csvFile);

		AddData(pItem);
	}


}

void AODB::FieldMapFile::WriteData( CSVFile& csvFile )
{
	csvFile.writeField(_T("CODE, PROCESSOR"));//header
	csvFile.writeLine();

	int nCount = GetCount();
	for (int nItem =0; nItem < nCount; ++ nItem)
	{
		 FieldMapItem *pItem =GetItem(nItem);
		 if(pItem)
			 pItem->WriteData(csvFile);

		 csvFile.writeLine();
	}

}

FieldMapItem * AODB::FieldMapFile::GetItem( int nIndex )const
{
	return (FieldMapItem *)GetData(nIndex);
}


//CString AODB::FieldMapFile::GetMapValue( const CString& strCode ) const
//{
//	ASSERT(!strCode.IsEmpty());
//	if(strCode.IsEmpty())
//		return CString(_T(""));
//
//	std::map<CString, CString>::const_iterator iter = m_mapCodeValue.find(strCode);
//	if(iter != m_mapCodeValue.end())
//	{
//		return (*iter).second;
//	}
//
////	ASSERT(0);
//	return CString("");
//}

std::vector<CString> AODB::FieldMapFile::GetMapValue( const CString& strCode ) const
{
	std::vector<CString> vProcName;
	ASSERT(!strCode.IsEmpty());
	if(strCode.IsEmpty())
		return vProcName;

	std::vector<BaseDataItem *>::const_iterator iter = m_vDataItems.begin();
	for(;iter != m_vDataItems.end(); ++iter)
	{
		FieldMapItem *pItem = (FieldMapItem *)*iter;
		if(pItem)
		{
			if(strCode.CompareNoCase(pItem->m_strCode) == 0)
				vProcName.push_back(pItem->m_strObject);
		}
	}

	return vProcName;
}

void AODB::FieldMapFile::AddMap( const CString& strCode, const CString& strObject )
{
	FieldMapItem *pItem = new FieldMapItem;
	pItem->m_strCode = strCode;
	pItem->m_strObject = strObject;

	AddData(pItem);

	//m_mapCodeValue.insert(std::make_pair(strCode,strObject));


}
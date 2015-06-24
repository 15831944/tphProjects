#pragma once
#include "AODBBaseDataItem.h"

class CSVFile;

namespace AODB
{
	class ConvertParameter;

class BaseFile
{
public:
	BaseFile(void);
	virtual ~BaseFile(void);


public:
	CString getFileName() const;
	void setFileName(CString strFileName);
	



	//read file, if failed would throw AODBException
	//need to call OpenFile first
	
	virtual void ReadData(CSVFile& csvFile) = 0;
	virtual void WriteData(CSVFile& csvFile) = 0;


	//Read file and will call ReadData()
	void Read();
	//Save Data, will call SaveData()
	void Save();


	//date operation
public:
	int GetCount() const;
	void AddData(BaseDataItem *pItem);
	bool DeleteData(BaseDataItem *pItem);

	void Clear();

	//all items are success 
	bool IsReady() const;

protected:
	BaseDataItem *GetData(int nIndex)const;


protected:
	CString m_strFileName;//file path & name


	std::vector<BaseDataItem *> m_vDataItems;
};
}

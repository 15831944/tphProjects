#pragma once
#include "aodbbasefile.h"
#include "AODBBaseDataItem.h"


namespace AODB
{
	class FieldMapItem : public BaseDataItem
	{
		friend class FieldMapFile;
	public:
		FieldMapItem();
		~FieldMapItem();

	public:
		void ReadData(CSVFile& csvFile);
		void WriteData(CSVFile& csvFile);

	public:
		CString m_strCode;

		CString m_strObject;
		//ProcMapField m_proc;

		CString m_strType;

	private:
	};



class FieldMapFile :
	public BaseFile
{
public:
	FieldMapFile(void);
	~FieldMapFile(void);

	FieldMapFile(const FieldMapFile& mapFile);
	AODB::FieldMapFile& operator = (const FieldMapFile& mapFile);

public:
	virtual void ReadData(CSVFile& csvFile);
	virtual void WriteData(CSVFile& csvFile);

public:
	FieldMapItem *GetItem(int nIndex)const;
	

	//CString GetMapValue(const CString& strCode) const;
	
	std::vector<CString> GetMapValue(const CString& strCode) const;


	void AddMap(const CString& strCode, const CString& strObject);
protected:
	//std::map<CString, CString> m_mapCodeValue;
};
}
#pragma once

/********************************************************************
	created:	2010/04/28
	created:	28:4:2010   14:40
	filename:	IniFile.h
	author:		Benny
	
	purpose:	Class to analyze an .ini file


	syntax:		1. use ';' to comment a line or all characters following it in one line
				2. use '[' and ']' to mark the section name
				3. use '=' to separate key and value

	steps:		1. read in one line, goto step 1 if empty, return true if it has no more
				2. try to find ';', if found, discard all characters after it and to to step 1
				3. try to find '[', if not found, goto step 5
				4. try to find ']', if found treat characters in '['']' pair as section name, else return false
				5. try to find '=', if not found, return false
				6. treat all characters left of '=' as key name, all characters right of '=' as value, space not discarded
				7. goto step 1
*********************************************************************/


#include <vector>
#include <utility>
#include <string>


// will fetch value from this section if not found any value match
#define INI_FILE_DEFAULT_SECTION_NAME "Default Values"


class CIniFile
{
public:
	CIniFile(void);
	~CIniFile(void);

	bool ReadFile(std::string strFileName);
	bool ReadFile(const char* strFileName);

	bool GetSectionName(size_t nSection, std::string& strSection) const;
	bool GetKeyValue(size_t nSection, size_t nIndex, std::string& strKey, std::string& strValue) const;
	bool GetKeyValue(std::string strSection, size_t nIndex, std::string& strKey, std::string& strValue) const;
	bool GetValue(size_t nSection, std::string strKey, std::string& strValue) const;
	bool GetValue(std::string strSection, std::string strKey, std::string& strValue) const;

protected:
	bool AnalyzeText(const std::string& strText);
	bool GetDefaultValue(std::string strSection, std::string& strValue) const;

private:
	typedef std::pair<std::string/*Key*/, std::string/*Value*/> KeyValuePair;
	typedef std::vector<KeyValuePair>                           KeyValueVector;
	typedef std::pair<std::string, KeyValueVector>              IniSection;
	typedef std::vector<IniSection>                             IniData;

	IniData m_data;
	KeyValueVector m_defData;

	class MatchFirstString
	{
	public:
		MatchFirstString(std::string strFirst)
			: m_strFirst(strFirst)
		{

		}

		template <typename PairType>
		bool operator()(PairType keyvalue)
		{
			return 0 == _stricmp(keyvalue.first.c_str(), m_strFirst.c_str());
		}

	private:
		std::string m_strFirst;
	};
};

#include "StdAfx.h"
#include ".\inifile.h"

/********************************************************************
	created:	2010/04/28
	created:	28:4:2010   15:19
	filename:	IniFile.cpp
	author:		Benny
	
	purpose:	
*********************************************************************/

#include <algorithm>
#include <fstream>
#include <sstream>

CIniFile::CIniFile(void)
{
}

CIniFile::~CIniFile(void)
{
}

bool CIniFile::GetSectionName( size_t nSection, std::string& strSection ) const
{
	if (nSection<m_data.size())
	{
		strSection = m_data.at(nSection).first;
		return true;
	}
	return false;
}

bool CIniFile::GetKeyValue( size_t nSection, size_t nIndex, std::string& strKey, std::string& strValue ) const
{
	if (nSection<m_data.size())
	{
		const KeyValueVector& vec = m_data.at(nSection).second;
		if (nIndex<vec.size())
		{
			const KeyValuePair& keyvalue = vec.at(nIndex);
			strKey = keyvalue.first;
			strValue = keyvalue.second;
			return true;
		}
	}
	return false;
}

bool CIniFile::GetKeyValue( std::string strSection, size_t nIndex, std::string& strKey, std::string& strValue ) const
{
	IniData::const_iterator ite = std::find_if(m_data.begin(), m_data.end(), MatchFirstString(strSection));
	if (ite != m_data.end())
	{
		const KeyValueVector& vec = ite->second;
		if (nIndex<vec.size())
		{
			const KeyValuePair& keyvalue = vec.at(nIndex);
			strKey = keyvalue.first;
			strValue = keyvalue.second;
			return true;
		}
	}
	return false;
}

bool CIniFile::GetValue( size_t nSection, std::string strKey, std::string& strValue ) const
{
	if (nSection<m_data.size())
	{
		const KeyValueVector& vec = m_data.at(nSection).second;
		KeyValueVector::const_iterator ite = std::find_if(vec.begin(), vec.end(), MatchFirstString(strKey));
		if (ite != vec.end())
		{
			strValue = ite->second;
			return true;
		}
		return GetDefaultValue(m_data.at(nSection).first, strValue);
	}
	return false;
}

bool CIniFile::GetValue( std::string strSection, std::string strKey, std::string& strValue ) const
{
	IniData::const_iterator ite = std::find_if(m_data.begin(), m_data.end(), MatchFirstString(strSection));
	if (ite != m_data.end())
	{
		const KeyValueVector& vec = ite->second;
		KeyValueVector::const_iterator v_ite = std::find_if(vec.begin(), vec.end(), MatchFirstString(strKey));
		if (v_ite != vec.end())
		{
			strValue = v_ite->second;
			return true;
		}
	}
	return GetDefaultValue(strSection, strValue);
}
bool CIniFile::GetDefaultValue(std::string strSection, std::string& strValue) const
{
	KeyValueVector::const_iterator ite = std::find_if(m_defData.begin(), m_defData.end(), MatchFirstString(strSection));
	if (ite != m_defData.end())
	{
		strValue = ite->second;
		return true;
	}
	return false;
}

bool CIniFile::ReadFile( std::string strFileName )
{
	return ReadFile(strFileName.c_str());
}

bool CIniFile::ReadFile( const char* strFileName )
{
	m_data.clear();
	m_defData.clear();
	std::ifstream is;
	is.open(strFileName);
	if (is.is_open())
	{
		std::stringbuf buf;
		is.get(buf, '\x00'); // read all text, truncated before '\x00' or eof
		bool bOK = AnalyzeText(buf.str());
		if (bOK == false)
		{
			m_data.clear();
			m_defData.clear();
		}
		return bOK;
	}
	return false;

}
bool CIniFile::AnalyzeText( const std::string& strText )
{
	size_t nTextLen = strText.size();
	size_t nNextPos = 0;
	KeyValueVector* pLastKeyValueVector = NULL;
	while (nNextPos<nTextLen)
	{
		char c = strText.at(nNextPos);
		if (c == '\r' || c == '\n' || c == '\x00')
		{
			nNextPos++;
			continue;
		}

		size_t nLineEndPos = strText.find_first_of("\n\r", nNextPos);
		std::string strLine = strText.substr(nNextPos,//fetch one line
			std::string::npos == nLineEndPos ? std::string::npos : (nLineEndPos - nNextPos));
		if (strLine.size())
		{
			size_t nCommentBegin = strLine.find(';');
			strLine = std::string::npos == nCommentBegin ? strLine : strLine.substr(0, nCommentBegin);
			if (strLine.size())
			{
				size_t nSectionBegin = strLine.find('['); // try to find section
				if (std::string::npos != nSectionBegin)
				{
					nSectionBegin++;
					size_t nSectionEnd = strLine.find(']');
					if (std::string::npos == nSectionEnd || nSectionEnd<nSectionBegin)
					{
						ASSERT(FALSE);
						return false; // invalid file format, '['']' are not paired
					}
					std::string strSection = strLine.substr(nSectionBegin, nSectionEnd - nSectionBegin);
					if (strSection == INI_FILE_DEFAULT_SECTION_NAME)
					{
						pLastKeyValueVector = &m_defData;
					} 
					else
					{
						m_data.push_back(IniSection(strSection, KeyValueVector()));
						pLastKeyValueVector = &m_data.back().second;
					}
				}
				else
				{
					size_t nEqual = strLine.find('='); // try to find key value pair
					if (std::string::npos != nEqual)
					{
						if (NULL == pLastKeyValueVector)
						{
							ASSERT(FALSE);
							return false; // invalid file format, no section ahead
						}
						pLastKeyValueVector->push_back(KeyValuePair(strLine.substr(0, nEqual), strLine.substr(nEqual + 1)));
					}
					else
					{
						// no '['']''=', discarded
					}
				}
			}
		}
		if (std::string::npos == nLineEndPos)
			break;

		nNextPos = nLineEndPos + 1;
	}
	return true;
}
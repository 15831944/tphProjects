#pragma once
#include <iostream>
#include <fstream>

#define ITEMSEP ";\t"
class CFileOutput
{
public:
	CFileOutput(const CString& strFile)
	{
		outstream.open(strFile.GetString(),std::ios::app);
	}
	~CFileOutput()
	{
		outstream.close();
	}
	
	
	operator bool () const{ return !outstream.bad(); }

	CFileOutput& LogLine(const CString& str)
	{
		outstream << str.GetString() << std::endl;
		return *this;
	}

	CFileOutput& Line()
	{
		outstream << std::endl;
		return *this;
	}

	CFileOutput& Item()
	{
		outstream << ITEMSEP;
		return *this;
	}

	CFileOutput& Log(const CString& str)
	{
		outstream<< str.GetString() << ITEMSEP;		
		return *this;
	}

	template<class T>
		CFileOutput& Log(const T& t)
	{
		outstream<< t ;
		return *this;
	}

	CFileOutput& LogItem(const CString& str)
	{
		outstream<< str.GetString() << ITEMSEP;		
		return *this;
	}


	template<class T>
	CFileOutput& LogItem(const T& t)
	{
		outstream<< t << ITEMSEP;
		return *this;
	}

	//log source code debug line
	CFileOutput& LogSourceLine(const CString& file, int nLine)
	{
		CString fileName;
		int nPos = file.ReverseFind('\\');

		if(nPos>=0)
		{
			fileName = file.Mid(nPos+1);
		}
		else
			fileName = file;
		return LogItem(fileName.MakeLower()).LogItem(nLine);
	}

	std::ofstream outstream;

	static void Clear(const CString& strFile)
	{ 
		std::ofstream outstream;
		outstream.open(strFile.GetString());			
		outstream.close();
	}
};
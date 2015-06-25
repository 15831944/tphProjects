#pragma once

#include <boost/array.hpp>
#include <vector>
#include <set>


#include "CommonDll.h"
class ArctermFile;
class ALTObjectIDList;

#define OBJECT_STRING_LEVEL 4
class COMMON_TRANSFER ALTObjectID{
public:
	typedef boost::array<std::string, OBJECT_STRING_LEVEL> val_type;

	ALTObjectID();
	ALTObjectID(const char * );
	ALTObjectID(const ALTObjectID&);
	~ALTObjectID();

	//ALTObjectID(const ProcessorID& procID);	
	CString GetIDString()const;
	int idLength()const;
	BOOL readALTObjectID(ArctermFile& file);
	int writeALTObjectID (ArctermFile& file) const;
	void printID (char* buf, const char *sep = "-") const;
	BOOL idFits (const ALTObjectID& checkID) const; //detail fit  not detail


	void FromString( const char * strID );
	ALTObjectID& operator =(const ALTObjectID& );
	bool operator == (const  ALTObjectID& )const;
	bool operator < (const ALTObjectID& )const;
	
	std::string& at(size_t i);
	const std::string& at(size_t i) const;

	bool IsFits(const ALTObjectIDList& )const;
	bool IsBlank()const;

	ALTObjectID GetNext()const;

	static std::string GetNextString(const std::string& from);

public:
	val_type m_val; 
};


typedef std::set<CString> SortedStringList;

class COMMON_TRANSFER ALTObjectIDList : public std::vector<ALTObjectID>{
public:
	ALTObjectIDList();
	~ALTObjectIDList();
	void Add(const ALTObjectID& newID);
	void DeleteItem(int nidx);

	void push_back(const ALTObjectID& _Val);

	ALTObjectID& at(size_type i);
	const ALTObjectID& at(size_type i)const;

	const size_t size()const;
	void clear();

	size_t GetLevel1StringList(SortedStringList& strlist)const;
	size_t GetLevel2StringList(const CString& level1str,SortedStringList& strlist)const;
	size_t GetLevel3StringList(const CString& level1str,const CString& level2str,SortedStringList& strlist)const;
	size_t GetLevel4StringList(const CString& level1str,const CString& level2str,const CString& level3str, SortedStringList& strlist)const;


	size_t GetStringListAtLevel(int nLevel ,SortedStringList& nameList);
};

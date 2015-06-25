#pragma once
//////////////////////////////////////////////////////////////////////////
//Author : cjchen
//DATE : 4/20/2008
//////////////////////////////////////////////////////////////////////////
#ifndef _SECTOR_SELECTE_DOC_H
#define _SECTOR_SELECTE_DOC_H
#include "../InputAirside/ALTObject.h"
class CTree_Node
{
public:
	TCHAR name[1024] ;
	int  id ;
public:
	CTree_Node():id(0) { memset(name , 0 ,sizeof(TCHAR) * 1024 ) ; };
};
class CSectorSelectDoc
{
public:
	typedef std::vector<ALTObject>  SECTOR_VECTOR ;
	typedef SECTOR_VECTOR::const_iterator ITERATOR_SECTOR_CONST ;
    typedef  SECTOR_VECTOR&   SECTOR_REF ;
private:
     void initSectors();
private:
	int  m_projectID ; 
	SECTOR_VECTOR sector ;
public:
    SECTOR_REF  getSectors() ;
	CSectorSelectDoc(int projid);
	void setProjectId(int proid) ;
    ~CSectorSelectDoc(void);
};
#endif


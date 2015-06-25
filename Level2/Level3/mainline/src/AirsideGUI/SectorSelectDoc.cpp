#include "StdAfx.h"
#include ".\sectorselectdoc.h"

CSectorSelectDoc::CSectorSelectDoc(int projid): m_projectID(projid)
{
	initSectors();
}

CSectorSelectDoc::~CSectorSelectDoc(void)
{
}
void CSectorSelectDoc::initSectors()
{
	ALTObject::GetObjectList(ALT_SECTOR,m_projectID,sector);
}
 std::vector<ALTObject>&   CSectorSelectDoc::getSectors() 
{
	return sector ;
}
 void CSectorSelectDoc::setProjectId(int proid)
 {
	 m_projectID = proid ;
 }
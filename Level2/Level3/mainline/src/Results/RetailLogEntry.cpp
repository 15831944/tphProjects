#include "StdAfx.h"
#include ".\retaillogentry.h"
#include "CommonData\PROCID.H"

CRetailLogEntry::CRetailLogEntry(void)
{
}

CRetailLogEntry::~CRetailLogEntry(void)
{
}

void CRetailLogEntry::SetID( const ProcessorID& _strID )
{
	for(int i=0;i<MAX_PROC_IDS;i++)
		item.name[i] = _strID.getIdIdxAtLevel(i); 
	//strcpy( item.name, _strID );
}
//
//CString CRetailLogEntry::GetID() const
//{
//	return item.name;
//}

void CRetailLogEntry::SetIndex( int idx )
{
	item.indexNum = idx;
}

int CRetailLogEntry::GetIndex() const
{
	return item.indexNum;
}

void CRetailLogEntry::SetProcessorType( char ch )
{
	item.procType = ch;
}

char CRetailLogEntry::GetProcessorType() const
{
	return item.procType;
}

void CRetailLogEntry::SetSubType( char ch )
{
	item.subType = ch;
}

char CRetailLogEntry::GetSubType() const
{
	return item.subType;
}

void CRetailLogEntry::echo( ofsstream& p_stream, const CString& _csProjPath ) const
{

}

void CRetailLogEntry::SetProcIndex( int procIndex )
{
	item.procIndex = procIndex;
}

int CRetailLogEntry::GetProcIndex() const
{
	return item.procIndex;
}



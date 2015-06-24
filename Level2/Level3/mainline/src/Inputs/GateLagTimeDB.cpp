// GateLagTimeDB.cpp: implementation of the CGateLagTimeDB class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GateLagTimeDB.h"
#include "GageLagTimeData.h"
#include "common\termfile.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGateLagTimeDB::CGateLagTimeDB()
{

}

CGateLagTimeDB::~CGateLagTimeDB()
{

}
void CGateLagTimeDB::WriteToFile( ArctermFile& _file )const
{
	int iCount = this->getCount();
	_file.writeInt( iCount );
	_file.writeLine();

	for( int i=0; i<iCount; ++i )
	{
		CGageLagTimeData* pData = (CGageLagTimeData*)getItem( i );
		pData->WriteToFile( _file  );
	}
	
}

void CGateLagTimeDB::ReadFromFile( ArctermFile& _file , InputTerminal* _pInTerm)
{
	clear();
	int iCount;
	_file.getLine();
	
	_file.getInteger( iCount );
	for( int i=0; i<iCount; ++i )
	{
		CGageLagTimeData* pData = new CGageLagTimeData();
		pData->ReadFromFile( _file, _pInTerm );
		this->addItem( pData );
	}
	buildHierarchy();
	
}

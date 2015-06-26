#include "StdAfx.h"
#include ".\bridgeconnectorlogentry.h"
#include "CommonData\PROCID.H"

CBridgeConnectorLogEntry::CBridgeConnectorLogEntry(void)
:TerminalLogItem<BridgeConnectorDescStruct,BridgeConnectorEventStruct>()
{
}

CBridgeConnectorLogEntry::~CBridgeConnectorLogEntry(void)
{
}

void CBridgeConnectorLogEntry::SetID( const ProcessorID&  id )
{
	memset(item.name, 0, MAX_PROC_IDS * sizeof(int));

	for(int i=0;i<id.idLength();i++)
		item.name[i] = id.getIdIdxAtLevel(i); 

	//strcpy( item.name, _strID );
}
//void CBridgeConnectorLogEntry::getIDs (int *p_ids) const
//{
//	for (int i = 0; i < MAX_PROC_IDS; i++)
//		p_ids[i] = item.name[i];
//}
//
//void CBridgeConnectorLogEntry::getIDs (short *p_ids) const
//{
//	for (int i = 0; i < MAX_PROC_IDS; i++)
//		p_ids[i] = item.name[i];
//}

//void CBridgeConnectorLogEntry::getID (char *p_str) const
//{
//	assert( m_pOutTerm );
//	strcpy (p_str, m_pOutTerm->outStrDict->getString (item.name[0]));
//	for (int i = 1; i < MAX_PROC_IDS && item.name[i] != EMPTY_ID; i++)
//	{
//		strcat (p_str, "-");
//		strcat (p_str, m_pOutTerm->outStrDict->getString (item.name[i]));
//	}
//}

//void CBridgeConnectorLogEntry::setIDs (const int *p_ids)
//{
//	for (int i = 0; i < MAX_PROC_IDS; i++)
//		item.name[i] = (short)p_ids[i];
//}


void CBridgeConnectorLogEntry::SetStartTime( const ElapsedTime& _time )
{
	item.startTime = (long)_time;
}

void CBridgeConnectorLogEntry::SetEndTime( const ElapsedTime& _time )
{
	item.endTime = (long)_time;
}

ElapsedTime CBridgeConnectorLogEntry::GetStartTime() const
{
	ElapsedTime t;
	t.setPrecisely( item.startTime );
	return t;	
}

ElapsedTime CBridgeConnectorLogEntry::GetEndTime() const
{
	ElapsedTime t;
	t.setPrecisely( item.endTime );
	return t;	
}
void CBridgeConnectorLogEntry::SetIndex( long _lIdx )
{
	item.indexNum = _lIdx;
}
long CBridgeConnectorLogEntry::GetIndex()const
{
	return item.indexNum ;
}
void CBridgeConnectorLogEntry::echo (ofsstream& p_stream, const CString& _csProjPath) const
{
	assert( m_pOutTerm );
	p_stream	<< item.name << ',' 
		<< GetStartTime() << ',' 
		<< GetEndTime()   << '\n';
}

void CBridgeConnectorLogEntry::SetLocation( const Point& ptLocation )
{
	item.xLocation = ptLocation.getX();
	item.yLocation = ptLocation.getY();
	item.zLocation = ptLocation.getZ();

}

void CBridgeConnectorLogEntry::SetWidth( double dWidth )
{
	item.dWidth = dWidth;
}

void CBridgeConnectorLogEntry::SetLength( double dLength )
{
	item.dLength = dLength;
}

void CBridgeConnectorLogEntry::SetDirFromTo( const Point& ptDirFrom, const Point& ptDirTo ) /*long startPos*/
{
	item.xDirFrom = ptDirFrom.getX();
	item.yDirFrom = ptDirFrom.getY();
	item.zDirFrom = ptDirFrom.getZ();


	item.xDirTo = ptDirTo.getX();
	item.yDirTo = ptDirTo.getY();
	item.zDirTo = ptDirTo.getZ();


}
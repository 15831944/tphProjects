// CollisonGrid.cpp: implementation of the CCollisonGrid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CollisonGrid.h"
#include "common\states.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollisonGrid::CCollisonGrid()
{

}

CCollisonGrid::~CCollisonGrid()
{
	m_vPersonPos.clear();
}
void CCollisonGrid::AddPerson( PAXPOSE& _personInfo )
{
	m_vPersonPos.push_back( _personInfo );
}
void CCollisonGrid::WritePersonInGrid( ArctermFile& p_file , ElapsedTime& _time ,const CMobileElemConstraint& paxType,Terminal* _pTerm)
{
	int iSize = m_vPersonPos.size();
	if( iSize<2 )
		return ;
	CString sPaxStr;
	//char sPaxStr [64];
	paxType.screenPrint(sPaxStr);

	CString sFirstPaxId;
	ReportPaxEntry element, anotherElement;
	_pTerm->paxLog->getItem( element , m_vPersonPos[0].m_iPaxIdx );
	element.SetOutputTerminal( _pTerm );
	element.SetEventLog( _pTerm->m_pMobEventLog );  
	int groupMember = element.getGroupMember();
	int iState = m_vPersonPos[0].m_iState;
	sFirstPaxId.Format( "%ld",element.getID() );
	CString sSecondPaxId;
	for(int i=1; i < iSize ; ++i )
	{
		
		if(element.fits(paxType))
		{
			_pTerm->paxLog->getItem( anotherElement , m_vPersonPos[i].m_iPaxIdx );
			anotherElement.SetOutputTerminal( _pTerm );
			anotherElement.SetEventLog( _pTerm->m_pMobEventLog );  
			int iOtherState = m_vPersonPos[i].m_iState;
			if( iState == WaitInQueue && iState == iOtherState 
				|| iState == WaitForBaggage && iState == iOtherState )

				continue;
			
			if(anotherElement.isGreeter())					//filter0.9: no greeter/pax collision.
			{
				if( anotherElement.getOwnerID() == element.getID() )
						continue;
			}
			else
			{
				if(element.isGreeter())
					if(element.getOwnerID() == anotherElement.getID())
						continue;
			}
					
			int groupMember_tmp = anotherElement.getGroupMember();	//filter0.95: no pax collision in a same group
			if(anotherElement.getID() - element.getID() == groupMember_tmp - groupMember)
			continue;

			/*
			p_file.writeField( sPaxStr );
			p_file.writeTime( _time ,TRUE );
			p_file.writeField( sFirstPaxId );	
			sSecondPaxId.Format( "%ld",anotherElement.getID() );
			p_file.writeField( sSecondPaxId );
			p_file.writeLine();	
			*/
		}		
	}						 	
}
void CCollisonGrid::WritePersonInOtherGrid( CCollisonGrid& _otherGrid,ArctermFile& p_file , ElapsedTime& _time ,const CMobileElemConstraint& paxType,Terminal* _pTerm,DistanceUnit _gridWidth)
{
	int iOtherSize = _otherGrid.m_vPersonPos.size();
	if( iOtherSize < 1 )
		return ;

	//char sPaxStr [64];
	CString  sPaxStr;
	paxType.screenPrint(sPaxStr);
	int iSize = m_vPersonPos.size();
	for( int i=0 ; i<iSize ; ++i ) 
	{
		ReportPaxEntry element;
		_pTerm->paxLog->getItem( element , m_vPersonPos[i].m_iPaxIdx );
		element.SetOutputTerminal( _pTerm );
		element.SetEventLog( _pTerm->m_pMobEventLog );  
	
		if(element.fits(paxType))
		{
			int iState = m_vPersonPos[i].m_iState;
			int groupMember = element.getGroupMember();
			Point p1( m_vPersonPos[i].m_x,m_vPersonPos[i].m_y,0 );
			for( int j=0; j<iOtherSize; ++j )
			{
				ReportPaxEntry anotherElement;
				_pTerm->paxLog->getItem( anotherElement , m_vPersonPos[i].m_iPaxIdx );
				anotherElement.SetOutputTerminal( _pTerm );
				anotherElement.SetEventLog( _pTerm->m_pMobEventLog );  

				int iOtherState = m_vPersonPos[i].m_iState;
				if( iState == WaitInQueue && iState == iOtherState 
					|| iState == WaitForBaggage && iState == iOtherState )
						continue;
			
				if(anotherElement.isGreeter())					//filter0.9: no greeter/pax collision.
				{
					if(element.getID() == anotherElement.getOwnerID())
						continue;
				}
				else
				{
					if(element.isGreeter())
						if(anotherElement.getID() == element.getOwnerID())
							continue;
				}
						
				int groupMember_tmp = anotherElement.getGroupMember();	//filter0.95: no pax collision in a same group
				if(anotherElement.getID() - element.getID() == groupMember_tmp - groupMember)
					continue;
					
				Point p2( _otherGrid.m_vPersonPos[j].m_x,_otherGrid.m_vPersonPos[j].m_y,0 );
				if( p1.distance(p2) <= _gridWidth )
				{
					/*
					p_file.writeField( sPaxStr );
					p_file.writeTime( _time ,TRUE);			
			
					CString sFirstPaxId;
					
					sFirstPaxId.Format( "%ld",element.getID() );
					p_file.writeField( sFirstPaxId );

					CString sSecondPaxId;
					//_pTerm->paxLog->getItem( element , _otherGrid.m_vPersonPos[j].m_iPaxIdx );
					sSecondPaxId.Format( "%ld",anotherElement.getID() );
					p_file.writeField( sSecondPaxId );
					p_file.writeLine();	
*/

				}
			}
		}
	}
}
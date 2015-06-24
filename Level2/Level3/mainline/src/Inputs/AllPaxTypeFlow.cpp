i// AllPaxTypeFlow.cpp: implementation of the CAllPaxTypeFlow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "AllPaxTypeFlow.h"

#include "PaxFlowConvertor.h"

#include<algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAllPaxTypeFlow::CAllPaxTypeFlow()
{

}

CAllPaxTypeFlow::~CAllPaxTypeFlow()
{
	ClearAll();
}
void CAllPaxTypeFlow::ClearAll()
{
	ALLPAXFLOW::iterator iter = m_vAllPaxFlow.begin();
	ALLPAXFLOW::iterator iterEnd = m_vAllPaxFlow.end();
	for(; iter != iterEnd; ++iter) 
	{
		delete *iter;
	}
	m_vAllPaxFlow.clear();
}
CAllPaxTypeFlow::CAllPaxTypeFlow(const CAllPaxTypeFlow& _another )
{
	ALLPAXFLOW::const_iterator iter = _another.m_vAllPaxFlow.begin();
	ALLPAXFLOW::const_iterator iterEnd = _another.m_vAllPaxFlow.end();
	for(; iter != iterEnd; ++iter) 
	{
		CSinglePaxTypeFlow* pFlow = new CSinglePaxTypeFlow( **iter );
		m_vAllPaxFlow.push_back( pFlow );
	}
}
CAllPaxTypeFlow& CAllPaxTypeFlow::operator = ( const CAllPaxTypeFlow& _another )
{
	if( this != & _another )
	{
		ALLPAXFLOW::iterator iter = m_vAllPaxFlow.begin();
		ALLPAXFLOW::iterator iterEnd = m_vAllPaxFlow.end();
		for(; iter != iterEnd; ++iter) 
		{
			delete *iter;
		}
		m_vAllPaxFlow.clear();

		ALLPAXFLOW::const_iterator iterConst = _another.m_vAllPaxFlow.begin();
		ALLPAXFLOW::const_iterator iterEndConst = _another.m_vAllPaxFlow.end();
		for(; iterConst != iterEndConst; ++iterConst) 
		{
			CSinglePaxTypeFlow* pFlow = new CSinglePaxTypeFlow( **iterConst );
			m_vAllPaxFlow.push_back( pFlow );
		}
	}

	return *this;
	
}
void CAllPaxTypeFlow::DeleteSinglePaxTypeFlowAt( int _iIdx ,bool _bAutoSort )
{
	ASSERT( _iIdx >= 0 && _iIdx < static_cast<int>(m_vAllPaxFlow.size()) );
	delete m_vAllPaxFlow[_iIdx];
	m_vAllPaxFlow.erase( m_vAllPaxFlow.begin() + _iIdx );
	if( _bAutoSort )
	{
		Sort();
		InitHierachy();
	}	
}
void CAllPaxTypeFlow::ReplaceSinglePasFlowAt( int _iIdx, CSinglePaxTypeFlow* _pNewPaxFlow )
{
	ASSERT( _iIdx >= 0 && _iIdx < static_cast<int>(m_vAllPaxFlow.size()) );
	delete m_vAllPaxFlow[ _iIdx ];
	m_vAllPaxFlow[ _iIdx ] = _pNewPaxFlow;
}
void CAllPaxTypeFlow::AddPaxFlow( CSinglePaxTypeFlow* _pNewPaxFlow ,bool _bAutoBuildHiearachy )
{
	//_pNewPaxFlow->SetChangedFlag( true );
	m_vAllPaxFlow.push_back( _pNewPaxFlow );
	if( _bAutoBuildHiearachy )
	{
		Sort();
		InitHierachy();
	}
}
/************************************************************************
FUNCTION: Convent the old struction(distribution) into diagraph structure(singpaxTypeFlow)
INPUT	: _convertor, the convert tool                                                                      
/************************************************************************/
void CAllPaxTypeFlow::FromOldToDigraphStructure( CPaxFlowConvertor& _convertor )
{
	_convertor.ToDigraphStructure( *this );
}
void CAllPaxTypeFlow::FromDigraphToOldStructure( CPaxFlowConvertor& _convertor )
{
	_convertor.ToOldStructure( *this );
}
const CSinglePaxTypeFlow* CAllPaxTypeFlow::GetSinglePaxTypeFlowAt( int _iIdx ) const 
{
	ASSERT( _iIdx >= 0 && _iIdx < static_cast<int>(m_vAllPaxFlow.size()) );
	return  m_vAllPaxFlow[ _iIdx ];
}
CSinglePaxTypeFlow* CAllPaxTypeFlow::GetSinglePaxTypeFlowAt( int _iIdx )
{
	ASSERT( _iIdx >= 0 && _iIdx < static_cast<int>(m_vAllPaxFlow.size()) );
	return  m_vAllPaxFlow[ _iIdx ];
}
void CAllPaxTypeFlow::BuildHierarchyFlow( CPaxFlowConvertor& _convertor , int _iIdx, CSinglePaxTypeFlow& _resultFlow )
{
	ASSERT( _iIdx >= 0 && _iIdx < static_cast<int>(m_vAllPaxFlow.size()) );
	_convertor.BuildHierarchyFlow( *m_vAllPaxFlow[_iIdx], _resultFlow );
}
void CAllPaxTypeFlow::BuildSinglePaxFlow( CPaxFlowConvertor& _convertor, int _iOldFlowIdx,const CSinglePaxTypeFlow& _compareFlow ,CSinglePaxTypeFlow& _resultFlow ) const
{
	ASSERT( _iOldFlowIdx >= 0 && _iOldFlowIdx < static_cast<int>(m_vAllPaxFlow.size()) );
	_convertor.BuildSinglePaxFlow( *m_vAllPaxFlow[_iOldFlowIdx],_compareFlow,_resultFlow );
}
bool SingleFlowSort( const CSinglePaxTypeFlow* _first, const CSinglePaxTypeFlow* _second )
{
	return (*_first) < (*_second);
}
void CAllPaxTypeFlow::Sort()
{
	std::sort( m_vAllPaxFlow.begin(), m_vAllPaxFlow.end() ,SingleFlowSort );
}

void CAllPaxTypeFlow::InitHierachy()
{
	//char nouse[128];
	int iSingleFlowCount = m_vAllPaxFlow.size();
	for( int i=0; i<iSingleFlowCount; ++i )
	{
		const CSinglePaxTypeFlow* pParentPaxCon =NULL;
		const CMobileElemConstraint* pPaxCon = m_vAllPaxFlow[i]->GetPassengerConstrain();
		for( int j=i+1; j<iSingleFlowCount; ++j )
		{
			 const CMobileElemConstraint* pTester = m_vAllPaxFlow[j]->GetPassengerConstrain();
			 ASSERT( pTester );
	//		 pPaxCon->screenPrint( nouse );
	//		 // TRACE("paxcon %s\n",  nouse );
			 
	//		 pTester->screenPrint( nouse );
	//		 // TRACE("tester %s\n",  nouse );
			 if(  pTester->fitsparent( * pPaxCon ) )
			 {
				 pParentPaxCon =  m_vAllPaxFlow[j];
				 break;
			 }    
		}
		m_vAllPaxFlow[i]->SetParentPaxFlow( pParentPaxCon );
		
	}
}
CSinglePaxTypeFlow* CAllPaxTypeFlow::GetClosestFlow( const CMobileElemConstraint* _constraint )
{
	//char str[256];
	//_constraint->screenPrint( str,0,256 );
///	// TRACE("%s\n  ", str);
	
	int iSingleFlowCount = m_vAllPaxFlow.size();
	for( int i=0; i<iSingleFlowCount; ++i )
	{		
		const CMobileElemConstraint* pPaxCon = m_vAllPaxFlow[i]->GetPassengerConstrain();
		//pPaxCon->screenPrint( str,0,256 );
//		// TRACE("\n%s  ", str);
		if(  pPaxCon->fitsparent( * _constraint ) )
			 {
				 return m_vAllPaxFlow[ i ];		 
			 }    
	}

	return NULL;
}
int CAllPaxTypeFlow::GetSingleFlowIndex ( CString _sPrintString ) const 
{
	int iIdx = -1;
	int iCount = m_vAllPaxFlow.size();
	//char tempS [ 256 ]; matt
	char tempS [ 256 ];
	for(int i=0; i<iCount ; ++i ) 
	{
		m_vAllPaxFlow[ i ]->GetPassengerConstrain()->WriteSyntaxStringWithVersion( tempS );
		if( _sPrintString.Compare( tempS ) )
			return i;			
	}

	return -1;
	
}
void CAllPaxTypeFlow::PrintAllStructure()const
{
	//char sOutStr[256]; matt
	//char sOutStr[2560];
	CString sOutStr;
	CString sTrace;
	ALLPAXFLOW::const_iterator iterConst = m_vAllPaxFlow.begin();
	ALLPAXFLOW::const_iterator iterEndConst = m_vAllPaxFlow.end();
	for(; iterConst != iterEndConst; ++iterConst) 
	{
		//(*iterConst)->GetPassengerConstrain()->WriteSyntaxString( sOutStr);
		(*iterConst)->GetPassengerConstrain()->screenPrint(sOutStr,0,256);
		sTrace = " PaxType=" ;
		sTrace += sOutStr;
//		// TRACE("***********\n");
//		// TRACE("%s\n", sTrace );
//		(*iterConst)->PrintAllStructure();
//		// TRACE("***********\n");
	}
}
bool CAllPaxTypeFlow::IfAllFlowValid( bool _bOnlyCheckChangedFlow ) const
{
	ALLPAXFLOW::const_iterator iterConst = m_vAllPaxFlow.begin();
	ALLPAXFLOW::const_iterator iterEndConst = m_vAllPaxFlow.end();
	for(; iterConst != iterEndConst; ++iterConst) 
	{
		if( _bOnlyCheckChangedFlow )
		{
			if( (*iterConst)->IfFlowBeChanged() )
			{
				if( !(*iterConst)->IfFlowValid() )
					return false;
			}
		}
		else
		{
			if( !(*iterConst)->IfFlowValid() )
				return false;
		}
		
	}
	return true;
}
int CAllPaxTypeFlow::GetIdxIfExist( const CSinglePaxTypeFlow& _testFlow ) const
{
	for( unsigned i=0; i<m_vAllPaxFlow.size(); ++i )
	{
		if( *( m_vAllPaxFlow[i]->GetPassengerConstrain() ) == *( _testFlow.GetPassengerConstrain() ) )
		{
			return i;
		}
	}
	return -1;
}
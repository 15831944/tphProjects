// FLowChannelList.cpp: implementation of the CFLowChannelList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FLowChannelList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFLowChannelList::CFLowChannelList()
{
	
}

CFLowChannelList::~CFLowChannelList()
{
	while (m_stackChannelNum.size() > 0)
	{
		CFlowChannel* popChannel =  m_stackChannelNum.back();
		m_stackChannelNum.pop_back();
		delete popChannel;
	}
}

CFlowChannel* CFLowChannelList::PopChannel()
{
	if (m_stackChannelNum.size() > 0)
	{
		CFlowChannel* popChannel =  m_stackChannelNum.back();
		m_stackChannelNum.pop_back();
		return popChannel;
	}
	else
	{
		return NULL;
	}
}

/***********************************************************************                                              
 return only one destination processor which OneToOne with m_pInProcessor when leave channel.
 return NULL if no one can fit OneToOne with m_pInProcessor 

 _destProcessorArray : The all destination processor of the channel's tail
 _nDestIdLength      : The destionation processorID's Length (The processorID is defined in passenger flow);
************************************************************************/
Processor* CFlowChannel::GetOutChannelProc(const ProcessorArray& _destProcessorArray,int _nDestIdLength ) const
{
	return CFlowChannel::GetOutChannelProc(*m_pInProcessor,_destProcessorArray,_nDestIdLength);
}


Processor* CFlowChannel::GetOutChannelProc(const Processor& _pSourceProcessor, const ProcessorArray& _destProcessorArray,int _nDestIdLength )
{
	// TRACE( "-%s-=\n", _pSourceProcessor.getID()->GetIDString());
	ProcessorArray arrayResult;
	ProcessorArray arrayCandidated( _destProcessorArray );
	int nDeltaFromLeaf = 0;
	
	while( arrayCandidated.getCount() > 0 )
	{
		// move 
		int nCount = arrayCandidated.getCount();
		if( nCount == 1 )
			// A-1  ->  G-1-2 is ok
			return arrayCandidated.getItem( 0 );
		
		ProcessorArray arrayTemp( arrayCandidated );
		arrayCandidated.clear();
		char szSourceLevelName[128];
		int nSourceIdLength = _pSourceProcessor.getID()->idLength();
		nSourceIdLength -= nDeltaFromLeaf + 1;
		if( nSourceIdLength == 0 )
			break;	// stop on the first level.
		_pSourceProcessor.getID()->getNameAtLevel( szSourceLevelName, nSourceIdLength );
		nCount = arrayTemp.getCount();
		for( int i=0; i<nCount; i++ )
		{
			Processor* pProc = arrayTemp.getItem( i );

//			// TRACE( "id = %s--\n", pProc->getID()->GetIDString() );
			
			int nThisIdLenght = pProc->getID()->idLength();
			nThisIdLenght -= nDeltaFromLeaf;
			
			if( nThisIdLenght <= _nDestIdLength )
			{
				// satisfy the checking. add to the result array.
								// TRACE( "-arrayResult = %s--\n", pProc->getID()->GetIDString() );
				arrayResult.addItem( pProc );
			}
			else
			{
				char szLevelName[128];
				pProc->getID()->getNameAtLevel( szLevelName, nThisIdLenght-1 );
				if( strcmp( szLevelName, szSourceLevelName ) == 0 )
				{
					// put into the candidate list.
//										// TRACE( "-arrayCandidated = %s--\n", pProc->getID()->GetIDString() );
					arrayCandidated.addItem( pProc );
				}
			}
		}
		
		if( arrayCandidated.getCount() == 0 )
		{
			return NULL;
		}
		nDeltaFromLeaf++;
	}
	int nCount = arrayResult.getCount();
	//	if( nCount == 0 && nCount>1)
	if( nCount == 0 || nCount > 1 )
		return NULL;
	return arrayResult.getItem(0);	
}
void CFLowChannelList::InitFlowChannelList(const CFLowChannelList& flowChannelList)
{
	int nCount = static_cast<int>(flowChannelList.m_stackChannelNum.size());
	
	for (int i=0; i < nCount; i++)
	{
		CFlowChannel *flowChannel = new CFlowChannel(flowChannelList.m_stackChannelNum[i]->GetInChannelProc());
		m_stackChannelNum.push_back(flowChannel);
	}


}
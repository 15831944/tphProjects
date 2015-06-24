// ProcessorDistributionWithPipe.cpp: implementation of the CProcessorDistributionWithPipe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcessorDistributionWithPipe.h"
#include "common\termfile.h"
#include "common\exeption.h"
#include "in_term.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessorDistributionWithPipe::CProcessorDistributionWithPipe( bool _bReadProcessorDistribution )
{
	m_bReadProcessorDistribution = _bReadProcessorDistribution;
	m_nCurDestIndex = -1;
}

CProcessorDistributionWithPipe::~CProcessorDistributionWithPipe()
{

}
CProcessorDistributionWithPipe::CProcessorDistributionWithPipe( const CProcessorDistributionWithPipe& _otherDistribution )
{
	m_bReadProcessorDistribution=_otherDistribution.m_bReadProcessorDistribution;
	m_nCurDestIndex=-1;
	m_vPipeIndex.assign(_otherDistribution.m_vPipeIndex.begin(),_otherDistribution.m_vPipeIndex.end());
	initDestinations( _otherDistribution.destinations, _otherDistribution.probs,_otherDistribution.count);

}
void CProcessorDistributionWithPipe::initDestinations (const ProcessorID *p_dests,
														const double *p_percents, 
														int p_count)
{
	/*
	m_vPipeName.clear();
	int nSize = m_vPipeName.size();
	for( int i=0; i<nSize; i++ )
	{
		m_vPipeName.push_back( CPipeNameList() );
	}
*/
	ProcessorDistribution::initDestinations( p_dests, p_percents, p_count );
}

void CProcessorDistributionWithPipe::readDistribution (ArctermFile& p_file, InputTerminal* _pInTerm)
{
	float fltVersion=p_file.getVersion();;
	if( fltVersion < 2.6 ||fltVersion == 2.6)//fltVersion==2.5
	{
		readObsoleteDistribution25(p_file,_pInTerm);
		return;
	}

	if( m_bReadProcessorDistribution )
	{
		ProcessorDistribution::readDistribution( p_file, _pInTerm );
		int nDestCount = getCount();
		for( int j=0; j<nDestCount; ++j )
		{
			CFlowItemEx temp;
			m_vPipeIndex.push_back( temp );
		}
	}
	else
	{
		//p_file.getLine();
	    char temp[256];
	    p_file.getField (temp, 256);

	    // create ProbabilityDistribution
	    //double aFloat;
	  //  int maxCount = p_file.countColumns();
	    //ProcessorID *destList = new ProcessorID[maxCount];
	    //unsigned char *probabilities = new unsigned char[maxCount];
		int iCount =0;
		int iDestCount=0;
		p_file.getInteger( iCount );
		p_file.getInteger( iDestCount);
		ProcessorID *destList = new ProcessorID[iDestCount];
		double *probabilities = new double[iCount];
		
	    //unsigned char *probabilities = new unsigned char[maxCount];
	    // read processor ID / percentage pairs until invalid field found
		double aFloat;
	    for (int i = 0; i<iDestCount; i ++)
	    {
			p_file.getLine();
			destList[i].SetStrDict( _pInTerm->inStrDict );
	        destList[i].readProcessorID (p_file);
			if(i< iCount)
			{
				p_file.getFloat (aFloat);
				probabilities[i] = (int) (aFloat * 100.0 + 0.5);
			}
			else
			{
				p_file.getFloat (aFloat);//placeholder data , no use.
			}

			CFlowItemEx temp;
			temp.ReadData( p_file );
			m_vPipeIndex.push_back( temp );
			//int iPipeCount =-1;
		//	p_file.getInteger( iPipeCount );
	    }
		ProcessorDistribution::initDestinations (destList, iDestCount,probabilities, iCount);		
	    delete [] destList;
	    delete [] probabilities;
	}
}

void CProcessorDistributionWithPipe::readDistribution( ArctermFile& p_file )
{
	ASSERT(false);
}
void CProcessorDistributionWithPipe::readObsoleteDistribution25 (ArctermFile& p_file, InputTerminal* _pInTerm)
{
	if( m_bReadProcessorDistribution )
	{
		ProcessorDistribution::readDistribution( p_file, _pInTerm );
		int nDestCount = getCount();
		for( int j=0; j<nDestCount; ++j )
		{
			CFlowItemEx temp;
			m_vPipeIndex.push_back( temp );
		}
	}
	else
	{
		//p_file.getLine();
		char temp[256];
		p_file.getField (temp, 256);
		
		// create ProbabilityDistribution
		//double aFloat;
		//  int maxCount = p_file.countColumns();
		//ProcessorID *destList = new ProcessorID[maxCount];
		//unsigned char *probabilities = new unsigned char[maxCount];
		int iCount =0;
		p_file.getInteger( iCount );
		ProcessorID *destList = new ProcessorID[iCount];
		double *probabilities = new double[iCount];
		
		//unsigned char *probabilities = new unsigned char[maxCount];
		// read processor ID / percentage pairs until invalid field found
		double aFloat;
		for (int i = 0; i<iCount; i ++)
		{
			p_file.getLine();
			destList[i].SetStrDict( _pInTerm->inStrDict );
			destList[i].readProcessorID (p_file);
			p_file.getFloat (aFloat);
			probabilities[i] = (int) (aFloat * 100.0 + 0.5);
			CFlowItemEx temp;
			temp.ReadData( p_file );
			m_vPipeIndex.push_back( temp );
			//int iPipeCount =-1;
			//	p_file.getInteger( iPipeCount );
		}
		initDestinations (destList, probabilities, iCount);		
		delete [] destList;
		delete [] probabilities;
	}
}
void CProcessorDistributionWithPipe::writeDistribution (ArctermFile& p_file) const
{
    p_file.writeField (getProbabilityName());
	p_file.writeInt( count );
	//  [7/5/2004]p_file.writeInt( m_nDestCount);
	p_file.writeInt( GetDestCount());
		
	p_file.writeLine();

	//int nEvenPercent=0;
	//if( GetDestCount()>getCount())
	//{
	//	nEvenPercent=probs[getCount()-1]/(GetDestCount()-getCount());
	//}
	int nPipeSize = m_vPipeIndex.size();
	ASSERT( nPipeSize == GetDestCount() );	
    for (int i = 0; i < GetDestCount(); i++)
    {
        destinations[i].writeProcessorID (p_file);
///		TRACE("%d\n",probs[i] );
/*
		if(GetDestCount() == getCount())
		{
			p_file.writeFloat ((double)(probs[i]) / 100.0, 2);	
		}
		else //DestCount>getCount(), it's said that some destprocs is not appointed percent.
		{
			if(i>=getCount()-1)
			{
				if(i==GetDestCount()-1)
				{//ensure remain percent == probs[getCount()-1]
					p_file.writeFloat ((double)(probs[getCount()-1] - nEvenPercent* (GetDestCount()-getCount())) / 100.0, 2);	
				}
				else
				{//save even percent.
					p_file.writeFloat ((double)(nEvenPercent) / 100.0, 2);	
				}
			}
			else
			{//save appointed destprocs' percent.
				p_file.writeFloat ((double)(probs[i]) / 100.0, 2);	
			}
		}
*/
		if(i<getCount())
		{
			p_file.writeFloat ( (float)((double)(probs[i]) / 100.0), 2);	
		}
		else
		{
			p_file.writeFloat (0.0, 2);	
		}
		m_vPipeIndex[i].WriteData( p_file );		
		if( i < GetDestCount()-1 )//it will be add by caller
		{
			p_file.writeLine();
		}
    }
}

void CProcessorDistributionWithPipe::AddNewOneItem(const ProcessorID& id, const CFlowItemEx& item)
{
	bool bFind = false;
	for(int i = 0; i < count; i++)
	{
		if( id.idFits( destinations[i]) )
		{
			bFind = true;
			break;
		}
	}
	if(!bFind)
	{
		m_vPipeIndex.push_back(item);

		ProcessorDistribution::AddNewOneItem( id );
	}
}

void CProcessorDistributionWithPipe::DeleteOnePipe( int _iPipeIndex)
{
	ASSERT(0 <= _iPipeIndex && (UINT)_iPipeIndex < m_vPipeIndex.size());

	m_vPipeIndex.erase(m_vPipeIndex.begin() + _iPipeIndex);
}

void CProcessorDistributionWithPipe::DeletePipe( int _iPipeIdx )
{
	int nPipeSize = m_vPipeIndex.size();
	for( int m=0; m<nPipeSize; m++ )
	{
		m_vPipeIndex[m].DeletePipeOrAdjust( _iPipeIdx );
	}
}
bool CProcessorDistributionWithPipe::IfUseThisPipe( int _iPipeIdx )const
{
	int nPipeSize = m_vPipeIndex.size();
	for( int m=0; m<nPipeSize; m++ )
	{
		if( m_vPipeIndex[m].IfUseThisPipe( _iPipeIdx ) )
			return true;
	}
	return false;
}	



bool CProcessorDistributionWithPipe::IsCurOneToOne() const
{
	if( m_vPipeIndex.empty() )
	{
		return ProcessorDistribution::IsCurOneToOne();
	}
	const CFlowItemEx& info = m_vPipeIndex[m_nCurDestIndex];
	return info.GetOneToOneFlag() == TRUE ? true : false;
}

int CProcessorDistributionWithPipe::GetOneXOneState() const
{
	if( m_vPipeIndex.empty() )
	{
		return ProcessorDistribution::GetOneXOneState();
	}
	const CFlowItemEx& info = m_vPipeIndex[m_nCurDestIndex];
	return info.GetChannelState();		
}

bool CProcessorDistributionWithPipe::GetFollowState()
{
	if(m_vPipeIndex.empty())
	{
		return false;
	}
	const CFlowItemEx& info = m_vPipeIndex[m_nCurDestIndex];
	return info.GetFollowState();
}

void CProcessorDistributionWithPipe::InitRules()
{
	for(std::vector<CFlowItemEx>::iterator iter=m_vPipeIndex.begin(); iter!=m_vPipeIndex.end(); iter++)
	{
		iter->InitRules();
	}
}
void CProcessorDistributionWithPipe::ReClacDestinations()
{

		ProcessorDistribution::ReClacDestinations();

}

//void CProcessorDistributionWithPipe::InitDesinations()
//{
//	int nDestCount = static_cast<int>(m_vPipeIndex.size());
//	if(nDestCount <= 0)
//		return;
//
//	ProcessorID* pDestList = new ProcessorID[ nDestCount ];
//	unsigned char* pProbabilities = new unsigned char[ nDestCount ];
//
//
//	for (int nDest = 0; nDest < nDestCount; ++nDest)
//	{
//		CFlowItemEx& flowItem = m_vPipeIndex[nDest];
//		
//		pDestList[ nDest] = flowItem.GetProcID()();
//		pProbabilities[ nDest ] = flowItem.GetProbality();
//	}
//	initDestinations( pDestList, nDestCount,pProbabilities, nDestCount );
//
//}







































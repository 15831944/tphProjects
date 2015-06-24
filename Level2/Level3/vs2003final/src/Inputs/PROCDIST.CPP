#include "stdafx.h"
#include "inputs\procdist.h"
#include <CommonData/procid.h>
#include "common\termfile.h"
#include "common\util.h"
#include "common\exeption.h"
#include "in_term.h"


void ProcessorDistribution::initDestinations (const ProcessorID *p_dests,
    const double *p_percents, int p_count)
{
	if( p_count <=0 )
		return;
	
    double *indexes = new double[p_count];
    for (int i = 0; i < p_count; i++)
        indexes[i] = (double) i;

	//set m_nDestCount == count
	//  [7/5/2004]m_nDestCount=p_count;
    init (p_count, indexes, p_percents);
    delete [] indexes;

	//if( destinations )
    delete [] destinations;
    destinations = new ProcessorID[p_count];
    memcpy ((void *)destinations, (void *)p_dests,
        (sizeof (ProcessorID) * p_count));

	if( m_pHaveDrawn )
		delete []m_pHaveDrawn;
	m_pHaveDrawn = new bool[p_count];
	ResetDrawStatus();
}

void ProcessorDistribution::initDestinations (const ProcessorID *p_dests,int _nDestProcsCount,
	const double *p_percents, int _nPercentCount)
{

	if( _nPercentCount <=0 || _nPercentCount>_nDestProcsCount || _nDestProcsCount==0)
		return;
/*	
    double *indexes = new double[_nPercentCount];
    for (int i = 0; i < _nPercentCount; i++)
        indexes[i] = (double) i;
	
	m_nDestCount=_nDestProcsCount;
    init (_nPercentCount, indexes, p_percents);
    delete [] indexes;
	if( destinations )
		delete [] destinations;
    destinations = new ProcessorID[_nDestProcsCount];
    memcpy ((void *)destinations, (void *)p_dests,
        (sizeof (ProcessorID) * _nDestProcsCount));

	if( m_pHaveDrawn )
		delete m_pHaveDrawn;
	m_pHaveDrawn = new bool[_nDestProcsCount];
	ResetDrawStatus();
*/
	double* pPercent=new double[_nDestProcsCount];
	memcpy((void*)pPercent, (void*)p_percents, sizeof(double)* _nPercentCount);

	if(_nDestProcsCount> _nPercentCount)//compatible nonpercent before.
	{
		int nNonpercentCount = _nDestProcsCount - _nPercentCount +1;
		double nNonpercent=p_percents[_nPercentCount-1];

		double nPrepercent = 0.0;
		if(_nPercentCount > 1)
		{
			nPrepercent= p_percents[_nPercentCount-2];
			nNonpercent -= nPrepercent;
		}
		double nPartEventPercent = nNonpercent / nNonpercentCount;

		for( int i = _nPercentCount-1; i< _nDestProcsCount; i++ )
		{
			pPercent[i]= nPrepercent + nPartEventPercent;
			nPrepercent += nPartEventPercent;
		}
		pPercent[_nDestProcsCount-1]=100;

	}
	ProcessorDistribution::initDestinations( p_dests, pPercent, _nDestProcsCount );
	delete[] pPercent;
}

void ProcessorDistribution::DeletePipe(int _iIdx )
{

}

void ProcessorDistribution::DeleteOnePipe( int _iPipeIndex)
{

}

void ProcessorDistribution::AddNewOneItem(const ProcessorID& p_id)
{
	int nCount = count+1;

 	ProcessorID* newID = new ProcessorID[nCount];
	memset( (void*)newID, 0, sizeof(ProcessorID)*nCount );
	memcpy( (void*)newID, (void*)destinations, sizeof(ProcessorID)*count );
	memcpy( (void*)( (char*)newID+sizeof(ProcessorID)*count ),
		(const void*)&p_id, sizeof(ProcessorID) );
    
	double *newProbs = new double[nCount];
	memset((void*)newProbs, 0, sizeof(double)*nCount);

	//because the current proc has not deleted,so the count of procs must be reduce 1
	for(int c = 0; c < nCount-1; c++)
	{
		newProbs[c] = 100 / nCount * (c+1);
	}
	newProbs[nCount-1] = 100.0;
		
	initDestinations(newID, newProbs, nCount);

	delete[] newID;
	delete[] newProbs;
}

// removes any group indexes that reference passed id
void ProcessorDistribution::removeEntry (const ProcessorID& p_id,InputTerminal *_pTerm)
{
	for(int i = count - 1; i >= 0; i--)
	{
		if( p_id.idFits( destinations[i]) )
		{

			for (int j = i; j < count - 1; j++)
            {
				destinations[j] = destinations[j+1];
            }

			DeleteOnePipe(i);
			count--;
		}
	}
	//modified by hans ,pvcs arcport bug 146
	//if(count  == 1)
	//{
	//	for(int c = 0; c < count-1; c++)
	//	{
	//		probs[c] = 100 / count * (c+1);
	//	}
	//	probs[count-1] = 100;
	//}

 /*   for (int i = count - 1; i >= 0; i--)
    {
        if (p_id.idFits (destinations[i]))
        {
            if (i && i == count - 1)//set prob of the end one equal to 100;
			{
				probs[i-1] = 100;
			}
            else 
			{
				//set the others' prob equal to the next one;
                for (int j = i; j < count - 1; j++)
                {
					if(j< count-1)
					{
						probs[j] = probs[j+1];
						count--;
						DeleteOnePipe(j);
					}
                    destinations[j] = destinations[j+1];
                }
			}
						
//by frank 0628            count--; 
		}
		else if(destinations[i].idFits(p_id)&&!destinations[i].isDefault())
		{
			if(!_pTerm->HasBrother(p_id,destinations[i]))
			{
				if (i && i == count - 1)
					probs[i-1] = 100;
				else
					for (int jj = i; jj < count - 1; jj++)
					{
						if(jj< count-1)
						{
							probs[jj] = probs[jj+1];
							count--;
						}
						destinations[jj] = destinations[jj+1];
					}
				count--;
			}
		}
    }

	if(count > 0)
	{
		for(int c = 0; c < count-1; c++)
		{
			probs[c] = 100 / count;
		}
		probs[count-1] = 100;
	}
	*/

}

bool ProcessorDistribution::getUsedProInfo(const ProcessorID& p_id,InputTerminal *_pTerm,std::vector<CString>& _strVec)
{
	bool bRet= false;
	for (int i = count - 1; i >= 0; i--)
    {
        if (p_id.idFits (destinations[i]))
        {
			bRet = true;
        }
		else if(destinations[i].idFits(p_id) && !destinations[i].isBlank())
		{
			if(!_pTerm->HasBrother(p_id,destinations[i]))
			{	
				bRet = true;
			}
		}
    }
	return bRet;
}
// replaces all references to old ID with new ID
void ProcessorDistribution::changeEntry (const ProcessorID& p_old,
    const ProcessorID& p_new,InputTerminal*_pTerm)
{
    for (int i = count - 1; i >= 0; i--)
	{
        if (p_old.idFits (destinations[i]))
            destinations[i].copyID (p_new);
		else if(destinations[i].idFits(p_old) && !destinations[i].isBlank())
		{
			if(!destinations[i].idFits(p_new))
			{
				destinations[i].copyID(p_new);
			}
		}
	}
}

void ProcessorDistribution::readDistribution (ArctermFile& p_file, InputTerminal* _pInTerm)
{
    char temp[256];
    p_file.getField (temp, 256);
    readVersion1 (p_file, _pInTerm );
}

void ProcessorDistribution::readDistribution( ArctermFile& p_file )
{
	ASSERT(false);
}
void ProcessorDistribution::readVersion1 (ArctermFile& p_file, InputTerminal* _pInTerm )
{
    // create ProbabilityDistribution
    double aFloat;
    int maxCount = p_file.countColumns();
    ProcessorID *destList = new ProcessorID[maxCount];
    double *probabilities = new double[maxCount];

    // read processor ID / percentage pairs until invalid field found
    for (int i = 0; !p_file.eol() && !p_file.isBlankField(); i ++)
    {
		destList[i].SetStrDict( _pInTerm->inStrDict );
        destList[i].readProcessorID (p_file);
        p_file.getFloat (aFloat);
        probabilities[i] = (unsigned char) (aFloat * 100.0 + 0.5);
    }
    initDestinations (destList, probabilities, i);
    delete [] destList;
    delete [] probabilities;
}

void ProcessorDistribution::writeDistribution (ArctermFile& p_file) const
{
    p_file.writeField (getProbabilityName());
    for (int i = 0; i < count; i++)
    {
        destinations[i].writeProcessorID (p_file);
        p_file.writeFloat (( (float)((double)(probs[i]) / 100.0)), 2);
    }
}

// returns a destination group index
// if the flag RANDOM is passed the probability distribution is used
// if the flag SEQUENTIAL is used the groups are returned in order
// when using SEQUENTIAL selection, NULL is returned when all groups in
// the tranistion roq have been returned once
// other flags may be added to allow different logic to be used in
// selecting a destination group from the list
const ProcessorID *ProcessorDistribution::getDestinationGroup (int p_flag)
{
    switch (p_flag)
    {
        case RANDOM:
			ResetDrawStatus();
            destIndex = 0;
			m_nCurDestIndex = (int)getRandomValue();
			m_nCurDestIDLength = destinations[m_nCurDestIndex].idLength();
			/*[7/5/2004]
			if(m_nCurDestIndex == count-1 && count!=m_nDestCount)
			{
				int nV = m_nDestCount-count+1;
				m_nCurDestIndex += random( nV );

				m_nCurDestIDLength = destinations[m_nCurDestIndex].idLength();
				m_pHaveDrawn[m_nCurDestIndex]=true;
				return &(destinations[m_nCurDestIndex]);
			}
			*/
			m_pHaveDrawn[m_nCurDestIndex]=true;
            return &(destinations[m_nCurDestIndex]);

        case SEQUENTIAL:
			{
//            if (destIndex < m_nDestCount)
//			{
//				m_nCurDestIndex = destIndex++;
//				m_nCurDestIDLength = destinations[m_nCurDestIndex].idLength();
//                return &(destinations[m_nCurDestIndex]);
//			}
//            else
//                return NULL;
//
			int nDrawNumber = GetDrawNumber();
			if( nDrawNumber >= 0 )
			{
				int nDest= GetIndexOnDrawNumber( nDrawNumber );
				m_nCurDestIndex = nDest;
				m_pHaveDrawn[nDest] = true;
				m_nCurDestIDLength = destinations[nDest].idLength();
				return &(destinations[nDest]);				
			}
			else
				return NULL;
			}			
        default:
            throw new StringError ("Unknown Passenger flow algorithm ");
    }
}

bool ProcessorDistribution::IfProcessorExistInDist( const ProcessorID* _pProc ) const
{
	for( int i=0; i<this->count; ++i )
	{
		if( *_pProc == destinations[ i ] )
		{
			return true;
		}
		//else if( _pProc->isChildOf( destinations[ i ]  ) )
		else if( destinations[ i ].idFits( *_pProc ))
		{
			return true;
		}
		
	}
	return false;
}
// simple accessor
ProcessorID *ProcessorDistribution::getGroup (int p_ndx) const
{
    if (p_ndx < 0 || p_ndx >= count)
        return NULL;

    return destinations + p_ndx;
}


// reset the draw status
void ProcessorDistribution::ResetDrawStatus()
{
	for( int i=0; i<count; i++ )
	{
		m_pHaveDrawn[i] = false;
	}
}

// get index on draw number
int ProcessorDistribution::GetIndexOnDrawNumber( int _nDrawNumber ) const
{
	int nCount = 0;
	for( int i=0; i<count; i++ )
	{
		if( !m_pHaveDrawn[i] )
		{
			if( _nDrawNumber == nCount++ )
				return i;
		}
	}	
	assert( 0 );
	return 0;
}

int ProcessorDistribution::GetDrawNumber() const
{
	int nResult=-1;
	int nUndrawCount=0;
	int nWithinNopercent=0;
	int i;
	//analyse undraw item count.
	/*[7/5/2004]
	for(i=0; i<m_nDestCount; i++ )
	{
		if( !m_pHaveDrawn[i] )
		{
			nUndrawCount++;
			if(i>=count-1)
			{
				nWithinNopercent++;
			}
		}
	}
	*/
	for(i=0; i<count; i++ )
	{
		if( !m_pHaveDrawn[i] )
		{
			nUndrawCount++;
		}
	}
	//  [7/5/2004]if(nWithinNopercent>0)nUndrawCount =nUndrawCount - nWithinNopercent+1;

	if(nUndrawCount==0) return nResult;
	//construct undraw item's percent;
	double* pProbs=new double[nUndrawCount];
	memset((void*)pProbs,0, sizeof(double)*nUndrawCount);
	int nCurUndraw=0;
	double nUsedUndrawProbs=0.0;
	
	if( m_pHaveDrawn[0]==false )
	{
		pProbs[nCurUndraw]=probs[0];
		nUsedUndrawProbs  = probs[0];
		nCurUndraw++;			
	}
	for( i=1; i< count; i++)
	{
		if( m_pHaveDrawn[i]==false )
		{
			pProbs[nCurUndraw]=probs[i]-probs[i-1] + nUsedUndrawProbs;
			nUsedUndrawProbs += (probs[i]-probs[i-1]);
			nCurUndraw++;			
		}
	}
	//one or more nopercent has been drawn.
	if(nCurUndraw<nUndrawCount)
	{
		if(count==1)
		{
			pProbs[nCurUndraw]=probs[0];
			nUsedUndrawProbs  += probs[0];
			nCurUndraw++;			

		}
		else//count > 1
		{
			pProbs[nCurUndraw]= nUsedUndrawProbs + probs[count-1] - probs[count-2];
			nUsedUndrawProbs  += probs[count-1] - probs[count-2];
			nCurUndraw++;			
		}
	}
	//gain an Undraw item index.
	if(nUsedUndrawProbs==0.0)return nResult;

	int nDrawProb=random( (int)nUsedUndrawProbs );
	for( i =0 ;i< nUndrawCount;i++ )
	{
		if (pProbs[i] >= nDrawProb)
		{
			nResult=i;
			break;
		}
	}

	/*[7/5/2004]
	//if no percent group processor is selected, then gain a processor from this group.
	if(nResult== nUndrawCount-1 && nWithinNopercent>1)
	{
		nResult+=random(nWithinNopercent);
	}
	*/
	delete[] pProbs;
	return nResult;
}
const ProcessorID* ProcessorDistribution::getZeropercentDestGroup()
{
	double nCurBranchProb = 0.0;
	double nPerBranchProb = 0.0;
	std::vector<int> vZeropercentDestIndex;
	//add all zeropercent branch into a vector.
	for(int i=0;i<GetDestCount();i++)
	{
		nCurBranchProb = probs[i] - nPerBranchProb;
		nPerBranchProb = probs[i] ;
		if(nCurBranchProb== 0.0 && m_pHaveDrawn[i]==false)
			vZeropercentDestIndex.push_back(i);
	}
	int nZeropercentCount = vZeropercentDestIndex.size();

	if (nZeropercentCount==0) return NULL;// no zeropercen or all zeropercent have been drawn.

	int nDest = vZeropercentDestIndex[0];//if vZeropercentDestIndex.size()==1, it will return first zeropercent branch.
	if(nZeropercentCount>1)
	{
		nDest= vZeropercentDestIndex[ random(vZeropercentDestIndex.size()) ];
	}

	m_nCurDestIndex = nDest;
	m_pHaveDrawn[nDest] = true;
	m_nCurDestIDLength = destinations[nDest].idLength();
	return &(destinations[nDest]);				
}
void ProcessorDistribution::ReClacDestinations()
{


	ProcessorID* newID = new ProcessorID[count];
	memset( (void*)newID, 0, sizeof(ProcessorID)*count );
	memcpy( (void*)newID, (void*)destinations, sizeof(ProcessorID)*count );

	double *newProbs = new double[count];
	memset((void*)newProbs, 0, sizeof(double)*count);

	//because the current proc has not deleted,so the count of procs must be reduce 1
	for(int c = 0; c < count-1; c++)
	{
		newProbs[c] = 100 / count * (c+1);
	}
	newProbs[count-1] = 100;

	initDestinations(newID, newProbs, count);

	delete[] newID;
	delete[] newProbs;

}

ProcessorDistribution::~ProcessorDistribution()
{
	if (destinations) delete [] destinations; if( m_pHaveDrawn ) delete[] m_pHaveDrawn;
}
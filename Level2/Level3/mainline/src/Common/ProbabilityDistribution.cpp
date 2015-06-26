#include "stdafx.h"
#include "Common/replace.h"
#include "ProbabilityDistribution.h"
#include "../inputs/probab.h"
#include "../inputs/fltdist.h"
#include "termfile.h"
#include "util.h"
#include "exeption.h"
#include "../Database/ADODatabase.h"
#include "../Database/ARCportADODatabase.h"
#include "AirportDatabase.h"
#include "../Common/FloatUtils.h"

static const char* errormessages[] = {
    "Maximum value must be greater than minimum value",
    "Parameter 1st Probability must be in range [0.0,1.0]",
    "Parameter Std Dev must be greater or equal to zero.",
    "Parameter Truncation must be in the range [3, 20]",
    "Parameter Mode must be greater than the minimum value",
    "Parameter Mode cannot exceed the maximum value",
    "Parameter Lambda must be greater than zero.",
    "Parameter Alpha must be greater than zero",
    "Parameter Beta must be greater than zero",
    "Parameter Gamma must be greater than zero",
    "Parameter Beta must be greater than zero",
    "Parameter Gamma must be greater than zero",
    "Parameter Beta must be greater than zero",
    "Parameter Alpha must be greater than zero",
    "Parameter Gamma must be greater than zero",
};

//////////////////////////////////////////////////////////////////////////
//theterface for database
//////////////////////////////////////////////////////////////////////////
/////old code version, no more use///////////////////////////////////////////////////////
int  ProbabilityDistribution::WriteDistributionToDB(CString pProBname ,int _airportDBID,DATABASESOURCE_TYPE type )
{
     if(_airportDBID == -1)
		 return -1;
	 CString SQL ;
	 TCHAR val[1024] = {0} ;
	 printDistribution(val) ;
	 SQL.Format(_T("INSERT INTO TB_AIRPORTDB_PRODISTMANAGER (PRODIST_NAME,PROBDISTDATA,AIRPORTDB_ID) VALUES('%s','%s',%d)") ,pProBname,val,_airportDBID ) ;
	 int id = -1 ;
	 id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
	 return id ;
}
////////////////////////////////////////////////////////////////////////////////////////////
void ProbabilityDistribution::readDistributionFromDB(CString str)
{
    setDistribution(str.GetBuffer(0)) ;
}

//new version database read and save/////////////////////////////////////////////////////////
int ProbabilityDistribution::saveDistribution(const CString& pProbName,CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	TCHAR val[1024] = {0};
	printDistribution(val);

	strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_PRODISTMANAGER (PRODIST_NAME,PROBDISTDATA) VALUES('%s','%s')"),pProbName,val);

	int nID = -1;
	nID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());
	return nID;
}

//////////////////////////////////////////////////////////////////////////
static int strcnt (const char *p_str, char p_char)
{
	int count = 0;
	for (int i = strlen (p_str) - 1; i >= 0; i--)
		if (p_str[i] == p_char)
			count++;
	return count;
}

void ProbabilityDistribution::readDistribution (ArctermFile& p_file)
{
	char str[1024];
	p_file.getField (str, 1024);

	// trim distribution name
	char *endOfName = strstr (str, ":");
	if (!endOfName)
	{
		p_file.getField (str, 1024);
		setDistribution (str);
		//        throw new TwoStringError ("Problem reading distribution", str);
	}
	else
		setDistribution (endOfName + 1);
}

void ProbabilityDistribution::writeDistribution (ArctermFile& p_file) const
{
	char str[1024];
	printDistribution (str);
	p_file.writeField (str);
}

double ProbabilityDistribution::getMinXValue() const
{
	return 0.0;
}

double ProbabilityDistribution::getMaxXValue() const
{
	return 1.0;
}

double ProbabilityDistribution::pdf(double _x) const
{
	//ASSERT(0);
	return 0.0;
}

double ProbabilityDistribution::cdf(double _x) const
{
	//ASSERT(0);
	return 0.0;
}

CString ProbabilityDistribution::screenPrint() const
{
	char tmp[1024];
	screenPrint(tmp);
	return tmp;
}

CString ProbabilityDistribution::printDistribution() const
{
	char tmp[1024];
	printDistribution(tmp);
	return tmp;
}

ProbabilityDistribution* ProbabilityDistribution::CopyProbDistribution( ProbabilityDistribution* _pPD )
{

	if(!_pPD)
		return NULL;

	ProbabilityDistribution* pDist = NULL;
	switch( _pPD->getProbabilityType() ) 
	{
	case BERNOULLI:
		pDist = new BernoulliDistribution( ((BernoulliDistribution*)_pPD)->getValue1(),
			((BernoulliDistribution*)_pPD)->getValue2(),
			((BernoulliDistribution*)_pPD)->getProb1()	);
		break;
	case BETA:
		pDist = new BetaDistribution( ((BetaDistribution*)_pPD)->getMin(),
			((BetaDistribution*)_pPD)->getMax(),
			((BetaDistribution*)_pPD)->getAlpha(),
			((BetaDistribution*)_pPD)->getBeta() );
		break;
	case CONSTANT:
		pDist = new ConstantDistribution( ((ConstantDistribution*)_pPD)->getMean() );
		break;
	case EXPONENTIAL:
		pDist = new ExponentialDistribution( ((ExponentialDistribution*)_pPD)->getLambda() );
		break;
	case NORMAL:
		pDist = new NormalDistribution( ((NormalDistribution*)_pPD)->getMean(),
			((NormalDistribution*)_pPD)->getStdDev(),
			((NormalDistribution*)_pPD)->getTruncation());
		break;
	case UNIFORM:
		pDist = new UniformDistribution( ((UniformDistribution*)_pPD)->getMin(),
			((UniformDistribution*)_pPD)->getMax() );
		break;
	case WEIBULL:
		pDist = new WeibullDistribution( ((WeibullDistribution*)_pPD)->getAlpha(),
			((WeibullDistribution*)_pPD)->getGamma(),
			((WeibullDistribution*)_pPD)->getMu());
		break;
	case GAMMA:
		pDist = new GammaDistribution( ((GammaDistribution*)_pPD)->getGamma(),
			((GammaDistribution*)_pPD)->getBeta(),
			((GammaDistribution*)_pPD)->getMu());
		break;
	case ERLANG:
		pDist = new ErlangDistribution( ((ErlangDistribution*)_pPD)->getGamma(),
			((ErlangDistribution*)_pPD)->getBeta(),
			((ErlangDistribution*)_pPD)->getMu() );
		break;
	case TRIANGLE:
		pDist = new TriangleDistribution( ((TriangleDistribution*)_pPD)->getMin(),
			((TriangleDistribution*)_pPD)->getMax(),
			((TriangleDistribution*)_pPD)->getMode());
		break;
	case EMPIRICAL:
		{
			pDist = new EmpiricalDistribution();
			char szDist[1024];
			((EmpiricalDistribution*)_pPD)->printDistribution( szDist );
			char* pStr = strstr( szDist, ":" ) + 1;
			((EmpiricalDistribution*)pDist)->setDistribution( pStr );
			break;
		}
	case HISTOGRAM:
		{
			pDist = new HistogramDistribution();
			char szDist[1024];
			((HistogramDistribution*)_pPD)->printDistribution( szDist );
			char* pStr = strstr( szDist, ":" ) + 1;
			((HistogramDistribution*)pDist)->setDistribution( pStr );
			break;
		}
	default:
		ASSERT(0);
	}

	return pDist;

}

ProbabilityDistribution* ProbabilityDistribution::GetTerminalRelateProbDistributionFromDB( CString pProbData, bool _bHandleProcDistWithPipe /*=false*/ )
{
	TCHAR  pProbType[2048] = {0} ;
	TCHAR strdata[2048] = {0};

	strncpy(pProbType,pProbData.GetBuffer(0) ,pProbData.FindOneOf(":")) ;
	strcpy(strdata,pProbData.GetBuffer(0) + pProbData.FindOneOf(":")+1) ;
	ProbabilityDistribution *prob = NULL;
	if (isNumeric (pProbType[0]))
		return new ConstantDistribution (atof (pProbType));
	if (!_stricmp (pProbType, "HISTORICAL") || !_stricmp (pProbType, "DISCRETE") || !_stricmp (pProbType, "HISTOGRAM"))
		prob = new HistogramDistribution;
	else if (!_stricmp (pProbType, "EMPIRICAL"))
		prob = new EmpiricalDistribution;
	else if (!_stricmp (pProbType, "BERNOULLI"))
		prob = new BernoulliDistribution;
	else if (!_stricmp (pProbType, "UNIFORM"))
		prob = new UniformDistribution;
	else if (!_stricmp (pProbType, "BETA"))
		prob = new BetaDistribution;
	else if (!_stricmp (pProbType, "CONSTANT"))
		prob = new ConstantDistribution;
	else if (!_stricmp (pProbType, "NORMAL") || !_stricmp (pProbType, "GAUSSIAN"))
		prob = new NormalDistribution;
	else if (!_stricmp (pProbType, "EXPONENTIAL"))
		prob = new ExponentialDistribution;
	else if (!_stricmp (pProbType, "WEIBULL"))
		prob = new WeibullDistribution;
	else if (!_stricmp (pProbType, "GAMMA"))
		prob = new GammaDistribution;
	else if (!_stricmp (pProbType, "ERLANG"))
		prob = new ErlangDistribution;
	else if (!_stricmp (pProbType, "TRIANGLE"))
		prob = new TriangleDistribution;
	else
	{
		throw new TwoStringError ("Unknown Probability Distribution ", pProbType);
		return NULL ;
	}

	prob->readDistributionFromDB (strdata);
	return prob;
}

CString ProbabilityDistribution::getErrorMessage(int iErr)
{
    return CString(errormessages[iErr]);
}

bool ProbabilityDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    return true;
}

/*****
*
*   Bernoulli Distribution
*
*****/

double BernoulliDistribution::getRandomValue (void) const
{
	double draw = ((double)rand()) / ((double)RAND_MAX);
	return (draw < prob)? value1: value2;
}

void BernoulliDistribution::readVersion1 (ArctermFile& p_file)
{
	p_file.getFloat (value1);
	p_file.getFloat (prob);
	p_file.getFloat (value2);
}

void BernoulliDistribution::setDistribution (char *p_str)
{
	char *str2, *str1 = strstr (p_str, ";");
	str1[0] = '\0';
	value1 = (float) atof (p_str);
	str2 = strstr (str1+1, ";");
	str2[0] = '\0';
	prob = (float) atof (str1+1);
	str1 = strstr (str2+1, ";");
	str1[0] = '\0';
	value2 = (float) atof (str2+1);
}

void BernoulliDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%.2f, %.2f;  %.2f, %.2f]", value1, prob, value2, 1.0);
}

void BernoulliDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%.2f;%.2f;%.2f;%.2f", value1, prob, value2, 1.0);
}

bool BernoulliDistribution::resetValues(int& iErr, double val1, double val2, double prob1)
{
    if(prob1 < 0.0 || prob1 > 1.0)
    {
        iErr = PDERROR_BERNOULLI_PROB1;
        return false;
    }
    value1 = val1;
    value2 = val2;
    prob = prob1;
    return true;
}

bool BernoulliDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    BernoulliDistribution* pProb = (BernoulliDistribution*)pOther;
    if(abs(value1 - pProb->value1)<0.00001 && 
        abs(value2 - pProb->value2)<0.00001 &&
        abs(prob - pProb->prob)<0.00001)
        return true;
    else
        return false;
}

/*****
*
*   Histogram Distribution
*
*****/

// allocates required amount of memory
void HistogramDistribution::allocate (int Count)
{
//	if (count != Count)
	{
		clear();
		count = Count;
		values = new double [count];
		probs = new double [count];
		memset((void*)values, 0, sizeof(double)*count);
		memset((void*)probs, 0, sizeof(double)*count);
	}
}

// deallocates all memory
void HistogramDistribution::clear (void)
{
	if (probs)
	{
		delete [] probs;
		probs = NULL;
	}
	if (values)
	{
		delete [] values;
		values = NULL;
	}
	count = 0;
}

// initialize distribution with values that are already accumulated
void HistogramDistribution::initAccum (int Count, const double *Values,
									   const double *Probs)
{
	if (Probs[Count-1] != 100 )
		throw new TwoStringError ("Invalid probability ", getProbabilityName());

	allocate (Count);
	memcpy ((char *)probs, (char *)Probs, sizeof (double) * count);
	memcpy ((char *)values, (char *)Values, sizeof (double) * count);
}

void HistogramDistribution::init (int Count, const double *Values,
								  const double *Probs)
{
	if (Probs[Count-1] == 100)
	{
		// if the probabilities are already cumulative, copy values
		initAccum (Count, Values, Probs);
		return;
	}

	allocate (Count);
	resetValues (Values, Probs);
}

BOOL HistogramDistribution::InitFromVector(std::vector<double>* pvdbValue, 
										   std::vector<pair_range>* pvPairRange)
{
	//--------------
	if(pvdbValue == NULL || pvPairRange == NULL)
		return FALSE;
	clear();

	int nCount = pvPairRange->size() + 1;
	allocate(nCount);

	std::vector<double> vSample = *pvdbValue;
	int nSize = vSample.size();
	int j = 0;
	int i=0;
	//TRACE("\n\n");
	//for(j=0; j<nSize; j++)
	//	TRACE("%f-", vSample[j]);

	for(i = 0; i < count-1; i++)
	{
		values[i] = pvPairRange->at(i).first;
	}
	values[count-1] = pvPairRange->back().second;

	//TRACE("\n\n");
	//for(j=0; j<count; j++)
	//	TRACE("%f-", values[j]);

	int nIndex = 0;
	for( i = 0; i < nSize; i++)
	{
		// check vValues[i]
		for( int j=1; j<count; j++ )
			if( vSample[i] <= values[j] )
			{
				probs[j-1]++;
				break;
			}

	}

	//TRACE("\n\n");
	//for(j=0; j<count; j++)
	//	TRACE("%d-", probs[j]);

	/*	unsigned char nTotal = 0;
	for( i = 0; i < count-2; i++)
	{
	probs[i] = (unsigned char)(probs[i]*100 / nSize);
	nTotal += probs[i];
	}
	probs[count-2] = 100-nTotal;
	*/

	double nTotalCount = 0;
	for( int i=0; i< count; i++ )
		nTotalCount += probs[i];

	for( int i=0; i< count; i++ )
	{
		probs[i] = RoundDouble(probs[i] * 100 / nTotalCount, 2) ;
	}

	//TRACE("\n\n");
	//for(j=0; j<count; j++)
	//	TRACE("%d-", probs[j]);

	resetValues(values, probs);

	return TRUE;
}



void HistogramDistribution::EvenPercent()
{
	if( count > 0 )
	{
		int iPercent = 100 / count ;
		int iAccunm = 0;
		if( 100 % count == 0 )
		{
			for( int i=0; i< count; ++i )
			{
				iAccunm += iPercent;
				probs[ i ] = iAccunm ;
			}
		}
		else
		{
			for( int i=0; i< count-1; ++i )
			{
				iAccunm += iPercent;
				probs[ i ] = iAccunm ;

			}

			probs[ count-1 ] = 100;
		}
	}
}
void HistogramDistribution::setProb (double p_percent, int p_ndx)
{
	double diff = 0.0;
	if( p_ndx > 0 )
		diff = p_percent - probs[p_ndx] + probs[p_ndx-1]; //Jan98 : record difference between old and new values
	else
		diff = p_percent - probs[p_ndx];

	for( int i=p_ndx; i<count; i++ )
	{
		probs[i] += diff;
	}
	/* Lincoln's formula
	probs[p_ndx] = p_percent;
	if (p_ndx)
		probs[p_ndx] += probs[p_ndx-1];
	*/

		//Jan98 : do accumulating for each of rest items
		//if the value of current item is less than 100, there must be other
		//items following.


		/* Aug02: becuase of the 0 prob can not be change to other value, so it commented out for
		// now.

		int i = p_ndx;
		while(probs[i] != '\0')
		{
		probs[i] += diff;
		if(probs[i] >= 100)
		break;
		i++;
		}*/
}

double HistogramDistribution::getNoAccumProb(int index) const
{
	if(index)
		return probs[index]-probs[index-1];
	else
		return probs[index];
}

// accumulate values
void HistogramDistribution::resetValues (const double *Values,
										 const double *Probs)
{
	memcpy ((double *)values, (double *)Values, sizeof (double) * count);

	double accumulatedProb = 0;
	for (int i = 0; i < count; i++)
	{
		accumulatedProb += Probs[i];
		probs[i] = accumulatedProb;
	}
	//    if (accumulatedProb != 100 );
	//        throw new TwoStringError ("Invalid probability ", getProbabilityName());
	//        probs[count-1] = 100;
}

double HistogramDistribution::getRandomValue (void) const
{
	// get a random from (0,100)
	double y = (((double)rand()) / ((double)RAND_MAX) * 100.0);
	while (!y)
		y = (((double)rand()) / ((double)RAND_MAX) * 100.0);

	// get i, where probs[i] >= y
	int i=0;
	for ( i = 0; i < count; i++)
		if (probs[i] >= y)
			break;

	if (i == count)
		throw new StringError ("Invalid discrete probability");

	double low, high;
	if (i < count-1) 
	{
		// get random number between values[i] and values[i+1]
		low = values[i];
		return (low + (((double)rand())/((double)RAND_MAX)) * (values[i+1] - low));
	}
	else
	{
		// TRACE("problem with histogram distribution.  last % not 0\n");
		low  = values[max(count-2,0)];
		high = values[max(count-1,1)];
		return (low + (((double)rand())/((double)RAND_MAX)) * (high - low));
	}
}


double HistogramDistribution::getMean (void) const
{
	double mean = 0;
	for (int i = 0; i < count; i++)
		mean += (((double)probs[i]) / 100.0) * values[i];
	return mean;
}

double HistogramDistribution::getVarience (void) const
{
	double dMean1 = 0.0;
	double dMean2 = 0.0;
	double dVarience = 0.0;

	dMean1=getMean();
	for(int i = 0; i < count;i++)
		dMean2 += (((double)probs[i]) / 100.0) * pow (values[i],2);

	dVarience = dMean2 - pow (dMean1,2);
	return dVarience;
}

void HistogramDistribution::readVersion1 (ArctermFile& inputFile)
{
	int probCount = 0;
	double aFloat;
	double vals[64];
	double probabilities[64];
	int flag = 1;

	// read integer / percentage pairs until invalid field found
	do {
		flag = flag && inputFile.getFloat (vals[probCount]);
		flag = flag && inputFile.getFloat (aFloat);

		// only calculate probability and inc count if values valid
		if (flag)
		{
			probabilities[probCount] = (unsigned char) (aFloat * 100.0 + 0.5);
			probCount++;
		}
		// flag set to FALSE as soon as either value invalid
	} while (flag);

	// initialize instance
	init (probCount, vals, probabilities);
}

void HistogramDistribution::setDistribution (char *p_str)
{
	int probCount = (1 + strcnt (p_str, ';')) / 2;
	double *vals = new double[probCount];
	double *probabilities = new double[probCount];
	char *str1, *str2 = p_str - 1;

	// read integer / percentage pairs
	for (int i = 0; i < probCount; i++)
	{
		str1 = strstr (str2+1, ";");
		str1[0] = '\0';
		vals[i] = atof (str2+1);

		str2 = strstr (str1+1, ";");
		if (str2)
			str2[0] = '\0';
		probabilities[i] = atof (str1+1) * 100.0;
	}

	// initialize instance
	init (probCount, vals, probabilities);

	delete [] vals;
	delete [] probabilities;
}

void HistogramDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%.2f,%.2f%%", values[0], probs[0] );
	for (int i = 1; i < count; i++)
		sprintf (p_str + strlen (p_str), "; %.2f,%.2f%%", values[i],
		probs[i]-probs[i-1] );
	strcat( p_str, "]" );
}

void HistogramDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%.2f;%.4f", values[0], (double)(probs[0]) / 100.0);
	for (int i = 1; i < count; i++)
		sprintf (p_str + strlen (p_str), ";%.2f;%.4f", values[i],
		(probs[i]) / 100.0);
}

double HistogramDistribution::getMinXValue() const
{
	if(count>0)
		return values[0];
	else
		return 0.0;
}

double HistogramDistribution::getMaxXValue() const
{
	if(count>0)
		return values[count-1];
	else
		return 0.0;
}

double HistogramDistribution::pdf(double _x) const
{
	ASSERT(0);
	return 0.0;
}

double HistogramDistribution::cdf(double _x) const
{
	ASSERT(0);
	return 0.0;
}

bool HistogramDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    HistogramDistribution* pProb = (HistogramDistribution*)pOther;
    if(count != pProb->count)
        return false;
    for(int i=0; i<count; i++)
    {
        if(probs[i] != pProb->probs[i])
            return false;
        if(values[i] != pProb->values[i])
            return false;
    }
    return true;
}


/*****
*
*   Empirical Distribution
*
****/
BOOL EmpiricalDistribution::InitFromVector(std::vector<double>* pvdbValue, std::vector<pair_range>* pvPairRange)
{
	if(pvdbValue == NULL)
		return TRUE;

	clear();

	std::vector<double> vValue = *pvdbValue;
	int nSize = vValue.size();
	int j = 0;

	// TRACE("\n\n");
	//	for(j=0; j<nSize; j++)
	// TRACE("%f-", vValue[j]);

	std::sort(vValue.begin(), vValue.end());

	// TRACE("\n\n");
	//	for(j=0; j<nSize; j++)
	// TRACE("%f-", vValue[j]);

	std::vector< std::pair<int, int> > vPairValue;

	int i = 0;
	
	for( i = 0; i < nSize; i++)
	{
		int nValue = (int)vValue[i];
		UINT k = 0;
		for( k = 0; k < vPairValue.size(); k++)
		{
			if(nValue == vPairValue[k].first)
			{
				vPairValue[k].second += 1;
				break;
			}
		}
		if(k == vPairValue.size())
		{
			std::pair<int, int> pair;
			pair.first = nValue;
			pair.second = 1;
			vPairValue.push_back(pair);
		}
	}

	allocate(vPairValue.size());
	for( i = 0; i < (int)vPairValue.size(); i++)
	{
		values[i] = (double)vPairValue[i].first;
	}
	int nSizeTotal = 0;
	int nProbTotal = 0;
	int nPreProbTotal = 0;
	for(i = 0; i < (int)vPairValue.size(); i++)
	{
		nSizeTotal += vPairValue[i].second;
		nProbTotal = nSizeTotal*100 / nSize;
		probs[i] = (unsigned char)(nProbTotal - nPreProbTotal);
		nPreProbTotal = nProbTotal;
	}

	resetValues(values, probs);
	return TRUE;
}

double EmpiricalDistribution::getRandomValue (void) const
{
	// get a random from 0 to 100
	double y = (((double)rand()) / ((double)RAND_MAX) * 100.0);
	int i = 0;
	for (i = 0; i < count; i++)
		if (probs[i] >= y)
			break;

	if (i == count)
		throw new StringError ("Invalid Empirical probability");

	return values[i];
}

/*****
*
*   Uniform Distribution
*
*****/

double UniformDistribution::getRandomValue (void) const
{
	double x = ((double)rand()) / ((double)RAND_MAX);
	return (min + x * interval);
}

void UniformDistribution::readVersion1 (ArctermFile& p_file)
{
	// read min and max values (fixed for uniform distributions)
	p_file.getFloat (min);
	p_file.getFloat (max);
	interval = max - min;
}

void UniformDistribution::setDistribution (char *p_str)
{
	char *str1 = strstr (p_str, ";");
	str1[0] = '\0';
	min = (float) atof (p_str);
	max = (float) atof (str1+1);
	interval = max - min;
}

void UniformDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%.2f, %.2f]", min, max);
}

void UniformDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%.2f;%.2f", min, max);
}

double UniformDistribution::getMinXValue() const
{
	return min-(interval/10.0);
}

double UniformDistribution::getMaxXValue() const
{
	return max+(interval/10.0);
}

double UniformDistribution::pdf(double _x) const
{
	if(_x >= min && _x <= max)
		return 1.0/interval;
	else
		return 0.0;
}

double UniformDistribution::cdf(double _x) const
{
	if(_x >= max)
		return 1.0;
	else if(_x >= min) {
		return (1.0/interval)*(_x-min);
	}
	else {
		return 0.0;
	}
}

bool UniformDistribution::resetValues(int& iErr, double Min, double Max)
{
    if(Min > Max)
    {
        iErr = PDERROR_MINEXCEEDMAX;
        return false;
    }
    interval = Max - Min;
    max = Max;
    min = Min;
    return true;
}

bool UniformDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    UniformDistribution* pProb = (UniformDistribution*)pOther;
    if(abs(min - pProb->min)<0.00001 &&
        abs(max - pProb->max)<0.00001 &&
        abs(interval - pProb->interval)<0.00001)
        return true;
    else 
        return false;
}


/*****
*
*   Triangle Distribution
*
*****/

double TriangleDistribution::getRandomValue (void) const
{
	double y = ((double)rand()) / ((double)RAND_MAX);
	double M = 2.0/((b-a)*(mode-a));
	double cutoff = (M/2.0)*(mode-a)*(mode-a);
	if(y < cutoff) { //use 1st part
		return a + sqrt(2.0*y/M);
	}
	else { //use 2nd part
		M = -2.0/((b-a)*(b-mode));
		return b - sqrt(2.0*(y-1)/M);
	}
}

void TriangleDistribution::readVersion1 (ArctermFile& p_file)
{
	// read min and max values (fixed for uniform distributions)
	p_file.getFloat (a);
	p_file.getFloat (b);
	p_file.getFloat (mode);
}

void TriangleDistribution::setDistribution (char *p_str)
{
	char *str2, *str1 = strstr (p_str, ";");
	str1[0] = '\0';
	a = (float) atof (p_str);
	str2 = strstr (str1+1, ";");
	str2[0] = '\0';
	b = (float) atof (str1+1);
	str1 = strstr (str2+1, ";");
	str1[0] = '\0';
	mode = (float) atof (str2+1);
}

void TriangleDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%.2f, %.2f, %.2f]", a, b, mode);
}

void TriangleDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%.2f;%.2f;%.2f;", a, b, mode);
}

double TriangleDistribution::getMinXValue() const
{
	return a;
}

double TriangleDistribution::getMaxXValue() const
{
	return b;
}

double TriangleDistribution::pdf(double _x) const
{
	//if(_x >= min && _x <= max)
	//	return 1.0/interval;
	//else
	return 0.0;
}

double TriangleDistribution::cdf(double _x) const
{
	if(_x < a)
		return 0.0;
	if(_x > b)
		return 1.0;
	if(_x < mode) {
		double M = 2.0/((b-a)*(mode-a));
		double B = -a * M;
		double C = -(M/2.0)*a*a - B*a;
		return (M/2.0)*_x*_x + B*_x + C;
	}
	else {
		double M = -2.0/((b-a)*(b-mode));
		double B = -b * M;
		double C = 1.0 -(M/2.0)*b*b - B*b;
		return (M/2.0)*_x*_x + B*_x + C;
	}
}

bool TriangleDistribution::resetValues(int& iErr, double _min, double _max, double _mode)
{
    if(_min > _max)
    {
        iErr = PDERROR_MINEXCEEDMAX;
        return false;
    }
    if(_mode < _min)
    {
        iErr = PDERROR_TRIANGLE_MINEXCEEDMODE;
        return false;
    }
    if(_mode > _max)
    {
        iErr = PDERROR_TRIANGLE_MODEEXCEEDMAX;
        return false;
    }
    a=_min;
    b=_max;
    mode=_mode;
    return true;
}

bool TriangleDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    TriangleDistribution* pProb = (TriangleDistribution*)pOther;
    if(abs(a - pProb->a)<0.00001 &&
        abs(b - pProb->b)<0.00001 &&
        abs(mode - pProb->mode)<0.00001)
        return true;
    else 
        return false;
}

/*****
*
*   Normal Distribution
*
*****/

double NormalDistribution::getNormalRandomValue()
{
	double r = ((double)rand()) / ((double)(RAND_MAX+1));
	double p = sqrt(-2*log(r));
	r = ((double)rand()) / ((double)(RAND_MAX+1));
	double s = 2*3.141592654*r;
	return p*sin(s);
}
/*
// cumulative probability, * by 10000, corresponding to values from
// -30 to 0
int NormalDistribution::probs [] = { 13, 19, 26, 35, 47, 62, 82,
107, 139, 179, 228, 287, 359, 446, 548, 668, 808, 968, 1151, 1357,
1587, 1841, 2119, 2420, 2743, 3085, 3446, 3821, 4207, 4602, 5000 };

int NormalDistribution::count = sizeof (NormalDistribution::probs) /
sizeof (int);
*/

/*
static const double normalcdf[] = {
	0.5,0.539827896,0.579259687,0.617911357,0.655421697,0.691462467,0.725746935,
		0.758036422,0.788144666,0.815939908,0.84134474,0.864333898,0.884930268,0.903199451,
		0.919243289,0.933192771,0.945200711,0.955434568,0.964069734,0.971283507,0.977249938,
		0.982135643,0.986096601,0.989275919,0.991802471,0.99379032,0.995338778,0.996532977,
		0.997444809,0.99813412,0.998650033,0.999032329,0.999312798,0.999516517,0.999663019,
		0.999767327,0.999840854,0.99989217,0.999927628,0.999951884,0.999968314
};
*/

static const double normalcdf[] = {
	0.5,0.519938873,0.539827896,0.559617712,0.579259687,0.598706274,0.617911357,0.63683059,
		0.655421697,0.673644759,0.691462467,0.708840345,0.725746935,0.742153956,0.758036422,
		0.77337272,0.788144666,0.802337508,0.815939908,0.828943888,0.84134474,0.853140919,
		0.864333898,0.874928011,0.884930268,0.894350161,0.903199451,0.911491948,0.919243289,
		0.9264707,0.933192771,0.939429229,0.945200711,0.950528549,0.955434568,0.959940886,
		0.964069734,0.967843287,0.971283507,0.97441201,
		0.977249938,0.979817852,0.982135643,0.984222449,0.986096601,0.987775567,0.989275919,
		0.990613313,0.991802471,0.992857185,0.99379032,0.99461383,0.995338778,0.995975369,
		0.996532977,0.997020181,0.997444809,0.997813974,0.99813412,0.998411062,0.998650033,
		0.998855724,0.999032329,0.999183581,0.999312798,0.999422914,0.999516517,0.999595887,
		0.999663019,0.999719659,0.999767327,0.999807344,0.999840854,0.999868846,0.99989217,
		0.999911555,0.999927628,0.999940919,0.999951884,0.999960908,0.999968314
};

double NormalDistribution::getRandomValue (void) const
{
	/*
	// first get random from -3 to 0
	// get a random from 0 to 5000
	int y = rand() % probs[count-1];
	for (int i = 0; i < count; i++)
		if (probs[i] >= y)
			break;

	double Y = (-30 + i) / 30.0;       // Y from -1.0 to 0.0
	if (rand() % 2)
		Y = -Y;
	return (Y*stdDeviation + mean);
	*/

		// get a random from 0 to 1
		double r = ((double)rand()) / ((double)(RAND_MAX+1));
	double p = stdDeviation*sqrt(-2*log(r));
	r = ((double)rand()) / ((double)(RAND_MAX+1));
	double s = 2*M_PI*r;
	s = mean+p*sin(s);
	if(fabs(s-mean)>truncation*stdDeviation)
		return getRandomValue();
	else
		return s;
}

void NormalDistribution::readVersion1 (ArctermFile& p_file)
{
	p_file.getFloat (mean);
	p_file.getFloat (stdDeviation);
	truncation = 6;
}

void NormalDistribution::setDistribution (char *p_str)
{
	char *str2, *str1 = strstr (p_str, ";");
	str1[0] = '\0';
	mean = (float) atof (p_str);
	str2 = strstr (str1+1, ";");
	if(str2) {
		str2[0] = '\0';
		stdDeviation = (float) atof (str1+1);
		truncation =  atoi (str2+1);
	}
	else {
		stdDeviation = (float) atof (str1+1);
		truncation = 6;
	}
}

void NormalDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%.2f, %.2f, %d]", mean, stdDeviation, truncation);
}

void NormalDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%.2f;%.2f;%d;", mean, stdDeviation, truncation);
}

double NormalDistribution::getMinXValue() const
{
	return mean-4.0*stdDeviation;
}

double NormalDistribution::getMaxXValue() const
{
	return mean+4.0*stdDeviation;
}

double NormalDistribution::pdf(double _x) const
{
	if(fabs(_x-mean) > truncation*stdDeviation)
		return 0.0;
	else 
		return exp(-(_x-mean)*(_x-mean)/(2.0*stdDeviation*stdDeviation))/(stdDeviation*sqrt(2.0*M_PI));
}

double NormalDistribution::cdf(double _x) const
{
	_x = (_x-mean)/stdDeviation;
	double absX = fabs(_x);
	if(absX > truncation)
		return 0.0;
	else {
		int idx = (int) (absX*20.0);
		if(_x > 0.0)
			return normalcdf[idx];
		else
			return 0.5 + (0.5-normalcdf[idx]);
	}
}

bool NormalDistribution::resetValues(int& iErr, double Mean, double stdDev, int _truncation)
{
    if(stdDev < 0.0f)
    {
        iErr = PDERROR_NORMAL_STDDEV;
        return false;
    }
    if(_truncation < 3 || _truncation > 20)
    {
        iErr = PDERROR_NORMAL_TRUNCRANGE;
        return false;
    }
    mean = Mean;
    stdDeviation = stdDev;
    truncation = _truncation;
    return true;
}

bool NormalDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    NormalDistribution* pProb = (NormalDistribution*)pOther;
    if(abs(mean - pProb->mean)<0.00001 &&
        abs(stdDeviation - pProb->stdDeviation)<0.00001 &&
        truncation == pProb->truncation)
        return true;
    else 
        return false;
}

/*****
*
*   Weibull Distribution
*
*****/
double WeibullDistribution::getRandomValue (void) const
{
	// get a random from 0 to 1
	double y = ((double)rand()) / ((double)(RAND_MAX+1));
	return mu + alpha*pow(-log(1-y),1.0/gamma);
}

double WeibullDistribution::getMean (void) const
{
	return mu + alpha*exp(GammaDistribution::gammln((gamma+1)/gamma));
}

void WeibullDistribution::setDistribution (char *p_str)
{
	char *str2, *str1 = strstr (p_str, ";");
	str1[0] = '\0';
	alpha = (float) atof (p_str);
	str2 = strstr (str1+1, ";");
	str2[0] = '\0';
	gamma = (float) atof (str1+1);
	str1 = strstr (str2+1, ";");
	str1[0] = '\0';
	mu = (float) atof (str2+1);
}

void WeibullDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%.2f, %.2f;  %.2f]", alpha, gamma, mu);
}

void WeibullDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%.2f;%.2f;%.2f;", alpha, gamma, mu);
}

double WeibullDistribution::getMinXValue() const
{
	return mu;
}

double WeibullDistribution::getMaxXValue() const
{
	return mu+2.0*alpha;
}

double WeibullDistribution::pdf(double _x) const
{
	return (gamma/alpha)*pow((_x-mu)/alpha, gamma-1)*exp( -pow((_x-mu)/alpha, gamma)  );
}

double WeibullDistribution::cdf(double _x) const
{
	return 1.0 - exp(-pow((_x-mu)/alpha, gamma));
}

bool WeibullDistribution::resetValues(int& iErr, double _alpha, double _gamma, double _mu)
{
    if(_alpha < 0.00001f)
    {
        iErr = PDERROR_WEIBULL_ALPHA;
        return false;
    }
    if(_gamma < 0.00001f)
    {
        iErr = PDERROR_WEIBULL_GAMMA;
        return false;
    }
    alpha = _alpha;
    gamma = _gamma;
    mu = _mu;
    return true;
}

bool WeibullDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    WeibullDistribution* pProb = (WeibullDistribution*)pOther;
    if(abs(alpha - pProb->alpha)<0.00001 &&
        abs(gamma - pProb->gamma)<0.00001 &&
        abs(mu - pProb->mu)<0.00001)
        return true;
    else 
        return false;
}

/*****
*
*   Gamma Distribution
*
*****/

//return ln(Gamma(x)) for x > 0
double GammaDistribution::gammln(double _x) 
{
	double x, y, tmp, ser;
	static double cof[6] = {
		76.18009172947146,
			-86.50532032941677,
			24.01409824083091,
			-1.231739572450155,
			0.1208650973866179e-2,
			-0.5395239384953e-5
	};
	int j;
	y = x = _x;
	tmp = x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser = 1.000000000190015;
	for(j=0; j<=5; j++)
		ser += cof[j]/++y;

	return -tmp+log(2.5066282746310005*ser/x);
}

double GammaDistribution::gammp(double _a, double _x)
//Returns the incomplete gamma function P (a, x).
{
	double gamser,gammcf,gln;
	if (_x < 0.0 || _a <= 0.0) {
		///		// TRACE("Invalid arguments in GammaDistribution::gammp");
		ASSERT(0);
	}
	if (_x < (_a+1.0)) {
		//Use the series representation.
		gser(&gamser,_a,_x,&gln);
		return gamser;
	}
	else {
		//Use the continued fraction representation
		gcf(&gammcf,_a,_x,&gln);
		return 1.0-gammcf; //and take its complement.
	}
}

void GammaDistribution::gser(double* _gamser, double _a, double _x, double* _gln)
{
	static const double EPS = 3.0e-7;
	int n;
	double sum,del,ap;
	*_gln=gammln(_a);
	if (_x <= 0.0) {
		if (_x < 0.0) {
			//			// TRACE("x less than 0 in GammaDistribution::gser");
			ASSERT(0);
		}
		*_gamser=0.0;
		return;
	}
	else {
		ap=_a;
		del=sum=1.0/_a;
		for (n=1;n<=100;n++) {
			++ap;del *= _x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS) {
				*_gamser=sum*exp(-_x+_a*log(_x)-(*_gln));
				return;
			}
		}
		//		// TRACE("a too large, ITMAX too small in GammaDistribution::gser");
		ASSERT(0);
		return;
	}
}

void GammaDistribution::gcf(double* _gammcf, double _a, double _x, double* _gln)
{
	static const double EPS = 3.0e-7;
	static const double FPMIN = 1.0e-30;
	int i;
	double an,b,c,d,del,h;
	*_gln=gammln(_a);
	b = (float)(_x + 1.0 - _a);
	//Set up for evaluating continued fractionby modified Lentz's method (?.2)with b0= 0.
	c = (float)(1.0 / FPMIN);
	d = (float)(1.0 / b);
	h=d;
	for (i=1;i<=100;i++) {
		//Iterate to convergence.
		an = (float)(-i * ((double)i - _a));
		b += (float)2.0;
		d = an * d + b;
		if (fabs(d) < FPMIN) 
			d = (float)FPMIN;c=b+an/c;
		if (fabs(c) < FPMIN) 
			c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < EPS) 
			break;
	}
	if (i > 100) {
		//		// TRACE("a too large, ITMAX too small in GammaDistribution::gcf");
		ASSERT(0);
	}
	*_gammcf=exp(-_x+_a*log(_x)-(*_gln))*h; //Put factors in front
}

double GammaDistribution::getRandomValue (void) const
{
	// get a random from 0 to 1
	double r = NormalDistribution::getNormalRandomValue();
	double c = 1.0/(9.0*gamma);
	return mu + gamma*beta*pow(r*sqrt(c)+1-c,3);
}

void GammaDistribution::setDistribution (char *p_str)
{
	char *str2, *str1 = strstr (p_str, ";");
	str1[0] = '\0';
	gamma = (float) atof (p_str);
	gammaln = gammln(gamma);
	str2 = strstr (str1+1, ";");
	str2[0] = '\0';
	beta = (float) atof (str1+1);
	str1 = strstr (str2+1, ";");
	str1[0] = '\0';
	mu = (float) atof (str2+1);
}

void GammaDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%.2f, %.2f; %.2f]", gamma, beta, mu);
}

void GammaDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%.2f;%.2f;%.2f;", gamma, beta, mu);
}

double GammaDistribution::getMinXValue() const
{
	return mu;
}

double GammaDistribution::getMaxXValue() const
{
	return mu+3.0*(beta*gamma);
}

double GammaDistribution::pdf(double _x) const
{
	return pow((_x-mu)/beta,gamma-1)*exp(-(_x-mu)/beta)/(beta*exp(gammaln));
}

double GammaDistribution::cdf(double _x) const
{
	return gammp(gamma, (_x-mu)/beta);
	return 0.0;
}

bool GammaDistribution::resetValues(int& iErr, double _gamma, double _beta, double _mu)
{
    if(_gamma < 0.00001f)
    {
        iErr = PDERROR_GAMMA_GAMMA;
        return false;
    }
    if(_beta < 0.00001f)
    {
        iErr = PDERROR_GAMMA_BETA;
        return false;
    }
    gamma = _gamma;
    beta = _beta;
    mu = _mu;
    return true;
}

bool GammaDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    GammaDistribution* pProb = (GammaDistribution*)pOther;
    if(abs(beta  -pProb->beta)<0.00001 &&
        abs(gamma - pProb->gamma)<0.00001 &&
        abs(mu - pProb->mu)<0.00001 &&
        abs(gammaln - pProb->gammaln)<0.00001)
        return true;
    else 
        return false;
}



/*****
*
*   Erlang Distribution
*
*****/

double ErlangDistribution::getRandomValue (void) const
{
	// get a random from 0 to 1
	double r = NormalDistribution::getNormalRandomValue();
	double c = 1.0/(9.0*gamma);
	return mu + gamma*beta*pow(r*sqrt(c)+1-c,3);
}

void ErlangDistribution::setDistribution (char *p_str)
{
	char *str2, *str1 = strstr (p_str, ";");
	str1[0] = '\0';
	gamma = (int) atoi (p_str);
	str2 = strstr (str1+1, ";");
	str2[0] = '\0';
	beta = (float) atof (str1+1);
	str1 = strstr (str2+1, ";");
	str1[0] = '\0';
	mu = (float) atof (str2+1);
}

void ErlangDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%d, %.2f; %.2f]", gamma, beta, mu);
}

void ErlangDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%d;%.2f;%.2f;", gamma, beta, mu);
}

double ErlangDistribution::getMinXValue() const
{
	return mu;
}

double ErlangDistribution::getMaxXValue() const
{
	return mu+3.0*(beta*gamma);
}

double ErlangDistribution::pdf(double _x) const
{
	return pow((_x-mu)/beta,gamma-1)*exp(-(_x-mu)/beta)/(beta*factorial(gamma-1));
}

double ErlangDistribution::cdf(double _x) const
{
	return GammaDistribution::gammp(gamma, (_x-mu)/beta);
}

bool ErlangDistribution::resetValues(int& iErr, int _gamma, double _beta, double _mu)
{
    if(_gamma < 0.00001f)
    {
        iErr = PDERROR_ERLANG_GAMMA;
        return false;
    }
    if(_beta < 0.00001f)
    {
        iErr = PDERROR_ERLANG_BETA;
        return false;
    }
    gamma = _gamma;
    beta = _beta;
    mu = _mu;
    return true;
}

bool ErlangDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    ErlangDistribution* pProb = (ErlangDistribution*)pOther;
    if(abs(beta  -pProb->beta)<0.00001 &&
        gamma == pProb->gamma &&
        abs(mu - pProb->mu)<0.00001 &&
        abs(gammaln - pProb->gammaln)<0.00001)
        return true;
    else 
        return false;
}





/*****
*
*   Exponential Distribution
*
*****/

double ExponentialDistribution::getRandomValue (void) const
{
	// get a random from 0 to 1
	double y = ((double)rand()) / ((double)(RAND_MAX+1));
	//return (-lambda * log (1-y)); //fixed by Nic May26 2003
	return (-log(1-y))/lambda;
}

void ExponentialDistribution::setDistribution (char *p_str)
{
	lambda = atof (p_str);
}

void ExponentialDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%.2f]", lambda);
}

void ExponentialDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%.2f", lambda);
}

double ExponentialDistribution::getMinXValue() const
{
	return 0.0;
}

double ExponentialDistribution::getMaxXValue() const
{
	return 3.0/lambda;
}

double ExponentialDistribution::pdf(double _x) const
{
	if(lambda < 0.001) {
		double inv = 1.0/lambda;
		return (exp(-_x/inv))/inv;
	}
	else
		return lambda*exp(-lambda*_x);
}

double ExponentialDistribution::cdf(double _x) const
{
	return 1.0-exp(-lambda*_x);
}

bool ExponentialDistribution::resetValues(int& iErr, double Lambda)
{
    if(Lambda < 0.00001f)
    {
        iErr = PDERROR_EXPO_LAMBDA;
        return false;
    }
    lambda = Lambda;
    return true;
}

bool ExponentialDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    ExponentialDistribution* pProb = (ExponentialDistribution*)pOther;
    if(abs(lambda  -pProb->lambda)<0.00001)
        return true;
    else 
        return false;
}

/*****
*
*   Beta Distribution
*
*****/

//  Given alpha, beta, calculate the accumulated probablity for
//  intervals starting from 0. To preserve accuracy, the resulted
//  accumulated probablity is multiplied by 100

const double BaseBetaDistribution::Interval = 0.01;
const int BaseBetaDistribution::Count = (int)(1/BaseBetaDistribution::Interval);
BaseBetaDistribution * defaultBaseBetaDistribution = new BaseBetaDistribution();

double BaseBetaDistribution::Beta(double _a, double _b)
{
	return exp(GammaDistribution::gammln(_a)+GammaDistribution::gammln(_b)-GammaDistribution::gammln(_a+_b));
}

double BaseBetaDistribution::Beta(int _a, int _b)
{
	//return exp(GammaDistribution::GammaLn(_a)+GammaDistribution::GammaLn(_b)-GammaDistribution::GammaLn(_a+_b));
	return factorial(_a-1)*factorial(_b-1)/factorial(_a+_b-1);
}

double BaseBetaDistribution::betai(double _a, double _b, double _x)
{
	//Returns the incomplete beta function Ix(a,b).
	double bt;
	if (_x < 0.0 || _x > 1.0) {
		//		// TRACE("Bad x in BaseBetaDistribution::betai");
		ASSERT(0);
	}

	if (_x == 0.0 || _x == 1.0)
		bt=0.0;
	else
		//Factors in front of the continued fraction.
		bt=exp(GammaDistribution::gammln(_a+_b)-GammaDistribution::gammln(_a)-GammaDistribution::gammln(_b)+_a*log(_x)+_b*log(1.0-_x));

	if (_x < (_a+1.0)/(_a+_b+2.0))
		//Use continued fraction directly.
		return bt*betacf(_a,_b,_x)/_a;
	else
		//Use continued fraction after making the sym-metry transformation.
		return 1.0-bt*betacf(_b,_a,1.0-_x)/_b;
}

double BaseBetaDistribution::betacf(double _a, double _b, double _x)
//Used by betai: Evaluates continued fraction for incomplete beta function by modified Lentz'smethod (?.2).
{
	static const double FPMIN = 1.0e-30;
	static const double EPS = 3.0e-7;
	int m,m2;
	double aa,c,d,del,h,qab,qam,qap;
	qab=_a+_b;
	//These q's will be used in factors that occurin the coefficients (6.4.6).
	qap=_a+1.0;
	qam=_a-1.0;
	c=1.0;
	//First step of Lentz's method.
	d=1.0-qab*_x/qap;
	if (fabs(d) < FPMIN)
		d=FPMIN;
	d=1.0/d;h=d;
	for (m=1;m<=100;m++) {
		m2=2*m;
		aa=m*(_b-m)*_x/((qam+m2)*(_a+m2));
		d=1.0+aa*d;
		//One step (the even one) of the recurrence.
		if (fabs(d) < FPMIN)
			d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN) 
			c=FPMIN;
		d=1.0/d;
		h *= d*c;
		aa = -(_a+m)*(qab+m)*_x/((_a+m2)*(qap+m2));
		d=1.0+aa*d;
		//Next step of the recurrence (the odd one).
		if (fabs(d) < FPMIN) 
			d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN) 
			c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < EPS) 
			break;
		//Are we done?
	}
	if (m > 100) {
		//		// TRACE("a or b too big, or MAXIT too small in BaseBetaDistribution::betacf");
		ASSERT(0);
	}
	return h;
}

BaseBetaDistribution::BaseBetaDistribution (int Alpha, int Beta) : probs( NULL)
{
	init( Alpha, Beta );
}

void BaseBetaDistribution::init( int _alpha, int _beta )
{
	alpha = _alpha;
	beta = _beta;
	ASSERT(alpha>0 && beta>0);

	clear();
	probs = new int[Count];
	int accumulatedProb = 0;
	double prob, y;

	for (int i = 0; i < Count; i++)
	{
		y = i * Interval;
		prob = factorial (alpha + beta - 1) /
			(factorial (alpha - 1) * factorial (beta - 1))
			*  Pow (y, alpha-1) * Pow (1-y, beta-1);
		accumulatedProb += (int)(prob * 100);
		probs[i] = accumulatedProb;
	}
	probs[Count-1] = (int)(prob * 100.0);
}


double BaseBetaDistribution::getRandomValue (void) const
{
	// get a random number from 0 to 100*Count
	int y = rand() % (Count*100) ; // get a random from 0 to 10000
	int i=0 ;
	for (i = 0; i < Count; i++)
		if (probs[i] >= y)
			break;
	return ((double)i) * Interval;
}

void BaseBetaDistribution::readVersion1 (ArctermFile& p_file)
{
	int value;
	p_file.getInteger (value);
	alpha = value;

	p_file.getInteger (value);
	beta = value;

	init( alpha, beta );
}

void BaseBetaDistribution::setDistribution (char *p_str)
{
	char *str1 = strstr (p_str, ";");
	str1[0] = '\0';
	alpha = atoi (p_str);
	beta = atoi (str1+1);

	init( alpha, beta );
}

void BaseBetaDistribution::screenPrint (char *p_str) const
{
	sprintf (p_str, "; %d,%d", alpha, beta);
}

void BaseBetaDistribution::printDistribution (char *p_str) const
{
	sprintf (p_str, ";%d;%d", alpha, beta);
}

bool BaseBetaDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    BaseBetaDistribution* pProb = (BaseBetaDistribution*)pOther;
    if(alpha != pProb->alpha || beta != pProb->beta)
        return false;

    for(int i=0; i<Count; i++)
    {
        if(probs[i] != pProb->probs[i])
            return false;
    }
    return true;
}


/*****
*
*   Beta Distribution
*
*****/

BetaDistribution::~BetaDistribution ()
{
	if (basebeta && basebeta != defaultBaseBetaDistribution)
		delete basebeta;
}

void BetaDistribution::init (double Min, double Max, double Alpha, double Beta)
{
	min = Min;
	max = Max;
	if (Alpha == DEFAULT_ALPHA && Beta == DEFAULT_BETA)
		basebeta = defaultBaseBetaDistribution;
	else
		basebeta = new BaseBetaDistribution ((int)Alpha, (int)Beta);
}

void BetaDistribution::resetValues (double Min, double Max, int Alpha,
									int Beta)
{
	min = Min;
	max = Max;
	if (basebeta && Alpha == basebeta->getAlpha() && Beta == basebeta->getBeta())
		return;

	if (basebeta && basebeta->getAlpha() != DEFAULT_ALPHA || basebeta->getBeta() != DEFAULT_BETA)
		delete basebeta;

	if (Alpha == DEFAULT_ALPHA && Beta == DEFAULT_BETA)
		basebeta = defaultBaseBetaDistribution;
	else
		basebeta = new BaseBetaDistribution ((int)Alpha, (int)Beta);
}

bool BetaDistribution::resetValues(int& iErr, double Min, double Max, int Alpha, int Beta)
{
    min = Min;
    max = Max;
    if(Alpha <= 0)
    {
        iErr = PDERROR_BETA_ALPHA;
        return false;
    }
    if(Beta <= 0)
    {
        iErr = PDERROR_BETA_BETA;
        return false;
    }
    if(Min > Max)
    {
        iErr = PDERROR_MINEXCEEDMAX;
        return false;
    }

    if (basebeta && Alpha == basebeta->getAlpha() && Beta == basebeta->getBeta())
        return true;

    if (basebeta && basebeta->getAlpha() != DEFAULT_ALPHA || basebeta->getBeta() != DEFAULT_BETA)
        delete basebeta;

    if (Alpha == DEFAULT_ALPHA && Beta == DEFAULT_BETA)
        basebeta = defaultBaseBetaDistribution;
    else
        basebeta = new BaseBetaDistribution (Alpha, Beta);
    return true;
}

double BetaDistribution::getRandomValue (void) const
{
	// get a beta random number from 0 to 1
	double y = basebeta->getRandomValue();
	return (min + y * (max-min));
}

void BetaDistribution::readVersion1 (ArctermFile &p_file)
{
	p_file.getFloat (min);
	p_file.getFloat (max);
	if (!p_file.isBlankField())
	{
		basebeta = new BaseBetaDistribution;
		basebeta->readVersion1 (p_file);
	}
	else
		basebeta = defaultBaseBetaDistribution;
}

void BetaDistribution::setDistribution (char *p_str)
{
	char *str1 = strstr (p_str, ";");
	str1[0] = '\0';
	min = atof (p_str);
	max = atof (str1+1);
	str1 = strstr (str1+1, ";");
	if (str1)
	{
		basebeta = new BaseBetaDistribution;
		basebeta->setDistribution (str1+1);
	}
	else
		basebeta = defaultBaseBetaDistribution;
}

void BetaDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%.2f,%.2f", min, max);
	if (basebeta != defaultBaseBetaDistribution)
		basebeta->printDistribution (p_str + strlen (p_str));
	strcat( p_str, "]");
}


void BetaDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%.2f;%.2f", min, max);
	if (basebeta != defaultBaseBetaDistribution)
		basebeta->printDistribution (p_str + strlen (p_str));
}

double BetaDistribution::getMinXValue() const
{
	return min;
}
double BetaDistribution::getMaxXValue() const
{
	return max;
}
double BetaDistribution::pdf(double _x) const
{
	double a = basebeta->getAlpha();
	double b = basebeta->getBeta();
	return pow(_x-min,a-1)*pow(max-_x, b-1)/( BaseBetaDistribution::Beta(a,b) * pow(max-min, a+b-1) );
}
double BetaDistribution::cdf(double _x) const
{
	double a = basebeta->getAlpha();
	double b = basebeta->getBeta();
	return BaseBetaDistribution::betai(a, b, (_x-min)/(max-min));
}

bool BetaDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    BetaDistribution* pProb = (BetaDistribution*)pOther;
    if(abs(min - pProb->min) >= 0.00001)
        return false;
    if(abs(max - pProb->max) >= 0.00001)
        return false;
    if(!basebeta->isEqual(pProb->basebeta))
        return false;
    return true;
}

/*****
*
*   Constant Distribution
*
*****/

void ConstantDistribution::readVersion1 (ArctermFile &p_file)
{
	p_file.getFloat (value);
}

void ConstantDistribution::setDistribution (char *p_str)
{
	value = atof (p_str);
}

void ConstantDistribution::screenPrint (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ": [%.2f]", value);
}

void ConstantDistribution::printDistribution (char *p_str) const
{
	strcpy (p_str, getProbabilityName());
	sprintf (p_str + strlen (p_str), ":%.2f", value);
}

double ConstantDistribution::getMinXValue() const
{
	return value-1.0;
}

double ConstantDistribution::getMaxXValue() const
{
	return value+1.0;
}

double ConstantDistribution::pdf(double _x) const
{
	if(_x != value)
		return 0.0;
	else
		return 1.0;
}

double ConstantDistribution::cdf(double _x) const
{
	if(_x != value)
		return 0.0;
	else
		return 1.0;
}

bool ConstantDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    ConstantDistribution* pProb = (ConstantDistribution*)pOther;
    if(abs(value - pProb->value)<0.00001)
        return true;
    else
        return false;
}

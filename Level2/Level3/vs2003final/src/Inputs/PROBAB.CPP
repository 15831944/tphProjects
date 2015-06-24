#include "stdafx.h"
#include "probab.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/termfile.h"
#include "../Common/util.h"
#include "../Common/exeption.h"
#include "fltdist.h"
#include "ProcessorDistributionWithPipe.h"
#include "HistogramManager.h"
#include "../Common/AirportDatabase.h"
#include "in_term.h"

#define isNumeric(x)  ((x>=48&&x<=57)||x==43||x==45||x==46)

ProbabilityDistribution* CopyProbDistribution(ProbabilityDistribution* _pPD)
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

// Reads a probability distribution into the appropriate subclass of
// ProbabilityDistribution and returns a generic pointer to it
// Assumes that the current position is valid
// Exception: FileFormatError
ProbabilityDistribution* GetProbDistribution (ArctermFile &p_file )
{
    // get ProbabiltyDistribution field
    if (p_file.isBlankField())
        return NULL;

    char str[1024];
    int fieldNum = p_file.getColumn();
    p_file.getField (str, 1024);

    char *endOfName = strstr (str, ":");
    if (endOfName)
        endOfName[0] = '\0';

    ProbabilityDistribution *prob;
    if (isNumeric (str[0]))
        return new ConstantDistribution (atof (str));
    if (!stricmp (str, "HISTORICAL") || !stricmp (str, "DISCRETE") || !stricmp (str, "HISTOGRAM"))
        prob = new HistogramDistribution;
    else if (!stricmp (str, "EMPIRICAL"))
        prob = new EmpiricalDistribution;
    else if (!stricmp (str, "BERNOULLI"))
        prob = new BernoulliDistribution;
    else if (!stricmp (str, "UNIFORM"))
        prob = new UniformDistribution;
    else if (!stricmp (str, "BETA"))
        prob = new BetaDistribution;
    else if (!stricmp (str, "CONSTANT"))
        prob = new ConstantDistribution;
    else if (!stricmp (str, "NORMAL") || !stricmp (str, "GAUSSIAN"))
        prob = new NormalDistribution;
    else if (!stricmp (str, "EXPONENTIAL"))
        prob = new ExponentialDistribution;
	else if (!stricmp (str, "WEIBULL"))
		prob = new WeibullDistribution;
	else if (!stricmp (str, "GAMMA"))
		prob = new GammaDistribution;
	else if (!stricmp (str, "ERLANG"))
		prob = new ErlangDistribution;
	else if (!stricmp (str, "TRIANGLE"))
		prob = new TriangleDistribution;  
    // other probability types yet to be written
    else
        throw new TwoStringError ("Unknown Probability Distribution ", str);

    if (p_file.getVersion() < 2.0)
        prob->readVersion1 (p_file);
	else
    {
        p_file.setToField (fieldNum);
        prob->readDistribution (p_file);
    }
    return prob;
}
//ProbabilityDistribution* GetTerminalRelateProbDistributionFromDB(CString pProbData, InputTerminal * pInterm, bool _bHandleProcDistWithPipe ) 
//{
//
//	TCHAR  pProbType[2048] = {0} ;
//	TCHAR strdata[2048] = {0};
//	
//	strncpy(pProbType,pProbData.GetBuffer(0) ,pProbData.FindOneOf(":")) ;
//	strcpy(strdata,pProbData.GetBuffer(0) + pProbData.FindOneOf(":")+1) ;
//	ProbabilityDistribution *prob = NULL;
//	if (isNumeric (pProbType[0]))
//		return new ConstantDistribution (atof (pProbType));
//	if (!stricmp (pProbType, "HISTORICAL") || !stricmp (pProbType, "DISCRETE") || !stricmp (pProbType, "HISTOGRAM"))
//		prob = new HistogramDistribution;
//	else if (!stricmp (pProbType, "EMPIRICAL"))
//		prob = new EmpiricalDistribution;
//	else if (!stricmp (pProbType, "BERNOULLI"))
//		prob = new BernoulliDistribution;
//	else if (!stricmp (pProbType, "UNIFORM"))
//		prob = new UniformDistribution;
//	else if (!stricmp (pProbType, "BETA"))
//		prob = new BetaDistribution;
//	else if (!stricmp (pProbType, "CONSTANT"))
//		prob = new ConstantDistribution;
//	else if (!stricmp (pProbType, "NORMAL") || !stricmp (pProbType, "GAUSSIAN"))
//		prob = new NormalDistribution;
//	else if (!stricmp (pProbType, "EXPONENTIAL"))
//		prob = new ExponentialDistribution;
//	else if (!stricmp (pProbType, "WEIBULL"))
//		prob = new WeibullDistribution;
//	else if (!stricmp (pProbType, "GAMMA"))
//		prob = new GammaDistribution;
//	else if (!stricmp (pProbType, "ERLANG"))
//		prob = new ErlangDistribution;
//	else if (!stricmp (pProbType, "TRIANGLE"))
//		prob = new TriangleDistribution;
//	else
//	{
//		throw new TwoStringError ("Unknown Probability Distribution ", pProbType);
//		return NULL ;
//	}
//
//	prob->readDistributionFromDB (strdata);
//	return prob;
//}
ProbabilityDistribution* GetTerminalRelateProbDistribution( ArctermFile &p_file, InputTerminal * pInterm, bool _bHandleProcDistWithPipe /*= false*/ )
{
	// get ProbabiltyDistribution field
	if (p_file.isBlankField())
		return NULL;

	char str[1024];
	int fieldNum = p_file.getColumn();
	p_file.getField (str, 1024);

	char *endOfName = strstr (str, ":");
	if (endOfName)
		endOfName[0] = '\0';

	ProbabilityDistribution *prob;
	if (isNumeric (str[0]))
		return new ConstantDistribution (atof (str));
	if (!stricmp (str, "HISTORICAL") || !stricmp (str, "DISCRETE") || !stricmp (str, "HISTOGRAM"))
		prob = new HistogramDistribution;
	else if (!stricmp (str, "EMPIRICAL"))
		prob = new EmpiricalDistribution;
	else if (!stricmp (str, "BERNOULLI"))
		prob = new BernoulliDistribution;
	else if (!stricmp (str, "UNIFORM"))
		prob = new UniformDistribution;
	else if (!stricmp (str, "BETA"))
		prob = new BetaDistribution;
	else if (!stricmp (str, "CONSTANT"))
		prob = new ConstantDistribution;
	else if (!stricmp (str, "NORMAL") || !stricmp (str, "GAUSSIAN"))
		prob = new NormalDistribution;
	else if (!stricmp (str, "EXPONENTIAL"))
		prob = new ExponentialDistribution;
	else if (!stricmp (str, "WEIBULL"))
		prob = new WeibullDistribution;
	else if (!stricmp (str, "GAMMA"))
		prob = new GammaDistribution;
	else if (!stricmp (str, "ERLANG"))
		prob = new ErlangDistribution;
	else if (!stricmp (str, "TRIANGLE"))
		prob = new TriangleDistribution;
	else if (!stricmp (str, "PROCESSOR"))
	{
		if( _bHandleProcDistWithPipe ){
			CProcessorDistributionWithPipe * pProb = new CProcessorDistributionWithPipe( true );
			if (p_file.getVersion() < 2.0)
				pProb->readVersion1 (p_file,pInterm);
			else
			{
				p_file.setToField (fieldNum);
				pProb->readDistribution (p_file,pInterm);
			}
			return pProb;
		}
		else{
			ProcessorDistribution * pProb = new ProcessorDistribution;
			if (p_file.getVersion() < 2.0)
				pProb->readVersion1 (p_file,pInterm);
			else
			{
				p_file.setToField (fieldNum);
				pProb->readDistribution (p_file,pInterm);
			}
			return pProb;
		}
	}
	else if (!stricmp (str, "PROCESSORWITHPIPE")){
		CProcessorDistributionWithPipe * pProb = new CProcessorDistributionWithPipe( false );
		if (p_file.getVersion() < 2.0)
			pProb->readVersion1 (p_file,pInterm);
		else
		{
			p_file.setToField (fieldNum);
			pProb->readDistribution (p_file,pInterm);
		}
		return pProb;
	}
	else if (!stricmp (str, "FLIGHT_TYPE")){

		FlightTypeDistribution *pProb = new FlightTypeDistribution;
		if (p_file.getVersion() < 2.0)
			pProb->readVersion1 (p_file,pInterm);
		else
		{
			p_file.setToField (fieldNum);
			pProb->readDistribution (p_file,pInterm);
		}
		return pProb;
	}// other probability types yet to be written
	else
		throw new TwoStringError ("Unknown Probability Distribution ", str);

	if (p_file.getVersion() < 2.0)
		prob->readVersion1 (p_file);
	else
	{
		p_file.setToField (fieldNum);
		prob->readDistribution (p_file);
	}
	return prob;
}

//global function
CProbDistManager* _g_GetActiveProbMan(  InputTerminal* _pInterm)
{	
	assert(_pInterm);
	assert( _pInterm->m_pAirportDB );	
	return _pInterm->m_pAirportDB->getProbDistMan();
}

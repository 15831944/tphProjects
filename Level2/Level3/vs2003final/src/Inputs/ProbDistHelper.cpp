// ProbDistHelper.cpp: implementation of the ProbDistHelper class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/floatutils.h"
#include "ProbDistHelper.h"


enum {
	PDERROR_NEGATIVE = 0,
	PDERROR_MAXEXCEEDSMIN,
	PDERROR_ZEROORLESS,
	PDERROR_NOTZEROTOONE,
	PDERROR_NORMALTRUNCRANGE,
	PDERROR_MINEXCEEDSMODE,
	PDERROR_MODEEXCEEDSMAX,
};

static const char* errormessages[] = {
	"Parameter must be greater or equal to zero",
	"Maximum value must be greater than minimum value",
	"Parameter must be strictly greater than zero",
	"Parameter value must be in range [0.0,1.0]",
	"Truncation parameter must be in the range [3, 20]",
	"The mode must be greater than the minimum value",
	"The mode cannot exceed the maximum value",
};

static const char* constantnames[] = { "Constant Value" };
static const char* uniformnames[] = { "Maximum Value", "Minimum Value" };
static const char* normalnames[] = { "Mean", "Std. Dev.", "Truncate at" };
static const char* gammanames[] = { "Gamma (shape)", "Beta (scale)", "Mu (location)" };
static const char* betanames[] = { "Alpha", "Beta", "Maximum Value", "Minimum Value" };
static const char* weibullnames[] = { "Alpha (scale)", "Gamma (shape)", "Mu (location)" };
static const char* exponentialnames[] = { "Lambda", "Mean" };
static const char* bernoullinames[] = { "First Value", "Second Value", "First Probability" };
static const char* erlangnames[] = { "Gamma (shape)", "Beta (scale)", "Mu (location)" };
static const char* trianglenames[] = { "Maximum Value", "Minimum Value", "Mode" };

static const char** parameternames[] = {
	bernoullinames,
	betanames,
	constantnames,
	NULL,
	exponentialnames,
	NULL,
	normalnames,
	uniformnames,
	NULL,
	NULL,
	NULL,
	NULL,
	weibullnames,
	gammanames,
	erlangnames,
	trianglenames,
};

const char* ProbDistHelper::GetErrorMsg(int nErrorID)
{
	return errormessages[nErrorID];
}


int ProbDistHelper::GetParamCount(const ProbabilityDistribution* _pd)
{
	switch(_pd->getProbabilityType()) {
	case CONSTANT:
		return 1;
	case EXPONENTIAL:
	case UNIFORM:
		return 2;
	case GAMMA:
	case WEIBULL:
	case BERNOULLI:
	case ERLANG:
	case TRIANGLE:
	case NORMAL:
		return 3;
	case BETA:
		return 4;
	case HISTOGRAM:
	case EMPIRICAL: {
		//const HistogramDistribution* hist = dynamic_cast<const HistogramDistribution*>(_pd);
		const HistogramDistribution* hist = (const HistogramDistribution*)_pd;
		return hist->getCount();
		}
	default:
		ASSERT(0);
		return 0;
	}
}

const char* ProbDistHelper::GetParamName(const ProbabilityDistribution* _pd, int idx)
{
	if (_pd->getProbabilityType() == HISTOGRAM ||
		_pd->getProbabilityType() == EMPIRICAL)
	{
		ASSERT(0);
		return "";
	}
	else {
		return parameternames[_pd->getProbabilityType()][idx];
	}
}

BOOL ProbDistHelper::SetParamValue(const ProbabilityDistribution* _pd, int idx, double val, int* pError)
{
	switch(_pd->getProbabilityType()) {
	case CONSTANT:
		((ConstantDistribution*)_pd)->setConstant(val);
		break;
	case EXPONENTIAL:
		switch(idx) {
		case 0:
			if(val < 0.0) {
				if(pError) *pError = PDERROR_NEGATIVE;
				return FALSE;
			}
			((ExponentialDistribution*)_pd)->setLambda(val);
			break;
		case 1:
			if(val < 0.0) {
				if(pError) *pError = PDERROR_NEGATIVE;
				return FALSE;
			}
			((ExponentialDistribution*)_pd)->setLambda(1.0/val);
			break;
		}
		break;
	case NORMAL:
		switch(idx) {
		case 0:
			((NormalDistribution*)_pd)->setMean(val);
			break;
		case 1:
			if(val < 0.0) {
				if(pError) *pError = PDERROR_NEGATIVE;
				return FALSE;
			}
			((NormalDistribution*)_pd)->setStdDev(val);
			break;
		case 2:
			if(val < 3.0 || val > 20.0) {
				if(pError) *pError = PDERROR_NORMALTRUNCRANGE;
				return FALSE;
			}
			((NormalDistribution*)_pd)->setTruncation((int) RoundDouble(val, 0));
			break;
		}
		break;
	case TRIANGLE:
		switch(idx) {
		case 1: {
			double max = ((TriangleDistribution*)_pd)->getMax();
			if(val > max){
				if(pError) *pError = PDERROR_MAXEXCEEDSMIN;
				return FALSE;
			}
			((TriangleDistribution*)_pd)->setMin(val);
			break;
		}
		case 0: {
			double min = ((TriangleDistribution*)_pd)->getMin();
			if(min > val) {
				if(pError) *pError = PDERROR_MAXEXCEEDSMIN;
				return FALSE;
			}
			((TriangleDistribution*)_pd)->setMax(val);
			break;
		}
		case 2: {
			double min = ((TriangleDistribution*)_pd)->getMin();
			double max = ((TriangleDistribution*)_pd)->getMax();
			if(val < min) {
				if(pError) *pError = PDERROR_MINEXCEEDSMODE;
				return FALSE;
			}
			if(val > max) {
				if(pError) *pError = PDERROR_MODEEXCEEDSMAX;
				return FALSE;
			}
			((TriangleDistribution*)_pd)->setMode(val);
			break;
		}
		}
		break;
	case UNIFORM:
		switch(idx) {
		case 1: {
			double max = ((UniformDistribution*)_pd)->getMax();
			if(val > max){
				if(pError) *pError = PDERROR_MAXEXCEEDSMIN;
				return FALSE;
			}
			((UniformDistribution*)_pd)->setMin(val);
			break;
		}
		case 0: {
			double min = ((UniformDistribution*)_pd)->getMin();
			if(min > val) {
				if(pError) *pError = PDERROR_MAXEXCEEDSMIN;
				return FALSE;
			}
			((UniformDistribution*)_pd)->setMax(val);
			break;
		}
		}
		break;
	case GAMMA:
			switch(idx) {
		case 0:
			if(val <= 0.0){
				if(pError) *pError = PDERROR_ZEROORLESS;
				return FALSE;
			}
			((GammaDistribution*)_pd)->setGamma(val);
			break;
		case 1:
			if(val <= 0.0){
				if(pError) *pError = PDERROR_ZEROORLESS;
				return FALSE;
			}
			((GammaDistribution*)_pd)->setBeta(val);
			break;
		case 2:
			((GammaDistribution*)_pd)->setMu(val);
			break;
		}
		break;
	case ERLANG:
		switch(idx) {
		case 0:
			if(val <= 0.0){
				if(pError) *pError = PDERROR_ZEROORLESS;
				return FALSE;
			}
			((ErlangDistribution*)_pd)->setGamma((int) RoundDouble(val,0));
			break;
		case 1:
			if(val <= 0.0){
				if(pError) *pError = PDERROR_ZEROORLESS;
				return FALSE;
			}
			((ErlangDistribution*)_pd)->setBeta(val);
			break;
		case 2:
			((ErlangDistribution*)_pd)->setMu(val);
			break;
		}
		break;

	case WEIBULL:
		switch(idx) {
		case 0:
			if(val <= 0.0){
				if(pError) *pError = PDERROR_ZEROORLESS;
				return FALSE;
			}
			((WeibullDistribution*)_pd)->setAlpha(val);
			break;
		case 1:
			if(val <= 0.0){
				if(pError) *pError = PDERROR_ZEROORLESS;
				return FALSE;
			}
			((WeibullDistribution*)_pd)->setGamma(val);
			break;
		case 2:
			((WeibullDistribution*)_pd)->setMu(val);
			break;
		}
		break;
	case BERNOULLI:
		switch(idx) {
		case 0:
			((BernoulliDistribution*)_pd)->setValue1(val);
			break;
		case 1:
			((BernoulliDistribution*)_pd)->setValue2(val);
			break;
		case 2:
			if(val < 0.0 || val > 1.0){
				if(pError) *pError = PDERROR_NOTZEROTOONE;
				return FALSE;
			}
			((BernoulliDistribution*)_pd)->setProb1(val);
			break;
		}
		break;
	case BETA:
		switch(idx) {
		case 0: {
			int iVal = (int) val;
			if(iVal<=0){
				if(pError) *pError = PDERROR_ZEROORLESS;
				return FALSE;
			}
			int beta = ((BetaDistribution*)_pd)->getBeta();
			double min = ((BetaDistribution*)_pd)->getMin();
			double max = ((BetaDistribution*)_pd)->getMax();
			((BetaDistribution*)_pd)->resetValues(min, max, iVal, beta);
			break;
		}
		case 1: {
			int iVal = (int) val;
			if(iVal<=0){
				if(pError) *pError = PDERROR_ZEROORLESS;
				return FALSE;
			}
			int alpha = ((BetaDistribution*)_pd)->getAlpha();
			double min = ((BetaDistribution*)_pd)->getMin();
			double max = ((BetaDistribution*)_pd)->getMax();
			((BetaDistribution*)_pd)->resetValues(min, max, alpha, iVal);
			break;
		}
		case 3: {
			
			double max = ((BetaDistribution*)_pd)->getMax();
			if(max < val) {
				if(pError) *pError = PDERROR_MAXEXCEEDSMIN;
				return FALSE;
			}
			int alpha = ((BetaDistribution*)_pd)->getAlpha();
			int beta = ((BetaDistribution*)_pd)->getBeta();
			((BetaDistribution*)_pd)->resetValues(val, max, alpha, beta);
			break;
		}
		case 2: {
			double min = ((BetaDistribution*)_pd)->getMin();
			if(val < min) {
				if(pError) *pError = PDERROR_MAXEXCEEDSMIN;
				return FALSE;
			}
			int alpha = ((BetaDistribution*)_pd)->getAlpha();
			int beta = ((BetaDistribution*)_pd)->getBeta();
			((BetaDistribution*)_pd)->resetValues(min, val, alpha, beta);
			break;
		}
		}
		break;
		
	default:
		ASSERT(0);
		break;
	}

	return TRUE;
}

double ProbDistHelper::GetParamValue(const ProbabilityDistribution* _pd, int idx)
{
	switch(_pd->getProbabilityType()) {
	case CONSTANT:
		return ((ConstantDistribution*)_pd)->getMean();
	case EXPONENTIAL:
		switch(idx) {
		case 0:
			return ((ExponentialDistribution*)_pd)->getLambda();
		case 1:
			return 1.0/((ExponentialDistribution*)_pd)->getLambda();
		}
	case NORMAL:
		switch(idx) {
		case 0:
			return ((NormalDistribution*)_pd)->getMean();
		case 1:
			return ((NormalDistribution*)_pd)->getStdDev();
		case 2:
			return ((NormalDistribution*)_pd)->getTruncation();
		}
	case UNIFORM:
		switch(idx) {
		case 1:
			return ((UniformDistribution*)_pd)->getMin();
		case 0:
			return ((UniformDistribution*)_pd)->getMax();
		}
	case GAMMA:
		switch(idx) {
		case 0:
			return ((GammaDistribution*)_pd)->getGamma();
		case 1:
			return ((GammaDistribution*)_pd)->getBeta();
		case 2:
			return ((GammaDistribution*)_pd)->getMu(); 
		}
	case WEIBULL:
		switch(idx) {
		case 0:
			return ((WeibullDistribution*)_pd)->getAlpha();
		case 1:
			return ((WeibullDistribution*)_pd)->getGamma();
		case 2:
			return ((WeibullDistribution*)_pd)->getMu();
		}
	case ERLANG:
		switch(idx) {
		case 0:
			return ((ErlangDistribution*)_pd)->getGamma();
		case 1:
			return ((ErlangDistribution*)_pd)->getBeta();
		case 2:
			return ((ErlangDistribution*)_pd)->getMu();
		}
	case TRIANGLE:
		switch(idx) {
		case 1:
			return ((TriangleDistribution*)_pd)->getMin();
		case 0:
			return ((TriangleDistribution*)_pd)->getMax();
		case 2:
			return ((TriangleDistribution*)_pd)->getMode();
		}		
	case BERNOULLI:
		switch(idx) {
			case 0:
			return ((BernoulliDistribution*)_pd)->getValue1();
		case 1:
			return ((BernoulliDistribution*)_pd)->getValue2();
		case 2:
			return ((BernoulliDistribution*)_pd)->getProb1();
		}
	case BETA:
		switch(idx) {
		case 0:
			return ((BetaDistribution*)_pd)->getAlpha();
		case 1:
			return ((BetaDistribution*)_pd)->getBeta();
		case 3:
			return ((BetaDistribution*)_pd)->getMin();
		case 2:
			return ((BetaDistribution*)_pd)->getMax();
		}
	default:
		ASSERT(0);
		return 0;
	}
}

bool ProbDistHelper::HasParams(const ProbabilityDistribution* _pd)
{
	if (!_pd ||
		_pd->getProbabilityType() == HISTOGRAM ||
		_pd->getProbabilityType() == EMPIRICAL)
	{
		return false;
	}
	else {
		return true;
	}
}

double ProbDistHelper::GetSampleValue(const ProbabilityDistribution* _pd, int idx)
{
	ASSERT(_pd->getProbabilityType() == HISTOGRAM || _pd->getProbabilityType() == EMPIRICAL);
	const HistogramDistribution* hist = (const HistogramDistribution*)_pd;
	ASSERT(idx < hist->getCount());
	return static_cast<double>(hist->getValue(idx));
}

double ProbDistHelper::GetSamplePct(const ProbabilityDistribution* _pd, int idx)
{
	ASSERT(_pd->getProbabilityType() == HISTOGRAM || _pd->getProbabilityType() == EMPIRICAL);
	const HistogramDistribution* hist = (const HistogramDistribution*)_pd;
	ASSERT(idx < hist->getCount());
	return static_cast<double>(hist->getNoAccumProb(idx));
}

double ProbDistHelper::GetTotalSamplePct(const ProbabilityDistribution* _pd)
{
	ASSERT(_pd->getProbabilityType() == HISTOGRAM || _pd->getProbabilityType() == EMPIRICAL);
	const HistogramDistribution* hist = (const HistogramDistribution*)_pd;
	return static_cast<double>(hist->getProb(hist->getCount()-1));
}

ProbabilityDistribution* ProbDistHelper::CreateProbabilityDistribution(int nType)
{
	switch(nType) {
	case CONSTANT:
		return new ConstantDistribution(0.0);
	case EXPONENTIAL:
		return new ExponentialDistribution(1.0);
	case NORMAL:
		return new NormalDistribution(0.0, 1.0);
	case UNIFORM:
		return new UniformDistribution(0.0, 1.0);
	case GAMMA:
		return new GammaDistribution(2.0, 1.0, 0.0);
	case ERLANG:
		return new ErlangDistribution(2, 1.0, 0.0);
	case TRIANGLE:
		return new TriangleDistribution(0.0, 1.0, 0.5);
	case WEIBULL:
		return new WeibullDistribution(1.0, 2.0, 0.0);
	case BERNOULLI:
		return new BernoulliDistribution(0.0, 1.0, 0.5);
	case BETA:
		return new BetaDistribution(0.0, 1.0);
	case HISTOGRAM:
		return new HistogramDistribution;
	case EMPIRICAL:
		return new EmpiricalDistribution;
	default:
		ASSERT(0);
		return NULL;
	}
}

bool ProbDistHelper::IsValidHistogram(const ProbabilityDistribution* _pd)
{
	ASSERT(_pd->getProbabilityType() == HISTOGRAM || _pd->getProbabilityType() == EMPIRICAL);
	if(_pd->getProbabilityType() == EMPIRICAL)
		return true;
	const HistogramDistribution* hist = (const HistogramDistribution*)_pd;
	if(hist->getCount()>1 && (hist->getProb(hist->getCount()-1)-hist->getProb(hist->getCount()-2) == 0))
		return true;
	return false;
}

bool ProbDistHelper::Is100Pct(const ProbabilityDistribution* _pd)
{
	ASSERT(_pd->getProbabilityType() == HISTOGRAM || _pd->getProbabilityType() == EMPIRICAL);
	const HistogramDistribution* hist = (const HistogramDistribution*)_pd;
	if(hist->getCount() > 0)
	{
		double dValue = hist->getProb(hist->getCount()-1); 
		if( RoundDouble(dValue, 2) == 100.00 )
			return true;
	}
	return false;
}

int ProbDistHelper::GetPDFGraphPointCount(const ProbabilityDistribution* _pd)
{
	switch(_pd->getProbabilityType()) 
	{
	case CONSTANT:
		return 5;
	case UNIFORM:
		return 6;
	case TRIANGLE:
		return 3;
	case HISTOGRAM:
		return 2*(((HistogramDistribution*)_pd)->getCount()-1) + 2;
	case EMPIRICAL:
		return 3*((EmpiricalDistribution*)_pd)->getCount();
	default:
		return 101;
	}
}

bool ProbDistHelper::GetPDFGraphPoints(const ProbabilityDistribution* _pd, double* x, double* y)
{
	int nCount = ProbDistHelper::GetPDFGraphPointCount(_pd);
	switch(_pd->getProbabilityType()) {
		case CONSTANT: {
			double c = ((ConstantDistribution*)_pd)->getConstant();
			x[0] = c-1.0; y[0] = 0.0;
			x[1] = c; y[1] = 0.0;
			x[2] = c; y[2] = 1.0;
			x[3] = c; y[3] = 0.0;
			x[4] = c+1.0; y[4] = 0.0;
			break;
		}
		case UNIFORM: {
			double min = ((UniformDistribution*)_pd)->getMin();
			double max = ((UniformDistribution*)_pd)->getMax();
			x[0] = _pd->getMinXValue(); y[0] = 0.0;
			x[1] = min; y[1] = 0.0;
			x[2] = min; y[2] = 1.0/(max-min);
			x[3] = max; y[3] = 1.0/(max-min);
			x[4] = max; y[4] = 0.0;
			x[5] = _pd->getMaxXValue(); y[5] = 0.0;
			break;
		}
		case TRIANGLE: {
			double min = ((TriangleDistribution*)_pd)->getMin();
			double max = ((TriangleDistribution*)_pd)->getMax();
			double mode = ((TriangleDistribution*)_pd)->getMode();
			x[0] = min; y[0] = 0.0;
			x[1] = mode; y[1] = 2/(max-min);
			x[2] = max; y[2] = 0.0;
			break;			
		}
		case HISTOGRAM: {
			HistogramDistribution* hist = (HistogramDistribution*)_pd;
			int nHistCount = hist->getCount();
			if(nHistCount > 0) {
			
				double dMin = _pd->getMinXValue();
				double dMax = _pd->getMaxXValue();
				
				for(int i=0; i<nHistCount-1; i++) {
					int twoi = 1+2*i;
					x[twoi] = hist->getValue(i);
					x[twoi+1] = hist->getValue(i+1);
					y[twoi] = y[twoi+1] = hist->getNoAccumProb(i)/(x[twoi+1]-x[twoi]);
				}
				x[0] = x[1]; y[0] = 0.0;
				x[nCount-1] = x[nCount-2]; y[nCount-1] = 0.0;
			}
			break;
		}
		case EMPIRICAL: {
			EmpiricalDistribution* emp = (EmpiricalDistribution*)_pd;
			int nEmpCount = emp->getCount();
			for(int i=0; i<nEmpCount; i++) {
				int threei = 3*i;
				x[threei] = x[threei+1] = x[threei+2] = emp->getValue(i);
				y[threei] = y[threei+2] = 0.0;
				y[threei+1] = emp->getNoAccumProb(i);
			}
			break;
		}
		default: {
			double dMin = _pd->getMinXValue();
			double dMax = _pd->getMaxXValue();
			double dStep = (dMax-dMin)*(1.0/(nCount-1));
			for(int i=0; i<nCount; i++) {
				x[i] = dMin + i*dStep;
				y[i] = _pd->pdf(x[i]);
			}
		}
	}
	
	return true;
}

int ProbDistHelper::GetCDFGraphPointCount(const ProbabilityDistribution* _pd)
{
	switch(_pd->getProbabilityType()) {
	case CONSTANT:
		return 5;
	case UNIFORM:
		return 6;
	case NORMAL:
		return 161;
	case HISTOGRAM:
		return ((HistogramDistribution*)_pd)->getCount();
	case EMPIRICAL:
		return 3*((EmpiricalDistribution*)_pd)->getCount();
	default:
		return 101;
	}
}

bool ProbDistHelper::GetCDFGraphPoints(const ProbabilityDistribution* _pd, double* x, double* y)
{
	int nCount = ProbDistHelper::GetCDFGraphPointCount(_pd);
	switch(_pd->getProbabilityType()) {
		case CONSTANT: {
			double c = ((ConstantDistribution*)_pd)->getConstant();
			x[0] = c-1.0; y[0] = 0.0;
			x[1] = c; y[1] = 0.0;
			x[2] = c; y[2] = 1.0;
			x[3] = c; y[3] = 0.0;
			x[4] = c+1.0; y[4] = 0.0;
			break;
		}
		case UNIFORM: {
			double min = ((UniformDistribution*)_pd)->getMin();
			double max = ((UniformDistribution*)_pd)->getMax();
			x[0] = _pd->getMinXValue(); y[0] = 0.0;
			x[1] = min; y[1] = 0.0;
			x[2] = min; y[2] = 0.0;
			x[3] = max; y[3] = 1.0;
			x[4] = max; y[4] = 0.0;
			x[5] = _pd->getMaxXValue(); y[5] = 0.0;
			break;
		}
		case HISTOGRAM: {
			HistogramDistribution* hist = (HistogramDistribution*)_pd;
			int nHistCount = hist->getCount();
			if(nHistCount > 0) {
			
				double dMin = _pd->getMinXValue();
				double dMax = _pd->getMaxXValue();

				x[0] = hist->getValue(0);
				y[0] = 0.0;
				for(int i=1; i<nHistCount; i++) {
					x[i] = hist->getValue(i);
					y[i] = hist->getProb(i-1)/100.0;
				}
			}
			break;
		}
		case EMPIRICAL: {
			EmpiricalDistribution* emp = (EmpiricalDistribution*)_pd;
			int nEmpCount = emp->getCount();
			for(int i=0; i<nEmpCount; i++) {
				int threei = 3*i;
				x[threei] = x[threei+1] = x[threei+2] = emp->getValue(i);
				y[threei] = y[threei+2] = 0.0;
				y[threei+1] = emp->getProb(i);
			}
			break;
		}
		default: {
			double dMin = _pd->getMinXValue();
			double dMax = _pd->getMaxXValue();
			double dStep = (dMax-dMin)*(1.0/(nCount-1));
			for(int i=0; i<nCount; i++) {
				x[i] = dMin + i*dStep;
				y[i] = _pd->cdf(x[i]);
			}
		}
	}
	
	return true;
}
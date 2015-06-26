#pragma once

#include <math.h>
#include "template.h"
#include "containr.h"
#include "util.h"
#include "../Database/ARCportDatabaseConnection.h"
class CAirportDatabase;
enum ProbTypes
{
	BERNOULLI=0,
	BETA,
	CONSTANT,
	EMPIRICAL,
	EXPONENTIAL,
	HISTOGRAM,
	NORMAL,
	UNIFORM,
	PASSENGER_TYPE,
	PROCESSOR,
	FLIGHT_TYPE,
	PROCESSORWITHPIPE,
	WEIBULL,
	GAMMA,
	ERLANG,
	TRIANGLE,
};

enum RandomVariableType
{
	DISCRETE,
	CONTINUOUS,
};


enum
{
    PDERROR_MINEXCEEDMAX = 0,
    PDERROR_BERNOULLI_PROB1,
    PDERROR_NORMAL_STDDEV,
    PDERROR_NORMAL_TRUNCRANGE,
    PDERROR_TRIANGLE_MINEXCEEDMODE,
    PDERROR_TRIANGLE_MODEEXCEEDMAX,
    PDERROR_EXPO_LAMBDA,
    PDERROR_BETA_ALPHA,
    PDERROR_BETA_BETA,
    PDERROR_ERLANG_GAMMA,
    PDERROR_ERLANG_BETA,
    PDERROR_GAMMA_GAMMA,
    PDERROR_GAMMA_BETA,
    PDERROR_WEIBULL_ALPHA,
    PDERROR_WEIBULL_GAMMA,
};

class ProbabilityDistribution
{
public:
	ProbabilityDistribution () {};
	virtual ~ProbabilityDistribution () {};

	virtual double getRandomValue (void) const = 0;
	virtual double getMean (void) const = 0;
	virtual double getVarience(void) const { return 0.0; }
	virtual void readDistribution (ArctermFile& p_file);
	virtual void readVersion1 (ArctermFile& p_file)
	{ readDistribution (p_file); }
	virtual void writeDistribution (ArctermFile& p_file) const;
	virtual void setDistribution (char *p_str) {};
	virtual void screenPrint (char *p_str) const {};
	virtual void printDistribution (char *p_str) const {};
	virtual const char *getProbabilityName (void) const = 0;
	virtual int getProbabilityType (void) const = 0;
	virtual const char *getRandomVariableTypeName(void) const { return "CONTINUOUS"; }
	virtual int getRandomVariableType(void) const { return CONTINUOUS; }

	// simple encapsulation of virtual void screenPrint (char *p_str) const {};
	// return string for display
	CString screenPrint () const;
	// simple encapsulation of virtual void printDistribution (char *p_str) const {};
	// return string for persistence
	CString printDistribution () const;

    CString getErrorMessage(int iErr);

	//for graphing
	virtual double getMinXValue() const;
	virtual double getMaxXValue() const;
	virtual double pdf(double _x) const;
	virtual double cdf(double _x) const;
	virtual void InitRules(){};
    virtual bool isEqual(const ProbabilityDistribution* pOther)=0;
//the interface for database 
public:
	virtual void readDistributionFromDB(CString str);
	virtual int WriteDistributionToDB(CString pProBname ,int _airportDBID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;

	//new version database save
	int saveDistribution(const CString& pProbName,CAirportDatabase* pAirportDatabase);
	//creates a copy of the PD pointed to buy _pPD and returns a generic ProbabilityDistribution*
	static ProbabilityDistribution* CopyProbDistribution(ProbabilityDistribution* _pPD);

	// Reads a probability distribution into the appropriate subclass of
	// ProbabilityDistribution and returns a generic pointer to it
	// Assumes that the current position is valid
	// Exception: FileFormatError
	//From  DataBase ,added for database 
	//if no object create , throw TwoStringError exception and return NULL 
	static ProbabilityDistribution* GetTerminalRelateProbDistributionFromDB(CString pProbData, bool _bHandleProcDistWithPipe =false) ;


};

class BernoulliDistribution : public ProbabilityDistribution
{
protected:

	double value1;
	double value2;
	double prob;

public:
	BernoulliDistribution () { value1 = value2 = prob = 0.0; }
	BernoulliDistribution (double val1, double val2, double prob1)
	{ init (val1, val2, prob1); }
	virtual ~BernoulliDistribution () {};

	virtual double getRandomValue (void) const;
	virtual double getMean (void) const
	{ return (prob * value1) + ((1.0 - prob) * value2); }

	void init (double val1, double val2, double prob1)
	{ value1 = val1; value2 = val2; prob = prob1; }
    bool resetValues(int& iErr, double val1, double val2, double prob1);
	double getValue1 (void) { return value1; }
	double getValue2 (void) { return value2; }
	double getProb1 (void) { return prob; }
	double getProb2 (void) { return 1.0 - prob; }

	void setValue1(double _val) { value1 = _val; }
	void setValue2(double _val) { value2 = _val; }
	void setProb1(double _prob) { prob = _prob; }
	void setProb2(double _prob) { prob = 1.0-_prob; }

	virtual void setDistribution (char *p_str);
	virtual void readVersion1 (ArctermFile& p_file);
	virtual void screenPrint (char *p_str) const;
	virtual void printDistribution (char *p_str) const;
	virtual const char *getProbabilityName (void) const { return "Bernoulli"; }
	virtual int getProbabilityType (void) const { return BERNOULLI; }
	virtual int getRandomVariableType(void) const { return DISCRETE; }
	virtual const char *getRandomVariableTypeName(void) const { return "DISCRETE"; }
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};

typedef std::pair<double, double> pair_range;

class HistogramDistribution : public ProbabilityDistribution
{
protected:

	int count;              // number of values
	double *probs;   // 1/100 of the accumulated probability
	double *values;

public:
	HistogramDistribution () { probs = NULL; values = NULL; count = 0; }
	HistogramDistribution (int p_count, const double *p_vals,
		const double* p_probs) { probs = NULL; values = NULL;
	count = 0; init (p_count, p_vals, p_probs); }
		virtual ~HistogramDistribution () { clear(); }


		void allocate (int Count);
		void clear (void);
		void init (int Count, const double *Values, const double *Probs);
		void initAccum (int Count, const double *Values,
			const double *Probs);
		virtual BOOL InitFromVector(std::vector<double>* pvdbValue, std::vector<pair_range>* pvPairRange);

		virtual double getRandomValue (void) const;
		double getMean (void) const;
		double getVarience(void) const;

		int getCount (void) const { return count; }
		double getValue (int index) const { return values[index]; }
		double getProb (int index) const { return probs[index]; }
		double getNoAccumProb(int index) const;
		void setProb (double p_percent, int p_ndx);
		double getFloatProb (int index) const
		{ return ((double)probs[index]) / 100.0; }
		void resetValues (const double *Values, const double *Probs);
		void EvenPercent();

		virtual const char *getProbabilityName (void) const { return "Histogram"; }
		virtual int getProbabilityType (void) const { return HISTOGRAM; }
		virtual void setDistribution (char *p_str);
		virtual void readVersion1 (ArctermFile& p_file);
		virtual void screenPrint (char *p_str) const;
		virtual void printDistribution (char *p_str) const;

		virtual double getMinXValue() const;
		virtual double getMaxXValue() const;
		virtual double pdf(double _x) const;
		virtual double cdf(double _x) const; 
        virtual bool isEqual(const ProbabilityDistribution* pOther);
};

class EmpiricalDistribution : public HistogramDistribution
{
public:
	EmpiricalDistribution () {};
	EmpiricalDistribution (int p_count, const double *p_vals,
		const double *p_probs) { probs = NULL; values = NULL;
	count = 0; init (p_count, p_vals, p_probs); }
		virtual ~EmpiricalDistribution () {};

		virtual double getRandomValue (void) const;
		virtual const char *getProbabilityName (void) const { return "Empirical"; }
		virtual int getProbabilityType (void) const { return EMPIRICAL; }
		virtual int getRandomVariableType(void) const { return DISCRETE; }
		virtual const char *getRandomVariableTypeName(void) const { return "DISCRETE"; }

		//BOOL InitFromVector(std::vector<double>* pvdbValue);
		virtual BOOL InitFromVector(std::vector<double>* pvdbValue, std::vector<pair_range>* pvPairRange);

};

class UniformDistribution : public ProbabilityDistribution
{
protected:

	double min, max, interval;
public:
	UniformDistribution () {};
	UniformDistribution (double p_min, double p_max) { init (p_min, p_max); }
	virtual ~UniformDistribution () {};


	void init (double Min, double Max) { interval = Max - Min;
	max = Max; min = Min; }
    bool resetValues(int& iErr, double Min, double Max);

	double getRandomValue (void) const;
	double getMean (void) const { return (max + min) / 2.0; }
	double getVarience (void) const { return pow(max - min, 2) / 12.0; } 
	double getInterval (void) const { return interval; }

	double getMin (void) const { return min; }
	double getMax (void) const { return max; }

	void setMin(double _min) { min=_min; interval=max-min; }
	void setMax(double _max) { max=_max; interval=max-min; }

	virtual const char *getProbabilityName (void) const { return "Uniform"; }
	virtual int getProbabilityType (void) const { return UNIFORM; }
	virtual void setDistribution (char *p_str);
	virtual void readVersion1 (ArctermFile& p_file);
	virtual void screenPrint (char *p_str) const;
	virtual void printDistribution (char *p_str) const;

	virtual double getMinXValue() const;
	virtual double getMaxXValue() const;
	virtual double pdf(double _x) const;
    virtual double cdf(double _x) const;
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};


class ConstantDistribution : public ProbabilityDistribution
{
protected:

	double value;
public:
	ConstantDistribution () {};
	ConstantDistribution (double p_val) { init (p_val); }
	virtual ~ConstantDistribution (){};

	void init (double Value) { value = Value; }
    bool resetValues(double Value){ value = Value; return true; }

	double getRandomValue (void) const { return value; }
	double getMean (void) const { return value; }
	double getConstant() const { return value; }
	void setConstant(double _c) { value=_c; }

	virtual const char *getProbabilityName (void) const { return "Constant"; }
	virtual int getProbabilityType (void) const { return CONSTANT; }
	virtual void readVersion1 (ArctermFile &p_file);
	virtual void setDistribution (char *p_str);
	virtual void screenPrint (char *p_str) const;
	virtual void printDistribution (char *p_str) const;
	virtual int getRandomVariableType(void) const { return DISCRETE; }
	virtual const char *getRandomVariableTypeName(void) const { return "DISCRETE"; }

	virtual double getMinXValue() const;
	virtual double getMaxXValue() const;
	virtual double pdf(double _x) const;
    virtual double cdf(double _x) const;
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};

class TriangleDistribution : public ProbabilityDistribution
{
protected:
	double a;
	double b;
	double mode;
public:
	TriangleDistribution() {};

	TriangleDistribution (double _a, double _b, double _mode) { init (_a, _b, _mode); }
	virtual ~TriangleDistribution (){};

	void init (double _a, double _b, double _mode) { a=_a; b=_b; mode=_mode; }
    bool resetValues(int& iErr, double _min, double _max, double _mode);
	double getRandomValue (void) const;
	double getMean (void) const { return (a+b+mode)/3; }
	double getMin() const { return a; }
	double getMax() const { return b; }
	double getMode() const { return mode; }

	void setMin(double _a) { a=_a; }
	void setMax(double _b) { b=_b; }
	void setMode(double _mode) { mode=_mode; }

	virtual const char *getProbabilityName (void) const { return "Triangle"; }
	virtual int getProbabilityType (void) const { return TRIANGLE; }
	virtual void readVersion1 (ArctermFile &p_file);
	virtual void setDistribution (char *p_str);
	virtual void screenPrint (char *p_str) const;
	virtual void printDistribution (char *p_str) const;

	virtual double getMinXValue() const;
	virtual double getMaxXValue() const;
	virtual double pdf(double _x) const;
	virtual double cdf(double _x) const;
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};

class NormalDistribution : public ProbabilityDistribution
{
protected:

	//static int probs[];
	//static int count;
	double mean, stdDeviation;
	int truncation;
public:
	static double getNormalRandomValue(void);
	NormalDistribution () {};
	NormalDistribution (double p_mean, double p_stdDev, int _truncation=6)
	{ init (p_mean, p_stdDev, _truncation); }

	virtual ~NormalDistribution (){};
	void init (double Mean, double stdDev, int _truncation)
	{ mean = Mean; stdDeviation = stdDev; truncation = _truncation; }
    bool resetValues(int& iErr, double Mean, double stdDev, int _truncation);
	double getRandomValue (void) const;
	double getMean (void) const { return mean; }
	double getStdDev (void) const { return stdDeviation; }
	double getVarience (void) const { return stdDeviation; }
	int getTruncation() const { return truncation; }
	void setMean(double _mean) { mean = _mean; }
	void setStdDev(double _stddev) { stdDeviation = _stddev; }
	void setTruncation(int _trunc) { truncation = _trunc; }


	virtual const char *getProbabilityName (void) const { return "Normal"; }
	virtual int getProbabilityType (void) const { return NORMAL; }
	virtual void setDistribution (char *p_str);
	virtual void readVersion1 (ArctermFile& p_file);
	virtual void screenPrint (char *p_str) const;
	virtual void printDistribution (char *p_str) const;

	virtual double getMinXValue() const;
	virtual double getMaxXValue() const;
	virtual double pdf(double _x) const;
	virtual double cdf(double _x) const;
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};

class WeibullDistribution : public ProbabilityDistribution
{
protected:
	double alpha; //scale
	double gamma; //shape
	double mu;	  //location
public:
	WeibullDistribution () {};
	WeibullDistribution (double _alpha, double _gamma, double _mu) { init (_alpha, _gamma, _mu); }
	virtual ~WeibullDistribution (){};

	void init (double _alpha, double _gamma, double _mu)
	{ alpha = _alpha; gamma = _gamma; mu = _mu; }
    bool resetValues(int& iErr, double _alpha, double _gamma, double _mu);
	double getRandomValue (void) const;
	double getMean (void) const;

	double getAlpha() const { return alpha; }
	double getGamma() const { return gamma; }
	double getMu() const { return mu; }

	void setAlpha(double _alpha) { alpha=_alpha; }
	void setGamma(double _gamma) { gamma=_gamma; }
	void setMu(double _mu) { mu=_mu; }

	virtual const char *getProbabilityName (void) const { return "Weibull"; }
	virtual int getProbabilityType (void) const { return WEIBULL; }
	virtual void setDistribution (char *p_str);
	virtual void screenPrint (char *p_str) const;
	virtual void printDistribution (char *p_str) const;

	virtual double getMinXValue() const;
	virtual double getMaxXValue() const;
	virtual double pdf(double _x) const;
    virtual double cdf(double _x) const;
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};

class GammaDistribution : public ProbabilityDistribution
{
protected:
	double beta; //scale
	double gamma;//shape
	double mu;	 //location
	double gammaln; //set to GammaLn(gamma)
public:
	GammaDistribution () {};
	GammaDistribution (double _gamma, double _beta, double _mu) { init (_gamma, _beta, _mu); }
	virtual ~GammaDistribution (){};

	void init (double _gamma, double _beta, double _mu)
	{ beta = _beta; gamma = _gamma; mu = _mu; gammaln = gammln(gamma); }
    bool resetValues(int& iErr, double _gamma, double _beta, double _mu);
	double getRandomValue (void) const;
	double getMean (void) const { return mu+beta*gamma; }

	double getBeta() const { return beta; }
	double getGamma() const { return gamma; }
	double getMu() const { return mu; }

	void setBeta(double _beta) { beta = _beta; }
	void setGamma(double _gamma) { gamma = _gamma; gammaln = gammln(gamma); }
	void setMu(double _mu) { mu = _mu; }

	virtual const char *getProbabilityName (void) const { return "Gamma"; }
	virtual int getProbabilityType (void) const { return GAMMA; }
	virtual void setDistribution (char *p_str);
	virtual void screenPrint (char *p_str) const;
	virtual void printDistribution (char *p_str) const;

	virtual double getMinXValue() const;
	virtual double getMaxXValue() const;
	virtual double pdf(double _x) const;
	virtual double cdf(double _x) const;

	static double gammln(double _x);
	static double gammp(double _a, double _x);
	static void gser(double* _gamser, double _a, double _x, double* _gln);
    static void gcf(double* _gammcf, double _a, double _x, double* _gln);
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};

class ErlangDistribution : public ProbabilityDistribution
{
protected:
	double beta;	//scale
	int gamma;		//shape
	double mu;		//location

	double gammaln; //set to GammaLn(gamma)

public:
	ErlangDistribution () {};
	ErlangDistribution (int _gamma, double _beta, double _mu) { init (_gamma, _beta, _mu); }
	virtual ~ErlangDistribution (){};

	void init (int _gamma, double _beta, double _mu)
	{ beta = _beta; gamma = _gamma; mu = _mu; }
    bool resetValues(int& iErr, int _gamma, double _beta, double _mu);
	double getRandomValue (void) const;
	double getMean (void) const { return mu+beta*gamma; }

	double getBeta() const { return beta; }
	int getGamma() const { return gamma; }
	double getMu() const { return mu; }

	void setBeta(double _beta) { beta = _beta; }
	void setGamma(int _gamma) { gamma = _gamma; }
	void setMu(double _mu) { mu = _mu; }

	virtual const char *getProbabilityName (void) const { return "Erlang"; }
	virtual int getProbabilityType (void) const { return ERLANG; }
	virtual void setDistribution (char *p_str);
	virtual void screenPrint (char *p_str) const;
	virtual void printDistribution (char *p_str) const;

	virtual double getMinXValue() const;
	virtual double getMaxXValue() const;
	virtual double pdf(double _x) const;
    virtual double cdf(double _x) const;
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};

class ExponentialDistribution : public ProbabilityDistribution
{
protected:

	double lambda;
public:
	ExponentialDistribution () {};
	ExponentialDistribution (double p_lambda) { init (p_lambda); }
	virtual ~ExponentialDistribution (){};

	void init (double Lambda) { lambda = Lambda; }
    bool resetValues(int& iErr, double Lambda);
	double getRandomValue (void) const;
	double getMean (void) const { return 1.0/lambda; }
	double getLambda() const { return lambda; }

	void setLambda(double _lambda) { lambda=_lambda; }

	virtual const char *getProbabilityName (void) const { return "Exponential"; }
	virtual int getProbabilityType (void) const { return EXPONENTIAL; }
	virtual void setDistribution (char *p_str);
	virtual void screenPrint (char *p_str) const;
	virtual void printDistribution (char *p_str) const;

	virtual double getMinXValue() const;
	virtual double getMaxXValue() const;
	virtual double pdf(double _x) const;
    virtual double cdf(double _x) const;
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};


#define DEFAULT_ALPHA   3
#define DEFAULT_BETA    3
class BaseBetaDistribution : public ProbabilityDistribution
{
private:
	static const double Interval;
	static const int Count;

protected:
	int *probs;
	int alpha, beta;
	void init( int _alpha, int _beta );
	void clear( void ) { if (probs) delete [] probs; probs = NULL; }

public:
	static double Beta(double _a, double _b);
	static double Beta(int _a, int _b);
	static double betai(double _a, double _b, double _x);
	static double betacf(double _a, double _b, double _x);

	//    static BaseBetaDistribution * far defaultBaseBetaDistribution;
	BaseBetaDistribution (int alpha = DEFAULT_ALPHA, int beta = DEFAULT_BETA);
	virtual ~BaseBetaDistribution () { clear(); }

	double getRandomValue (void) const ;
	double getMean (void) const { return ((double)alpha / (alpha+beta)); }
	int getAlpha (void) const { return alpha; }
	int getBeta (void) const { return beta; }

	virtual const char *getProbabilityName (void) const { return "Beta"; }
	virtual int getProbabilityType (void) const { return BETA; }
	virtual void setDistribution (char *p_str);
	virtual void readVersion1 (ArctermFile& p_file);
	virtual void screenPrint (char *p_str) const;
    virtual void printDistribution (char *p_str) const;
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};

class BetaDistribution : public ProbabilityDistribution
{
protected:

	BaseBetaDistribution* basebeta;
	double min, max;

public:
	BetaDistribution () { basebeta = NULL; }
	BetaDistribution (double p_min, double p_max,
		double p_alpha = DEFAULT_ALPHA, double p_beta = DEFAULT_BETA)
	{ init (p_min, p_max, p_alpha, p_beta); }
	virtual ~BetaDistribution ();


	void init (double min, double max, double alpha = DEFAULT_ALPHA,
		double beta = DEFAULT_BETA);
    

	double getRandomValue (void) const;
	double getMean (void) const { return basebeta->getMean() * (max-min); }
	double getMin (void) const { return min; }
	double getMax (void) const { return max; }
	int getAlpha (void) const { return basebeta->getAlpha(); }
	int getBeta (void) const { return basebeta->getBeta(); }

	void resetValues (double Min, double Max, int Alpha, int Beta);
    bool resetValues(int& iErr, double Min, double Max, int Alpha, int Beta);
	virtual const char *getProbabilityName (void) const { return "Beta"; }
	virtual int getProbabilityType (void) const { return BETA; }
	void readVersion1 (ArctermFile &p_file);
	virtual void setDistribution (char *p_str);
	virtual void screenPrint (char *p_str) const;
	virtual void printDistribution (char *p_str) const;

	virtual double getMinXValue() const;
	virtual double getMaxXValue() const;
	virtual double pdf(double _x) const;
    virtual double cdf(double _x) const;
    virtual bool isEqual(const ProbabilityDistribution* pOther);
};

extern BaseBetaDistribution * defaultBaseBetaDistribution;
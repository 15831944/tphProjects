#include "stdafx.h"
#include "ProDistrubutionData.h"
#include "../Database/ADODatabase.h"
#include "ProbDistEntry.h"


CProDistrubution::CProDistrubution()
:ID(-1)
,distName(_T("Constant:[0.00]"))
,proType(CONSTANT)
,printDist(_T("Constant:0.00"))
,pProbailityDistrution(NULL)
{
	initProbilityDistribution();
}

CProDistrubution::CProDistrubution( const CProDistrubution& proDist )
{
	ID = -1;
	distName = proDist.distName;
	proType = proDist.proType;
	printDist = proDist.printDist;

	pProbailityDistrution = copyProbalityDistribution();
}

const CProDistrubution& CProDistrubution::operator=( const CProDistrubution& proDist )
{
	ID = -1;
	distName = proDist.distName;
	proType = proDist.proType;
	printDist = proDist.printDist;
	pProbailityDistrution = copyProbalityDistribution();
	return *this;
}

CProDistrubution::~CProDistrubution()
{
	if (pProbailityDistrution)
	{
		delete pProbailityDistrution;
		pProbailityDistrution = NULL;
	}
}

void CProDistrubution::SaveData()
{
	CString strSQL = _T("");
	if (ID != -1)
	{
		return UpdateData();
	}

	strSQL.Format(_T("INSERT INTO PROBABILITYDISTRIBUTION(DISTNAME,PROTYPE,PRINTDIST) VALUES ('%s',%d,'%s')"),distName,(int)proType,printDist);
	ID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CProDistrubution::ReadData(int ndistID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM PROBABILITYDISTRIBUTION WHERE ID = %d"),ndistID);
	CADORecordset rs;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,rs);

	if(!rs.IsEOF())
	{
		ID = ndistID;
		rs.GetFieldValue(_T("DISTNAME"),distName);
		int nProType=0;
		rs.GetFieldValue(_T("PROTYPE"),nProType);
		proType = (ProbTypes)nProType;
		rs.GetFieldValue(_T("PRINTDIST"),printDist);

		initProbilityDistribution();
	}
}

void CProDistrubution::RemoveData()
{
	if(ID >= 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("DELETE FROM PROBABILITYDISTRIBUTION WHERE ID = %d"),ID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

}

void CProDistrubution::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE PROBABILITYDISTRIBUTION SET DISTNAME = '%s',PROTYPE = %d,PRINTDIST = '%s' WHERE ID = %d"),distName,(int)proType,printDist,ID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CProDistrubution::SetProDistrubution(CProbDistEntry* pPDEntry)
{
	ASSERT(pPDEntry);
	distName = pPDEntry->m_csName;
	//ID = pPDEntry->GetID();
	ProbabilityDistribution* pProbDist = NULL;
	pProbDist = pPDEntry->m_pProbDist;
	proType = (ProbTypes)pProbDist->getProbabilityType();
	char szBuffer[1024];
	memset(szBuffer,0,1024*sizeof(char));
	pProbDist->printDistribution(szBuffer);
	printDist = szBuffer;
	initProbilityDistribution();
}

void CProDistrubution::setDistName( const CString& val )
{
	distName = val;
}

const CString& CProDistrubution::getDistName() const
{
	return distName;
}

const int& CProDistrubution::getID() const
{
	return ID;
}

void CProDistrubution::setID( int nID )
{
	ID = nID;
}

ProbTypes CProDistrubution::getProType() const
{
	return proType;
}

void CProDistrubution::setProType( ProbTypes val )
{
	proType = val;
}

CString CProDistrubution::getPrintDist() const
{
	char buffer[1024] = {0};
	if (pProbailityDistrution)
	{
		pProbailityDistrution->screenPrint(buffer);
	}
	return CString(buffer);
}

void CProDistrubution::setPrintDist( CString val )
{
	printDist = val;
}

ProbabilityDistribution* CProDistrubution::GetProbDistribution(void) const
{
	return pProbailityDistrution;
}

void CProDistrubution::initProbilityDistribution()
{
	if(pProbailityDistrution)
	{
		delete pProbailityDistrution;
		pProbailityDistrution = NULL;
	}

	switch(proType) 
	{
	case BERNOULLI:
		pProbailityDistrution = new BernoulliDistribution;
		break;
	case BETA:
		pProbailityDistrution = new BetaDistribution;
		break;
	case CONSTANT:
		pProbailityDistrution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		pProbailityDistrution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		pProbailityDistrution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		pProbailityDistrution = new HistogramDistribution;
		break;
	case NORMAL:
		pProbailityDistrution = new NormalDistribution;
		break;
	case UNIFORM:
		pProbailityDistrution = new UniformDistribution;
		break;
	case WEIBULL:
		pProbailityDistrution = new WeibullDistribution;
		break;
	case GAMMA:
		pProbailityDistrution = new GammaDistribution;
		break;
	case ERLANG:
		pProbailityDistrution = new ErlangDistribution;
		break;
	case TRIANGLE:
		pProbailityDistrution = new TriangleDistribution;
		break;
	default:
		break;
	}
	if(NULL == pProbailityDistrution)
	{
		return;
	}

	char szprintDist[1024] = {0};
	char *endOfName = (char*) strstr (printDist, ":");

	strcpy(szprintDist,endOfName + 1);

	pProbailityDistrution->setDistribution(szprintDist);
}

ProbabilityDistribution* CProDistrubution::copyProbalityDistribution()
{
	ProbabilityDistribution* pNewProbailityDistrution = NULL;

	char szprintDist[1024] = {0};
	char *endOfName = (char*) strstr (printDist, ":");

	strcpy(szprintDist,endOfName + 1);
	switch(proType) 
	{
	case BERNOULLI:
		{
			pNewProbailityDistrution = new BernoulliDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case BETA:
		{
			pNewProbailityDistrution = new BetaDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case CONSTANT:
		{
			pNewProbailityDistrution = new ConstantDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case EMPIRICAL:
		{
			pNewProbailityDistrution = new EmpiricalDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case EXPONENTIAL:
		{
			pNewProbailityDistrution = new ExponentialDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case HISTOGRAM:
		{
			pNewProbailityDistrution = new HistogramDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case NORMAL:
		{
			pNewProbailityDistrution = new NormalDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case UNIFORM:
		{
			pNewProbailityDistrution = new UniformDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case WEIBULL:
		{
			pNewProbailityDistrution = new WeibullDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case GAMMA:
		{
			pNewProbailityDistrution = new GammaDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case ERLANG:
		{
			pNewProbailityDistrution = new ErlangDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	case TRIANGLE:
		{
			pNewProbailityDistrution = new TriangleDistribution();
			pNewProbailityDistrution->setDistribution(szprintDist);
		}
		break;
	default:
		break;
	}
	return pNewProbailityDistrution; 
}

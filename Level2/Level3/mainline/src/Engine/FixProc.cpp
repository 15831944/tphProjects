// FixProc.cpp: implementation of the FixProc class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FixProc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const CString FixProc::OperationTypeString[TypeCount]= {
	"Cruise", "Descend", "Approach", "ClimbOut",
};

FixProc::FixProc()
{
	m_nNumber=-1;
	m_dLowerLimit =0.0;
	m_dUpperLimit =0.0;
	m_pLatitude=new CLatitude();
	m_pLongitude=new CLongitude();
	m_operateType = Cruise;

}

FixProc::~FixProc()
{
	if (m_pLatitude)
		delete m_pLatitude;
	if(m_pLongitude)
		delete m_pLongitude;
}
int FixProc::readSpecialField(ArctermFile& procFile)
{
	//number
	procFile.getInteger( m_nNumber );
	//lower limit
	procFile.getFloat( m_dLowerLimit );
	//upper limit
	procFile.getFloat( m_dUpperLimit );
	m_pLongitude->ReadData(procFile);
	m_pLatitude->ReadData(procFile);
	int nOperationType = 0;
	procFile.getInteger(nOperationType);
	m_operateType = (OperationType)nOperationType;
	return TRUE;
}
int FixProc::writeSpecialField(ArctermFile& procFile) const
{
	//number
	procFile.writeInt( m_nNumber );
	//lower limit
	procFile.writeFloat( (float)m_dLowerLimit );
	//upper limit
	procFile.writeFloat( (float)m_dUpperLimit );
	m_pLongitude->WriteData(procFile);
	m_pLatitude->WriteData(procFile);
	procFile.writeInt(static_cast<int>(m_operateType));
	return TRUE;
}

Processor* FixProc::CreateNewProc()
{
	Processor* pProc = new FixProc;
	return pProc;
}

bool FixProc::CopyOtherData(Processor* _pDestProc)
{
	FixProc *pProc = (FixProc*)_pDestProc;

	pProc->SetLowerLimit(GetLowerLimit());
	pProc->SetUpperLimit(GetUpperLimit());
	pProc->SetNumber(GetNumber());
	pProc->setOperationType(getOperationType());

	return true;
}
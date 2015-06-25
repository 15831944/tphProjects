// ReportToCompare.cpp: implementation of the CReportToCompare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReportToCompare.h"
#include "ModelToCompare.h"
#include <Inputs/in_term.h>
#include "InputParameter.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportToCompare::CReportToCompare()
{
	//m_strName = _T("");
	m_strName.Empty();
	m_nReportCategory = 0;
	m_isChecked = TRUE;
	m_nType = 0;
}

CReportToCompare::~CReportToCompare()
{

}

BOOL CReportToCompare::LoadData( const CString& strPath )
{
	if(strPath == "")
		return FALSE;

	return TRUE;
}
void CReportToCompare::DeleteModelParameter(const CString& strModelUniqueName)
{
	m_reportParam.DeleteModelParameter(strModelUniqueName);
}
//////////////////////////////////////////////////////////////////////////
//	CReportToCompareDataSet
CReportToCompareDataSet::CReportToCompareDataSet() : DataSet(ReportInfoFile)
{
	 m_pModelsManager = NULL;
}

CReportToCompareDataSet::~CReportToCompareDataSet()
{
}

void CReportToCompareDataSet::clear()
{
}

void CReportToCompareDataSet::readData(ArctermFile& p_file)
{
	char buf[256];
	int nCategory;
	ElapsedTime et;
	
//	p_file.skipLine();
	
	m_vReports.clear();

	p_file.getLine();
	int nReportCount = 0;
	p_file.getInteger(nReportCount);

	
	while (nReportCount - 1 >=0)
	{
		p_file.getLine();
		nReportCount -= 1;
		CReportToCompare report;
		
		p_file.getField(buf, 256);
		report.SetName(buf);


		p_file.getInteger(nCategory);
		report.SetCategory(nCategory);

		char cFlag = 'F';
		p_file.getChar(cFlag);
		if(cFlag == 'T')
		{
			report.SetChecked(TRUE);
		}
		else
		{
			report.SetChecked(FALSE);
		}

		p_file.getChar(cFlag);
		if(cFlag == 'D') // Detail
		{
			report.SetReportType(0);
		}
		else if(cFlag == 'S') // Summary
		{
			report.SetReportType(1);
		}
		else
		{
			report.SetReportType(0);
		}

		CReportParamToCompare param;
		p_file.getTime(et, TRUE);
		param.SetStartTime(et);

		p_file.getTime(et, TRUE);
		param.SetEndTime(et);

		p_file.getTime(et, TRUE);
		param.SetInterval(et);
		
		long lInterval = 0;
		p_file.getInteger(lInterval);
		param.SetInterval(lInterval);
        
		p_file.getLine();
		//Get parameter of the project
		
		std::vector<CModelParameter> vModelparam;	
		int nModelCount;
		p_file.getInteger(nModelCount);
		p_file.getLine();

		for (int nModel = 0; nModel < nModelCount; ++ nModel)
		{
			CModelParameter modelParam;
			
			p_file.getField(buf, 256);
			modelParam.SetModelUniqueName(buf);
			//get model
			CModelToCompare *pModel = GetModelByUniqueName(modelParam.GetModelUniqueName());

			p_file.getField(buf, 256);
			modelParam.SetArea(buf);
			p_file.getLine();

			//pax type
			int iCount = 0;
			p_file.getInteger(iCount);

			std::vector<CMobileElemConstraint> vPaxType;
			vPaxType.clear();
			for (int i = 0; i < iCount; i++)
			{
				if (pModel == NULL)
				{
					p_file.skipField(1);
				}
				else
				{
					CMobileElemConstraint pax(pModel->GetInputTerminal());
					//pax.SetInputTerminal(pModel->GetInputTerminal());
					pax.readConstraint(p_file);
					vPaxType.push_back(pax);
				}
			}
			modelParam.SetPaxType(vPaxType);
			
			p_file.getLine();

			//	Processor type
			std::vector<ProcessorID> vProcGroup;
			vProcGroup.clear();
			p_file.getInteger(iCount);
			for (int ii = 0; ii < iCount; ii++)
			{		
				if (pModel == NULL)
				{
					p_file.skipField(1);
				}
				else
				{
					ProcessorID proc;
					proc.SetStrDict(pModel->GetInputTerminal()->inStrDict);
					proc.readProcessorID(p_file);
					vProcGroup.push_back(proc);
				}

			}
			modelParam.SetProcessorID(vProcGroup);
		
			p_file.getLine();

			CReportParameter::FROM_TO_PROCS _fromtoprocs ;
			vProcGroup.clear();
			p_file.getInteger(iCount);
			for (int i = 0; i < iCount; i++)
			{
				if (pModel == NULL)
				{
					p_file.skipField(1);
				}
				else
				{
					ProcessorID proc;
					proc.SetStrDict(pModel->GetInputTerminal()->inStrDict);
					proc.readProcessorID(p_file);
					vProcGroup.push_back(proc);
				}
			}
			_fromtoprocs.m_vFromProcs = vProcGroup;
		
			p_file.getLine();
			vProcGroup.clear();
			p_file.getInteger(iCount);
			for (int i = 0; i < iCount; i++)
			{
				if (pModel == NULL)
				{
					p_file.skipField(1);
				}
				else
				{
					ProcessorID proc;
					proc.SetStrDict(pModel->GetInputTerminal()->inStrDict);
					proc.readProcessorID(p_file);
					vProcGroup.push_back(proc);
				}
			}
			_fromtoprocs.m_vToProcs = vProcGroup;

			modelParam.SetFromToProcs(_fromtoprocs);
			vModelparam.push_back(modelParam);

			p_file.getLine();
		}

		param.SetModelParameter(vModelparam);

		report.SetParameter(param);
		
		m_vReports.push_back(report);
	}
}

void CReportToCompareDataSet::readObsoleteData(ArctermFile& p_file)
{
}

void CReportToCompareDataSet::writeData(ArctermFile& p_file) const
{
	int nReportSize = static_cast<int>(m_vReports.size());
	p_file.writeInt(nReportSize);
	for (int i = 0; i < nReportSize; i++)
	{
		p_file.writeLine();
		p_file.writeField(m_vReports[i].GetName());
		p_file.writeInt(m_vReports[i].GetCategory());
		if(m_vReports[i].GetChecked() == TRUE)
		{
			p_file.writeChar('T');
		}
		else
		{
			p_file.writeChar('F');
		}

		if(m_vReports[i].GetReportType() == 0)// Detail
		{
			p_file.writeChar('D');
		}
		else if(m_vReports[i].GetReportType() == 1)// Summary
		{
			p_file.writeChar('S');
		}
		else
		{
			p_file.writeChar('D');
		}

		CReportParamToCompare param = m_vReports[i].GetParameter();

		p_file.writeTime(param.GetStartTime(), TRUE);
		p_file.writeTime(param.GetEndTime(), TRUE);
		p_file.writeTime(param.GetInterval(), TRUE);
		long lInterval  = 0;
		param.GetInterval(lInterval);
		p_file.writeInt(lInterval);		
		p_file.writeLine();

		//Model Parameter
		std::vector<CModelParameter> vModelParam;
		param.GetModelParameter(vModelParam);
		int nModelCount = static_cast<int>(vModelParam.size());
		p_file.writeInt(nModelCount);
		p_file.writeField(_T("Model Count"));
		p_file.writeLine();

		for (int nModel = 0; nModel < nModelCount; ++nModel)
		{		

			CModelParameter& modelParam = vModelParam[nModel];
			
			p_file.writeField(modelParam.GetModelUniqueName());

			p_file.writeField(modelParam.GetArea());
			p_file.writeLine();
			// write pax
			std::vector<CMobileElemConstraint> vPaxType;
			int iCount = modelParam.GetPaxType(vPaxType);
			p_file.writeInt(iCount);


			for (int m = 0; m < iCount; m++)
			{
				vPaxType[m].writeConstraint( p_file );
			}
			p_file.writeLine();
			std::vector<ProcessorID> vProcGroup;
			iCount = modelParam.GetProcessorID(vProcGroup);
			p_file.writeInt(iCount);
			for (int k = 0; k < iCount; k++)
			{
				vProcGroup[k].writeProcessorID(p_file);
			}
			p_file.writeLine();
			CReportParameter::FROM_TO_PROCS _fromtoprocs ;
			modelParam.GetFromToProcs(_fromtoprocs);

			int nCount = static_cast<int>(_fromtoprocs.m_vFromProcs.size());
			p_file.writeInt(nCount);
			for (int nFrom =0; nFrom < nCount; ++nFrom)
			{
				_fromtoprocs.m_vFromProcs.at(nFrom).writeProcessorID(p_file);
			}

			p_file.writeLine();
			nCount = static_cast<int>(_fromtoprocs.m_vToProcs.size());
			p_file.writeInt(nCount);
			for (int nTo =0; nTo < nCount; ++nTo)
			{
				_fromtoprocs.m_vToProcs.at(nTo).writeProcessorID(p_file);
			}
			p_file.writeLine();
		}
	}

	if (m_vReports.empty())
		p_file.writeLine();
}

int CReportToCompareDataSet::GetReports(OUT std::vector<CReportToCompare>& vReports)
{
	vReports = m_vReports;
	return m_vReports.size();
}

void CReportToCompareDataSet::SetReports(const std::vector<CReportToCompare>& vReports)
{
	m_vReports = vReports;
}
void CReportToCompareDataSet::SetInputParam( CModelsManager *pModelsManager)
{
	m_pModelsManager = pModelsManager;
}

CModelToCompare* CReportToCompareDataSet::GetModelByUniqueName(const CString& strUniqueName)
{
	if(m_pModelsManager)
		return m_pModelsManager->GetModelByUniqueName(strUniqueName);

	ASSERT(0);
	return NULL;
}
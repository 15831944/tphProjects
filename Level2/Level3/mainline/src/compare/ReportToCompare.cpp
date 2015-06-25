// ReportToCompare.cpp: implementation of the CReportToCompare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReportToCompare.h"
#include "ModelToCompare.h"
#include <Inputs/in_term.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportToCompare::CReportToCompare()
{
	//m_strName = _T("");
	m_strName.Empty();
	m_nReportCategory = 0;
	m_isChecked = TRUE;
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

		BOOL isChecked = FALSE;
		char cFlag = 'F';
		p_file.getChar(cFlag);
		if(cFlag == 'T')
		{
			isChecked = TRUE;
		}
		report.SetChecked(isChecked);

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

			for (int i = 0; i < iCount; i++)
			{
				if (pModel == NULL)
				{
					p_file.skipField(1);
				}
				else
				{
					CMobileElemConstraint paxType(pModel->GetInputTerminal());
					paxType.readConstraint(p_file);
					isChecked = FALSE;
					cFlag = 'F';
					p_file.getChar(cFlag);
					if(cFlag == 'T')
					{
						isChecked = TRUE;
					}
					modelParam.AddPaxType(paxType, isChecked);
				}
			}
			
			p_file.getLine();

			//	Processor type
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
					isChecked = FALSE;
					cFlag = 'F';
					p_file.getChar(cFlag);
					if(cFlag == 'T')
					{
						isChecked = TRUE;
					}
					modelParam.AddProcID(proc, isChecked);
				}
			}
		
			p_file.getLine();

			std::vector<ProcessorID> vProcGroup;
			vProcGroup.clear();
			CReportParameter::FROM_TO_PROCS _fromtoprocs ;
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



		////	pax type
		//int iCount = 0;
		//p_file.getInteger(iCount);

		//std::vector<CMobileElemConstraint> vPaxType;
		//vPaxType.clear();
		//for (int i = 0; i < iCount; i++)
		//{
		//	CMobileElemConstraint pax;
		//	pax.SetInputTerminal(m_pInTerm);
		//	pax.readConstraint(p_file);
		//	vPaxType.push_back(pax);
		//}
		//param.SetPaxType(vPaxType);


		////	Processor type
		//std::vector<ProcessorID> vProcGroup;
		//vProcGroup.clear();
		//p_file.getInteger(iCount);
		//for (i = 0; i < iCount; i++)
		//{
		//	ProcessorID proc;
		//	proc.SetStrDict(m_pInTerm->inStrDict);
		//	proc.readProcessorID(p_file);
		//	vProcGroup.push_back(proc);
		//}
		//param.SetProcessorID(vProcGroup);

		//CReportParameter::FROM_TO_PROCS _fromtoprocs ;
		//vProcGroup.clear();
		//p_file.getInteger(iCount);
		//for (i = 0; i < iCount; i++)
		//{
		//	ProcessorID proc;
		//	proc.SetStrDict(m_pInTerm->inStrDict);
		//	proc.readProcessorID(p_file);
		//	vProcGroup.push_back(proc);
		//}
		//_fromtoprocs.m_vFromProcs = vProcGroup;

		//vProcGroup.clear();
		//p_file.getInteger(iCount);
		//for (i = 0; i < iCount; i++)
		//{
		//	ProcessorID proc;
		//	proc.SetStrDict(m_pInTerm->inStrDict);
		//	proc.readProcessorID(p_file);
		//	vProcGroup.push_back(proc);
		//}
		//_fromtoprocs.m_vToProcs = vProcGroup;

		//param.SetFromToProcs(_fromtoprocs);

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
		
		const CReportParamToCompare& param = m_vReports[i].GetParameterConst();
		p_file.writeTime(param.GetStartTime(), TRUE);
		p_file.writeTime(param.GetEndTime(), TRUE);
		p_file.writeTime(param.GetInterval(), TRUE);
		long lInterval  = 0;
		param.GetInterval(lInterval);
		p_file.writeInt(lInterval);		
		p_file.writeLine();

		//Model Parameter
		const std::vector<CModelParameter>& vModelParam = param.GetModelParameterConst();
		int nModelCount = static_cast<int>(vModelParam.size());
		p_file.writeInt(nModelCount);
		p_file.writeField(_T("Model Count"));
		p_file.writeLine();

		for (int nModel=0; nModel<nModelCount; nModel++)
		{
			CModelParameter& modelParam = const_cast<CModelParameter&>(vModelParam[nModel]);
			p_file.writeField(modelParam.GetModelUniqueName());

			p_file.writeField(modelParam.GetArea());
			p_file.writeLine();

			// write passenger types.
			int iCount = modelParam.GetPaxTypeCount();
			p_file.writeInt(iCount);
			std::vector<MobConstWithCheckedFlag>& vPaxType = modelParam.GetPaxTypeWithCheckedFlagList();
			char bFlag = 'F';
			for (int m = 0; m < iCount; m++)
			{
				vPaxType[m].GetPaxType().writeConstraint(p_file);
				if(vPaxType[m].GetChecked() == TRUE)
				{
					bFlag = 'T';
				}
				else
				{
					bFlag = 'F';
				}
				p_file.writeChar(bFlag);
			}
			p_file.writeLine();

			iCount = modelParam.GetProcIDCount();
			p_file.writeInt(iCount);
			std::vector<ProcessIDWithCheckedFlag>& vProcID = modelParam.GetProcIDWithCheckedFlagGroup();
			for (int k = 0; k<iCount; k++)
			{
				vProcID[k].GetProcID().writeProcessorID(p_file);
				if(vProcID[k].GetChecked() == TRUE)
				{
					bFlag = 'T';
				}
				else
				{
					bFlag = 'F';
				}
				p_file.writeChar(bFlag);
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

		//p_file.writeField(param.GetArea());

		//// write pax
		//std::vector<CMobileElemConstraint> vPaxType;
		//int iCount = param.GetPaxType(vPaxType);
		//p_file.writeInt(iCount);
		//for (int m = 0; m < iCount; m++)
		//{
		//	vPaxType[m].writeConstraint( p_file );
		//}

		//std::vector<ProcessorID> vProcGroup;
		//iCount = param.GetProcessorID(vProcGroup);
		//p_file.writeInt(iCount);
		//for (int k = 0; k < iCount; k++)
		//{
		//	vProcGroup[k].writeProcessorID(p_file);
		//}

		//CReportParameter::FROM_TO_PROCS _fromtoprocs ;
		//param.GetFromToProcs(_fromtoprocs);

		//int nCount = static_cast<int>(_fromtoprocs.m_vFromProcs.size());
		//p_file.writeInt(nCount);
		//for (int nFrom =0; nFrom < nCount; ++nFrom)
		//{
		//	_fromtoprocs.m_vFromProcs.at(nFrom).writeProcessorID(p_file);
		//}
		//nCount = static_cast<int>(_fromtoprocs.m_vToProcs.size());
		//p_file.writeInt(nCount);
		//for (int nTo =0; nTo < nCount; ++nTo)
		//{
		//	_fromtoprocs.m_vToProcs.at(nTo).writeProcessorID(p_file);
		//}
		//p_file.writeLine();
	}

	if (m_vReports.empty())
		p_file.writeLine();
}

void CReportToCompareDataSet::GetReports(OUT std::vector<CReportToCompare>& vReports)
{
	vReports = m_vReports;
}

void CReportToCompareDataSet::SetReports(const std::vector<CReportToCompare>& vReports)
{
	m_vReports = vReports;
}
void CReportToCompareDataSet::SetModels(const std::vector<CModelToCompare *>& vModel )
{
	m_vModel = vModel;
}

CModelToCompare* CReportToCompareDataSet::GetModelByUniqueName(const CString& strUniqueName)
{
	CModelToCompare *pModel = NULL;
	std::vector<CModelToCompare *>::size_type i = 0;
	for (;i< m_vModel.size(); ++i)
	{
		if (strUniqueName.CompareNoCase(m_vModel.at(i)->GetUniqueName()) == 0)
		{	
			pModel = m_vModel.at(i);
//			m_vModel.erase(m_vModel.begin()+ i);
			break;
		}
	}
	return pModel;
}
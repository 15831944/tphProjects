// GatePriorityInfo.cpp: implementation of the GatePriorityInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GatePriorityInfo.h"
//#include "termplan.h"
//#include "termplandoc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GatePriorityInfo::GatePriorityInfo() : DataSet(GatePriorityInfoFile)
{

}

GatePriorityInfo::~GatePriorityInfo()
{

}

//extern CTermPlanApp theApp;
void GatePriorityInfo::readData (ArctermFile& p_file)
{
	p_file.reset();
	p_file.skipLines (3);

	/*
	POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
	CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
    InputTerminal* inputTerminal = (InputTerminal*)&pDoc->GetTerminal();*/


    while (p_file.getLine())
	{
		/*ProcessorID *newProcID = new ProcessorID;
		newProcID->SetStrDict(DataSet::m_pInTerm->inStrDict);
		newProcID->readProcessorID(p_file);
		procIDList.addItem(newProcID);*/
		ALTObjectID newObjID;
		newObjID.readALTObjectID(p_file);
		altObjectIDList.push_back(newObjID);
	}
	
}

void GatePriorityInfo::writeData (ArctermFile& p_file) const
{
	/*int nCount = procIDList.getCount();
	for(int i = 0; i < nCount; i++)
	{
		procIDList.getItem(i)->writeProcessorID(p_file);
		p_file.writeLine();
	}*/
	for(size_t i=0; i<altObjectIDList.size();i++){
		altObjectIDList.at(i).writeALTObjectID(p_file);
		p_file.writeLine();
	}
}

void GatePriorityInfo::clear (void)
{
	//procIDList.clear();
	altObjectIDList.clear();
}




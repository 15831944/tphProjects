#include "StdAfx.h"
#include "ApproachSeparation.h"
#include "AirsideImportExportManager.h"

ApproachSeparation::ApproachSeparation(int nProjID)
:m_nProjID(nProjID)
{
	m_pLandingBehindLandingSameRunway = 
		new LandingBehindLandingSameRunway(nProjID);
	m_pLandingBehindLandingIntersectionRuway_LAHSO = 
		new LandingBehindLandingIntersectionRuwayLAHSO(nProjID);
	m_pLandingBehindLandingIntersectionRuway_NON_LAHSO = 
		new LandingBehindLandingIntersectionRuwayNONLAHSO(nProjID);

	m_pLandingBehindTakeoffSameRunway = 
		new LandingBehindTakeoffSameRunway(nProjID);
	m_pLandingBehindTakeoffIntersectionRunway =
		new LandingBehindTakeoffIntersectionRunway(nProjID);
}

ApproachSeparation::ApproachSeparation()
{
	m_nProjID = -1;
	m_pLandingBehindLandingSameRunway = 
		new LandingBehindLandingSameRunway(m_nProjID);
	m_pLandingBehindLandingIntersectionRuway_LAHSO = 
		new LandingBehindLandingIntersectionRuwayLAHSO(m_nProjID);
	m_pLandingBehindLandingIntersectionRuway_NON_LAHSO = 
		new LandingBehindLandingIntersectionRuwayNONLAHSO(m_nProjID);

	m_pLandingBehindTakeoffSameRunway = 
		new LandingBehindTakeoffSameRunway(m_nProjID);
	m_pLandingBehindTakeoffIntersectionRunway =
		new LandingBehindTakeoffIntersectionRunway(m_nProjID);
}

ApproachSeparation::~ApproachSeparation(void)
{
	if (m_pLandingBehindLandingSameRunway)
		delete m_pLandingBehindLandingSameRunway;
	if (m_pLandingBehindLandingIntersectionRuway_LAHSO)
		delete m_pLandingBehindLandingIntersectionRuway_LAHSO;
	if (m_pLandingBehindLandingIntersectionRuway_NON_LAHSO)
		delete m_pLandingBehindLandingIntersectionRuway_NON_LAHSO;
	if (m_pLandingBehindTakeoffSameRunway)
		delete m_pLandingBehindTakeoffSameRunway;
	if (m_pLandingBehindTakeoffIntersectionRunway)
		delete m_pLandingBehindTakeoffIntersectionRunway;
}


void ApproachSeparation::SetProjectID(int nProjID)
{
	if (m_pLandingBehindLandingSameRunway)
		delete m_pLandingBehindLandingSameRunway;
	if (m_pLandingBehindLandingIntersectionRuway_LAHSO)
		delete m_pLandingBehindLandingIntersectionRuway_LAHSO;
	if (m_pLandingBehindLandingIntersectionRuway_NON_LAHSO)
		delete m_pLandingBehindLandingIntersectionRuway_NON_LAHSO;
	if (m_pLandingBehindTakeoffSameRunway)
		delete m_pLandingBehindTakeoffSameRunway;
	if (m_pLandingBehindTakeoffIntersectionRunway)
		delete m_pLandingBehindTakeoffIntersectionRunway;

	m_nProjID = nProjID;

	m_pLandingBehindLandingSameRunway = 
		new LandingBehindLandingSameRunway(nProjID);
	m_pLandingBehindLandingIntersectionRuway_LAHSO = 
		new LandingBehindLandingIntersectionRuwayLAHSO(nProjID);
	m_pLandingBehindLandingIntersectionRuway_NON_LAHSO = 
		new LandingBehindLandingIntersectionRuwayNONLAHSO(nProjID);

	m_pLandingBehindTakeoffSameRunway = 
		new LandingBehindTakeoffSameRunway(nProjID);
	m_pLandingBehindTakeoffIntersectionRunway =
		new LandingBehindTakeoffIntersectionRunway(nProjID);
}

void ApproachSeparation::ReadData()
{
	m_pLandingBehindLandingSameRunway->ReadData();
	m_pLandingBehindLandingIntersectionRuway_LAHSO->ReadData();
	m_pLandingBehindLandingIntersectionRuway_NON_LAHSO->ReadData();
	m_pLandingBehindTakeoffSameRunway->ReadData();
	m_pLandingBehindTakeoffIntersectionRunway->ReadData();
}

void ApproachSeparation::SaveData()
{
	((LandingBehindLandingSameRunway*)m_pLandingBehindLandingSameRunway)->SaveData();
	((LandingBehindLandingIntersectionRuwayLAHSO*)m_pLandingBehindLandingIntersectionRuway_LAHSO)->SaveData();
	((LandingBehindLandingIntersectionRuwayNONLAHSO*)m_pLandingBehindLandingIntersectionRuway_NON_LAHSO)->SaveData();
	((LandingBehindTakeoffSameRunway*)m_pLandingBehindTakeoffSameRunway)->SaveData();
	((LandingBehindTakeoffIntersectionRunway*)m_pLandingBehindTakeoffIntersectionRunway)->SaveData();
}

void ApproachSeparation::ConvertToUseUnit(CUnitsManager * pGlobalUnits)
{
	ACBasisItemVector * pItemVector = 0;
	ACBasisItemIter itrItem;
	pItemVector = ((LandingBehindLandingSameRunway*)m_pLandingBehindLandingSameRunway)->GetItemsVector();
	if(pItemVector)
	{
		for (itrItem = pItemVector->begin();itrItem != pItemVector->end();++ itrItem) {
			if(*itrItem){
				(*itrItem)->m_dMinSeparation = pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,pGlobalUnits->GetLengthUnits(),(*itrItem)->m_dMinSeparation);
				(*itrItem)->m_dTimeDiffInFltPathIntersecting /= 60;
				(*itrItem)->m_dTimeDiffOnRunwayIntersecting /= 60;
				(*itrItem)->m_dDistanceDiffInFltPathIntersecting = pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,pGlobalUnits->GetLengthUnits(),(*itrItem)->m_dDistanceDiffInFltPathIntersecting);
				(*itrItem)->m_dDistanceDiffOnRunwayIntersecting = pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,pGlobalUnits->GetLengthUnits(),(*itrItem)->m_dDistanceDiffOnRunwayIntersecting);
			}
		}
	}
	
	pItemVector = ((LandingBehindLandingIntersectionRuwayNONLAHSO*)m_pLandingBehindLandingIntersectionRuway_NON_LAHSO)->GetItemsVector();
	if(pItemVector)
	{
		for (itrItem = pItemVector->begin();itrItem != pItemVector->end();++ itrItem) {
			if(*itrItem){
				(*itrItem)->m_dMinSeparation = pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,pGlobalUnits->GetLengthUnits(),(*itrItem)->m_dMinSeparation);
				(*itrItem)->m_dTimeDiffInFltPathIntersecting /= 60;
				(*itrItem)->m_dTimeDiffOnRunwayIntersecting /= 60;
				(*itrItem)->m_dDistanceDiffInFltPathIntersecting = pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,pGlobalUnits->GetLengthUnits(),(*itrItem)->m_dDistanceDiffInFltPathIntersecting);
				(*itrItem)->m_dDistanceDiffOnRunwayIntersecting = pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,pGlobalUnits->GetLengthUnits(),(*itrItem)->m_dDistanceDiffOnRunwayIntersecting);
			}
		}
	}

	((LandingBehindTakeoffSameRunway*)m_pLandingBehindTakeoffSameRunway)->SetLeadDisFromThreshold(pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,pGlobalUnits->GetLengthUnits(),((LandingBehindTakeoffSameRunway*)m_pLandingBehindTakeoffSameRunway)->GetLeadDisFromThreshold()));
	
	pItemVector = ((LandingBehindTakeoffIntersectionRunway*)m_pLandingBehindTakeoffIntersectionRunway)->GetItemsVector();
	if(pItemVector)
	{
		for (itrItem = pItemVector->begin();itrItem != pItemVector->end();++ itrItem) {
			if(*itrItem){
				(*itrItem)->m_dMinSeparation = pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,pGlobalUnits->GetLengthUnits(),(*itrItem)->m_dMinSeparation);
				(*itrItem)->m_dTimeDiffInFltPathIntersecting /= 60;
				(*itrItem)->m_dTimeDiffOnRunwayIntersecting /= 60;
				(*itrItem)->m_dDistanceDiffInFltPathIntersecting = pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,pGlobalUnits->GetLengthUnits(),(*itrItem)->m_dDistanceDiffInFltPathIntersecting);
				(*itrItem)->m_dDistanceDiffOnRunwayIntersecting = pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,pGlobalUnits->GetLengthUnits(),(*itrItem)->m_dDistanceDiffOnRunwayIntersecting);
			}
		}
	}
}

void ApproachSeparation::ConvertToDBUnit(CUnitsManager * pGlobalUnits)
{
	ACBasisItemVector * pItemVector = 0;
	ACBasisItemIter itrItem;
	pItemVector = ((LandingBehindLandingSameRunway*)m_pLandingBehindLandingSameRunway)->GetItemsVector();
	if(pItemVector)
	{
		for (itrItem = pItemVector->begin();itrItem != pItemVector->end();++ itrItem) {
			if(*itrItem){
				(*itrItem)->m_dMinSeparation = pGlobalUnits->ConvertLength(pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,(*itrItem)->m_dMinSeparation);
				(*itrItem)->m_dTimeDiffInFltPathIntersecting *= 60;
				(*itrItem)->m_dTimeDiffOnRunwayIntersecting *= 60;
				(*itrItem)->m_dDistanceDiffInFltPathIntersecting = pGlobalUnits->ConvertLength(pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,(*itrItem)->m_dDistanceDiffInFltPathIntersecting);
				(*itrItem)->m_dDistanceDiffOnRunwayIntersecting = pGlobalUnits->ConvertLength(pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,(*itrItem)->m_dDistanceDiffOnRunwayIntersecting);
			}
		}
	}

	pItemVector = ((LandingBehindLandingIntersectionRuwayNONLAHSO*)m_pLandingBehindLandingIntersectionRuway_NON_LAHSO)->GetItemsVector();
	if(pItemVector)
	{
		for (itrItem = pItemVector->begin();itrItem != pItemVector->end();++ itrItem) {
			if(*itrItem){
				(*itrItem)->m_dMinSeparation = pGlobalUnits->ConvertLength(pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,(*itrItem)->m_dMinSeparation);
				(*itrItem)->m_dTimeDiffInFltPathIntersecting *= 60;
				(*itrItem)->m_dTimeDiffOnRunwayIntersecting *= 60;
				(*itrItem)->m_dDistanceDiffInFltPathIntersecting = pGlobalUnits->ConvertLength(pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,(*itrItem)->m_dDistanceDiffInFltPathIntersecting);
				(*itrItem)->m_dDistanceDiffOnRunwayIntersecting = pGlobalUnits->ConvertLength(pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,(*itrItem)->m_dDistanceDiffOnRunwayIntersecting);
			}
		}
	}

	((LandingBehindTakeoffSameRunway*)m_pLandingBehindTakeoffSameRunway)->SetLeadDisFromThreshold(pGlobalUnits->ConvertLength(pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,((LandingBehindTakeoffSameRunway*)m_pLandingBehindTakeoffSameRunway)->GetLeadDisFromThreshold()));

	pItemVector = ((LandingBehindTakeoffIntersectionRunway*)m_pLandingBehindTakeoffIntersectionRunway)->GetItemsVector();
	if(pItemVector)
	{
		for (itrItem = pItemVector->begin();itrItem != pItemVector->end();++ itrItem) {
			if(*itrItem){
				(*itrItem)->m_dMinSeparation = pGlobalUnits->ConvertLength(pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,(*itrItem)->m_dMinSeparation);
				(*itrItem)->m_dTimeDiffInFltPathIntersecting *= 60;
				(*itrItem)->m_dTimeDiffOnRunwayIntersecting *= 60;
				(*itrItem)->m_dDistanceDiffInFltPathIntersecting = pGlobalUnits->ConvertLength(pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,(*itrItem)->m_dDistanceDiffInFltPathIntersecting);
				(*itrItem)->m_dDistanceDiffOnRunwayIntersecting = pGlobalUnits->ConvertLength(pGlobalUnits->GetLengthUnits(),UM_LEN_CENTIMETERS,(*itrItem)->m_dDistanceDiffOnRunwayIntersecting);
			}
		}
	}
}

ApproachSeparationItem* ApproachSeparation::GetApproachSeparationItem(ApproachSeparationType emType)
{
	switch(emType) {
	case LandingBehindLandingOnSameRunway:
        return m_pLandingBehindLandingSameRunway;
	case LandingBehindLandingOnIntersectingRunway_LAHSO:
		return m_pLandingBehindLandingIntersectionRuway_LAHSO;
	case LandingBehindLandingOnIntersectingRunway_NON_LAHSO:
		return m_pLandingBehindLandingIntersectionRuway_NON_LAHSO;
	case LandingBehindTakeoffOnSameRunway:
		return m_pLandingBehindTakeoffSameRunway;
	case LandingBehindTakeoffOnIntersectingRunway:
		return m_pLandingBehindTakeoffIntersectionRunway;
	default:
		ASSERT(FALSE);
	}
	return NULL;
}
/*
LandingBehindLandingOnSameRunway = 0,
LandingBehindLandingOnIntersectingRunway_LAHSO,
LandingBehindLandingOnIntersectingRunway_NON_LAHSO,
LandingBehindTakeoffOnSameRunway,
LandingBehindTakeoffOnIntersectingRunway,

ApproachSeparationType_Count,
*/
void ApproachSeparation::ImportData(CAirsideImportFile& importFile)
{
	/*TCHAR szField[] = {_T("ApproachSeparation")};
	importFile.getFile().getField(szField,sizeof(szField));*/
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	if (m_pLandingBehindLandingSameRunway)
	m_pLandingBehindLandingSameRunway->ImportData(importFile);

	if(m_pLandingBehindLandingIntersectionRuway_LAHSO)
	m_pLandingBehindLandingIntersectionRuway_LAHSO->ImportData(importFile);

	if(m_pLandingBehindLandingIntersectionRuway_NON_LAHSO)
	m_pLandingBehindLandingIntersectionRuway_NON_LAHSO->ImportData(importFile);

	if(m_pLandingBehindTakeoffSameRunway)
	m_pLandingBehindTakeoffSameRunway->ImportData(importFile);

	if (m_pLandingBehindTakeoffIntersectionRunway)
	m_pLandingBehindTakeoffIntersectionRunway->ImportData(importFile);

	importFile.getFile().getLine();

}

void ApproachSeparation::ImportApproachSeparation(CAirsideImportFile& importFile)
{	
	ApproachSeparation approachSep;
	approachSep.ImportData(importFile);
	approachSep.SaveData();
}
void ApproachSeparation::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("ApproachSeparation"));
	exportFile.getFile().writeLine();
	exportFile.getFile().writeInt(m_nProjID);

	if (m_pLandingBehindLandingSameRunway)
	m_pLandingBehindLandingSameRunway->ExportData(exportFile);

	if(m_pLandingBehindLandingIntersectionRuway_LAHSO)
	m_pLandingBehindLandingIntersectionRuway_LAHSO->ExportData(exportFile);

	if(m_pLandingBehindLandingIntersectionRuway_NON_LAHSO)
	m_pLandingBehindLandingIntersectionRuway_NON_LAHSO->ExportData(exportFile);

	if(m_pLandingBehindTakeoffSameRunway)
	m_pLandingBehindTakeoffSameRunway->ExportData(exportFile);

	if (m_pLandingBehindTakeoffIntersectionRunway)
	m_pLandingBehindTakeoffIntersectionRunway->ExportData(exportFile);

	exportFile.getFile().writeLine();

}
void ApproachSeparation::ExportApproachSeparation(CAirsideExportFile& exportFile)
{
	ApproachSeparation approachSep(exportFile.GetProjectID());
	approachSep.ReadData();
	approachSep.ExportData(exportFile);
	exportFile.getFile().endFile();

}

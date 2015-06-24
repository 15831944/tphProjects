#include "StdAfx.h"
#include "MeshEditFuncs.h"
#include "../Resource.h"
#include "./DlgSectionProp.h"
#include "inputonboard/ComponentEditContext.h"
#include "InputOnboard/ComponentModel.h"


AddSectionCommand* MeshEditFuncs::OnAddSection( CComponentEditContext* pCtx,double dAtPos )
{
	CComponentMeshModel* pEditModel = pCtx->GetCurrentComponent();

	CString strName;
	int nCount = pEditModel->GetSectionCount();
	strName.Format("Default-%d", nCount+1);
	if (nCount >0)
	{
		CComponentMeshSection* pPreviousSection = pEditModel->GetSection(nCount -1);
		if (pPreviousSection)
			pPreviousSection->GetNextSectionName(strName);
	}

	CDlgSectionProp dlgProp(NULL, strName, NULL);
	dlgProp.SetPos(dAtPos);
	if(dlgProp.DoModal() == IDOK)
	{		
		CComponentMeshSection* pNewSection = dlgProp.createSection( pEditModel );
		if(pNewSection)
		{
			int idx = pEditModel->GetSectionCount();
			AddSectionCommand* pCmd = new AddSectionCommand(pNewSection,idx);
			if( pCtx->AddCommand(pCmd) )
			{
				pCmd->Do();				
				return pCmd;
			}
			else{ delete pCmd; }
		}
	}
	return NULL;
}

EditSectionPropCommand* MeshEditFuncs::OnEditSection( CComponentEditContext* pCtx,CComponentMeshSection* pSection )
{	
	if(pSection)
	{
		CDlgSectionProp dlgProp(pSection, pSection->getName(), NULL);
		EditSectionPropCommand* pCmd = new EditSectionPropCommand(pSection);
		pCmd->BeginOp();
		if(dlgProp.DoModal() == IDOK  && pCtx->AddCommand(pCmd))
		{			
			return pCmd;
		}else{ delete pCmd; }
	}
	return NULL;
}

DelSectionCommand* MeshEditFuncs::OnDelSection( CComponentEditContext* pCtx,CComponentMeshSection* pSection )
{
	DelSectionCommand * pCmd = new DelSectionCommand(pSection);
	if(pCtx->AddCommand(pCmd))
	{
		pCmd->Do();
		return pCmd;
	}
	else{ delete pCmd; }
	return NULL;
}

AddSectionCommand* MeshEditFuncs::OnCopySection( CComponentEditContext* pCtx,CComponentMeshSection* pSection )
{
	CComponentMeshSection* pnewSection = pSection->GetCopy();
	CComponentModelData* pModel = pnewSection->getCreator();
	ALTObjectID objID; objID.FromString(pnewSection->getName());
	do
	{
		objID = objID.GetNext();	
	}while(pModel->GetSecByName(objID.GetIDString()));

	pnewSection->setName(objID.GetIDString());
	int idx = pnewSection->getCreator()->GetSectionCount();
	AddSectionCommand* pCmd = new AddSectionCommand(pnewSection,idx);
	pCmd->BeginOp();
	if(pCtx->AddCommand(pCmd))
	{
		pCmd->Do();
		return pCmd;
	}
	else{ delete pCmd; }
	return NULL;
}
#include "StdAfx.h"
#include ".\ienvmodeeditcontext.h"
#include <common/ViewMsg.h>

void IEnvModeEditContext::DoneCurEditOp()
{
	if(mpCurEditOp)
	{
		mpCurEditOp->EndOp();
		m_editHistory.Add(mpCurEditOp);
		mpCurEditOp = NULL;
	}
}




void IEnvModeEditContext::CancelCurEditOp()
{
	if(mpCurEditOp)
	{
		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)mpCurEditOp);
		delete mpCurEditOp;
		mpCurEditOp = NULL;
	}
}

void IEnvModeEditContext::clearRClickItemData()
{
	mRClickItemData  = QueryData();
}

IEnvModeEditContext::IEnvModeEditContext()
{
	mpCurEditOp = NULL;
	UseActiveFloorHeight();
}


void IEnvModeEditContext::StartEditOp( CEditOperationCommand* pOp, bool bCancelPre /*= false*/ )
{
	if(mpCurEditOp && mpCurEditOp != pOp)
	{
		if(bCancelPre)
			CancelCurEditOp();
		else
			DoneCurEditOp();
	}

	mpCurEditOp = pOp;
	mpCurEditOp->BeginOp();

}

void IEnvModeEditContext::NotifyViewCurentOperation()
{
	if(mpCurEditOp)
		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)mpCurEditOp);
}

QueryData& IEnvModeEditContext::setRClickItemData( const QueryData& itemdata )
{
	mRClickItemData = itemdata; return mRClickItemData;
}

double IEnvModeEditContext::GetWorkingHeight() const
{
	if(m_bUseTempHeight){
		return m_TempWorkingHeight;
	}
	return GetActiveFloorHeight();
}

void IEnvModeEditContext::UseTempWorkingHeight( double d )
{
	m_bUseTempHeight = true;
	m_TempWorkingHeight = d;
}

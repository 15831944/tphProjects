#include "StdAfx.h"
#include ".\render3dcontext.h"

CRender3DContext::CRender3DContext(void)
	:m_bViewLock(false)
	, m_pCurCommand(NULL)
{
}

CRender3DContext::~CRender3DContext(void)
{
}

void CRender3DContext::SetSelNodeObj( CModeBase3DObject::SharedPtr pObj3D )
{
	/*if (m_pSelObj3D)
		m_pSelObj3D->UpdateSelected(false);
	m_pSelObj3D = pObj3D;
	if (m_pSelObj3D)
		m_pSelObj3D->UpdateSelected(true);*/
}
void CRender3DContext::StartNewEditOp(Terminal3DCommand::OperationType opType)
{
//	if (!m_pSelObj3D)
//	{
//		ASSERT(FALSE);
//		return;
//	}
//	DoneCurEditOp();
//	switch (opType)
//	{
//	case Terminal3DCommand::OP_move:
//		{
//			m_pCurCommand = new Terminal3DMoveCommand(m_pSelObj3D);
//			m_pCurCommand->BeginOp();
//		}
//		break;
//	case Terminal3DCommand::OP_rotate:
//		{
//			// not suitable for this framework, implemented by other routine
//// 			m_pCurCommand = new Terminal3DRotateCommand(m_pSelObj3D);
//// 			m_pCurCommand->StartOp();
//		}
//		break;
//	default:
//		{
//			ASSERT(FALSE);
//		}
//		break;
//	}
}

void CRender3DContext::DoneCurEditOp()
{
	if (m_pCurCommand)
	{
		m_pCurCommand->EndOp();
		m_editHistory.Add(m_pCurCommand);
		m_pCurCommand = NULL;
	}
}

void CRender3DContext::CancelCurEditOp()
{
	if (m_pCurCommand)
	{
		m_pCurCommand->Undo();
		delete m_pCurCommand;
		m_pCurCommand = NULL;
	}
}

bool CRender3DContext::IsViewLocked() const
{
	return true/*m_bViewLock*/; // Benny Tag
}

void CRender3DContext::flipViewLock()
{
	m_bViewLock = !m_bViewLock;
}

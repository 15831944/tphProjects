#include "StdAfx.h"
#include ".\3dobjmodeleditcontext.h"
#include <common\FileInDB.h>
#include "scenenodeserializer.h"

#include "3DNodeObjectListener.h"

#include <boost/bind.hpp>

using namespace Ogre;
C3DObjModelEditContext::C3DObjModelEditContext() 
	: m_editHistory(100)
	, m_bModelEditDirty(false)
	, m_pCurCommand(NULL)	
{
	mpEditModel = NULL;
}

C3DObjModelEditContext::~C3DObjModelEditContext(void)
{
}





C3DNodeObject C3DObjModelEditContext::GetCurModel() const
{
	return m_model3D;
}

void C3DObjModelEditContext::SetCurModel( C3DNodeObject rootNodeObj )
{
	m_editHistory.Clear();
	m_model3D = rootNodeObj;
}

bool C3DObjModelEditContext::AddCommand( C3DObjectCommand* pCmd )
{
	ASSERT(pCmd);
	if(pCmd->ToBeRecorded())
	{
		pCmd->EndOp();
		m_editHistory.Add(pCmd);
		return true;
	}		
	return false;
}

void C3DObjModelEditContext::DoneCurEditOp()
{
	if (m_pCurCommand)
	{
		if(!AddCommand(m_pCurCommand))
		{
			delete m_pCurCommand;
		}	
		m_pCurCommand = NULL;
	}
}

void C3DObjModelEditContext::CancelCurEditOp()
{
	if (m_pCurCommand)
	{
		m_pCurCommand->Undo();
		delete m_pCurCommand;
		m_pCurCommand = NULL;
	}
}

void C3DObjModelEditContext::StartNewEditOp( C3DNodeObject nodeObj, E3DObjectEditOperation eOpType, C3DObjectCommand::EditCoordination eCoord )
{
	DoneCurEditOp();
	if (nodeObj)
	{
		switch (eOpType)
		{
		case OP_move3DObj:
			{
				m_pCurCommand = new C3DObjectMoveCommand(nodeObj, eCoord);
			}
			break;
		case OP_rotate3DObj:
			{
				m_pCurCommand = new C3DObjectRotateCommand(nodeObj, eCoord);
			}
			break;
		case OP_scale3DObj:
			{
				m_pCurCommand = new C3DObjectScaleCommand(nodeObj, eCoord);
			}
			break;
		case OP_mirror3DObj:
			{
				m_pCurCommand = new C3DObjectMirrorCommand(nodeObj);
			}
			break;
		case OP_add3DObj:
			{
				m_pCurCommand = new C3DObjectAddCommand(nodeObj, m_model3D);
			}
			break;
		case OP_del3DObj:
			{
				m_pCurCommand = new C3DObjectDelCommand(nodeObj, m_model3D);
			}
			break;
		default:
			{
				ASSERT(FALSE);
			}
			break;
		}
		if (m_pCurCommand)
		{
			m_pCurCommand->BeginOp();
		}
	}
}

void C3DObjModelEditContext::StartNewEditOp( E3DObjectEditOperation eOpType, C3DObjectCommand::EditCoordination eCoord /*= C3DObjectCommand::COORD_NONE*/ )
{
	StartNewEditOp(m_selNodeObj, eOpType, eCoord);
}

void C3DObjModelEditContext::SaveEditModel()
{/*
	CString strDir = m_pEditModel->msTmpWorkDir;
	m_model3D.SnapShot(strDir);		
	m_model3D.ExportToMesh(m_pEditModel->msMeshFileName,strDir);
	SceneNodeSerializer nodeserlize;
	nodeserlize.InitSave(strDir);
	nodeserlize.Do(m_pEditModel->msTmpWorkDir+STR_MODEL_NODE_FILE,m_model3D.GetSceneNode());*/
}

void C3DObjModelEditContext::ChangeEditModel( CModel* pEditModel )
{
	if(mpEditModel!=pEditModel)
	{
		Clear();
		mpEditModel= pEditModel;
	}
}

void C3DObjModelEditContext::Clear()
{
	mpEditModel = NULL;
	m_model3D = C3DNodeObject();
	m_selNodeObj.SetShowBound(false);
	m_selNodeObj = C3DNodeObject();
	m_editHistory.Clear();
	m_pCurCommand = NULL;
}

void C3DObjModelEditContext::SetSelNodeObj( C3DNodeObject selNodeObj )
{
	m_selNodeObj.SetShowBound(false);
	m_selNodeObj = selNodeObj;
	m_selNodeObj.SetShowBound(true);

	std::for_each(mSelNodeObjListenerList.begin(), mSelNodeObjListenerList.end(),
		boost::BOOST_BIND(&C3DNodeObjectListener::On3DNodeObjectFired, _1, m_selNodeObj));
}
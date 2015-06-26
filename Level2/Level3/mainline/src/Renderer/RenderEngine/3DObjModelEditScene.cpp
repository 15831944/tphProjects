#include "StdAfx.h"
#include ".\3dobjmodeleditscene.h"

#include "3DObjModelEditContext.h"
#include "SceneState.h"
#include "ogreutil.h"

#include <boost/tuple/tuple.hpp>

#include <math.h>
#include <common\FileInDB.h>
#include ".\scenenodeserializer.h"
#include <common\Guid.h>
#include ".\RenderEngine.h"

#define GridXYName _T("GridXY")
// #define GridXZName _T("GridXZ")
// #define GridYZName _T("GridYZ")


#define EditModelName _T("EditModel")

#define ModelPickLine _T("ModelPickLine")


using namespace Ogre;
using namespace std;


C3DObjModelEditScene::C3DObjModelEditScene()
	: mpEditContext(NULL)
	, m_eMouseState(Default_state)
{


}

#define MAT_SOLID _T("")
#define MAT_SOLID_WIRE _T("")
#define MAT_TRANS _T("")
#define MAT_TRANS_WIRE _T("")
#define MAT_WIRE _T("")
#define MAT_HID_WIRE _T("")

LPCTSTR const C3DObjModelEditScene::m_lpszViewType[] =
{
	_T("MeshSolid"),
	_T("MeshTransparent"),
	_T("MeshWireframe"),
	_T("MeshSolidWire"),
	_T("MeshTransparentWire"),
	_T("MeshHiddenWireframe"),
};

void C3DObjModelEditScene::Setup(C3DObjModelEditContext* pEdCtx)
{
	mpEditContext = pEdCtx;
	if(!MakeSureInited())
	{
		ASSERT(FALSE);
		return;
	}	
	//set up lighting
	//mpScene->setAmbientLight(ColourValue(0.2,0.2,0.2));


	//set up grid	
	{		
		mGridXY = AddSceneNode(GridXYName);
		UpdateGrid();
	}

	//load model to scene
	LoadModel();
	

	m_picLine3D = AddSceneNode(ModelPickLine);
	m_picLine3D.AttachTo(mGridXY);
	
}

BOOL C3DObjModelEditScene::OnMouseMove( const MouseSceneInfo& mouseInfo )
{
	do 
	{
		if (Measure_state == m_eMouseState)
		{
			Plane movePlan(Vector3::UNIT_Z, Vector3(0, 0, 0)); // Plan XY
			bool b1;
			Real d1;
			boost::tie(b1,d1) = mouseInfo.mSceneRay.intersects(movePlan);
			if (b1)
			{
				Vector3 rawPos = mouseInfo.mSceneRay.getPoint(d1);
				ARCVector3 vecPos(rawPos.x, rawPos.y, rawPos.z);
				m_vecMoveOfXY = vecPos;
				UpdatePickLine();
				UpdateDrawing();
			}
			break;
		}

		if (GetEditContext()->IsViewLocked())
			break;

		C3DObjectCommand* pCurEditOP = GetEditContext()->GetCurCommand();
		if (NULL == pCurEditOP)
			break;

		long lMouseHorOffset = mouseInfo.mpt.x - GetEditContext()->GetOpStartPoint().x;

		E3DObjectEditOperation eOpType = pCurEditOP->getEditOp();
		switch (eOpType)
		{
		case OP_move3DObj:
			{
				C3DObjectMoveCommand* pMoveOp = (C3DObjectMoveCommand*)pCurEditOP;
				Vector3 normVec;
				ARCVector3 dirVec(0.0, 0.0, 0.0);

				
				Vector3 posCamera = mouseInfo.mSceneRay.getOrigin();
				ARCVector3 posCam(posCamera.x, posCamera.y, posCamera.z);
				DistanceUnit dScaler = lMouseHorOffset*posCam.DistanceTo(pMoveOp->GetStartPos())/120.0;

				switch (pMoveOp->GetCoord())
				{
				case C3DObjectCommand::COORD_X:
					{
						dirVec.x = 1.0;
						pMoveOp->GetNodeObj().SetPosition(pMoveOp->GetStartPos() + dirVec*dScaler);
					}
					break;
				case C3DObjectCommand::COORD_Y:
					{
						dirVec.y = 1.0;
						pMoveOp->GetNodeObj().SetPosition(pMoveOp->GetStartPos() + dirVec*dScaler);
					}
					break;
				case C3DObjectCommand::COORD_Z:
					{
						dirVec.z = 1.0;
						pMoveOp->GetNodeObj().SetPosition(pMoveOp->GetStartPos() + dirVec*dScaler/8.0);
					}
					break;
				case C3DObjectCommand::COORD_XY:
					{
						normVec = Vector3::UNIT_Z;
						ARCVector3 curPosTmp = pMoveOp->GetStartPos();
						Vector3 curPos(curPosTmp.x, curPosTmp.y, curPosTmp.z);
						Plane movePlan(normVec, curPos);
						bool b1;
						Real d1;
						boost::tie(b1,d1) = mouseInfo.mSceneRay.intersects(movePlan);
						if (b1)
						{
							Vector3 rawPos = mouseInfo.mSceneRay.getPoint(d1);
							ARCVector3 vecPos(rawPos.x, rawPos.y, rawPos.z);
							pMoveOp->GetNodeObj().SetPosition(vecPos +
								GetEditContext()->GetOpStartOffset());
						}
					}
					break;
				case C3DObjectCommand::COORD_YZ:
					{
						normVec = Vector3::UNIT_X;
						ARCVector3 curPosTmp = pMoveOp->GetStartPos();
						Vector3 curPos(curPosTmp.x, curPosTmp.y, curPosTmp.z);
						Plane movePlan(normVec, curPos);
						bool b1;
						Real d1;
						boost::tie(b1,d1) = mouseInfo.mSceneRay.intersects(movePlan);
						if (b1)
						{
							Vector3 rawPos = mouseInfo.mSceneRay.getPoint(d1);
							ARCVector3 vecPos(rawPos.x, rawPos.y, rawPos.z);
							pMoveOp->GetNodeObj().SetPosition(vecPos);
						}
					}
					break;
				case C3DObjectCommand::COORD_ZX:
					{
						normVec = Vector3::UNIT_Y;
						ARCVector3 curPosTmp = pMoveOp->GetStartPos();
						Vector3 curPos(curPosTmp.x, curPosTmp.y, curPosTmp.z);
						Plane movePlan(normVec, curPos);
						bool b1;
						Real d1;
						boost::tie(b1,d1) = mouseInfo.mSceneRay.intersects(movePlan);
						if (b1)
						{
							Vector3 rawPos = mouseInfo.mSceneRay.getPoint(d1);
							ARCVector3 vecPos(rawPos.x, rawPos.y, rawPos.z);
							pMoveOp->GetNodeObj().SetPosition(vecPos);
						}
					}
					break;
				default:
					{
						ASSERT(FALSE);
					}
					break;
				}
			}
			break;
		case OP_rotate3DObj:
			{
				C3DObjectRotateCommand* pRotateOp = (C3DObjectRotateCommand*)pCurEditOP;
				ARCVector3 rotateVec(0.0, 0.0, 0.0);
				DistanceUnit dScaler = lMouseHorOffset;

				switch (pRotateOp->GetCoord())
				{
				case C3DObjectCommand::COORD_X:
					{
						rotateVec.x = 1.0;
						pRotateOp->GetNodeObj().SetRotation(pRotateOp->GetStartRotation() + rotateVec*dScaler);
					}
					break;
				case C3DObjectCommand::COORD_Y:
					{
						rotateVec.y = 1.0;
						pRotateOp->GetNodeObj().SetRotation(pRotateOp->GetStartRotation() + rotateVec*dScaler);
					}
					break;
				case C3DObjectCommand::COORD_Z:
					{
						rotateVec.z = 1.0;
						pRotateOp->GetNodeObj().SetRotation(pRotateOp->GetStartRotation() + rotateVec*dScaler);
					}
					break;
				default:
					{
						ASSERT(FALSE);
					}
					break;
				}
			}
			break;
		case OP_scale3DObj:
			{
				C3DObjectScaleCommand* pScaleOp = (C3DObjectScaleCommand*)pCurEditOP;
				ARCVector3 scaleVec = pScaleOp->GetStartScale();
				DistanceUnit dScaler = exp(lMouseHorOffset/120.0);

				switch (pScaleOp->GetCoord())
				{
				case C3DObjectCommand::COORD_X:
					{
						scaleVec.x *= dScaler;
						pScaleOp->GetNodeObj().SetScale(scaleVec);
					}
					break;
				case C3DObjectCommand::COORD_Y:
					{
						scaleVec.y *= dScaler;
						pScaleOp->GetNodeObj().SetScale(scaleVec);
					}
					break;
				case C3DObjectCommand::COORD_Z:
					{
						scaleVec.z *= dScaler;
						pScaleOp->GetNodeObj().SetScale(scaleVec);
					}
					break;
				case C3DObjectCommand::COORD_XYZ:
					{
						scaleVec.x *= dScaler;
						scaleVec.y *= dScaler;
						scaleVec.z *= dScaler;
						pScaleOp->GetNodeObj().SetScale(scaleVec);
					}
					break;
				default:
					{
						ASSERT(FALSE);
					}
					break;
				}
			}
			break;
		case OP_mirror3DObj:
			{

			}
			break;
		default:
			{
				ASSERT(FALSE);
			}
			break;
		}
// 		UpdateDrawing();
	}while (false);	
	UpdateDrawing();
	return C3DSceneBase::OnMouseMove( mouseInfo );
}

BOOL C3DObjModelEditScene::OnLButtonDown( const MouseSceneInfo& mouseInfo)
{		
	do //for can not just return save as switch case  
	{
		C3DNodeObject selNodeObj = GetSel3DNodeObj(mouseInfo);
		GetEditContext()->SetSelNodeObj(selNodeObj);
		DEAL_LISTENER_HANDLER(OnSelect3DObj(selNodeObj));

		GetEditContext()->SetStartOffset(ARCVector3(0.0, 0.0, 0.0));

		Plane xyPlan(Vector3::UNIT_Z, Vector3(0, 0, 0)); // Plan XY
		bool b1;
		Real d1;
		boost::tie(b1,d1) = mouseInfo.mSceneRay.intersects(xyPlan);
		if (b1)
		{
			Vector3 rawPos = mouseInfo.mSceneRay.getPoint(d1);
			ARCVector3 vecPos = ARCVECTOR(rawPos);

			if (Measure_state == m_eMouseState)
			{
				m_vMousePosList.push_back(vecPos);
				m_vecMoveOfXY = vecPos;
				UpdatePickLine();
				UpdateDrawing();
				break;
			}
		}

		if (GetEditContext()->IsViewLocked())
			break;

		if (selNodeObj)
		{
			ARCVector3 objPos = selNodeObj.GetPosition();
			GetEditContext()->StartNewEditOp(OP_move3DObj, C3DObjectCommand::COORD_XY);
			Plane movePlan(Vector3::UNIT_Z, objPos.z);
			bool b2;
			Real d2;
			boost::tie(b2,d2) = mouseInfo.mSceneRay.intersects(movePlan);
			if (b1)
			{
				Vector3 rawPos = mouseInfo.mSceneRay.getPoint(d2);
				ARCVector3 vecPos(rawPos.x, rawPos.y, rawPos.z);
				GetEditContext()->SetStartOffset(objPos - vecPos);
			}
			else
			{
				GetEditContext()->SetStartOffset(ARCVector3(0.0, 0.0, 0.0));
			}
			break;
		}
		UpdateDrawing();
	}
	while (false);

	return C3DSceneBase::OnLButtonDown(mouseInfo);
}

BOOL C3DObjModelEditScene::OnLButtonUp( const MouseSceneInfo& mouseInfo )
{
	do //fake return use break
	{
		GetEditContext()->DoneCurEditOp();
	}
	while (false);		
	return C3DSceneBase::OnLButtonUp(mouseInfo);
}


BOOL C3DObjModelEditScene::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{
	do 
	{
		Plane xyPlan(Vector3::UNIT_Z, Vector3(0, 0, 0)); // Plan XY
		bool b1;
		Real d1;
		boost::tie(b1,d1) = mouseInfo.mSceneRay.intersects(xyPlan);
		if (b1)
		{
			Vector3 rawPos = mouseInfo.mSceneRay.getPoint(d1);
			ARCVector3 vecPos(rawPos.x, rawPos.y, rawPos.z);

			if (Measure_state == m_eMouseState)
			{
				m_vMousePosList.push_back(vecPos);
				m_vecMoveOfXY = vecPos;
				UpdatePickLine();
				UpdateDrawing();
				double dDistance = 0.0;
				for (std::vector<ARCVector3>::iterator ite = m_vMousePosList.begin();ite+1!=m_vMousePosList.end();ite++)
				{
					dDistance += ite->DistanceTo(*(ite+1));
				}
				DEAL_LISTENER_HANDLER(OnFinishMeasure(dDistance));
				m_vMousePosList.clear();
				m_eMouseState = Default_state;
				UpdatePickLine();
				UpdateDrawing();
				break;
			}
		}


		//check what kind of object is right click		
		C3DNodeObject selNodeObj = GetSel3DNodeObj(mouseInfo);
		GetEditContext()->SetSelNodeObj(selNodeObj);
		DEAL_LISTENER_HANDLER(OnSelect3DObj(selNodeObj));
		break;
		//check if click on the 
	} while (false);

	return C3DSceneBase::OnRButtonDown(mouseInfo);
}

void C3DObjModelEditScene::SaveModel()
{
	mpEditContext->GetEditModel()->MakeSureFileOpen();
	CString strWorkDir = mpEditContext->GetEditModel()->msTmpWorkDir;
	
	C3DNodeObject nodeobj = GetSceneNode(EditModelName);
	nodeobj.SnapShot(strWorkDir);
	mpEditContext->GetEditModel()->SetDimension(nodeobj.GetBounds());

	if(mpEditContext->GetEditModel()->mbExternalMesh)
	{
		
	}
	else
	{
		SceneNodeSerializer serlizer;
		serlizer.InitSave(strWorkDir);
		serlizer.Do(STR_MODEL_NODE_FILE,GetSceneNode(EditModelName) );

		CGuid& guid  = mpEditContext->GetEditModel()->mguid;
		nodeobj.ExportToMesh(guid,strWorkDir);		
	}

}

bool C3DObjModelEditScene::LoadModel()
{
	C3DNodeObject editModel = C3DNodeObject(AddSceneNode(EditModelName));
	CModel* pModel = mpEditContext->GetEditModel();
	pModel->MakeSureFileOpen();
	ResourceFileArchive::AddDirectory(pModel->msTmpWorkDir,(CString)pModel->mguid);	

	if(pModel->mbExternalMesh)
	{
		Ogre::Entity* pEnt = OgreUtil::createOrRetrieveEntity(editModel.GetIDName(),pModel->GetMeshFileName(),getSceneManager());
		ASSERT(pEnt);
		editModel.AddObject(pEnt);
	}
	else
	{
		SceneNodeLoader loader;
		CString strWorkDir = pModel->msTmpWorkDir;
		loader.InitRead(strWorkDir);
		loader.Do(STR_MODEL_NODE_FILE,editModel);
		mpEditContext->SetCurModel(editModel);
	}
	
	return true;
}

void C3DObjModelEditScene::UpdateGrid()
{
	mGridXY.Update(GetModelGrid(),CGrid3D::XYExtent,false);
// 	mGridXZ.Update(mGrid,CGrid3D::XZExtent,false);
// 	mGridYZ.Update(mGrid,CGrid3D::YZExtent,false);
}

C3DNodeObject C3DObjModelEditScene::AddNewComponent( CString strMeshFileName, const ARCVector3& droppos )
{
	/*Plane movePlan(Vector3::UNIT_Z, Vector3(0,0,dHeight));
	bool b1;
	Real d1;
	boost::tie(b1,d1) = dropRay.intersects(movePlan);
	if(b1)*/
	{
		C3DNodeObject newNode = mpEditContext->GetCurModel().CreateNewChild();
		if (newNode)
		{
			newNode.SetName(newNode.GetIDName());
			Entity* pEnt = OgreUtil::createOrRetrieveEntity(newNode.GetIDName(), strMeshFileName, getSceneManager());
			if (pEnt)
			{
				newNode.AddObject(pEnt);
				newNode.EnableSelect(true);
				//Vector3 rawPos = dropRay.getPoint(d1);
				newNode.SetPosition(droppos);
				return newNode;
			}
			newNode.Detach();
		}
	}

	return C3DNodeObject();
}



const Ogre::String PICKLINEMAT = _T("PickingLine");
void C3DObjModelEditScene::UpdatePickLine()
{
	CLines3D::VertexList vertexList;
	if (Measure_state == m_eMouseState)
	{
		vertexList = m_vMousePosList;
		vertexList.push_back(m_vecMoveOfXY);
	}
	else
	{
		//
	}
	m_picLine3D.Update(vertexList);
}

void C3DObjModelEditScene::StartDistanceMeasure()
{
	GetEditContext()->DoneCurEditOp();
	m_eMouseState = Measure_state;
}

void C3DObjModelEditScene::UpdateDrawing()
{
	DEAL_LISTENER_HANDLER(OnUpdateDrawing());
}

C3DObjModelEditScene::ViewType C3DObjModelEditScene::GetViewTypeFromString( LPCTSTR lpszViewType )
{
	CString strViewType = lpszViewType;
	for(int i=VT_TypeStart;i<VT_TypeEnd;i++)
	{
		if (strViewType == m_lpszViewType[i])
			return (ViewType)i;
	}
	return VT_Solid;
}

void C3DObjModelEditScene::SetModelViewType( ViewType modelViewType )
{
	mModelViewType = modelViewType;
	GetEditContext()->GetCurModel().SetMaterial(CString(m_lpszViewType[modelViewType]));
}

CGrid& C3DObjModelEditScene::GetModelGrid()
{
	return mpEditContext->GetEditModel()->mGrid;	
}

//CCameraData& C3DObjModelEditScene::GetActiveCam()
//{
//	return mpEditContext->GetEditModel()->mActiveCam;
//}
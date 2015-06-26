#pragma once

#include <Common/CommandHistory.h>
#include <Common/ListenerCommon.h>
#include "3DObjectCommands.h"
#include <common/ProjectEditContext.h>

#include "3DNodeObject.h"

class CModel;
class C3DNodeObjectListener;

class RENDERENGINE_API C3DObjModelEditContext : public  CProjectEditContext
{
public:
	//lister the scene's changes
	void AddSelNodeObjListener(C3DNodeObjectListener* pListener)
	{
		mSelNodeObjListenerList.push_back(pListener);
	}

public:
	C3DObjModelEditContext();
	virtual ~C3DObjModelEditContext(void);

	CModel* GetEditModel() const { return mpEditModel; }
	void ChangeEditModel(CModel* pEditModel);

	void SetCurModel(C3DNodeObject model3D);
	C3DNodeObject GetCurModel() const;

	

	CommandHistory& getHistory(){ return m_editHistory; }

	bool IsEditNotSave()const{ return m_bModelEditDirty; }

	C3DNodeObject GetSelNodeObj() const { return m_selNodeObj; }
	void SetSelNodeObj(C3DNodeObject selNodeObj);

	void StartNewEditOp(E3DObjectEditOperation eOpType, C3DObjectCommand::EditCoordination eCoord = C3DObjectCommand::COORD_NONE);
	void StartNewEditOp(C3DNodeObject nodeObj, E3DObjectEditOperation eOpType, C3DObjectCommand::EditCoordination eCoord = C3DObjectCommand::COORD_NONE);
	void DoneCurEditOp();
	void CancelCurEditOp();

	C3DObjectCommand* GetCurCommand() const { return m_pCurCommand; }
	bool AddCommand( C3DObjectCommand* pCmd );

	CPoint GetOpStartPoint() const { return m_ptOpStart; }
	void SetOpStartPoint(CPoint val) { m_ptOpStart = val; }

	ARCVector3 GetOpStartOffset() const { return m_vOpStartOffset; }
	void SetStartOffset(ARCVector3 val) { m_vOpStartOffset = val; }


	void SaveEditModel();
	

	void Clear();
protected:
	C3DNodeObject m_model3D;
	C3DNodeObject m_selNodeObj;
	CModel* mpEditModel;

	CommandHistory m_editHistory;
	C3DObjectCommand* m_pCurCommand;

	CPoint m_ptOpStart;
	ARCVector3 m_vOpStartOffset;

	bool m_bModelEditDirty;	

	typedef std::vector<C3DNodeObjectListener*> SelNodeObjListenerList;
	SelNodeObjListenerList mSelNodeObjListenerList;
};

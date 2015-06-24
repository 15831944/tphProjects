#pragma once
#include <common/IARCportLayoutEditContext.h>
#include <common/ENV_MODE.h>
class CTerminalEditContext;
class CLandsideEditContext;
class CAircraftLayoutEditor;
class CAirsideEditContext;
class CAirsideEditContext;
class CProjectEditContext;
class ILandsideEditObjectCmd;
class CTermPlanDoc;
class CNewMenu;

class QueryData;
class LandsideFacilityLayoutObject;
enum ALTOBJECT_TYPE;
class CAirsideLevelList;
class CARCLayoutObjectDlg;
//////////////////////////////////////////////////////////////////////////
//manager all layout edit context
//////////////////////////////////////////////////////////////////////////
class CARCportLayoutEditor : public IARCportLayoutEditContext
{
public:
	CARCportLayoutEditor(CTermPlanDoc* pDoc);
	~CARCportLayoutEditor(void);
	
	IEnvModeEditContext* GetContext(EnvironmentMode mode)const;
	virtual IEnvModeEditContext* GetCurContext()const;


	CNewMenu* GetContextMenu(CNewMenu& menu);
	void OnCtxProcproperties(LandsideFacilityLayoutObject* pObj,CWnd* pParent,QueryData* pData = NULL);
	void OnCtxDelLandsideProc(LandsideFacilityLayoutObject* pObj);
	void OnNewLandsideProc(ALTOBJECT_TYPE type,CWnd* pParent);
	void OnCtxProcproperties(CWnd* pParent);
	void OnCtxDelLandsideProc();
	void OnCtxDeleteControlPoint();
	void OnCtxBeginMoveControlPointVertical();
	void OnCtxAddControlPoint();
	void OnCtxCopyLayoutObject();
	void OnCtxChangeControlPointType();
	void OnCtxMoveCtrlPointToLevel(int iFloor);
	void OnSnapPointToParkLotLevel(int nLevel);

	void OnCopyParkLotItem();
	void OnDelParkLotItem();

	void OnCtxDeleteLayoutItem();
	void OnCtxLayoutObjectDisplayProp(CWnd* pParent);


	CTermPlanDoc* GetDocument()const;



	//interfaces
	virtual void* OnQueryDataObject(const CGuid& guid, const GUID& class_guid);
	virtual bool OnQueryDataObjectList(std::vector<void*>& vData, const GUID& cls_guid);
	virtual InputAirside* OnQueryInputAirside();
	virtual CRenderFloorList  OnQueryAirsideFloor(int nAirportID);
	virtual bool OnQueryAirportDataObjectList(int airportID,std::vector<void*>& vData, const GUID& cls_guid );

	virtual CString GetTexturePath()const;
	
	bool IsShowText()const;
	bool IsEditable()const; 

public:
	CAircraftLayoutEditor* mpAircraftLayoutEditor;
	CAirsideEditContext* m_airsideEdit;
	CTerminalEditContext* m_terminalEdit;
	CLandsideEditContext* m_landsideEdit;

	CTermPlanDoc* m_pDoc;
	
	LandsideFacilityLayoutObject*  GetRClickObject();
	std::map<int ,CAirsideLevelList*> m_AirsideLevelsMap;
	CARCLayoutObjectDlg* m_LayoutObjectDlg;
};

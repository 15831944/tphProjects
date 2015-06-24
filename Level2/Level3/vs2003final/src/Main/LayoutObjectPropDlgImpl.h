#pragma once

class CARCLayoutObjectDlg;
class LandsideFacilityLayoutObject;
class CPath2008;
class ALTObjectID;
#include <commondata/Fallback.h>
#include <common/ALTObjectID.h>
enum ARCUnit_Length;
enum ARCUnit_Velocity;

class CAirsideObjectTreeCtrl;
class QueryData;

class ILandsideEditContext;
class ITerminalEditContext; 
class CUnitPiece;
class ControlPath;

class ILayoutObjectPropDlgImpl
{
public:
	static ILayoutObjectPropDlgImpl* Create(LandsideFacilityLayoutObject* pObj, CARCLayoutObjectDlg* pDlg, QueryData* pData = NULL );
	ILayoutObjectPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );
	virtual ~ILayoutObjectPropDlgImpl(){}

	virtual CString getTitle()=0;
	virtual void LoadTree()=0;
	virtual int getBitmapResourceID()=0;
	virtual FallbackReason GetFallBackReason();

	
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *&pMenu){}	
	virtual void DoFallBackFinished( WPARAM wParam, LPARAM lParam ){}

	virtual bool SetObjectName(const ALTObjectID& id);
	ALTObjectID& GetObjectName();

	virtual void OnCancel();
	virtual bool OnOK(CString& errorMsg);

	bool IsObjectLocked();
	void SetObjectLock(bool b);

	void SetRClickItem(HTREEITEM hItem);
	HTREEITEM GetRClickItem(){ return m_hRClickItem; }

	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem){}
	//virtual void OnLButtonDownPropTree(HTREEITEM hTreeItem) {}
	virtual void OnSelChangedPropTree(HTREEITEM hTreeItem) {}
	virtual BOOL OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam){ return TRUE; }
	
	ALTObjectIDList& getListObjectName(){ return m_lstExistObjectName; };

	virtual void OnPropDelete(){}
	virtual void OnControlPathReverse(){}

	double ConvertLength(double cm); //convert from cm to dlg unit
	double UnConvertLength(double dlgunit); //convert form dlgunit to cm;

	double ConvertSpeed(double dbunit);
	double UnConvertSpeed(double dspunit);

	static void GetFallBackAsPath( WPARAM wParam, LPARAM lParam , CPath2008& path);

	void Update3D();

	virtual void GetFallBackReasonData(FallbackData& data){}

	virtual bool ChangeFocus()const {return false;}

	//--------------------------------------------------------------------------------------------------
	//Summary:
	//			default handle all layout object change vertical profile function
	//--------------------------------------------------------------------------------------------------
	virtual void HandleChangeVerticalProfile(){}

	void CreateChangeVerticalProfileMenu(CMenu *pMenu);

	ILandsideEditContext* GetLandsideEditContext()const;
	ITerminalEditContext* GetTerminalEditContext()const;

	virtual void InitUnitPiece(CUnitPiece* unit); 
protected:
	//--------------------------------------------------------------------------------------------------
	//Summary:
	//			layout object whether need to add change vertical profile menu
	//---------------------------------------------------------------------------------------------------
	virtual bool NeedChangeVerticalProfile() {return false;}

protected:
	CARCLayoutObjectDlg* m_pParentDlg;
	LandsideFacilityLayoutObject* m_pObject;	
	HTREEITEM m_hRClickItem;
	HTREEITEM m_hLClickItem;
	bool m_bPropModified ;
	bool m_bNameModified ;	
	
	ALTObjectIDList m_lstExistObjectName;	

	ARCUnit_Length GetCurLengthUnit();
	ARCUnit_Velocity GetCurSpeedUnit();

	CAirsideObjectTreeCtrl& GetTreeCtrl();
	CButton* getCheckBoxCellPhone();


	void LoadTreeSubItemCtrlPoints( HTREEITEM preItem,const ControlPath& path );


};




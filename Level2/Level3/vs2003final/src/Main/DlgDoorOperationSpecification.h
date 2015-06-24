#pragma once
#include "../MFCExControl/ARCTreeCtrl.h"

// DlgDoorOperationSpecification dialog
class InputTerminal;
class InputOnboard;
class DoorOperationalSpecification;
class FlightDoorOperationalSpec;
class DoorOperationalData;
class COnboardFlight;
class OnboardFlightSeatBlock;
class OnboardFlightSeatBlockContainr;
class DlgDoorOperationSpecification : public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgDoorOperationSpecification)

public:
	DlgDoorOperationSpecification(InputTerminal* pTerminal,InputOnboard* pInputOnboard, bool bEntryDoor, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgDoorOperationSpecification();

// Dialog Data
	enum { IDD = IDD_DLGDOOROPERATIONSPECIFICATION };

protected:
	enum SelTreeItemType
	{
		NoneSel = 0,
		TextSetting,
		FlightSel,
		TextDoors,
		DoorSel,
		DoorParaSel,
		TextSeatBlockRoot,
		TextSeatBlock,
		SeatBlockItem,
		TextPaxType,
		PaxTypeItem,

		Status_item,
		InitialTime,
		ClosureTime,
	};

	class ItemData
	{
	public:
		ItemData(SelTreeItemType type, DWORD_PTR   data)
		{
			enumType = type;
			pData	 = data;
		}
		~ItemData()
		{

		}

		SelTreeItemType enumType;
		DWORD_PTR   pData;
	protected:
	private:
	};



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void AddFlight();
	afx_msg void DelFlight();

	afx_msg void AddNewItem();
	afx_msg void RemoveItem();

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnTvnSelchangedTreeDooroperationsetting(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelChangeFlightList();
protected:
	//void AddFlightItem();
	//void RemoveFlightItem();

	void AddPaxType(HTREEITEM hSelItem);
	void RemovePaxType(HTREEITEM hSelItem);

	void AddBlock(HTREEITEM hSelItem);
	void RemoveBlock(HTREEITEM hSelItem);

	void InitFlightList();
//	void OnInitTreeCtrl();

	void RefreshTreeCtrl();

	SelTreeItemType GetItemType(HTREEITEM hSel);
	DWORD_PTR GetItemData(HTREEITEM hSel);


//	void InitFlightDataAndTreeItems(HTREEITEM hItem, FlightDoorOperationalSpec* pNewData);
	void InitDoorDataAndTreeItems(HTREEITEM hItem, DoorOperationalData*pDoorData);
	bool FlightExist(COnboardFlight *flight);
	FlightDoorOperationalSpec *getCurFlightDoorSpec();

	OnboardFlightSeatBlock *GetFlightSeatBlock(FlightDoorOperationalSpec* pFlight);

	void UpdateToolBarState();
private:
	InputTerminal* m_pInputTerminal;
	InputOnboard* m_pInputOnboard;

	CToolBar m_wndToolbar;
	CToolBar m_flightToolBar;
	CARCTreeCtrl m_wndTreeCtrl;
	HTREEITEM m_hFlightclickItem;
	CListBox m_listFlight;

	DoorOperationalSpecification* m_pDoorOperations;
	FlightDoorOperationalSpec* m_pCurFlightData;
	bool m_bEntryDoor; 

	DWORD_PTR NewItemData(SelTreeItemType enumType, DWORD_PTR pData)
	{
		ItemData *pItem = new ItemData(enumType, pData);
		m_vTreeItemData.push_back(pItem);
		return (DWORD_PTR)pItem;
	}
	std::vector<ItemData *> m_vTreeItemData;
	

	OnboardFlightSeatBlockContainr* m_pSeatBlocks;
	std::map<FlightDoorOperationalSpec *,OnboardFlightSeatBlock *> m_mapFlightSeatBlock;
};




















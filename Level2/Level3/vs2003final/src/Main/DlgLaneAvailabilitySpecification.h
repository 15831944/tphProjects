#pragma once

#include "../MFCExControl/ListCtrlEx.h"


class CLaneAvailabilitySpecificationList;
class InputLandside;
class CLaneAvailabilitySpecification;

class CDlgLaneAvailabilitySpecification : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgLaneAvailabilitySpecification)

public:
	CDlgLaneAvailabilitySpecification(InputLandside* pInputLand,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLaneAvailabilitySpecification();

	// Dialog Data
	enum { IDD = IDD_DIALOG_LANDSIDEVEHICLEASSIGNMENT };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();

	void OnInitToolbar();
	void UpdateToolBarState();
	void InitListControl();
	void SetListContent();
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	CLaneAvailabilitySpecificationList* getAssignData();
	void GetProbDropdownList( CStringList& strList );
	void SetModified( BOOL bModified = TRUE );
	void InsertItemToList( CLaneAvailabilitySpecification* pLaneAvailability );
	void UpdateItem( int nItemIndex, CLaneAvailabilitySpecification* pLaneAvailability );
	void UpdateListStyle();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnDbClickListCtrl(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBtnSave();

	

private:

	CListCtrlEx    m_wndListCtrl;
	CToolBar       m_wndToolBar;
	InputLandside*	m_pInLandside;
	CLaneAvailabilitySpecificationList* m_laneAvailabilitySpec;
};

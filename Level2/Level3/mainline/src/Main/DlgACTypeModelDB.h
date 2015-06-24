#pragma once
#include ".\MFCExControl\ListCtrlEx.h"
#include "DlgAircraftModelEdit.h"

class CACTypesManager;
class CACType;
class CAircraftModelDatabase;
class CAircraftModel;
class CAirportDatabase;
// CDlgACTypeModelDB dialog
#define  ACTYPEBMP_COUNT 4
class CModel;
class CDlgACTypeModelDB : public CDialog
{
	DECLARE_DYNAMIC(CDlgACTypeModelDB)

public:
	CDlgACTypeModelDB(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgACTypeModelDB();
	
// Dialog Data
	enum { IDD = IDD_ACTYPEMODELDB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	HBITMAP m_hBmp[ACTYPEBMP_COUNT];

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAddACTypeModel();
	afx_msg void OnRemoveACTypeModel();
	afx_msg void OnEditACTypeModel();
	afx_msg void OnSelChangeACTypeModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDisplayAircraftModel();
	afx_msg void OnSelListCtrlComboBox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);

	void ClearBmps(bool bDelete);

	void OnInitToolbar();
	void OnInitListCtrl();
	void UpdateToolbarState();
	void PopulateACModel();
	void DisplayAllBmpInCtrl();
	void DisplayBmpInCtrl(HBITMAP hBmp,UINT nID);

	void LoadBmp( CModel* pModel );

	CACType* GetAcTypeByName(CString strName);
public:
	void InitOpAirpotDB(CAirportDatabase* pAirportDB);

	CACType* GetEditACType() const { return m_pEditACType; }
	CAircraftModel* GetEditModel() const { return m_pEditModel; }
private:
	CListCtrlEx m_wndListCtrl;
	CStatic m_wndPicture;
	CToolBar m_wndACTypeModel;

	CAirportDatabase* m_pAirportDB;
	CACTypesManager*	m_pAcMan;
	CAircraftModelDatabase* m_pModelList;

	CACType* m_pEditACType;
	CAircraftModel* m_pEditModel;
};

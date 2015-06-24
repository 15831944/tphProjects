#pragma once
#include "../Common/ACTypesManager.h"
#include "../MFCExControl/SimpleToolTipListBox.h"
#include "CoolBtn.h"
#include "../MFCExControl/XTResizeDialog.h"

class AircraftAliasItem;
class CDlgAircraftAlias : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgAircraftAlias)

public:
	CDlgAircraftAlias(Terminal *pACManager, CWnd* pParent = NULL);
	virtual ~CDlgAircraftAlias();

protected:
	CComboBox m_wndComboShapes;
	CSimpleToolTipListBox m_wndListBoxAssoc;
	CSimpleToolTipListBox m_wndListBoxUnAssoc;

	std::vector<CString> m_vectShape;
	std::vector<CString> m_vectUnAssocAC;
	CACTypesManager *m_pACManager;
	CCoolBtn m_Load;
protected:
//Added by cjchen , using for check box function !
		typedef std::vector<CString> VECTOT_FLIGHT ;
		typedef VECTOT_FLIGHT::iterator ITERATOR_FLIGHT ;
		VECTOT_FLIGHT CheckedList ;
		Terminal * m_pTerminal;
		void setUnAssociatedListBoxwithFilter();
		void setUnAssociatedListBoxwithOutFilter();
//////////////////////////////////////////////////////////////////////////
protected:
	void InitShapesComboBox();
	void InitUnAssociatedListBox();
	void ResetAssociatedListBox(CString strShapename);
	
	void LoadShapesName();
public:
// Dialog Data
	enum { IDD = IDD_DIALOG_AIRCRAFTALIAS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnSelChangeComboShape();
	afx_msg void OnBnClickedMoveLeft();
	afx_msg void OnBnClickedMoveRight();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedOK();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void initCheckBoxByScheduleData();
	afx_msg void OnBnClickedCheckByschedule();
	afx_msg void OnSaveAsTemplate();
	void LoadFromTemplateDatabase();
	afx_msg void OnLoadFromTemplate();
	void OnClickMultiBtn();
};

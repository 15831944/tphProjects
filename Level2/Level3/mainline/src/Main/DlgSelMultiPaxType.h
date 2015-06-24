#pragma once
#include "afxwin.h"
#include "MoblieElemDlg.h"

#include "..\inputs\MobileElemConstraint.h"


// #define MULTI_PAX_TYPE_COUNT				8
#define MULTI_PAX_TYPE_COUNT				MAX_PAX_TYPES			// 16
class PassengerTypeList;

// DlgSelMultiPaxType dialog
struct TypeString
{
	int nIdx;
	CString str;
	int nLevel;
};


class DlgSelMultiPaxType : public CDialog
{
	DECLARE_DYNAMIC(DlgSelMultiPaxType)

public:
	DlgSelMultiPaxType(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgSelMultiPaxType();

// Dialog Data
	enum { IDD = IDD_SelMultiPaxType };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	InputTerminal* GetInputTerminal();

	DECLARE_MESSAGE_MAP()
private:
	//CTreeCtrl *m_pTree;
	CMobileElemConstraint *m_pConstraint;
	PassengerTypeList *m_pInTypeList;
public:
	void SetPointers(PassengerTypeList* pInTypeList,CMobileElemConstraint* pConstraint);
	std::vector<TypeString>  m_TypeStrings;
	std::vector<int> m_Sel;
	CComboBox m_allLevelCombox[MULTI_PAX_TYPE_COUNT];
	virtual BOOL OnInitDialog();
    void InsertPassengerType();
	afx_msg void OnBnClickedOk();
};

#pragma once

//#include <MFCExControl\EditCtrl\NumEditC.h>
// CDlgSectionProp dialog
//Component Section Dialog
#include <common/CADInfo.h>
class CComponentMeshSection;
class CComponentMeshModel;

class CDlgSectionProp : public CDialog
{
	DECLARE_DYNAMIC(CDlgSectionProp)

public:
	CDlgSectionProp(CComponentMeshSection* pSection,const CString& strName, CWnd* pParent = NULL);   // standard constructor if pSection = NULL , then is will new a 
	virtual ~CDlgSectionProp();

// Dialog Data
	enum { IDD = IDD_DIALOG_SECTION_PROP };
	bool mbMapChange;
	bool mbSectionChange;
	
	//create section from the user define
	CComponentMeshSection* createSection(CComponentMeshModel* pModel)const;
	void SetPos(double d);
protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CString m_sUnits;
	
	//controls
	CComboBox m_cmbPointCount;
	//cNumericEditCtrl mEditCtrlPos;
	//cNumericEditCtrl mEditCtrlRad;
	//cNumericEditCtrl mEditCtrlThick;
	//cNumericEditCtrl mEditOffsetX;
	//cNumericEditCtrl mEditOffsetY;
//section properties
	CCADFileDisplayInfo m_cadFileInfo;
	CString m_sName;  //section name	
	double m_dDefaultThick;
	ARCVector2 mCenter; //offset to (0,0)
	double m_dPos; //pos at the length 
	double m_dRad;
	int  m_iPtCount;
	int m_iSectType;
		
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	CComponentMeshSection* mpSection;

	CComboBox m_combsectionType;
	afx_msg void OnCbnSelchangeComboSectiontype();
	afx_msg void OnBnClickedButtonMapoptions();
};

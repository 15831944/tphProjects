#pragma once
#include "TreeCtrlEx.h"

#include <vector>
#include <Common/ARCVector.h>

class CStructure;
class CFloor2;
// CDlgStructureProp dialog
class CDlgStructureProp : public CDialog
{
	DECLARE_DYNAMIC(CDlgStructureProp)

public:
	CDlgStructureProp(CStructure* _pStructure=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgStructureProp();

	virtual BOOL OnInitDialog();
	enum { IDD = IDD_DIALOG_STURCTUREPROP };

	int DoFakeModal();
	bool IsNameValid(CString& strName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	CComboBox m_wndName1Combo;
	CComboBox m_wndName2Combo;
	CComboBox m_wndName3Combo;
	CComboBox m_wndName4Combo;
	CTreeCtrlEx m_wndPropTree;
	CListCtrl m_wndTextureList;
	CImageList m_TextureImgList;

	CString	m_csLevel1;
	CString	m_csLevel2;
	CString	m_csLevel3;
	CString	m_csLevel4;
	CString m_strTexture;
	ULONG_PTR	m_gdiplusToken;

	afx_msg void OnProcpropPickfrommap();
	afx_msg void OnNMRclickTreeProperties(NMHDR *pNMHDR, LRESULT *pResult);
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);

	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	afx_msg void OnSize(UINT nType , int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	void LoadTree();
	void ReloadTextureList();
	CFloor2* GetPointFloor(int nLevel);

	CStructure * m_pStructure ;

protected:
	
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	std::vector<ARCVector3> m_vServiceLocation;
	HTREEITEM m_hServiceLocation;


public:
	CString getText(){return m_strTexture;}
	CString & getcsLevel1(){return m_csLevel1;}
	CString & getcsLevel2(){return m_csLevel2;}
	CString & getcsLevel3(){return m_csLevel3;}
	CString & getcsLevel4(){return m_csLevel4;}
	std::vector<ARCVector3> & getServiceLocation(){
		return m_vServiceLocation;
	}
	void setcsLevel1(CString cstr){m_csLevel1=cstr;}
	void setcsLevel2(CString cstr){m_csLevel2=cstr;}
	void setcsLevel3(CString cstr){m_csLevel3=cstr;}
	void setcsLevel4(CString cstr){m_csLevel4=cstr;}
	void setText(CString strTexture){m_strTexture=strTexture;}
	void setServiceLocation(std::vector<ARCVector3> & AreaPath);
	
	afx_msg void OnBnClickedOk();
	virtual void OnOK();

private:
	void PreProcessName();

};

#pragma once
#include "afxwin.h"

#include <OgreRoot.h>
#include <OgreConfigOptionMap.h>

// CDlgRenderOptions dialog


class CDlgRenderOptions : public CDialog
{
	DECLARE_DYNAMIC(CDlgRenderOptions)

public:
	CDlgRenderOptions(bool bUseNewRenderEngine, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRenderOptions();

	bool IsUseNewRenderEngine() const { return m_bUseNewRenderEngine; }

// Dialog Data
	enum { IDD = IDD_DIALOG_RENDER_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnCbnSelchangeComboRenderSystem();
	afx_msg void OnLbnSelchangeListOptions();
	afx_msg void OnCbnSelchangeComboOptionSetting();

	void ResetListRenderOptions();
	static BOOL FilterOptionTag(const CString strOptionTag);

	DECLARE_MESSAGE_MAP()

private:
	CButton m_btnUseNewRenderEngine;
	CStatic m_staticOptionPrompt;
	CComboBox m_comboOptionSetting;
	CComboBox m_comboRenderSystem;
	CListBox m_listRenderOptions;

	//Ogre::RenderSystemList* m_lstRend;
	Ogre::RenderSystem* m_pSelRenderSystem;
	Ogre::ConfigOptionMap m_renderOptions;

	bool m_bUseNewRenderEngine;
};

// DlgRenderOptions.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgRenderOptions.h"
#include ".\dlgrenderoptions.h"

#include <OgreRoot.h>
#include <OgreConfigOptionMap.h>
#include <OgreRenderSystem.h>
// CDlgRenderOptions dialog

IMPLEMENT_DYNAMIC(CDlgRenderOptions, CDialog)
CDlgRenderOptions::CDlgRenderOptions(bool bUseNewRenderEngine, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_RENDER_OPTIONS, pParent)
	, m_bUseNewRenderEngine(bUseNewRenderEngine)
{
}

CDlgRenderOptions::~CDlgRenderOptions()
{
}

void CDlgRenderOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_RENDER_SYSTEM, m_comboRenderSystem);
	DDX_Control(pDX, IDC_LIST_OPTIONS, m_listRenderOptions);
	DDX_Control(pDX, IDC_STATIC_PROMPT, m_staticOptionPrompt);
	DDX_Control(pDX, IDC_COMBO_OPTION_SETTING, m_comboOptionSetting);
	DDX_Control(pDX, IDC_CHECK_USE_NEW_RENDERENGINE, m_btnUseNewRenderEngine);
}


BEGIN_MESSAGE_MAP(CDlgRenderOptions, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_RENDER_SYSTEM, OnCbnSelchangeComboRenderSystem)
	ON_LBN_SELCHANGE(IDC_LIST_OPTIONS, OnLbnSelchangeListOptions)
	ON_CBN_SELCHANGE(IDC_COMBO_OPTION_SETTING, OnCbnSelchangeComboOptionSetting)
END_MESSAGE_MAP()


// CDlgRenderOptions message handlers

BOOL CDlgRenderOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	// init the check
	m_btnUseNewRenderEngine.SetCheck(m_bUseNewRenderEngine);
	// Load saved settings
	m_pSelRenderSystem = Ogre::Root::getSingleton().getRenderSystem();
	// Get all render systems
	Ogre::RenderSystemList m_lstRend = Ogre::Root::getSingleton().getAvailableRenderers();
	if ( m_lstRend.size())
	{
		Ogre::RenderSystemList::iterator pRend = m_lstRend.begin();
		int i = 0;
		if (NULL == m_pSelRenderSystem)
		{
			m_pSelRenderSystem = *pRend;
		}
		while (pRend != m_lstRend.end())
		{
			m_comboRenderSystem.AddString((*pRend)->getName().c_str());

			if (*pRend == m_pSelRenderSystem)
			{
				// Select
				m_comboRenderSystem.SetCurSel(i);
			}

			++pRend;
			++i;
		}
		ResetListRenderOptions();
	}

// 	SetResize(IDC_STATIC_FRAME,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
// 	SetResize(IDC_STATIC_RENDERSYS,SZ_TOP_LEFT,SZ_TOP_RIGHT) ;
// 	SetResize(IDC_COMBO_RENDER_SYSTEM,SZ_TOP_LEFT,SZ_TOP_RIGHT);
// 	SetResize(IDC_LIST_OPTIONS,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
// 	SetResize(IDC_STATIC_RENDERSYS,SZ_BOTTOM_LEFT,SZ_BOTTOM_RIGHT);
// 	SetResize(IDC_COMBO_OPTION_SETTING,SZ_BOTTOM_LEFT,SZ_BOTTOM_RIGHT);
// 	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
// 	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRenderOptions::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	// Set render system
	if (!m_pSelRenderSystem)
	{
		MessageBox("Please choose a rendering system.", NULL, MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	Ogre::String err = m_pSelRenderSystem->validateConfigOptions();
	if (err.length() > 0)
	{
		// refresh options incase updated by validation
		ResetListRenderOptions();
		MessageBox(err.c_str(), NULL, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	Ogre::Root::getSingleton().setRenderSystem(m_pSelRenderSystem);
	Ogre::Root::getSingleton().saveConfig();

	m_bUseNewRenderEngine = m_btnUseNewRenderEngine.GetCheck() == BST_CHECKED;
	CDialog::OnOK();
}

void CDlgRenderOptions::OnCbnSelchangeComboRenderSystem()
{
	// TODO: Add your control notification handler code here
	// RenderSystem selected
	// Get selected index
	Ogre::RenderSystemList m_lstRend = Ogre::Root::getSingleton().getAvailableRenderers();
	int sel = m_comboRenderSystem.GetCurSel();
	if (sel != -1)
	{
		// Get RenderSystem selected
		Ogre::RenderSystemList::iterator pRend = m_lstRend.begin();
		m_pSelRenderSystem = pRend[sel];
		ResetListRenderOptions();

	} 
}

void CDlgRenderOptions::OnLbnSelchangeListOptions()
{
	// TODO: Add your control notification handler code here
	// Selection in list box of options changed
	// Update combo and label in edit section
	int i;
	int sel = m_listRenderOptions.GetCurSel();
	if (sel != -1)
	{
		int nDataIndex = m_listRenderOptions.GetItemData(sel);
		Ogre::ConfigOptionMap::iterator pOpt = m_renderOptions.begin();
		for (i = 0; i < nDataIndex; i++)
			++pOpt;
		// Set label text
		m_staticOptionPrompt.SetWindowText(pOpt->second.name.c_str());
		// Set combo options
		m_comboOptionSetting.ResetContent();
		Ogre::StringVector::iterator pPoss = pOpt->second.possibleValues.begin();
		i = 0;
		while (pPoss!=pOpt->second.possibleValues.end())
		{
			m_comboOptionSetting.AddString(pPoss[0].c_str());
			if (pPoss[0] == pOpt->second.currentValue)
				// Select current value
				m_comboOptionSetting.SetCurSel(i);
			++pPoss;
			++i;
		}
		// Enable/disable combo depending on (not)immutable
		m_comboOptionSetting.EnableWindow(!(pOpt->second.immutable));
	}
}

void CDlgRenderOptions::OnCbnSelchangeComboOptionSetting()
{
	// TODO: Add your control notification handler code here
	// Updated an option
	// Get option
	int i;
	int sel = m_listRenderOptions.GetCurSel();
	int savedSel = sel;
	int nDataIndex = m_listRenderOptions.GetItemData(sel);
	Ogre::ConfigOptionMap::iterator pOpt = m_renderOptions.begin();
	for (i = 0; i < nDataIndex; i++)
		++pOpt;
	// Get selected value
	sel = m_comboOptionSetting.GetCurSel();

	if (sel != -1)
	{
		Ogre::StringVector::iterator pPoss = pOpt->second.possibleValues.begin();

		// Set option
		m_pSelRenderSystem->setConfigOption(
			pOpt->second.name, pPoss[sel]);
		ResetListRenderOptions();
		// Select previously selected item
		m_listRenderOptions.SetCurSel(savedSel);
	}
}

void CDlgRenderOptions::ResetListRenderOptions()
{
	// refresh options
	// Get options from render system
	m_renderOptions = m_pSelRenderSystem->getConfigOptions();
	// Reset list box
	m_listRenderOptions.ResetContent();
	// Iterate through options
	Ogre::ConfigOptionMap::iterator pOpt = m_renderOptions.begin();
	Ogre::String strLine;
	int nDataIndex = 0;
	while (pOpt!=m_renderOptions.end())
	{
		if (!FilterOptionTag(pOpt->second.name.c_str()))
		{
			strLine = pOpt->second.name + ": " + pOpt->second.currentValue;
			int nListIndex = m_listRenderOptions.AddString(strLine.c_str());
			m_listRenderOptions.SetItemData(nListIndex, nDataIndex);
		}
		++pOpt;
		++nDataIndex;
	}
}

BOOL CDlgRenderOptions::FilterOptionTag(const CString strOptionTag)
{
	static const char* const cFilterTag[4] =
	{
		"Display Frequency",
		"Full Screen",
		"Video Mode",

		"Rendering Device",
	};

	/*for(int i=0;i<sizeof(cFilterTag)/sizeof(cFilterTag[0]);i++)
	{
		if (strOptionTag == cFilterTag[i])
		{
			return TRUE;
		}
	}*/
	return FALSE;
}

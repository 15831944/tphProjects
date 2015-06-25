#pragma once
#include "CUnitMenuButton.h"
#include "AirsideGUIAPI.h"

class CUnitPiece;
class CDlgUnitManagerSetting;

// CUnitButton

class AIRSIDEGUI_API CUnitButton : public CUnitMenuButton
{
	DECLARE_DYNAMIC(CUnitButton)

public:
	CUnitButton(CUnitPiece * pParent = 0);
	virtual ~CUnitButton();
protected:
	CUnitPiece * m_pParent;
protected:
	void UpdataSetting(void);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUnitSettingFirstRange(UINT nID);
	afx_msg void OnUnitSettingSecondRange(UINT nID);
	afx_msg void OnUnitSettingThirdRange(UINT nID);
	afx_msg void OnUnitSettingFourthRange(UINT nID);
	afx_msg void OnUnitSettingFifthRange(UINT nID);
};



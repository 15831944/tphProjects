#pragma once
#include "../MFCExControl/XListCtrl.h"
class ACTypeStandConstraintImportListCtrl : public CXListCtrl
{
		DECLARE_DYNAMIC(ACTypeStandConstraintImportListCtrl)
public:
	ACTypeStandConstraintImportListCtrl(void);
	virtual ~ACTypeStandConstraintImportListCtrl(void);

	virtual void DrawItem(LPDRAWITEMSTRUCT lpdis);
	HICON	GetIcon(int nVal);

	std::map<int, HICON>		m_mapElementIcon;
	HICON		m_hDefaultIcon;
protected:
	HICON LoadIcon(UINT uIDRes);

	DECLARE_MESSAGE_MAP()
};

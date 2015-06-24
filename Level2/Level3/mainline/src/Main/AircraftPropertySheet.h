#pragma once



// CAircraftPropertySheet
class CACType;
class CAircraftPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CAircraftPropertySheet)

public:
	CAircraftPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CAircraftPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CAircraftPropertySheet();

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	void setACType(CACType* pACType);

private:
	CPropertyPage* m_PropertyPageArr[2];
};



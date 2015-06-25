#pragma once
#include "afxwin.h"

// CDlgOccupiedAssignedStandSelectIntersection dialog

#include "../InputAirside/IntersectionNode.h"
class CAirportGroundNetwork;

class CDlgOccupiedSelectIntersection : public CDialog
{
	DECLARE_DYNAMIC(CDlgOccupiedSelectIntersection)
public:
	typedef std::vector<std::pair<CString,int> > AltObjectVector;
	typedef std::vector<std::pair<CString,int> >::iterator AltObjectVectorIter;

	typedef std::map<CString, AltObjectVector> AltObjectVectorMap;
	typedef std::map<CString, AltObjectVector>::iterator AltObjectVectorMapIter;

public:
	CDlgOccupiedSelectIntersection(CAirportGroundNetwork* pAltNetwork,
		std::vector<Taxiway>&vectTaxiway, AltObjectVectorMap* pTaxiwayVectorMap,
		int intersectionID=-1, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOccupiedSelectIntersection();

	int GetIntersectionTaxiway1ID(){return m_nIntersectionTaxiway1ID;}
	int GetIntersectionTaxiway2ID(){return m_nIntersectionTaxiway2ID;}
	int GetIntersectionIndex(){return m_nIntersectionIndex;}
	CString GetIntersectionName();
	int GetIntersectionID(){return m_nIntersectionID;}
	afx_msg void OnSize(UINT nType, int cx, int cy);


// Dialog Data
	enum { IDD = IDD_DIALOG_OCCUPIEDSELECTINTERSECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CComboBox m_wndTaxiwayA;
	CComboBox m_wndTaxiwayB;
	CComboBox m_ComboIntersectionIndex;

	AltObjectVectorMap* m_pTaxiwayVectorMap;
	int m_nIntersectionTaxiway1ID;
	CString m_strIntersectionTaxiway1Name;
	int m_nIntersectionTaxiway2ID;
	CString m_strIntersectionTaxiway2Name;
	int m_nIntersectionIndex;
	int m_nIntersectionID;
	CStringList m_strListTaxiway2;
	std::vector<IntersectionNode> m_vNodeList;
	void GetTaxiway2AndIndexByTaxiway1ID(int nTaxiway1ID);
	void GetIntersectionIndexAndIDByTaxiway2ID(int nTaxiway2ID);
	bool IsInStrListTaxiway2(CString str);


	typedef struct
	{
		Taxiway* pTaxiway;
		int nNodeIndex;
		bool bJustOneIntersection;
	}TaxiwayBData;

	CAirportGroundNetwork* m_pAltNetwork;
	std::vector<Taxiway>& m_vectTaxiway;
	std::vector<TaxiwayBData*> m_vectTaxiwayData;
	CString m_strIntName;

	void updateUIState();

public:
	
	afx_msg void OnCbnSelchangeTaxiwayA();
	afx_msg void OnCbnSelchangeTaxiwayB();
	afx_msg void OnCbnSelchangeComboIntersectionindex();
protected:
	virtual void OnOK();
};

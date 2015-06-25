#pragma once

#include "Resource.h"
#include "common/ALTObjectID.h"
#include "..\MFCExControl\CoolTree.h"
#include "../MFCExControl/XTResizeDialog.h"


class ALTObject; 
class CDlgTemporaryParkingList : public CXTResizeDialog
{


	DECLARE_DYNAMIC(CDlgTemporaryParkingList)

	enum { IDD=IDD_DIALOG_OCCUPIEDASSIGNEDSTAND_ORDER };

public:
	typedef std::vector<int> ParkingList;
	typedef std::vector<std::pair<CString,int> > AltObjectVector;
	typedef std::map<CString, AltObjectVector> AltObjectVectorMap;
	typedef std::map<CString, AltObjectVector>::iterator AltObjectVectorMapIter;
	typedef std::vector<std::pair<CString,int> >::iterator AltObjectVectorIter;

	CDlgTemporaryParkingList(const UINT nID,int nPrjID,ParkingList &ParkingList, AltObjectVectorMap *temporaryParkingVectorMap,CWnd* pParent = NULL);
	virtual ~CDlgTemporaryParkingList();


	ParkingList& GetParkingList();
	void SetParkingOrderContent(void);
	void addList(ParkingList &parkingList);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAddList();
	afx_msg void OnDelParking();
	afx_msg void OnParkingUp();
	afx_msg void OnParkingDown();
	afx_msg void OnTvnSelchangedTreeParking(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);


protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL OnInitDialog();

	virtual void OnOK();


	CToolBar m_toolBarParking;
	CCoolTree m_treeParkingOrder;
	HTREEITEM m_hTreeRoot;

	ParkingList m_vParkingList;
	AltObjectVectorMap *m_TemporaryParkingVectorMap;
	int m_nPrjID;


};



#pragma once
#include "..\src\mfcexcontrol\listctrlex.h"
#include "..\InputOnboard\SeatTypeSpecification.h"
#include "..\InputOnboard\CInputOnboard.h"
// #include "..\InputOnboard\OnBoardAnalysisCondidates.h"
#include "..\InputOnboard\AircaftLayOut.h"
#include "..\InputOnboard\AircraftPlacements.h"

class CSeatTypeDefListCtrl :
	public CListCtrlEx
{
public:
	CSeatTypeDefListCtrl(void);
	~CSeatTypeDefListCtrl(void);
	void resetListContent(CFlightSeatTypeDefine *CurFlightSeatTypeDef);
	void setInputOnboard(InputOnboard* pInputOnboard){m_pInputOnboard=pInputOnboard;}
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
private:
	InputOnboard* m_pInputOnboard;
	CFlightSeatTypeDefine *m_pCurFlightSeatTypeDef;
	CString m_seatTypeName[ST_SeatTypeNum];
	CStringList m_sSeatTypeList;
};

#pragma once
#include "AirsideGUI/UnitPiece.h"
class CAircraftElmentShapeDisplay;
class CDlgShapeTranformProp :  public CXTResizeDialog,public CUnitPiece
{
	DECLARE_DYNAMIC(CDlgShapeTranformProp)
public:
	CDlgShapeTranformProp(CAircraftElmentShapeDisplay* mpDisplay,int nProjeID, CWnd* pParent = NULL);
	~CDlgShapeTranformProp(void);
	void CommitChanges();

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	afx_msg void OnApply();

	void SetContent(int nControlID, double dValue, bool bLengtUnit = false);
	void ClearContent( int nControlID );
	double RetrieveContent(int nControlID, bool LengUnit = false);

	void LoadContent(CAircraftElmentShapeDisplay*);

	CAircraftElmentShapeDisplay* mpDisplay;
	virtual bool ConvertUnitFromDBtoGUI(void){ return true; }
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void){ return true; }

	DECLARE_MESSAGE_MAP()
private:
	int m_nProjectID;
};

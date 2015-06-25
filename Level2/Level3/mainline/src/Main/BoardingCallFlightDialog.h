#pragma once
#include "flightdialog.h"

class InputTerminal;

class BoardingCallFlightDialog : public CFlightDialog
{
private:
	DECLARE_DYNAMIC(BoardingCallFlightDialog)
public:
	BoardingCallFlightDialog(CWnd* pParent,  bool bShowThroughout = false);
	~BoardingCallFlightDialog();
	DECLARE_MESSAGE_MAP()
public:
	virtual void InitFltConst(const FlightConstraint&);

protected:
	virtual void ResetSeletion();
	virtual BOOL OnInitDialog();
};


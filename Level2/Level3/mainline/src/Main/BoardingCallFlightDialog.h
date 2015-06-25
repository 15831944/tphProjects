#pragma once
#include "flightdialog.h"

typedef enum 
{
	DIALOG_MODE_BOARDINGCALL = 1,
	DIALOG_MODE_DEFAULT
} dialog_mode;

class InputTerminal;

class BoardingCallFlightDialog : public CFlightDialog
{
private:
	dialog_mode m_mode;
	DECLARE_DYNAMIC(BoardingCallFlightDialog)
public:
	BoardingCallFlightDialog(CWnd* pParent, dialog_mode mode = DIALOG_MODE_DEFAULT, bool bShowThroughout = false);
	~BoardingCallFlightDialog();
	DECLARE_MESSAGE_MAP()
public:
	virtual void InitFltConst(const FlightConstraint&);

protected:
	virtual void ResetSeletion();
	virtual BOOL OnInitDialog();
};


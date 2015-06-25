#pragma once
#include "passengertypedialog.h"
class BoardingCallPassengerTypeDlg : public CPassengerTypeDialog
{
private:
	DECLARE_DYNAMIC(BoardingCallPassengerTypeDlg)
public:
	BoardingCallPassengerTypeDlg(CWnd* pParent,BOOL _bOnlyPax = FALSE, BOOL bNoDefault = FALSE);
	~BoardingCallPassengerTypeDlg(void);
	DECLARE_MESSAGE_MAP()
public:
	virtual void InitFltConst(const CMobileElemConstraint&);

protected:
	virtual void ResetSeletion();
	virtual void LoadPassengerTypeTree();
};


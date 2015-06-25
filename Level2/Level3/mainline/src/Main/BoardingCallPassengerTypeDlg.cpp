#include "StdAfx.h"
#include "termplan.h"
#include "BoardingCallPassengerTypeDlg.h"

IMPLEMENT_DYNAMIC(BoardingCallPassengerTypeDlg, CPassengerTypeDialog)
BoardingCallPassengerTypeDlg::BoardingCallPassengerTypeDlg(CWnd* pParent,BOOL _bOnlyPax, BOOL bNoDefault)
	: CPassengerTypeDialog(pParent, _bOnlyPax, bNoDefault)
{
}


BoardingCallPassengerTypeDlg::~BoardingCallPassengerTypeDlg(void)
{
}

BEGIN_MESSAGE_MAP(BoardingCallPassengerTypeDlg,CPassengerTypeDialog)
END_MESSAGE_MAP()

void BoardingCallPassengerTypeDlg::LoadPassengerTypeTree()
{
	//
	// Passenger Type Tree
	//

	char *type_labels[] = { "ALL TYPES", "ARRIVAL", "DEPARTURE",
		"TRANSIT", "TRANSFER" };

	for (int i = 2; i < 5; i++)
	{
		HTREEITEM hItem = m_PaxTypeTree.InsertItem( type_labels[i] );
		m_PaxTypeTree.SetItemData( hItem, i );

		InsertTreeItem( hItem, 0 );    // Insert all the branches of every level.

		m_PaxTypeTree.Expand( hItem, TVE_EXPAND );   // Expand it.
	}
}

void BoardingCallPassengerTypeDlg::InitFltConst( const CMobileElemConstraint& mobElemConst)
{
	m_MobileElem = mobElemConst;
	ResetSeletion();
}

void BoardingCallPassengerTypeDlg::ResetSeletion()
{
	if (!::IsWindow(GetSafeHwnd()))
		return;
	CPassengerTypeDialog::ResetSeletion();

	m_listPassenger.EnableWindow(FALSE);
	m_listAirlineGroup.EnableWindow(FALSE);
	m_AirlineTree.EnableWindow(FALSE);
	m_butEdit.EnableWindow(FALSE);
	m_comboMobileElem.EnableWindow(FALSE);
	m_Sector.EnableWindow(FALSE);
	m_Category.EnableWindow(FALSE);
	m_Airport.EnableWindow(FALSE);
	m_ACType.EnableWindow(FALSE);
	m_butOtherFlight.EnableWindow(FALSE);

	m_wndToolBar.EnableWindow(FALSE);

	GetDlgItem(IDC_STOPOVERAIRPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_ALLTYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PAX)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_NONPAX)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_ALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_ARRIVING)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_DEPARTING)->EnableWindow(FALSE);
}

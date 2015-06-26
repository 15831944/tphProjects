// SortableHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
//#include "TermPlan.h"
#include "resource.h"
#include "SortableHeaderCtrl.h"
//#include "MainFrm.h"
#include "Filter.h"
#include "../Common/ARCTermSettingHander.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSortableHeaderCtrl

IMPLEMENT_DYNCREATE(CSortableHeaderCtrl, CHeaderCtrl)

CSortableHeaderCtrl::CSortableHeaderCtrl()
{
	m_lColumnSortStates = 0;
	m_bSortAsc = FALSE;
	m_bWithFilter = FALSE;
	m_nSortCol = -1;
	m_nLastColumn = -1;
	m_bmpArrowUp.LoadBitmap(IDB_UPARROW);
	m_bmpArrowDown.LoadBitmap(IDB_DOWNARROW);

	memset(m_aCombinedSortedColumns, -1, MAX_COLUMNS * sizeof(int));
	memset(m_arrColType, dtNULL, MAX_COLUMNS * sizeof(EDataType));
}

CSortableHeaderCtrl::~CSortableHeaderCtrl()
{
	m_bmpArrowUp.DeleteObject();
	m_bmpArrowDown.DeleteObject();
}

void CSortableHeaderCtrl::InitSortableHeaderCtrl(void)
{
	m_lColumnSortStates = 0;
	m_bSortAsc = FALSE;
	m_bWithFilter = FALSE;
	m_nSortCol = -1;
	m_nLastColumn = -1;
	
	memset(m_aCombinedSortedColumns, -1, MAX_COLUMNS * sizeof(int));
	memset(m_arrColType, dtNULL, MAX_COLUMNS * sizeof(EDataType));
}

void CSortableHeaderCtrl::ResetAll(void)
{
	InitSortableHeaderCtrl();
}
BEGIN_MESSAGE_MAP(CSortableHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSortableHeaderCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortableHeaderCtrl message handlers
/////////////////////////////////////////////////////////////////////////////


//------------ public -------------------------------------------
bool CSortableHeaderCtrl::GetItemSortState(int iItem)
{
	return ((m_lColumnSortStates & ( 1 << iItem )) > 0);
}

const int CSortableHeaderCtrl::GetLastColumn() const
{
	return m_nLastColumn;
}


void CSortableHeaderCtrl::LoadSortList()
{
	CString			sortDesc;
	UINT			BegIdx	= 0;
	UINT			EndIdx = 0;
	bool			sortState;
	bool			currState;
	int				colNumber;
	int				colIdx		= -1;
	bool			singleSort	= true;

	if( m_csRegSect.IsEmpty() )
		return;
	sortDesc = CARCTermSettingHander::GetProfileString(m_csRegSect,m_csRegEntry,"") ;
//	sortDesc = AfxGetApp()->GetProfileString( m_csRegSect, m_csRegEntry, "" );

	if( sortDesc.IsEmpty() )
	{
		return;
	}


	CWaitCursor	wCursor;
	while ( EndIdx != -1 )
	{
		if ( (EndIdx = sortDesc.Find( "|", BegIdx ) ) != -1 )
		{
			sortState = ( sortDesc.Mid( BegIdx, 1 ) == "1" ) ? true : false;
			colNumber = atoi( sortDesc.Mid( BegIdx+2, EndIdx-BegIdx-3 ) );

			if ( singleSort )
			{
				SortColumn( colNumber, false );
				singleSort = false;
			}
			else
				SortColumn( colNumber, true );

			currState = GetItemSortState( m_aCombinedSortedColumns[ ++colIdx ] );

			if (  currState != sortState )
				SortColumn( colNumber, true );

			BegIdx = EndIdx+1;
		}
	}
}


void CSortableHeaderCtrl::RemoveAllSortImages()
{
	int iCount = GetItemCount();
	for( int i = 0; i < iCount; i++ )
		RemoveSortImage( i );
}

void CSortableHeaderCtrl::RemoveSortImage( int iItem )
{
	if( iItem != -1 )
	{
		HD_ITEM hditem;	
		hditem.mask = HDI_FORMAT;
		GetItem( iItem, &hditem );
		hditem.mask = HDI_FORMAT;	
		hditem.fmt &= ~HDF_BITMAP;
		SetItem( iItem, &hditem );
	}
}

void CSortableHeaderCtrl::SetItemSortState(int iItem, bool bSortState)
{
	if(bSortState != GetItemSortState(iItem))
		m_lColumnSortStates ^= (1 << iItem);
}

void CSortableHeaderCtrl::SetRegString( CString p_csRegSect, CString p_csRegEntry )
{
	m_csRegSect = p_csRegSect;
	m_csRegEntry = p_csRegEntry;
}

int CSortableHeaderCtrl::SetSortImage( int nCol, BOOL bAsc )
{
    CBitmap *pTempBmp = NULL;
	if( bAsc )
		pTempBmp = &m_bmpArrowUp;
	else
		pTempBmp = &m_bmpArrowDown;
	
	HD_ITEM Item;	
	Item.mask = HDI_FORMAT;
	GetItem( nCol, &Item );
	Item.mask = HDI_BITMAP | HDI_FORMAT;	
	Item.fmt |= HDF_BITMAP;
	Item.hbm = (HBITMAP)pTempBmp->GetSafeHandle();
	SetItem( nCol, &Item );
	m_nLastColumn = nCol;
    return 1;
}



//------------------ protected -----------------------------------------------
int CALLBACK CSortableHeaderCtrl::Compare(LPARAM p_lParam1, LPARAM p_lParam2, LPARAM p_lParamThis)
{
   CSortableHeaderCtrl* pHeaderCtrl = (CSortableHeaderCtrl*)p_lParamThis;
   return pHeaderCtrl->CompareItem(p_lParam1, p_lParam2);
}

int CSortableHeaderCtrl::CompareItem( LPARAM p_lParam1, LPARAM p_lParam2 )
{
   CListCtrl*	pListCtrl;
   LVFINDINFO	lvf;
	int			nIndex1;
	int			nIndex2;
	CString		csText1;
	CString		csText2;
	int			nSortedCol;
	int			nOrder;
	int			nNumSortedCols;

	pListCtrl = (CListCtrl*)GetParent();
	lvf.flags = LVFI_PARAM;
	lvf.lParam = p_lParam1;
	nIndex1 = pListCtrl->FindItem(&lvf, -1);
	lvf.lParam = p_lParam2;
	nIndex2 = pListCtrl->FindItem(&lvf, -1);
	if( nIndex1 < 0 || nIndex2 < 0 || nIndex1 == nIndex2 )
		return 0;

	// Now that we are setup, compare the items
	nNumSortedCols = GetNumCombinedSortedColumns();
	for( int i = 0; i < nNumSortedCols; i++ )
	{
		nSortedCol = m_aCombinedSortedColumns[i];
		nOrder = ( GetItemSortState(nSortedCol) ) ? 1 : -1;
		csText1 = pListCtrl->GetItemText( nIndex1, nSortedCol);
		csText2 = pListCtrl->GetItemText( nIndex2, nSortedCol);
		try{
			switch( m_arrColType[nSortedCol] )
			{
				case  dtINT:
				{
					int nValue1 = (int)atof( csText1 );
					int nValue2 = (int)atof( csText2 );
					if( nValue1 != nValue2)
						return ( nValue1 < nValue2 ? -1 : 1 ) * nOrder;
				}
				break;

				case  dtDEC:
				{
					float fValue1 = (float)atof( csText1 );
					float fValue2 = (float)atof( csText2 );
					if( fValue1 != fValue2 )
						return ( fValue1 < fValue2 ? -1 : 1 ) * nOrder;
				}
				break;

				case  dtDATETIME:
				{
					COleDateTime oleDT1, oleDT2;
					if( oleDT1.ParseDateTime( csText1 ) && oleDT2.ParseDateTime( csText2 ) )
						return ( oleDT1 < oleDT2 ? -1 : 1 ) * nOrder;
					else if( csText1.IsEmpty() && !csText2.IsEmpty() )
						return ( -1 * nOrder );
					else if( !csText1.IsEmpty() && csText2.IsEmpty() )
						return ( 1 + nOrder );
				}
				break;
				
				case dtTIME:
				{
					int nData1 = 0;
					int nPosH1 = csText1.Find(':');
					CString strHour1 = csText1.Left(nPosH1);
					nData1 += atoi(strHour1) * 3600;
					int nPosM1 = csText1.Find(':',nPosH1+1);
					CString strMin1;
					if (nPosM1 != -1)
						strMin1 = csText1.Mid(nPosH1+1, nPosM1-nPosH1-1);
					else
						strMin1 = csText1.Right(csText1.GetLength()-nPosH1-1);
					nData1 += atoi(strMin1) * 60;
					if (nPosM1 != -1)
					{
						CString strSec1 = csText1.Right(csText1.GetLength()-nPosM1-1);
						nData1 += atoi(strSec1);
					}
					int nSign1 = 1;
					if (csText1.Find('-') != -1)
						nSign1 = -1;
					nData1 *= nSign1;
					
					int nData2 = 0;
					int nPosH2 = csText2.Find(':');
					CString strHour2 = csText2.Left(nPosH2);
					nData2 += atoi(strHour2) * 3600;
					int nPosM2 = csText2.Find(':',nPosH2+1);
					CString strMin2;
					if (nPosM2 != -1)
						strMin2 = csText2.Mid(nPosH2+1, nPosM2-nPosH2-1);
					else
						strMin2 = csText2.Right(csText2.GetLength()-nPosH2-1);
					nData2 += atoi(strMin2) * 60;
					if (nPosM2 != -1)
					{
						CString strSec2 = csText2.Right(csText2.GetLength()-nPosM2-1);
						nData2 += atoi(strSec2);
					}
					int nSign2 = 1;
					if (csText2.Find('-') != -1)
						nSign2 = -1;
					nData2 *= nSign2;

					if (nData1 != nData2)
					{
						return ( nData1 < nData2 ? -1 : 1 ) * nOrder;
					}
				}
				break;

				case  dtSTRING:
				{
					int nValue = csText1.CompareNoCase( csText2 );
					if( nValue )
						return ( nValue * nOrder );
				}
				break;

				default:
					ASSERT("Error: attempt to sort a column without type.");
					return 0;
			}
		}
		catch(...)
		{
			return 0;
		}
	}
	return 0;
}


int CSortableHeaderCtrl::FindItemInCombinedSortedList( int p_nItem )
{
	int nNumCombinedSortedColumns = GetNumCombinedSortedColumns();
	for( int i = 0; i < nNumCombinedSortedColumns; i++ )
		if( m_aCombinedSortedColumns[i] == p_nItem )
			return i;
	return -1;
}

int CSortableHeaderCtrl::GetNumCombinedSortedColumns()
{
	for( int i = 0; i < MAX_COLUMNS; i++ )
		if( m_aCombinedSortedColumns[i] == -1 )
			return i;
	return MAX_COLUMNS;
}


int CSortableHeaderCtrl::HitText( CPoint p_point )
{
	int nSizeCol = GetItemCount();
	for( int i=0; i<nSizeCol; i++ )
	{
		CRect rect;
		GetItemRect( i, rect );
		if( rect.PtInRect( p_point ) )
			return i;
	}
	return -1;
}



void CSortableHeaderCtrl::MoveItemInCombinedSortedListToEnd( int p_nItem )
{
	int nNumCombinedSortedColumns;
	int aCombinedSortedColumns[MAX_COLUMNS];
	int nItemIndex;

	
	nNumCombinedSortedColumns = GetNumCombinedSortedColumns();
	memset( aCombinedSortedColumns, -1, MAX_COLUMNS * sizeof(int) );
	nItemIndex = FindItemInCombinedSortedList( p_nItem );
	if( nItemIndex != -1 )
	{
		if( nItemIndex > 0 )
		{
			memcpy( aCombinedSortedColumns, m_aCombinedSortedColumns, nItemIndex * sizeof( int ) );
			memcpy( &aCombinedSortedColumns[nItemIndex], &m_aCombinedSortedColumns[nItemIndex + 1], ( nNumCombinedSortedColumns - nItemIndex - 1) * sizeof(int) );
		}
	}
	aCombinedSortedColumns[ nNumCombinedSortedColumns - 1 ] = p_nItem;
	memcpy( m_aCombinedSortedColumns, aCombinedSortedColumns, MAX_COLUMNS * sizeof(int) );
}

bool CSortableHeaderCtrl::NotInCombinedSortedColumnList( int p_nItem )
{
	int nNumCombinedSortedColumns = GetNumCombinedSortedColumns();
	for( int i = 0; i < nNumCombinedSortedColumns; i++ )
		if( m_aCombinedSortedColumns[i] == p_nItem )
			return false;
	return true;
}

// save to register
void CSortableHeaderCtrl::SaveSortList()
{
	int					colNumber;
	bool				sortState;
	int					colQnt;
	int					i;
	CString				cs;
	CString				sortDesc;


	colQnt = GetNumCombinedSortedColumns();

	sortDesc = "";
	for ( i = 0; i < colQnt; i++ )
	{
		colNumber = m_aCombinedSortedColumns[i];
		sortState = GetItemSortState(m_aCombinedSortedColumns[i]);

		cs.Format( "%d-%d|", sortState ,colNumber );
		sortDesc += cs;
	}

	if( m_csRegSect.IsEmpty() )
		return;
	CARCTermSettingHander::WriteProfileString(m_csRegSect,m_csRegEntry,sortDesc) ;
	//AfxGetApp()->WriteProfileString( m_csRegSect, m_csRegEntry,	sortDesc );
}



void CSortableHeaderCtrl::SortColumn( int p_nSubItem, bool p_bSortingMultipleColumns/* = false */)
{	
	CListCtrl* pListCtrl;
	int nNumCombinedSortedCols;

	pListCtrl = (CListCtrl*)GetParent();
	nNumCombinedSortedCols = GetNumCombinedSortedColumns();
	if( p_bSortingMultipleColumns )
	{
		if( NotInCombinedSortedColumnList( p_nSubItem ) )
			m_aCombinedSortedColumns[nNumCombinedSortedCols++] = p_nSubItem;
		else
			MoveItemInCombinedSortedListToEnd( p_nSubItem );
		for(int i = nNumCombinedSortedCols - 1; i >= 0 ; i--)
		{
			bool bAsc = GetItemSortState( m_aCombinedSortedColumns[i] );
			if( nNumCombinedSortedCols - 1 != i )
				bAsc = !bAsc;
			
         if( i == nNumCombinedSortedCols - 1 )
         {	//Only swap the last column's sort order.
				SetSortImage(m_aCombinedSortedColumns[i], bAsc);
				SetItemSortState(m_aCombinedSortedColumns[i], !bAsc);			
			}
		}
		pListCtrl->SortItems( Compare, LPARAM(this) );      
	}
	else
	{
      RemoveAllSortImages();
      memset( m_aCombinedSortedColumns, -1, MAX_COLUMNS * sizeof(int) );
      m_aCombinedSortedColumns[0] = p_nSubItem;
      bool bAsc = GetItemSortState( p_nSubItem );
		SetSortImage( p_nSubItem, bAsc );
		SetItemSortState( p_nSubItem, !bAsc );
      pListCtrl->SortItems( Compare, LPARAM(this) );      
	}
}

void CSortableHeaderCtrl::SetDataType( int iItem, EDataType Type )
{
	m_arrColType[iItem] = Type;
}

void CSortableHeaderCtrl::SetWithFilter( bool p_bWithFilter/*=true */ )
{
	m_bWithFilter = p_bWithFilter;
}



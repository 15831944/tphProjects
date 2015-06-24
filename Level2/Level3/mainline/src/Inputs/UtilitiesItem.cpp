// UtilitiesItem.cpp: implementation of the CUtilitiesItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UtilitiesItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtilitiesItem::CUtilitiesItem()
{
	m_strComponet.Empty();
	m_fFixedCost = UTILITIES_ITEM_ERROEDATA;
	m_fLowCost = UTILITIES_ITEM_ERROEDATA;
	m_fLowHrs = UTILITIES_ITEM_ERROEDATA;
	m_fMedCost = UTILITIES_ITEM_ERROEDATA;
	m_fMedHrs = UTILITIES_ITEM_ERROEDATA;
	m_fHighCost = UTILITIES_ITEM_ERROEDATA;
	m_fHighHrs = UTILITIES_ITEM_ERROEDATA;
}

CUtilitiesItem::~CUtilitiesItem()
{

}

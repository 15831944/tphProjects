#include "StdAfx.h"
#include "carrierSpace.h"

carrierSpace::carrierSpace(LPCTSTR lpszACType)
:m_strACType(lpszACType)
{
}

carrierSpace::~carrierSpace(void)
{
	m_listDeck.Flush(TShouldDelete::Delete);
}
 
bool carrierSpace::isValid()
{
	// has door in deck
	return true; 
}

// set association with string, BAD IDEA!!
int carrierSpace::fits( LPCTSTR lpszACType)
{
	return (m_strACType.CompareNoCase( lpszACType ) == 0 );
}

void carrierSpace::addCarrierDeck( carrierDeck* pNewCarrierDeck)
{
	ASSERT( pNewCarrierDeck != NULL );
	m_listDeck.addItem( pNewCarrierDeck );
}

carrierDeck* carrierSpace::getCarrierDeck(size_t nLevel)
{
	if(nLevel < 0 || nLevel > m_listDeck.size())
		return 0;
	return m_listDeck.at( nLevel );
}

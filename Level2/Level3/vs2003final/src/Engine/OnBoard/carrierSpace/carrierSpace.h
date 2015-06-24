#pragma once
#include "carrierDeck.h"

class carrierDoor;
class carrierSpace
{
public:
	typedef UnsortedContainer< carrierDeck > DeckList;

public:
	carrierSpace(LPCTSTR lpszACType);
	~carrierSpace(void);
	int fits( LPCTSTR lpszACType);

	bool isValid();

public:
	void addCarrierDeck( carrierDeck* pNewCarrierDeck);
	carrierDeck* getCarrierDeck(size_t nLevel);

private:
	//data members.
	DeckList	m_listDeck;
	
private:
	CString m_strACType;

};






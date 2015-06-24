#pragma once

class CCongestionAreaCell
{
public:
	CCongestionAreaCell(void);
	~CCongestionAreaCell(void);

	long GetPersonsCount() const { return m_PersonsCount; }
protected:
	long m_PersonsCount; //The persons count.
public:	
	void EnterCell(int nPaxCount ) { m_PersonsCount+=nPaxCount ; }	//Pus	h a perseon;
	void LeaveCell(int nPaxCount );	//pop a person;
};

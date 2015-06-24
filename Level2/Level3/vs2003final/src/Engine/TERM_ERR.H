#ifndef ARCTERM_EXCEPTIONS_H
#define ARCTERM_EXCEPTIONS_H

#include "common\exeption.h"

class BaggageProcessor;
class Person;

class NoFlowError : public ARCError
{
protected:
    Person *aPerson;
    char *tempString;

public:
    NoFlowError (Person *p_person) : ARCError (NULL) { init (p_person); }
    virtual ~NoFlowError () { delete [] tempString; }

    virtual void init (Person *p_person);
    Person *getPerson (void) { return aPerson; }
};

class NoValidFlowError : public NoFlowError
{
public:
    NoValidFlowError (Person *p_person) : NoFlowError (p_person) {};
    virtual ~NoValidFlowError () {};
    virtual void init (Person *p_person);
};


class OneToOneFlowError : public NoValidFlowError
{
public:
    OneToOneFlowError (Person *p_person) : NoValidFlowError (p_person) {};
    virtual ~OneToOneFlowError () {};
    virtual void init (Person *p_person);
};
class ExceedMaxWaitTimeOfBagDevice : public NoFlowError
{
	const BaggageProcessor* m_pBagProc;
public:
	ExceedMaxWaitTimeOfBagDevice( Person* _pPerson ,const BaggageProcessor* _pBagProc)
		:NoFlowError ( _pPerson),m_pBagProc( _pBagProc ) {}
	virtual ~ExceedMaxWaitTimeOfBagDevice () {};
    virtual void init (Person *p_person);
	virtual void BuildErrorMsg();
};
#endif

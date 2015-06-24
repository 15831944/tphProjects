#include "stdafx.h"
#include <string.h>
#include "term_err.h"
#include "person.h"
#include "process.h"
#include <CommonData/procid.h>
#include "inputs\pax_cnst.h"
#include "baggage.h"
#include "inputs\flight.h"
#include "TerminalMobElementBehavior.h"

void NoFlowError::init (Person *p_person)
{
    errorName = ": Passenger ";
    aPerson = p_person;

    char IDstr[512];
    // (p_person->getType()).screenPrint (str);  MATT
   	CString pStr;
	(p_person->getType()).screenPrint (pStr,0,512);
    pStr +="\n    stranded without flow at ";
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior *)p_person->getBehavior(MobElementBehavior::TerminalBehavior);
    ((spTerminalBehavior->getProcessor())->getID())->printID (IDstr);
    pStr += IDstr;
    tempString = new char[pStr.GetLength()+1]; //[strlen (str) + 1];
    strcpy (tempString, pStr.GetBuffer(pStr.GetLength()));
    message = tempString;
}

void NoValidFlowError::init (Person *p_person)
{
    errorName = ": No destination available for passenger\n    ";
    aPerson = p_person;

    char str[512];
	CString sScreen;
//    (p_person->getType()).screenPrint(str); MATT
	 (p_person->getType()).screenPrint(sScreen,0,512);
    sScreen += " from ";
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior *)p_person->getBehavior(MobElementBehavior::TerminalBehavior);
    ((spTerminalBehavior->getProcessor())->getID())->printID (str);
    sScreen += str;
	CString s;
    tempString = new char [sScreen.GetLength()+1];
    strcpy (tempString, sScreen.GetBuffer(sScreen.GetLength()));
    message = tempString;
}

void OneToOneFlowError::init (Person *p_person)
{
    errorName = ": No One To One destination available for passenger\n    ";
    aPerson = p_person;

    char str[512];
	CString sScreenBuf;
   
    (p_person->getType()).screenPrint(sScreenBuf,0,512); //MATT
    sScreenBuf += " from ";
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior *)p_person->getBehavior(MobElementBehavior::TerminalBehavior);
    ((spTerminalBehavior->getProcessor())->getID())->printID (str);
	sScreenBuf += str;

    tempString = new char [sScreenBuf.GetLength()+1];
    strcpy (tempString, sScreenBuf.GetBuffer(sScreenBuf.GetLength()));
    message = tempString;
}

void ExceedMaxWaitTimeOfBagDevice::init (Person *p_person)
{
    errorName = ": Exceed Max Waiting Time Of Baggage Processor. \n  ";
    aPerson = p_person;

    char str[512];
	CString sScreeBuf;
    (p_person->getType()).screenPrint(sScreeBuf,0,512); //MATT
    sScreeBuf+=" From ";
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior *)p_person->getBehavior(MobElementBehavior::TerminalBehavior);
    ((spTerminalBehavior->getProcessor())->getID())->printID (str);
    sScreeBuf +=str;
	sScreeBuf+=" At Baggage Processor ";
	sScreeBuf+= m_pBagProc->getID()->GetIDString() ;

    tempString = new char [sScreeBuf.GetLength()+1];
    strcpy (tempString, sScreeBuf.GetBuffer(sScreeBuf.GetLength()+1));
    message = tempString;
}
void ExceedMaxWaitTimeOfBagDevice::BuildErrorMsg()
{
	errorName = ": Exceed Max Waiting Time Of Baggage Processor.\n";

	CString strPrint;
    (aPerson->getType()).screenPrint(strPrint,0,512); //MATT
	CString strTemp(" Element Type :");
	strTemp += strPrint;
	CString sID;
	sID.Format(" Element ID : %d ", aPerson->getID() );

	strTemp += sID;
	strTemp += " Baggage Processor : ";
	strTemp += m_pBagProc->getID()->GetIDString();
 
	if( tempString )
		delete []tempString;

    tempString = new char [strTemp.GetLength() + 1];
    strcpy (tempString, strTemp);
    message = tempString;
}


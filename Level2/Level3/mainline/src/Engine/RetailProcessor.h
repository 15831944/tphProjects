#pragma once
#include "lineproc.h"
#include "..\Common\pollygon.h"
#include "..\Common\point.h"
#include "..\Results\RetailLogEntry.h"
#include "..\Economic\ProcEconomicData.h"

class ArctermFile;
class MiscRetailProcData;
class MiscData;
class Person;
class CRetailFeeData;

class RetailProcessor :
	public LineProcessor
{
public:
	enum RetaiType{
		Retial_Shop_Section,
		Retial_By_Pass,
		Retial_Check_Out
	};
	RetailProcessor(void);
	virtual ~RetailProcessor(void);

	//-----------------------------------------------------------------------
	//Summary:
	//		retrieve processor type
	//------------------------------------------------------------------------
	virtual int getProcessorType (void) const { return RetailProc; }
	//-----------------------------------------------------------------------
	//Summary:
	//		Returns processor name.
	//----------------------------------------------------------------------
	virtual const char *getProcessorName (void) const { return "RETAIL"; }
	//----------------------------------------------------------------------

	//---------------------------------------------------------------------
	//Summary:
	//		check processor is valid passenger
	//---------------------------------------------------------------------
	virtual bool isPaxSkipVacant(Person* pPerson, const ElapsedTime& _curTime);
	virtual bool isQueueLengthVacant(Person* pPerson)const;
	//---------------------------------------------------------------------
	//Summary:
	//		Create current type processor
	//---------------------------------------------------------------------
	virtual Processor* CreateNewProc(){return new RetailProcessor();}

	//---------------------------------------------------------------------
	//Summary:
	//		Read/write some specific field
	//--------------------------------------------------------------------
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	virtual void initSpecificMisc(const MiscData *miscData);
	//-------------------------------------------------------------------
	//Summary:
	//		init store area pollygon
	//-------------------------------------------------------------------
	void initStoreArea (int pathCount, const Point *pointList);
	const Pollygon& GetStoreArea()const {return m_storeArea;}

	//--------------------------------------------------------------------
	//Summary:
	//		retail type set/get
	//-------------------------------------------------------------------
	RetaiType GetRetailType()const {return m_emType;}
	void SetRetailType(RetaiType emType){m_emType = emType;}

	//-------------------------------------------------------------------
	//Summary:
	//		init retail processor log entry
	//-------------------------------------------------------------------
	void SetLogEntryOfRetail(const CRetailLogEntry& logEntry);

	//-------------------------------------------------------------------
	//Summary:
	//		It is called by Person::processServerArrival
	//		The different processor types serve clients in different ways.
	//-------------------------------------------------------------------
	virtual void beginService (Person *aPerson, ElapsedTime curTime);

	//-------------------------------------------------------------------
	//Summary:
	//		init passenger shopping at shop processor
	//------------------------------------------------------------------
	void InitShoppingRule();

	//------------------------------------------------------------------
	//Summary:
	//		init shop section stocking
	//------------------------------------------------------------------
	void InitShopStocking();

	void FlushLog();

	void WriteLogCheckOutShopping(Person* aPerson,int nType,int nCount,RetailProcessor* pRetailProc,const ElapsedTime& curTime);
	void WriteLogCancelShopping(Person* aPerson,int nType,int nCount,RetailProcessor* pRetailProc,const ElapsedTime& curTime);

	CRetailLogEntry& GetRetailLogEntry();
private:
	//-------------------------------------------------------------------
	//Summary:
	//		write log for report 
	//-------------------------------------------------------------------
	void ShoppingProcess(Person *aPerson, ElapsedTime curTime);
	void ShoppingItemProcess(Person *aPerson, ElapsedTime curTime,const RetailFeeItem& feeData);
	void ByPassProcess(Person* aPerson,ElapsedTime curTime);
	void SkipProcess(Person *aPerson, ElapsedTime curTime);
	bool GetRetailFeeData( int nProductID,RetailFeeItem& feeData ) const;
private:
	Pollygon	m_storeArea;
	RetaiType	m_emType;
	MiscRetailProcData*  m_pRetailMisData;

	CRetailLogEntry m_retailLogEntry;

	std::map<int,int> m_productMap; //key: product id; value: cout of product
	CRetailFeeData*		m_pRetailFeeData;//passenger shopping rule

	
};

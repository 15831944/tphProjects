#pragma once
#include "Common/elaptime.h"
#include <typeinfo>
#include <string>

typedef std::string String;

class SignalHandler;
class SignalSource;
//signal
class SSignal
{
public:
	SSignal():mpSource(NULL){}
	virtual ~SSignal(){}
	virtual SignalSource* getSource()const{ return mpSource; }  //return the source of the agent 
	void setSource(SignalSource* pSrc){ mpSource = pSrc; }
	static ElapsedTime curTime();

	
	virtual CString getDesc()=0;
protected:
	SignalSource* mpSource;
};

class SNormalSignal : public SSignal
{
public:
	SNormalSignal(){}
	SNormalSignal(const CString& sDesc):m_sDesc(sDesc){}
	virtual CString getDesc(){ return m_sDesc; }
protected:
	CString m_sDesc;
};

typedef SSignal* SSignalPtr;

//class SMessage : public SSignal
//{
//public:
//	SMessage(const CString& str):mStrMsg(str){ }
//	CString mStrMsg;
//};

#ifndef s_sigslot
#define s_sigslot
#endif



typedef s_sigslot bool (SignalHandler::*SA_PMSG)(SSignal*);

struct SMsgMap_Entry
{
	const type_info& msg_type;
	SA_PMSG pmfn;
};
struct SMsgMap
{
	const SMsgMap* pBaseMap;
	const SMsgMap_Entry* lpEntries;
};

#define S_DECLARE_SIGNALMAP \
private: \
	static const SMsgMap_Entry _messageEntries[]; \
protected: \
	static const SMsgMap messageMap; \
	static const SMsgMap* GetThisMessageMap(); \
	virtual const SMsgMap* GetMessageMap() const; \


#define S_BEGIN_SIGNALMAP(theClass, baseClass) \
	const SMsgMap* theClass::GetThisMessageMap() \
{ return &theClass::messageMap; } \
	const SMsgMap* theClass::GetMessageMap() const \
{ return &theClass::messageMap; } \
	const SMsgMap theClass::messageMap = \
{ &baseClass::messageMap, &theClass::_messageEntries[0] }; \
	const SMsgMap_Entry theClass::_messageEntries[] = \
{ \


#define S_ON_SIGNAL(message, memberFxn) \
{ typeid(message),static_cast<SA_PMSG>(memberFxn) },


#define S_END_SIGNALMAP \
{ typeid(SSignal), (SA_PMSG)0 } \
}; \


//class SignalGlobalBroadcastor;
class SignalHandler
{
	S_DECLARE_SIGNALMAP
	friend class SignalSource;
public:
	bool SignalProc(SSignal*pmsg);

	void ListenTo(SignalSource *pSrc);
	void UnListenTo(SignalSource *pSrc);
	//void ListenToGlobal(bool b = true);
protected:
	bool _HandleMappedSignal(SSignal*pmsg);
	virtual bool DefaultHandleSignal(SSignal*pmsg){ return false; }	
};
typedef  std::vector<SignalHandler*> SignalHandlerList;


//signal sender
class SignalSource
{
public:

	template<class _Iter>
		void SendSignal(SSignalPtr& msg, _Iter from , _Iter end)
	{
		SignalHandlerList slist(from, end);
		SendSignal(msg,slist);
	}

	template<class _Iter>
		void SendSignal(SSignalPtr msg, const ElapsedTime& nextT,_Iter from , _Iter end)
	{
		SignalHandlerList slist(from, end);
		SendSignal(msg,nextT,slist);
	}
	//send to 
	void SendSignal(SSignalPtr msg, const ElapsedTime& nextT, SignalHandler* phandler);

	///*send signal immediately  but will delete the msg after send*/
	void SendSignal(SSignalPtr msg, const SignalHandlerList& extraDest = SignalHandlerList() );
	//send signal at the specified time
	void SendSignal(SSignalPtr msg, const ElapsedTime& nextT, const SignalHandlerList& extraDest = SignalHandlerList() ); 


	///*send signal immediately  but will not delete the msg after send*/
	void SendSignalNoDelMsg(SSignalPtr msg, const SignalHandlerList& extraDest = SignalHandlerList() ); 


	//clear all in sending signals of this source
	void ClearSignal(); //clear in sending signals of this 

	
	void RemoveListener( SignalHandler* pHandler );
	void ClearListener();
	void AddListener(SignalHandler* pHandler);	
	bool HaveListeners()const { return !m_sigListener.empty(); }
protected:
	SignalHandlerList m_sigListener;
};

//class SignalGlobalBroadcastor : public SignalSource
//{
//public:	
//	static SignalGlobalBroadcastor& getInstance();
//	virtual bool HandleSignal(SSignal*pmsg);	
//};
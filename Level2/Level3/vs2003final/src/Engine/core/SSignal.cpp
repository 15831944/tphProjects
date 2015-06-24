#include "StdAfx.h"
#include ".\SSignal.h"
#include <algorithm>
#include "../event.h"

const SMsgMap_Entry* _FindEntry(const SMsgMap_Entry* lpEntries, const type_info& msgtype)
{

	while(lpEntries[0].msg_type!=typeid(SSignal))
	{
		if(lpEntries[0].msg_type==msgtype)
		{
			return lpEntries;
		}
		lpEntries++;
	}
	return 0;
}
S_BEGIN_SIGNALMAP(SignalHandler,SignalHandler)
S_END_SIGNALMAP

bool SignalHandler::_HandleMappedSignal(SSignal*pmsg)
{
	const SMsgMap_Entry* lpEntry = 0;
	//find msg entries
	for (const SMsgMap* pMessageMap = GetMessageMap();  pMessageMap!=NULL && pMessageMap->pBaseMap != pMessageMap;
		pMessageMap = pMessageMap->pBaseMap )
	{
		lpEntry = _FindEntry(pMessageMap->lpEntries,typeid(*pmsg) );
		if(lpEntry)
			break;
	}

	if(lpEntry)
	{
		SA_PMSG pmfn = lpEntry->pmfn;
		if(pmfn)
		{
			return ((*this).*pmfn)(pmsg);			
		}
	}

	return false;
}

bool SignalHandler::SignalProc( SSignal*pMsg )
{
	if(_HandleMappedSignal(pMsg))
		return true;

	return DefaultHandleSignal(pMsg);
}


//void SignalHandler::ListenToGlobal( bool b /*= true*/ )
//{
//	if(b)
//		SignalGlobalBroadcastor::getInstance().AddListener(this);
//	else
//		SignalGlobalBroadcastor::getInstance().RemoveListener(this);
//}

void SignalHandler::ListenTo( SignalSource *pSrc )
{
	if(pSrc)
		pSrc->AddListener(this);
}

void SignalHandler::UnListenTo( SignalSource *pSrc )
{
	if(pSrc)
		pSrc->RemoveListener(this);
}
//

//send out signal event
class SendSignalEvent : public Event
{
public:
	SendSignalEvent(SSignal* pSig, const ElapsedTime& t,const SignalHandlerList& extraDest)
		:m_pSig(pSig)
		,m_extraDest(extraDest)
	{
		setTime(t);
	}

	int process(CARCportEngine* pEngine )
	{
		SignalSource* pAgent = m_pSig->getSource();
		pAgent->SendSignal(m_pSig,m_extraDest);
		return 1;
	}

	SignalSource* getSender()
	{
		if(m_pSig)
			return m_pSig->getSource();
		return NULL;
	}

	virtual int getEventType (void) const 
	{
		return SendOutSignalEvent;
	}
	virtual const char *getTypeName (void) const
	{
		if(m_pSig){
			return m_pSig->getDesc();
		}
		return _T("Sending Message");
	}

	virtual int kill(){ return 1; }

	static void delSourceEvents(SignalSource* pAgent)
	{
		int nCount = m_pEventList->getCount();
		for( int i=nCount-1; i>=0; --i )
		{
			Event* pEventInList = m_pEventList->getItem( i );

			if( pEventInList->getEventType() == SendOutSignalEvent)
			{
				SendSignalEvent *pMoveEvent = (SendSignalEvent*)pEventInList;
				if( pMoveEvent->getSender() == pAgent)
				{
					m_pEventList->deleteItem( i );					
				}
			}
		}
	}
	static ElapsedTime curTime()
	{
		return m_pEventList->GetCurTime();
	}
protected:
	SSignal* m_pSig;
	SignalHandlerList m_extraDest;
	//CString m_strDesc;

};



void SignalSource::SendSignal( SSignalPtr msg,const SignalHandlerList& extraDest ) /*send signal immediately */
{
	SendSignalNoDelMsg(msg,extraDest);	
	delete msg;
}

void SignalSource::SendSignal( SSignalPtr msg, const ElapsedTime& nextT ,const SignalHandlerList& extraDest)
{
	msg->setSource(this);
//	ASSERT(nextT>=curTime()); //you can not send signal to past
	SendSignalEvent* pEvt = new SendSignalEvent(msg,nextT,extraDest);
	pEvt->addEvent();
}

void SignalSource::SendSignal( SSignalPtr msg, const ElapsedTime& nextT, SignalHandler* phandler )
{
	SignalHandlerList list;
	list.push_back(phandler);
	SendSignal(msg,nextT,list);
}

void SignalSource::RemoveListener( SignalHandler* pAgent )
{
	SignalHandlerList::iterator itr = std::find(m_sigListener.begin(),m_sigListener.end(),pAgent);
	if(itr!=m_sigListener.end())
		m_sigListener.erase(itr);
}

void SignalSource::ClearListener()
{
	m_sigListener.clear();
}

void SignalSource::ClearSignal()
{
	SendSignalEvent::delSourceEvents(this);
}

void SignalSource::AddListener( SignalHandler* pHander )
{
	SignalHandlerList::iterator itr = std::find(m_sigListener.begin(),m_sigListener.end(),pHander);
	if(itr==m_sigListener.end())
		m_sigListener.push_back(pHander);

}

void SignalSource::SendSignalNoDelMsg( SSignalPtr msg, const SignalHandlerList& extraDest /*= SignalHandlerList() */ )
{
	msg->setSource(this);
	SignalHandlerList dest = m_sigListener;
	for(SignalHandlerList::const_iterator itr=extraDest.begin();itr!=extraDest.end();++itr)
	{
		SignalHandler* pInsert = *itr;
		if(dest.end() == std::find(dest.begin(),dest.end(),pInsert))
			dest.push_back(pInsert);
	}

	//SignalGlobalBroadcastor::getInstance().HandleSignal(msg);
	//for_each(dest.begin(),dest.end(),_OnSingal(msg) );
	for(SignalHandlerList::iterator itr=dest.begin();itr!=dest.end();++itr){
		(*itr)->SignalProc(msg); 
	}
}


ElapsedTime SSignal::curTime()
{
	return SendSignalEvent::curTime();
}

////bool SignalGlobalBroadcastor::HandleSignal( SSignal*msg )
////{	
////	SignalHandlerList dest = m_sigListener;	
////	for_each(dest.begin(),dest.end(),_OnSingal(msg) );
////	return true;
////}

//SignalGlobalBroadcastor& SignalGlobalBroadcastor::getInstance()
//{
//	static SignalGlobalBroadcastor inst;
//	return inst;
//}
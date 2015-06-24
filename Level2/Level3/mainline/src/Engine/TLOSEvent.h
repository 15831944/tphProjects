// TLOSEvent.h: interface for the TLOSEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLOSEVENT_H__DA190103_21CF_4A84_B415_82AE08BAF2AE__INCLUDED_)
#define AFX_TLOSEVENT_H__DA190103_21CF_4A84_B415_82AE08BAF2AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "terminalevent.h"
class TLOSEvent  :public TerminalEvent
{
private:
	const ProcessorID* m_pProcID;
	CString m_sPaxType;
public:
	TLOSEvent();
	virtual ~TLOSEvent();
public:
	void SetRelatedProcID( const ProcessorID* _pProcID ){ m_pProcID = _pProcID;	}
	void SetPaxTypeString( const CString& _csPaxType ){  m_sPaxType = _csPaxType;	}

	virtual int process ( CARCportEngine *_pEngine );
	

    //For shut down event
    virtual int kill (void){ return 1;};

    //It returns event's name
    virtual const char *getTypeName (void) const{ return "TLOS Event"; };

    //It returns event type
    virtual int getEventType (void) const { return TLOS;}
};

#endif // !defined(AFX_TLOSEVENT_H__DA190103_21CF_4A84_B415_82AE08BAF2AE__INCLUDED_)

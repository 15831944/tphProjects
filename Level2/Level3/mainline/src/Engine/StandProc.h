// StandProc.h: interface for the StandProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STANDPROC_H__081E4F86_73D3_427D_BE5B_2621EA07F084__INCLUDED_)
#define AFX_STANDPROC_H__081E4F86_73D3_427D_BE5B_2621EA07F084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AirfieldProcessor.h"
#include "../Common/path.h"
#include "../Common/ARCVector.h"
#include "../Main/Processor2.h"

class StandProc : public AirfieldProcessor  
{
public:
	StandProc();
	virtual ~StandProc();
	//processor type
	virtual int getProcessorType (void) const { return StandProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return "STAND"; }
	//processor i/o
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;
	//auto snap this processor
	virtual void autoSnap(ProcessorList *procList);
	//snap this proc to some processor:taxiway
	void SnapTo(Processor * proc);
	void setPushBackWay(const Path * path_);
	void setPushBackWay(const std::vector<ARCVector3> & _data );
	Path*  getPushBackWay();

	virtual void Rotate( DistanceUnit _degree );
	
	BOOL IsUsePushBack(){return m_busePushBack;}
	void UsePushBack(BOOL _b=TRUE ){m_busePushBack=_b;}

	virtual Processor* CreateNewProc();
	virtual bool CopyOtherData(Processor* _pDestProc);
protected:
	Path m_pushback;
	Path m_pushbackOffset;//offset to the 
	BOOL m_busePushBack;
};

#endif // !defined(AFX_STANDPROC_H__081E4F86_73D3_427D_BE5B_2621EA07F084__INCLUDED_)

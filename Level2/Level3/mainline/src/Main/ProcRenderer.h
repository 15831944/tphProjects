#ifndef __PROCESSOR_RENDERER
#define __PROCESSOR_RENDERER
#include <map>

#pragma once

class Processor;
class CProcessor2;
class C3DView;
class ProcessorRenderer
{
protected:
	ProcessorRenderer();
public:
	enum SelectMode{ SelectMain =0 ,SelectCtrlPoints,};
	virtual bool Selectable(){ return false; }
	void SetSelectMode(SelectMode selmode);
	SelectMode GetSelectMode(){ return m_selMode; } 

	ProcessorRenderer(CProcessor2  * pProc);
	virtual ~ProcessorRenderer(void);
	Processor* getProcessor();
	
	virtual void Render(C3DView* pView);
	virtual void RenderSelect(C3DView * pView,int selidx) ;
	
	virtual void DoSync();
	virtual bool NeedSync(){ return m_bNeedSync; }
	virtual void Update(){ m_bNeedSync = true; }
	
	
	

	static ProcessorRenderer* NewRenderer(CProcessor2  * pProc);
protected:
	bool m_bNeedSync;
	CProcessor2 * m_pProc2;
	SelectMode m_selMode;
};



#endif
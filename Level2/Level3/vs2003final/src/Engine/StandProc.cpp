// StandProc.cpp: implementation of the StandProc class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "StandProc.h"
#include "TaxiwayProc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StandProc::StandProc()
{
	m_busePushBack = FALSE;

}

StandProc::~StandProc()
{

}
void StandProc::autoSnap(ProcessorList *procList){
	//snap to all taixways
	ASSERT(procList!=NULL);
	int nProcCount=procList->getProcessorCount();
	for(int i=0;i<nProcCount;++i)
	{
		Processor *pProc=procList->getProcessor(i);
		if(pProc->getProcessorType()==TaxiwayProcessor)
		{
			SnapTo(pProc);
		}			
	}
}
void StandProc :: SnapTo(Processor * proc)
{
	if(proc->getProcessorType() ==TaxiwayProcessor)
	{
		//get the 
		if(m_busePushBack  )
		{
			//int pushCount=m_pushbackOffset.getCount();
			//if(pushCount>0)
			//{
			//	double dist=0;
			//	int itr=0;
			//	for(int i=0;i<pushCount;++i)
			//	{
			//		double localdist=m_pushbackOffset[i].length();
			//		if(localdist>dist){
			//			dist=localdist;itr=i;
			//		}
			//	}
			//	Point pt=getPushBackWay()->getPoint(itr);
			//	//pt.setPoint(m_pushbackOffset[itr][VX]+m_location.getPoint(0).getX(),m_pushbackOffset[itr][VY]+m_location.getPoint(0).getY(),m_pushbackOffset[itr][VZ]+m_location.getPoint(0).getZ());
			//	double width=((TaxiwayProc *)proc)->GetWidth();
			//	Point snapPoint=GetSnapPoint(proc->serviceLocationPath(),width,pt);
			//	//set the Point
			//	m_pushbackOffset[itr]=snapPoint-m_location.getPoint(0);
			//}			
			int inConsCount=In_Constr.getCount();
			if(inConsCount>0)
			{				
				double width=((TaxiwayProc *)proc)->GetWidth();
				Point snapPoint=GetSnapPoint(proc->serviceLocationPath(),width,In_Constr.getPoint(0));
				//set the Point
				In_Constr[0]=snapPoint;
				snapPoint  = GetSnapPoint(proc->serviceLocationPath(),width,In_Constr.getPoint(inConsCount-1));
				In_Constr[inConsCount-1] = snapPoint;
			}
		}else//use inconstrain or outconstrain
		{
			//snap the inconstrain 
			int inConsCount=In_Constr.getCount();
			if(inConsCount>0)
			{				
				double width=((TaxiwayProc *)proc)->GetWidth();
				Point snapPoint=GetSnapPoint(proc->serviceLocationPath(),width,In_Constr.getPoint(0));
				//set the Point
				In_Constr[0]=snapPoint;
			}
			//snap the outconstrain
			int outConsCount=Out_Constr.getCount();
			if(outConsCount>0)
			{
				double width=((TaxiwayProc *)proc)->GetWidth();
				Point snapPoint=GetSnapPoint(proc->serviceLocationPath(),width,Out_Constr.getPoint(outConsCount-1));
				//set the Point
				Out_Constr[outConsCount-1]=snapPoint;
			}
		
		}

		
	}
	else
	{NULL;}
}
int StandProc::readSpecialField(ArctermFile& procFile)
{
	int bUsePushback;
	procFile.getInteger(bUsePushback);
	m_busePushBack=(BOOL)bUsePushback;
	//Path Pushback
	int nCount;
	procFile.getInteger(nCount);
	Point pt;
	m_pushbackOffset.clear();
	m_pushbackOffset.init(nCount);
	for(int i=0;i<nCount;i++){
		procFile.getPoint(pt);
		m_pushbackOffset[i]=pt;
	}
	
	
	return TRUE;
}
int StandProc::writeSpecialField(ArctermFile& procFile) const
{
	procFile.writeInt((int)m_busePushBack);
	procFile.writeInt((int)m_pushbackOffset.getCount());
	for(int i=0;i<m_pushbackOffset.getCount();i++)
	{
		procFile.writePoint( m_pushbackOffset.getPoint(i) );
	}
	return TRUE;
}
void StandProc :: setPushBackWay(const Path * path_)
{
	return;
	
	m_pushbackOffset.clear();
	m_pushbackOffset.init(path_->getCount());
	Point centerPt;
	if(m_location.getCount()>=0)
		centerPt = m_location.getPoint(0);
	for(int i=0;i<path_->getCount();i++)
	{
		m_pushbackOffset[i]=path_->getPoint(i)-centerPt;
	}
}
void StandProc :: setPushBackWay(const std::vector <ARCVector3> & _data ){
	
	int ncount=_data.size();
	m_pushbackOffset.clear();
	//m_pushback.clear();
	Point centerPt;
	if(m_location.getCount()>=0)
		centerPt = m_location.getPoint(0);
	m_pushbackOffset.init(ncount);
	for(int i=0;i<ncount;i++){
		//m_pushback[i] = Point ( _data[i][VX], _data[i][VY],_data[i][VZ]   );
		m_pushbackOffset[i] = Point( _data[i][VX]-centerPt.getX(), _data[i][VY]-centerPt.getY(), _data[i][VZ]-centerPt.getZ() );
	}
}
Path*  StandProc::getPushBackWay()
{
	return inConstraint();
	
	int nCount=m_pushbackOffset.getCount();
	m_pushback.clear();
	m_pushback.init(nCount);
	Point centerPt=m_location.getPoint(0);
	for(int i=0;i<nCount ;i++)
	{
		m_pushback[i]=m_pushbackOffset[i]+centerPt;
	}
	return &m_pushback;

}
void StandProc::Rotate( DistanceUnit _degree ){
	Processor::Rotate(_degree);
	m_pushbackOffset.Rotate(_degree);
}

Processor* StandProc::CreateNewProc()
{
	Processor* pProc = new StandProc;
	return pProc;
}


bool StandProc::CopyOtherData(Processor* _pDestProc)
{
	((StandProc *)_pDestProc)->UsePushBack(IsUsePushBack());
	if(IsUsePushBack())
	{
		((StandProc *)_pDestProc)->setPushBackWay(getPushBackWay());
	}
	_pDestProc->SetBackup(IsBackup());

	return true;
}
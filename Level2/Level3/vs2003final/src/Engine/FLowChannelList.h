// FLowChannelList.h: interface for the CFLowChannelList class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_FLOWCHANNELLIST_H__4B39A81A_A144_4301_91B0_F82AECB03D8A__INCLUDED_)
#define AFX_FLOWCHANNELLIST_H__4B39A81A_A144_4301_91B0_F82AECB03D8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "process.h"
class Processor;
class ProcessorArray;

#include <stack>

/***********************************************************************
CFlowChannel used for 1x1 and 1:1
Create a new Flow Channel when 1x1 or 1:1 start
Destroy the Flow Channel when 1x1 or 1:1 end
************************************************************************/
class CFlowChannel
{
private:
	const Processor* m_pInProcessor; //The Processor before enter Channel 
public:
	//create Flow channel and remember the processor before enter Channel 
	CFlowChannel(const Processor* _proc):m_pInProcessor(_proc){};  

	const Processor* GetInChannelProc(){return m_pInProcessor;}

	/***********************************************************************                                              
	return only one destination processor which OneToOne with m_pInProcessor when leave channel.
	return NULL if no one can fit OneToOne with m_pInProcessor 

    _destProcessorArray : The all destination processor of the channel's tail
    _nDestIdLength      : The destionation processorID's Length (The processorID is defined in passenger flow);
	************************************************************************/
	Processor* GetOutChannelProc(const ProcessorArray& _destProcessorArray,int _nDestIdLength ) const;
	static Processor* GetOutChannelProc(const Processor& _pSourceProcessor, const ProcessorArray& _destProcessorArray,int _nDestIdLength );
	
};

//stack of CFlowChannel 
class CFLowChannelList  
{
private:
	std::vector <CFlowChannel*> m_stackChannelNum;
public:
	CFLowChannelList();
	virtual ~CFLowChannelList();
	void PushChannel(CFlowChannel* _pushChannel){ m_stackChannelNum.push_back(_pushChannel);};
	CFlowChannel* PopChannel();
	void InitFlowChannelList(const CFLowChannelList& flowChannelList);
};


#endif // !defined(AFX_FLOWCHANNELLIST_H__4B39A81A_A144_4301_91B0_F82AECB03D8A__INCLUDED_)


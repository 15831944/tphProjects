// MathFlow.cpp: implementation of the CMathFlow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MathFlow.h"
#include <boost\tuple\tuple.hpp>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathFlow::CMathFlow()
{
	m_strFlowName.Empty();
}

CMathFlow::CMathFlow( const CMathFlow& _other)
{
	*this = _other;
}

CMathFlow::~CMathFlow()
{
	ClearAll();
}

CMathFlow& CMathFlow::operator=(const CMathFlow& _other )
{
	m_strFlowName = _other.m_strFlowName;
	m_strPaxType = _other.m_strPaxType;
	m_flowGraph = _other.m_flowGraph;
	return *this;
}
std::pair<link_iterator,bool> CMathFlow::LinkProcess( FlowGraph& _flow, const std::string& _source, const std::string& _target)
{

	if(CMathFlow::HasLink(_flow, _source, _target)==false && CMathFlow::IsRootProc( _target ) == false)
	{

		for( process_iterator iterProc = _flow.begin() ; iterProc != _flow.end(); iterProc++)
		{
			if( EQUEL_PROCESS_NAME( _source, iterProc->first) )
			{
				MathLink link;
				link.name.first = _source;
				link.name.second = _target;
				link_vector& vOutLink = iterProc->second ;

				return std::pair<link_iterator,bool>( vOutLink.insert( vOutLink.end(),  link ), true );
			}
		}
		
	}

	return std::pair<link_iterator,bool>(link_iterator(), false);
}
//operation of link
bool CMathFlow::LinkProcess(const std::string& _source, const std::string& _target)
{
	return CMathFlow::LinkProcess( m_flowGraph, _source, _target).second;
}

// 1. get its all edges
// 2. check if there is a edge which  in process == the _source process and out process == the _target process
std::pair<link_iterator,bool> CMathFlow::FindLink( FlowGraph& _flow, const std::string& _source, const std::string& _target )
{
	bool bSrcFind = false;
	process_iterator iterProc;
	boost::tie( iterProc, bSrcFind ) = CMathFlow::FindProc( _flow, _source );
	
	if ( bSrcFind )
	{
		link_vector& vOutLink = iterProc->second;
		for( link_iterator iterLink=vOutLink.begin(); iterLink != vOutLink.end(); iterLink++)
		{
			if( EQUEL_PROCESS_NAME( _target, iterLink->name.second ))
				return std::pair<link_iterator, bool>( iterLink, true );
		}
	}

	return std::pair<link_iterator, bool>( link_iterator(), false );
}

bool CMathFlow::HasLink( const CMathFlow& _mflow, const std::string& _source, const std::string& _target)
{
	if( EQUEL_PROCESS_NAME( _source,std::string("")) || EQUEL_PROCESS_NAME(_target, std::string("")) )
		return false;

	return CMathFlow::HasLink( _mflow.m_flowGraph, _source, _target );
}


bool CMathFlow::AddLink( const MathLink& _newLink )
{
	bool bLinked = false;
	link_iterator iterLink;
	//link process
	boost::tie(iterLink, bLinked) = CMathFlow::LinkProcess( m_flowGraph, _newLink.name.first, _newLink.name.second);
	if( bLinked )
	{
		//assign MathLink
		*iterLink = _newLink;
	}

	return false;
}
//remove the link of _source process and _target process.
//if you want to remove the link of root process and other process, _source will be ignore
bool CMathFlow::RemoveLink( const std::string& _source, const std::string& _target)
{
	bool bFind = false;
	process_iterator iterProc;
	boost::tie( iterProc, bFind ) = CMathFlow::FindProc( m_flowGraph, _source );
	if( bFind )
	{
		link_vector& vOutLink = iterProc->second;
		for( link_iterator iterLink = vOutLink.begin(); iterLink != vOutLink.end(); iterLink++)
		{
			if (EQUEL_PROCESS_NAME( _target, iterLink->name.second ))
			{
				vOutLink.erase( iterLink );
				return true;
			}
		}
	}

	return false;
}

//link source process with a new target process, or link target process with a new source process
bool CMathFlow::Relink( const std::pair<std::string,std::string>& _oldLink, const std::pair<std::string,std::string>& _newLink)
{

	bool bFind =false;
	link_iterator iterOldLink, iterNewLink;
	boost::tie( iterOldLink, bFind ) = CMathFlow::FindLink( m_flowGraph, _oldLink.first, _oldLink.second );
	if( bFind )
	{
		MathLink tmpLink( *iterOldLink );
		bool bLinked = false;
		boost::tie( iterNewLink, bLinked ) = CMathFlow::LinkProcess( m_flowGraph, _newLink.first, _newLink.second );
		if( bLinked )
		{
			//copy old link info into new link
			*iterNewLink = tmpLink;
			iterNewLink->name = _newLink;

			//remove old link;
			RemoveLink( _oldLink.first, _oldLink.second );

			return true;
		}
	}
	return false;
}

//modify the _link's data in flow, except for MathLink::name
bool CMathFlow::UpdateLink( const MathLink& _link)
{
	bool bFind = false;
	link_iterator iterLink;
	boost::tie( iterLink, bFind ) = CMathFlow::FindLink( m_flowGraph, _link.name.first, _link.name.second );
	if( bFind )
	{
		//update link info
		*iterLink = _link;

		return true;
	}
	return false;
}

std::pair<process_iterator,bool> CMathFlow::FindProc(FlowGraph& _flow, const std::string& _proc )
{
	process_iterator iterProc;
	for( iterProc = _flow.begin(); iterProc != _flow.end(); iterProc++)
	{
		if(EQUEL_PROCESS_NAME( _proc, iterProc->first ))
			break;
	}
	return std::pair<process_iterator,bool>( iterProc, iterProc != _flow.end());
}

std::pair<const_process_iterator,bool> CMathFlow::FindProc(const FlowGraph& _flow, const std::string& _proc )
{
	const_process_iterator iterProc;
	for( iterProc = _flow.begin(); iterProc != _flow.end(); iterProc++)
	{
		if(EQUEL_PROCESS_NAME( _proc, iterProc->first ))
			break;
	}
	return std::pair<const_process_iterator,bool>( iterProc, iterProc != _flow.end());
}
//add _proc process into flow
bool CMathFlow::AddProcess( const std::string& _proc )
{
	if( CMathFlow::HasProc( m_flowGraph, _proc) ==false )
	{
		m_flowGraph.push_back( std::make_pair( _proc, link_vector()) );
		return true;
	}

	return false;
}

//remove _proc process from flow
bool CMathFlow::RemoveProcess( const std::string& _proc )
{
	if( CMathFlow::IsRootProc(_proc)==false || (CMathFlow::IsRootProc(_proc)==true && m_flowGraph.size() == 1) )
	{
		bool bFind = false;
		process_iterator iterProc;
		boost::tie(iterProc, bFind ) = CMathFlow::FindProc( m_flowGraph, _proc);
		if( bFind )
		{
			//remove all out links and this process
			m_flowGraph.erase( iterProc );

			link_iterator iterLink;
			//remove all in links
			for( iterProc = m_flowGraph.begin(); iterProc != m_flowGraph.end(); iterProc++)
			{
				link_vector& vOutLink = iterProc->second ;
				for(iterLink = vOutLink.begin(); iterLink != vOutLink.end(); )
				{
					if( EQUEL_PROCESS_NAME( _proc, iterLink->name.second ))
					{
						iterLink = vOutLink.erase( iterLink );
						continue;
					}
					iterLink++;
				}
			}

			return true;
		}

	}
	return false;
}

// 1. find process
// 2. change process name
// 3. change relative links' name
bool CMathFlow::RenameProcess( const std::string& _oldname, const std::string& _newname )
{
	if( IsRootProc( _oldname) || CMathFlow::HasProc( m_flowGraph, _newname) ) return false;
	bool bFind = false;
	process_iterator iterProc;
	boost::tie( iterProc, bFind ) = CMathFlow::FindProc( m_flowGraph, _oldname );
	if( bFind )
	{
		//change vertex name
		iterProc->first = _newname;

		//change edge info which contain _oldname
		for(iterProc = m_flowGraph.begin(); iterProc != m_flowGraph.end(); iterProc++)
		{
			link_vector& vOutLink = iterProc->second ;
			for( link_iterator iterLink = vOutLink.begin(); iterLink != vOutLink.end(); iterLink++)
			{
				if( EQUEL_PROCESS_NAME(_oldname, iterLink->name.first))
				{
					iterLink->name.first = _newname;
				}

				if( EQUEL_PROCESS_NAME(_oldname, iterLink->name.second))
				{
					iterLink->name.second = _newname;
				}

			}
		}

		return true;
	}
	return false;
}

//check if the flow exists a recycle
bool CMathFlow::IsRecycle( const std::string& _proc ) const
{
	// TRACE("Not implement!");
	return true;
}

//check if _proc is the root process of flow
bool CMathFlow::IsRootProc( const std::string& _proc)
{
	return EQUEL_PROCESS_NAME( _proc, std::string("start") );
}

//check if _proc is the leaf process of flow
bool CMathFlow::IsLeafProc( const std::string& _proc) const
{

	const_process_iterator iterProc;
	bool bFind = false;
	boost::tie(iterProc, bFind) = CMathFlow::FindProc( m_flowGraph, _proc );
	if(bFind)
	{
		return iterProc->second.size() == 0;
	}
	return false;

}
//travel all vertices to find the process
bool CMathFlow::HasProc( const FlowGraph& _flow, const std::string& _proc )
{
	return CMathFlow::FindProc( const_cast<FlowGraph&>(_flow), _proc ).second;
}

bool CMathFlow::HasProc( const CMathFlow& _mflow, const std::string& _proc)
{
	if( EQUEL_PROCESS_NAME( _proc, std::string("")) ) return false;
	return CMathFlow::HasProc( _mflow.m_flowGraph, _proc );
}

bool CMathFlow::HasLink( const FlowGraph& _flow, const std::string& _source, const std::string& _target)
{
	return CMathFlow::FindLink( const_cast<FlowGraph&>(_flow), _source, _target ).second;
}

//travel operation of process
void CMathFlow::Source( const std::string& _target, std::vector<std::string>& _vSources) const
{
	_vSources.clear();

	for(const_process_iterator iterProc = m_flowGraph.begin(); iterProc != m_flowGraph.end(); iterProc++)
	{
		const link_vector& vOutLink = iterProc->second;
		for( const_link_iterator iterLink = vOutLink.begin(); iterLink != vOutLink.end(); iterLink++)
		{
			if(EQUEL_PROCESS_NAME( _target, iterLink->name.second ))
			{
				_vSources.push_back( iterLink->name.first );
			}
		}
	}
}

//to get all target process of _source process
void CMathFlow::Target( const std::string& _source, std::vector<std::string>& _vTargets) const
{
	_vTargets.clear();
	
	bool bFind = false;
	const_process_iterator iterProc;
	boost::tie( iterProc, bFind ) = CMathFlow::FindProc( m_flowGraph, _source );
	if(bFind)
	{
		const link_vector& vOutLink = iterProc->second; 
		for( const_link_iterator iterLink = vOutLink.begin(); iterLink != vOutLink.end(); iterLink++)
		{
			_vTargets.push_back( iterLink->name.second );
		}
	}
}

//to get all leaves process of flow
void CMathFlow::Leaf(std::vector<std::string>& _vLeaves ) const
{
	_vLeaves.clear();
	for(const_process_iterator iterProc = m_flowGraph.begin(); iterProc != m_flowGraph.end(); iterProc++)
	{
		if( iterProc->second.size() == 0)
		{
			_vLeaves.push_back( iterProc->first );
		}
	}
}

//to get all out link of _source process
void CMathFlow::OutLink(const std::string& _source, std::vector<MathLink>& _vOutLinks) const
{
	bool bFind = false;
	const_process_iterator iterProc;
	boost::tie( iterProc, bFind ) = CMathFlow::FindProc( m_flowGraph, _source );
	if(bFind)
	{
		_vOutLinks = iterProc->second ;
	}
}

//to get all in link of _target process
void CMathFlow::InLink( const std::string& _target, std::vector<MathLink>& _vInLinks) const
{
	_vInLinks.clear();

	for(const_process_iterator iterProc = m_flowGraph.begin(); iterProc != m_flowGraph.end(); iterProc++)
	{
		const link_vector& vOutLink = iterProc->second;
		for( const_link_iterator iterLink = vOutLink.begin(); iterLink != vOutLink.end(); iterLink++)
		{
			if(EQUEL_PROCESS_NAME( _target, iterLink->name.second ))
			{
				_vInLinks.push_back( *iterLink );
			}
		}
	}
}

void CMathFlow::ClearAll()
{
	for( process_iterator iterProc = m_flowGraph.begin(); iterProc != m_flowGraph.end(); iterProc++)
	{
		iterProc->second.clear();
	}
	m_flowGraph.clear();
	m_strFlowName.Empty();
}

MathLink* CMathFlow::GetLink(const std::string& _source, const std::string& _target)
{
	bool bSrcFind = false;
	process_iterator iterProc;
	boost::tie(iterProc, bSrcFind) = CMathFlow::FindProc(m_flowGraph, _source);
	
	if (bSrcFind)
	{
		link_vector& vOutLink = iterProc->second;
		for (link_iterator iterLink = vOutLink.begin(); iterLink != vOutLink.end(); iterLink++)
		{
			if( EQUEL_PROCESS_NAME(_target, iterLink->name.second ))
				return &(*iterLink);
		}
	}

	return NULL;
}
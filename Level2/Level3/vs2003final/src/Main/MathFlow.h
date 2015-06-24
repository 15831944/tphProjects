// MathFlow.h: interface for the CMathFlow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHFLOW_H__40838047_093A_427A_9558_74A78AF968F4__INCLUDED_)
#define AFX_MATHFLOW_H__40838047_093A_427A_9558_74A78AF968F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable:4786)
#include <vector>

#define EQUEL_PROCESS_NAME(sp1, sp2)\
	strnicmp( sp1.c_str(), sp2.c_str(), sp1.length() ) ==0


struct MathLink
{
	std::pair<std::string, std::string> name;
	int percent;
	int distance;
	MathLink()
	{
		percent = 0;
		distance = 0;
	}
	MathLink(const MathLink& _other)
	{
		*this = _other;
	}
	MathLink& operator=(const MathLink& _other)
	{
		name = _other.name;
		percent = _other.percent;
		distance = _other.distance;
		return *this;
	}
	bool operator==(const MathLink& _other) const
	{
		return EQUEL_PROCESS_NAME(name.first ,_other.name.first) && EQUEL_PROCESS_NAME(name.second, _other.name.second);
	}
};

//container define
typedef std::vector< MathLink >					link_vector;
typedef std::pair<std::string, link_vector >	ProcOutPair;
typedef std::vector< ProcOutPair >				FlowGraph;
//iterator define
typedef FlowGraph::iterator						process_iterator;
typedef std::vector<MathLink>::iterator			link_iterator;
typedef FlowGraph::const_iterator				const_process_iterator;
typedef std::vector<MathLink>::const_iterator	const_link_iterator;


class CMathFlow  
{
public:
	CMathFlow();
	CMathFlow( const CMathFlow& _other);
	virtual ~CMathFlow();

protected:

	//============================================
	//	process0, ( outlink00, outlink01......)
	//	process1, ( outlink10, outlink11......)
	//	.
	//	.		FlowGraph layout
	//	.
	//============================================
	FlowGraph m_flowGraph;//to hold the relation of vertices;
	CString m_strFlowName;
	CString m_strPaxType;
public:
	void SetFlowName(const CString strFlowName)
	{
		ASSERT(strFlowName != "");
		m_strFlowName = strFlowName;
	}
	const CString GetFlowName() const {	return m_strFlowName; }

	void SetPaxType(LPCTSTR lpszPaxType) { m_strPaxType = lpszPaxType; }
	LPCTSTR GetPaxType() { return m_strPaxType; }
	
	void ClearAll();

	//operation of link
	bool LinkProcess(const std::string& _source, const std::string& _target);
	bool AddLink( const MathLink& _newLink );
	bool RemoveLink( const std::string& _source, const std::string& _target);
	bool Relink( const std::pair<std::string,std::string>& _oldLink, const std::pair<std::string,std::string>& _newLink);
	bool UpdateLink( const MathLink& _link);

	//operation of process
	bool AddProcess( const std::string& _proc );
	bool RemoveProcess( const std::string& _proc );
	bool RenameProcess( const std::string& _oldname, const std::string& _newname );

	//check of valid
	bool IsRecycle( const std::string& _proc ) const;
	bool static IsRootProc( const std::string& _proc);
	bool IsLeafProc( const std::string& _proc) const;

	//travel operation of process
	void Source( const std::string& _target, std::vector<std::string>& _vSources) const;
	void Target( const std::string& _source, std::vector<std::string>& _vTargets) const;
	void Leaf(std::vector<std::string>& _vLeaves ) const;

	//travel operation of link
	void OutLink(const std::string& _source, std::vector<MathLink>& _vOutLinks) const;
	void InLink( const std::string& _target, std::vector<MathLink>& _vInLinks) const;

	const FlowGraph& GetFlow() const{ return m_flowGraph; }
protected:
	//find operation of process and link
	std::pair<link_iterator,bool> static FindLink( FlowGraph& _flow, const std::string& _source, const std::string& _target );
	std::pair<process_iterator,bool> static FindProc(FlowGraph& _flow, const std::string& _proc );
	std::pair<const_process_iterator,bool> static FindProc(const FlowGraph& _flow, const std::string& _proc );
	std::pair<link_iterator,bool> static LinkProcess( FlowGraph& _flow, const std::string& _source, const std::string& _target);
public:
	bool static HasProc( const FlowGraph& _flow, const std::string& _proc );
	bool static HasProc( const CMathFlow& _mflow, const std::string& _proc);
	bool static HasLink( const FlowGraph& _flow, const std::string& _source, const std::string& _target);
	bool static HasLink( const CMathFlow& _mflow, const std::string& _source, const std::string& _target);
	CMathFlow& operator=(const CMathFlow& _other );
	
	MathLink* GetLink(const std::string& _source, const std::string& _target);
};

#endif // !defined(AFX_MATHFLOW_H__40838047_093A_427A_9558_74A78AF968F4__INCLUDED_)

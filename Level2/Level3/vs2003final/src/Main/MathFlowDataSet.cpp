// MathFlowDataSet.cpp: implementation of the CMathFlowDataSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MathFlowDataSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathFlowDataSet::CMathFlowDataSet() : DataSet(MMFlowInfoFile)
{
	m_pFlowVect = NULL;
}

CMathFlowDataSet::~CMathFlowDataSet()
{

}

	
void CMathFlowDataSet::clear()
{

}

void CMathFlowDataSet::readData(ArctermFile& p_file)
{
	char buf[256];
	memset((void*)buf, 0, sizeof(buf));

	m_pFlowVect->clear();

	CMathFlow flow;
	while (p_file.getLine() == 1)
	{
		// read the name
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0 )
		{
			if(flow.GetFlowName() != (LPCTSTR)buf)
			{
				if(flow.GetFlowName() != "")
				{
					m_pFlowVect->push_back(flow);
					flow.ClearAll();
				}
				flow.SetFlowName((LPCTSTR)buf);
			}
		}

		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			flow.AddProcess(buf);

		// read the children no
		int no = 0;
		p_file.getInteger(no);

		if(no <= 0)
			continue;

		std::pair<std::string, std::string> name;
		name.first = buf;

		for(int i=0; i<no; i++)
		{
			memset(buf, 0, sizeof(buf) / sizeof(char));
			if (p_file.getField(buf, 255) != 0)//process name
			{
				int iPower = 0;
				p_file.getInteger(iPower);//power value
				int iDistance = 0;
				p_file.getInteger(iDistance);
				name.second = buf;

				MathLink link;
				link.name = name;
				link.percent = iPower;
				link.distance = iDistance;
				flow.AddLink(link);
			}
		}
	}

	if(flow.GetFlowName() != "")
	{
		m_pFlowVect->push_back(flow);
	}
}

void CMathFlowDataSet::readObsoleteData(ArctermFile& p_file)
{

}

void CMathFlowDataSet::writeData(ArctermFile& p_file) const
{
	for (int i = 0; i < static_cast<int>(m_pFlowVect->size()); i++)
	{
		CMathFlow flow = m_pFlowVect->at(i);
		
		const FlowGraph &fGraph = flow.GetFlow();
		const_process_iterator iter = fGraph.begin();

		for(; iter != fGraph.end(); iter++)
		{
			p_file.writeField(flow.GetFlowName());

			ProcOutPair proc = *iter;
			p_file.writeField(proc.first.c_str());

			link_vector vLink;
			vLink = proc.second;

			int nChildren = vLink.size();
			p_file.writeInt(nChildren);

			const_link_iterator linkIter = vLink.begin();
			for( ; linkIter != vLink.end(); linkIter++)
			{
				const MathLink& link = *linkIter;

				p_file.writeField(link.name.second.c_str());
				p_file.writeInt(link.percent);
				p_file.writeInt(link.distance);
			}

			p_file.writeLine();
		}
	}
	
	if (m_pFlowVect->empty())
		p_file.writeLine();
}
	
int CMathFlowDataSet::GetFlows(std::vector<CMathFlow>* pFlowVect)
{
	pFlowVect = m_pFlowVect;
	return m_pFlowVect->size();
}

void CMathFlowDataSet::SetFlows(std::vector<CMathFlow>* pFlowVect)
{
	m_pFlowVect = pFlowVect;
}

int CMathFlowDataSet::AddFlow(const CMathFlow& flow)
{
	m_pFlowVect->push_back(flow);

	return m_pFlowVect->size();
}

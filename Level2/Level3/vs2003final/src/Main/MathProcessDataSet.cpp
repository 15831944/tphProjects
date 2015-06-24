// MathProcessDataSet.cpp: implementation of the CMathProcessDataSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MathProcessDataSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathProcessDataSet::CMathProcessDataSet() : DataSet(MMProcessInfoFile)
{
	m_pProcesses = NULL;
}

CMathProcessDataSet::~CMathProcessDataSet()
{

}

	
void CMathProcessDataSet::clear()
{

}

void CMathProcessDataSet::readData(ArctermFile& p_file)
{
	char buf[256];

	m_pProcesses->clear();
	//	"Number, Time, Discipline, Configuration, Capacity"

	while (p_file.getLine() == 1)
	{
		CMathProcess proc;

		// read the name
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			proc.SetName(buf);
		// read the Number 
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			proc.SetProcessCount(::atoi((char*)buf));
		
		// read the Time
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			proc.SetProbDistName(buf);

		// read the Discipline
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			proc.SetQDiscipline((QueueDiscipline)::atoi((char*)buf));

		// read the Configuration
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			proc.SetQConfig((QueueConfig)::atoi((char*)buf));

		// read the Capacity
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			proc.SetCapacity(::atoi((char*)buf));


		m_pProcesses->push_back(proc);
	}
}

void CMathProcessDataSet::readObsoleteData(ArctermFile& p_file)
{

}

void CMathProcessDataSet::writeData(ArctermFile& p_file) const
{
	CString str = "";

	for (int i = 0; i < static_cast<int>(m_pProcesses->size()); i++)
	{
		CMathProcess proc = m_pProcesses->at(i);
		str = proc.GetName();
		p_file.writeField(str);

		int k = proc.GetProcessCount();
		str.Format("%d", k);
		p_file.writeField(str);

		str = proc.GetProbDistName();
		p_file.writeField(str);

    	str.Format("%d", proc.GetQDiscipline());
		p_file.writeField(str);

		str.Format("%d", proc.GetQConfig());
		p_file.writeField(str);
	
		str.Format("%d", proc.GetCapacity());
		p_file.writeField(str);
		
		p_file.writeLine();
	}
	
	if (m_pProcesses->empty())
		p_file.writeLine();
}
	
int CMathProcessDataSet::GetProcesses(std::vector<CMathProcess>* pProces)
{
	pProces = m_pProcesses;
	return m_pProcesses->size();
}

void CMathProcessDataSet::SetProcesses(std::vector<CMathProcess>* pProces)
{
	m_pProcesses = pProces;
}

int CMathProcessDataSet::AddProcess(const CMathProcess& proc)
{
	m_pProcesses->push_back(proc);

	return m_pProcesses->size();
}

// BaseReport.cpp: implementation of the CBaseReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "BaseReport.h"
#include "results\out_term.h"
#include "..\common\termfile.h"
#include "..\common\projectmanager.h"
#include "ReportParameter.h"
#include "common\SimAndReportManager.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Description: Constructor
// Exception:	FileOpenError, StringError, FileVersionTooNewError
CBaseReport::CBaseReport( Terminal* _pTerm, const CString& _csProjPath )
{
	m_pTerm = _pTerm;
	m_pTerm->openLogs(_csProjPath);
	InitDefaultSpecs();
	InitProcList();

}

CBaseReport::~CBaseReport()
{
	if( m_procList )
		delete [] m_procList;
	m_procIDlist.clear();
	
}


void CBaseReport::ReadSpecifications(const CString& _csProjPath)
{
    char filename[_MAX_PATH];
    PROJMANAGER->getSpecFileName( _csProjPath, GetSpecFileType()/*PaxLogSpecFile*/, filename );
	

    if (!PROJMANAGER->fileExists (filename))
        return;

    ArctermFile specFile;
    specFile.openFile (filename, READ);

    specFile.getLine();
    specFile.getTime ( m_startTime );
    specFile.getTime ( m_endTime );
	m_endTime += WholeDay;
    if (!specFile.eol() && m_usesInterval)
        specFile.getTime (m_interval);

    specFile.getLine();
    if (!specFile.isBlank())
        m_multiConst.readConstraints (specFile, m_pTerm );

    specFile.getLine();
    if (!specFile.isBlank() && !specFile.allProcs())
        m_procIDlist.readIDList (specFile, m_pTerm );

    specFile.closeIn();

}


void CBaseReport::GenerateReport( const CString& _csProjPath, bool bSummary /* == true */ )
{
	//char filename[_MAX_PATH];
	//PROJMANAGER->getReportFileName ( _csProjPath, GetReportFileType(), filename);

	CString sFileName = m_pTerm->GetSimReportManager()->GetCurrentReportFileName( _csProjPath );
    ArctermFile p_file;
    p_file.openFile (sFileName, WRITE);

    // write all titles
    p_file.writeField( GetTitle() );
    char string[128];
    m_startTime.printTime (string);
    strcat (string, ";");
    m_endTime.printTime (string + strlen (string));

    if ( m_usesInterval)
    {
        strcat (string, ";");
        m_interval.printTime (string + strlen (string));
    }

    // write subtitles
    p_file.writeField (string);
    m_multiConst.writeConstraints (p_file, 1);
    m_procIDlist.writeIDList (p_file);

    p_file.writeLine();

    if( m_startTime <= m_endTime )
    {
		if( bSummary )
			GenerateSummary (p_file);
		else
            GenerateDetailed (p_file);
    }
    p_file.endFile();
}


// set report paramters to include all pax, all procs, & full 24 hours
// clear list of progress bars
void CBaseReport::InitDefaultSpecs()
{
	m_startTime = 0l;
	m_interval = 1800l;
	m_usesInterval = 0;
	m_endTime = 86400l;

	//m_multiConst.initDefault( m_pTerm );
	//m_procIDlist.initDefault( m_pTerm );
	m_procIDlist.OwnsElements( 1 );

}



// loads all processor types into array for easy retrieval
void CBaseReport::InitProcList()
{
	int procCount = m_pTerm->procLog->getCount();
	if (procCount)
		m_procList = new ProcLogEntry[procCount];
	else
		m_procList = NULL;

	for (int i = 0; i < procCount; i++)
		m_pTerm->procLog->getItem( m_procList[i], i );
}

int CBaseReport::ProcFitsSpecs( int p_ndx,ProcessorID& id )
{
	if (p_ndx >= m_pTerm->procLog->getCount() || p_ndx<0)
		return m_procIDlist.areAllBlank();
	return ProcFitsSpecs (m_procList[p_ndx],id);
}

int CBaseReport::ProcFitsSpecs( ProcLogEntry p_entry,ProcessorID& id )
{
	if (!m_procIDlist.getCount() || m_procIDlist.areAllBlank())
		return TRUE;

	int idList[MAX_PROC_IDS];
	p_entry.getIDs (idList);

	ProcessorID sid;
	sid.copyIDlist (idList);

	for (int i = 0; i < m_procIDlist.getCount(); i++)
	{
		if ((m_procIDlist.getID(i))->idFits (sid))
		{
			id = *(m_procIDlist.getID(i));
			return TRUE;
		}
	}
	return FALSE;
}
// simple test to determine whether a processor fits specifications
int CBaseReport::ProcFitsSpecs (int p_ndx)
{
    if (p_ndx >= m_pTerm->procLog->getCount() || p_ndx<0)
		return m_procIDlist.areAllBlank();
    return ProcFitsSpecs (m_procList[p_ndx]);
}

// simple test to determine whether a processor fits specifications
int CBaseReport::ProcFitsSpecs (ProcLogEntry p_entry)
{
	if (!m_procIDlist.getCount() || m_procIDlist.areAllBlank())
        return TRUE;

    int idList[MAX_PROC_IDS];
    p_entry.getIDs (idList);

    ProcessorID id;
    id.copyIDlist (idList);

    for (int i = 0; i < m_procIDlist.getCount(); i++)
    {
        if ((m_procIDlist.getID(i))->idFits (id))
            return TRUE;
    }
    return FALSE;
}

//DEL int CBaseReport::procFitsSpecs(int p_ndx)
//DEL {
//DEL 	if (p_ndx >= m_pTerm->procLog->getCount())
//DEL         return m_procIDlist.isDefault();
//DEL     return procFitsSpecs (m_procList[p_ndx]);
//DEL }

//DEL int CBaseReport::procFitsSpecs(ProcLogEntry p_entry)
//DEL {
//DEL 
//DEL }
int CBaseReport::getProcType (int p_ndx)
{
    return (p_ndx == 32767)? PointProc: m_procList[p_ndx].getProcType();
}
void CBaseReport::InitParameter( const CReportParameter* _pPara )
{
	_pPara->GetStartTime( m_startTime );
	_pPara->GetEndTime( m_endTime );
	long lInterval=0;
	_pPara->GetInterval( lInterval );
	m_interval.set( lInterval  );
	 
	// passenger and processor types of interest	
	std::vector<CMobileElemConstraint>vPaxType;
	if( _pPara->GetPaxType( vPaxType ) )
	{
		m_multiConst.freeConstraints();
		int iSize = vPaxType.size();
		for( int i=0; i<iSize; ++i )
		{
			m_multiConst.addConstraint( vPaxType[i] );
		}
	}

	std::vector<ProcessorID>vProcs;
	if( _pPara->GetProcessorGroup( vProcs) )
	{
		m_procIDlist.clear();
		int iSize = vProcs.size();
		for( int i=0; i<iSize; ++i )
		{
			ProcessorID* id = new ProcessorID;
			*id = vProcs[ i ];
			m_procIDlist.addItem( id );
		}
	}    
}
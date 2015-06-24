#include "stdafx.h"

#include "inputs\miscproc.h"
#include <CommonData/procid.h>
#include "common\termfile.h"
#include "in_term.h"


int MiscProcessorIDWithOne2OneFlag::readData (ArctermFile& file)
{
	char charflag;
	file.getChar( charflag );
	
	m_bOne2OneFlag = charflag == 'Y' ? true : false;
	return ProcessorID::readProcessorID( file );
}

int MiscProcessorIDWithOne2OneFlag::writeData (ArctermFile& file) const
{
	char charflag = m_bOne2OneFlag ? 'Y' : 'N';
	file.writeChar( charflag );
	
	return ProcessorID::writeProcessorID( file );
}

int MiscProcessorIDWithOne2OneFlag::GetPipeAt( int _iIdx ) const
{
	ASSERT( _iIdx >=0 && _iIdx < static_cast<int>(m_vUsedPipes.size()) );
	return m_vUsedPipes.at( _iIdx );
}

//////////////////////////////////////////////////////////////////////////
void MiscProcessorIDList::readIDList (ArctermFile& p_file, InputTerminal* _pInTerm , bool _bWithOneToOne/*= true*/ )
{
	clear();
    char *str1, *str2, *str3, str[1024];
    p_file.getField (str, 1024);
    if (!str[0])
        return;
	
    str2 = str - 1;
    MiscProcessorIDWithOne2OneFlag *id;
    while (str2)
    {
        str1 = str2 + 1;
        str2 = strstr (str1, ";");
        if (str2)
            str2[0] = '\0';

        id = new MiscProcessorIDWithOne2OneFlag;
		id->SetStrDict( _pInTerm->inStrDict );
		if( _bWithOneToOne )
		{
			str3 = strstr( str1, "@" );
			if( str3 )
				str3[0] = '\0';

			id->setID (str1);
			id->setOne2OneFlag( str3[1] == 'Y' ? true : false );
		}
		else
		{	
			id->setID( str1 );
			id->setOne2OneFlag( false );
		}
        addItem (id);
    }
}

void MiscProcessorIDList::writeIDList (ArctermFile& p_file) const
{
	char str[1024] = "";
	MiscProcessorIDWithOne2OneFlag* pProc = NULL;

    if (getCount())
	{	
		pProc = (MiscProcessorIDWithOne2OneFlag*)getItem (0);
		pProc->printID (str);
		strcat( str, pProc->getOne2OneFlag() ? "@Y" : "@N" );
	}

    for (int i = 1; i < getCount(); i++)
    {
		strcat (str, ";");
		pProc = (MiscProcessorIDWithOne2OneFlag*)getItem (i);
        pProc->printID (str + strlen (str));
		strcat( str, pProc->getOne2OneFlag() ? "@Y" : "@N" );
    }
    p_file.writeField (str);
	
}
//////////////////////////////////////////////////////////////////////////
MiscData::MiscData ()
{
	m_bDisallowGroup = false;
	m_bWaitInQueue = true;
    visitorFlag = 0;
    gateList		= new MiscProcessorIDList;
	curbSideList = new ALTObjectIDList;
	curbSideListAdd = new ALTObjectIDList;
	curbSideListDelete = new ALTObjectIDList;
	gateList->OwnsElements( 1 );
	m_pWaitAreaList = new MiscProcessorIDList;
	m_pWaitAreaList->OwnsElements( 1 );
	m_pLinkedDestList = new MiscProcessorIDList;
	m_pLinkedDestList->ownsElements( 1 );
	m_pInTerm = NULL;
	m_lQueueCapacity=0;
	m_nCyclicFreq = 10;
	m_nGroupSerTimeModIndex = 0;
	m_nGroupSerTimeModSum = 100;
	m_lTerminateTime = -1;

	m_pCapacityAttributes = new CapacityAttributes;

	m_bUseDependentProc = false;
	m_pDependentProcList = new MiscProcessorIDList;
	m_pDependentProcList->OwnsElements( 1 );

	m_pBridgeConnectorList = new MiscProcessorIDList;
	m_pBridgeConnectorList->OwnsElements( 1 );

}

void MiscData::readMiscData (ArctermFile& p_file)
{
	p_file.getInteger( m_nCyclicFreq );
	p_file.getInteger(m_lQueueCapacity);
	int nCapaAttrCount;
    p_file.getInteger( nCapaAttrCount);
	for (int i=0; i<nCapaAttrCount; i++)
	{
		CapacityAttribute* pCapaAttr = new CapacityAttribute;
		p_file.getInteger(pCapaAttr->m_nMobElementIndex);
		p_file.getInteger(pCapaAttr->m_nCapacity);
		m_pCapacityAttributes->AddCapacityAttribute(pCapaAttr);
		if(p_file.getVersion() > 3.2)
		{
			TCHAR name[1024] = {0} ;
			int version = 0 ;
			p_file.getInteger(version) ;
			pCapaAttr->m_Type = (CapacityAttributes::CapacityType)version ;
			p_file.getField(name ,1024) ;
			pCapaAttr->m_Name = name ;
			//p_file.getLine() ;
			int size = 0 ;
			p_file.getInteger(size) ;
			//p_file.getLine() ;
			CString strname ;
			for (int i = 0 ;i < size ;i++)
			{
				p_file.getField(name ,1024) ;
				pCapaAttr->AddCombinationContainTypeString(CString(name)) ;
			}
		}
		p_file.getLine() ;
	}
	p_file.getInteger( m_lTerminateTime);
	
	char disallowGroup;
    char visitors;
	char cWaitInQueue;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;
    
	p_file.getChar (visitors);
    visitorFlag = (visitors == 'Y');

	p_file.getChar(cWaitInQueue);
	m_bWaitInQueue = cWaitInQueue == 'N' ? false : true;

	assert( m_pInTerm );
    gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);

	char cUseDependentProc;
	p_file.getChar( cUseDependentProc );
	m_bUseDependentProc = cUseDependentProc == 'Y' ? true : false;
	m_pDependentProcList->readIDList(p_file, m_pInTerm);
    
	readSpecificData (p_file);
	p_file.getInteger(m_nGroupSerTimeModIndex);
	p_file.getInteger(m_nGroupSerTimeModSum);

}

void MiscData::readObsoleteMiscData24 (ArctermFile& p_file)
{
	p_file.getInteger( m_nCyclicFreq );
    p_file.getInteger(m_lQueueCapacity);
	
	char disallowGroup;
    char visitors;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;
    
	p_file.getChar (visitors);
    visitorFlag = (visitors == 'Y');
	assert( m_pInTerm );
    gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
    readSpecificData (p_file);
}

void MiscData::readObsoleteMiscData25 (ArctermFile& p_file)
{
	p_file.getInteger( m_nCyclicFreq );
    p_file.getInteger(m_lQueueCapacity);
	
	char disallowGroup;
    char visitors;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;
    
	p_file.getChar (visitors);
    visitorFlag = (visitors == 'Y');
	assert( m_pInTerm );
    gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
    readSpecificData (p_file);	
}

void MiscData::readObsoleteMiscData26(ArctermFile &p_file)
{
	p_file.getInteger( m_nCyclicFreq );
    p_file.getInteger(m_lQueueCapacity);
	
	char disallowGroup;
    char visitors;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;
    
	p_file.getChar (visitors);
    visitorFlag = (visitors == 'Y');
	assert( m_pInTerm );
    gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);
    readSpecificData (p_file);
}
void MiscData::readObsoleteMiscData271(ArctermFile&p_file)//2.71
{
	p_file.getInteger( m_nCyclicFreq );
    p_file.getInteger(m_lQueueCapacity);
	
	char disallowGroup;
    char visitors;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;
    
	p_file.getChar (visitors);
    visitorFlag = (visitors == 'Y');
	assert( m_pInTerm );
    gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);
    readSpecificData (p_file);
	p_file.getInteger(m_nGroupSerTimeModIndex);
	p_file.getInteger(m_nGroupSerTimeModSum);

}

void MiscData::readObsoleteMiscData28(ArctermFile&p_file)//2.8
{
	p_file.getInteger( m_nCyclicFreq );
    p_file.getInteger(m_lQueueCapacity);
	
	char disallowGroup;
    char visitors;
	char cWaitInQueue;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;
    
	p_file.getChar (visitors);
    visitorFlag = (visitors == 'Y');

	p_file.getChar(cWaitInQueue);
	m_bWaitInQueue = cWaitInQueue == 'N' ? false : true;

	assert( m_pInTerm );
    gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);
    readSpecificData (p_file);
	p_file.getInteger(m_nGroupSerTimeModIndex);
	p_file.getInteger(m_nGroupSerTimeModSum);
}

void MiscData::readObsoleteMiscData29(ArctermFile&p_file)//2.8
{
	p_file.getInteger( m_nCyclicFreq );
	p_file.getInteger( m_lQueueCapacity);
	p_file.getInteger( m_lTerminateTime);

	char disallowGroup;
	char visitors;
	char cWaitInQueue;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;

	p_file.getChar (visitors);
	visitorFlag = (visitors == 'Y');

	p_file.getChar(cWaitInQueue);
	m_bWaitInQueue = cWaitInQueue == 'N' ? false : true;

	assert( m_pInTerm );
	gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);
	readSpecificData (p_file);
	p_file.getInteger(m_nGroupSerTimeModIndex);
	p_file.getInteger(m_nGroupSerTimeModSum);
}

void MiscData::readObsoleteMiscData30(ArctermFile&p_file)//3.0
{
	p_file.getInteger( m_nCyclicFreq );
	int nCapaAttrCount;
	p_file.getInteger( nCapaAttrCount);
	for (int i=0; i<nCapaAttrCount; i++)
	{
		CapacityAttribute* pCapaAttr = new CapacityAttribute;
		p_file.getInteger(pCapaAttr->m_nMobElementIndex);
		p_file.getInteger(pCapaAttr->m_nCapacity);
		m_pCapacityAttributes->AddCapacityAttribute(pCapaAttr);
	}
	p_file.getInteger( m_lTerminateTime);

	char disallowGroup;
	char visitors;
	char cWaitInQueue;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;

	p_file.getChar (visitors);
	visitorFlag = (visitors == 'Y');

	p_file.getChar(cWaitInQueue);
	m_bWaitInQueue = cWaitInQueue == 'N' ? false : true;

	assert( m_pInTerm );
	gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);
	readSpecificData (p_file);
	p_file.getInteger(m_nGroupSerTimeModIndex);
	p_file.getInteger(m_nGroupSerTimeModSum);
}

void MiscData::readObsoleteMiscData31(ArctermFile&p_file)//3.1
{
	p_file.getInteger( m_nCyclicFreq );
	int nCapaAttrCount;
	p_file.getInteger( nCapaAttrCount);
	for (int i=0; i<nCapaAttrCount; i++)
	{
		CapacityAttribute* pCapaAttr = new CapacityAttribute;
		p_file.getInteger(pCapaAttr->m_nMobElementIndex);
		p_file.getInteger(pCapaAttr->m_nCapacity);
		m_pCapacityAttributes->AddCapacityAttribute(pCapaAttr);
	}
	p_file.getInteger( m_lTerminateTime);

	char disallowGroup;
	char visitors;
	char cWaitInQueue;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;

	p_file.getChar (visitors);
	visitorFlag = (visitors == 'Y');

	p_file.getChar(cWaitInQueue);
	m_bWaitInQueue = cWaitInQueue == 'N' ? false : true;

	assert( m_pInTerm );
	gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);
	readSpecificData (p_file);
	p_file.getInteger(m_nGroupSerTimeModIndex);
	p_file.getInteger(m_nGroupSerTimeModSum);
}

void MiscData::readObsoleteMiscData32( ArctermFile&p_file )
{
	p_file.getInteger( m_nCyclicFreq );
	int nCapaAttrCount;
	p_file.getInteger( nCapaAttrCount);
	for (int i=0; i<nCapaAttrCount; i++)
	{
		CapacityAttribute* pCapaAttr = new CapacityAttribute;
		p_file.getInteger(pCapaAttr->m_nMobElementIndex);
		p_file.getInteger(pCapaAttr->m_nCapacity);
		m_pCapacityAttributes->AddCapacityAttribute(pCapaAttr);
	}
	p_file.getInteger( m_lTerminateTime);

	char disallowGroup;
	char visitors;
	char cWaitInQueue;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;

	p_file.getChar (visitors);
	visitorFlag = (visitors == 'Y');

	p_file.getChar(cWaitInQueue);
	m_bWaitInQueue = cWaitInQueue == 'N' ? false : true;

	assert( m_pInTerm );
	gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);

	char cUseDependentProc;
	p_file.getChar( cUseDependentProc );
	m_bUseDependentProc = cUseDependentProc == 'Y' ? true : false;
	m_pDependentProcList->readIDList(p_file, m_pInTerm);

	readSpecificData (p_file);
	p_file.getInteger(m_nGroupSerTimeModIndex);
	p_file.getInteger(m_nGroupSerTimeModSum);
}

void MiscData::readObsoleteMiscData33( ArctermFile&p_file )	//3.3
{
	p_file.getInteger( m_nCyclicFreq );
	int nCapaAttrCount;
	p_file.getInteger( nCapaAttrCount);
	for (int i=0; i<nCapaAttrCount; i++)
	{
		CapacityAttribute* pCapaAttr = new CapacityAttribute;
		p_file.getInteger(pCapaAttr->m_nMobElementIndex);
		p_file.getInteger(pCapaAttr->m_nCapacity);
		m_pCapacityAttributes->AddCapacityAttribute(pCapaAttr);
		if(p_file.getVersion() > 3.2)
		{
			TCHAR name[1024] = {0} ;
			int version = 0 ;
			p_file.getInteger(version) ;
			pCapaAttr->m_Type = (CapacityAttributes::CapacityType)version ;
			p_file.getField(name ,1024) ;
			pCapaAttr->m_Name = name ;
			//p_file.getLine() ;
			int size = 0 ;
			p_file.getInteger(size) ;
			//p_file.getLine() ;
			CString strname ;
			for (int i = 0 ;i < size ;i++)
			{
				p_file.getField(name ,1024) ;
				pCapaAttr->AddCombinationContainTypeString(CString(name)) ;
			}
		}
		p_file.getLine() ;
	}
	p_file.getInteger( m_lTerminateTime);

	char disallowGroup;
	char visitors;
	char cWaitInQueue;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;

	p_file.getChar (visitors);
	visitorFlag = (visitors == 'Y');

	p_file.getChar(cWaitInQueue);
	m_bWaitInQueue = cWaitInQueue == 'N' ? false : true;

	assert( m_pInTerm );
	gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);

	char cUseDependentProc;
	p_file.getChar( cUseDependentProc );
	m_bUseDependentProc = cUseDependentProc == 'Y' ? true : false;
	m_pDependentProcList->readIDList(p_file, m_pInTerm);

	readSpecificData (p_file);
	p_file.getInteger(m_nGroupSerTimeModIndex);
	p_file.getInteger(m_nGroupSerTimeModSum);

}
//MobElemTypeStrDB.TXT
void MiscData::ConvertCapacityIssueIfNeed(float fFileVersion)
{
	//Convert m_lQueueCapacity to m_pCapacityAttributes
	if( (fFileVersion - 0.001)<2.9f && m_lQueueCapacity != 0)
	{
		//Passenger
		CapacityAttribute *pCapaAttr = new CapacityAttribute;
		pCapaAttr->m_nCapacity = m_lQueueCapacity;
		pCapaAttr->m_nMobElementIndex = 0;//"PASSENGER";
		m_pCapacityAttributes->AddCapacityAttribute(pCapaAttr);

	}

}

void MiscData::readObsoleteMiscData (ArctermFile& p_file )
{
	float fFileVersion = p_file.getVersion();
	if( (fFileVersion - 0.0001)<2.4f && (fFileVersion + 0.0001)>2.4f)//fFileVersion==2.4
	{
		readObsoleteMiscData24( p_file );
		ConvertCapacityIssueIfNeed(fFileVersion);
		return;
	}
	if( (fFileVersion - 0.0001)<2.5f && (fFileVersion + 0.0001)>2.5f)//fFileVersion==2.5
	{
		readObsoleteMiscData25( p_file );
		ConvertCapacityIssueIfNeed(fFileVersion);
		return;
	}

	if( (fFileVersion - 0.0001)<2.6f && (fFileVersion + 0.0001)>2.6f)//fFileVersion==2.6f
	{
		readObsoleteMiscData26( p_file );
		ConvertCapacityIssueIfNeed(fFileVersion);
		return;
	}	
	if( (fFileVersion - 0.0001)<2.7f && (fFileVersion + 0.0001)>2.7f)//fFileVersion==2.7f
	{
		readObsoleteMiscData26( p_file );
		ConvertCapacityIssueIfNeed(fFileVersion);
		return;
	}
	if( (fFileVersion - 0.0001)<2.71f && (fFileVersion + 0.0001)>2.71f)//fFileVersion==2.71f
	{									
		readObsoleteMiscData271( p_file );
		ConvertCapacityIssueIfNeed(fFileVersion);
		return;
	}	
	if( (fFileVersion - 0.0001)<2.8f && (fFileVersion + 0.0001)>2.8f)//fFileVersion==2.8f
	{									
		readObsoleteMiscData28( p_file );
		ConvertCapacityIssueIfNeed(fFileVersion);
		return;
	}	

 	if( (fFileVersion - 0.0001)<2.9f && (fFileVersion + 0.0001)>2.9f)//fFileVersion==2.9f
 	{									
 		readObsoleteMiscData29( p_file );
		ConvertCapacityIssueIfNeed(fFileVersion);
 		return;
 	}

	if( (fFileVersion - 0.0001)<3.0f && (fFileVersion + 0.0001)>3.0f)//fFileVersion==3.0f
	{									
		readObsoleteMiscData30( p_file );
		ConvertCapacityIssueIfNeed(fFileVersion);
		return;
	}

	if( (fFileVersion - 0.0001)<3.1f && (fFileVersion + 0.0001)>3.1f)//fFileVersion==3.1f
	{									
		readObsoleteMiscData31( p_file );
		return;
	}
	if((fFileVersion - 0.0001)<3.2f && (fFileVersion + 0.0001)>3.2f)//fFileVersion==3.2f
	{
		readObsoleteMiscData32(p_file) ;
		return ;
	}
	if( (fFileVersion - 0.0001)<3.3f && (fFileVersion + 0.0001)>3.3f)//fFileVersion==3.3f
	{									
		readObsoleteMiscData33( p_file );
		return;
	}
	if( (fFileVersion - 0.0001)<3.4f && (fFileVersion + 0.0001)>3.4f)//fFileVersion==3.4f
	{									
		readObsoleteMiscData34( p_file );
		return;
	}
	if ((fFileVersion - 0.0001)<3.5f && (fFileVersion + 0.0001)>3.5f)//fFileVersion==3.5f
	{
		readObsoleteMiscData35(p_file);
		return;
	}
	if ((fFileVersion - 0.0001)<3.6f && (fFileVersion + 0.0001)>3.6f)//fFileVersion==3.6f
	{
		readObsoleteMiscData35(p_file);
		return;
	}

	if( fFileVersion == 21 )
	{
		p_file.getInteger(m_lQueueCapacity);

		char disallowGroup;
		p_file.getChar( disallowGroup );
		m_bDisallowGroup = disallowGroup == 'Y' ? true : false;
	}
	// 2.1 is same as 2.2
	else if( fFileVersion > (float)2.0 )
	{
		p_file.getInteger( m_nCyclicFreq );
		p_file.getInteger(m_lQueueCapacity);
		
		char disallowGroup;
		p_file.getChar( disallowGroup );
		m_bDisallowGroup = disallowGroup == 'Y' ? true : false;
	}

    char visitors;
	p_file.getChar (visitors);
    visitorFlag = (visitors == 'Y');
	assert( m_pInTerm );
	// read gate list
	if( fFileVersion > (float)2.2 )	//more than 2.2(or >=2.3), shall read one2oneflag
		gateList->readIDList ( p_file, m_pInTerm );	
	else						//else, need not to read one2oneflag
		gateList->readIDList (p_file, m_pInTerm, false );	

	readSpecificData( p_file );
}


void MiscData::writeMiscData (ArctermFile& p_file) const
{
	p_file.writeInt( m_nCyclicFreq );
	p_file.writeInt(m_lQueueCapacity);

	p_file.writeInt(m_pCapacityAttributes->GetCount());
	for (int i=0; i< (int)m_pCapacityAttributes->GetCount(); i++)
	{
		p_file.writeInt(m_pCapacityAttributes->GetItem(i)->m_nMobElementIndex);
		p_file.writeInt(m_pCapacityAttributes->GetItem(i)->m_nCapacity);
		p_file.writeInt(m_pCapacityAttributes->GetItem(i)->m_Type) ;
		p_file.writeField(m_pCapacityAttributes->GetItem(i)->m_Name) ;
		//p_file.writeLine() ;
		p_file.writeInt((int)m_pCapacityAttributes->GetItem(i)->m_ContainData.size()) ;
		//p_file.writeLine() ;
		for (int j = 0 ;j < (int)m_pCapacityAttributes->GetItem(i)->m_ContainData.size() ;j++)
		{
			p_file.writeField(m_pCapacityAttributes->GetItem(i)->m_ContainData[j]) ;
		}
		p_file.writeLine() ;
	}

	p_file.writeInt(m_lTerminateTime);

    char disallowGroup = m_bDisallowGroup? 'Y': 'N';
    p_file.writeChar (disallowGroup);
    char visitors = (visitorFlag)? 'Y': 'N';
    p_file.writeChar (visitors);
    
	char cWaitQueue = (m_bWaitInQueue)?'Y':'N';
	p_file.writeChar(cWaitQueue);

    gateList->writeIDList (p_file);
	m_pWaitAreaList->writeIDList (p_file);
	m_pLinkedDestList->writeIDList(p_file);
	
	char useDependentPrc = m_bUseDependentProc? 'Y': 'N';
	p_file.writeChar( useDependentPrc );
	m_pDependentProcList->writeIDList(p_file);

	writeSpecificData (p_file);
	p_file.writeInt(m_nGroupSerTimeModIndex);
	p_file.writeInt(m_nGroupSerTimeModSum);
}

bool MiscData::IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID&_entryID)const
{
	bool bRet = false;
	if(gateList->IfUsedThisProcssor( _procID,_pTerm,_stringVec ))
	{

		for( int i=0; i<gateList->getCount(); ++i )
		{
			ProcessorID* pID =gateList->getItem( i );
		
			if( _procID.idFits( *pID ) )
			{
				CString str=pID->GetIDString();
				str +=" : ";
				str += "As GateList of ";
				str += _entryID.GetIDString();
				_stringVec.push_back(str);
			}
			else if( pID->idFits( _procID ) && !pID->isBlank() )
			{
				if(!_pTerm->HasBrother(_procID,*pID))
				{
					CString str=pID->GetIDString();
					str +=" : ";
					str += "As GateList of ";
					str += _entryID.GetIDString();
					_stringVec.push_back(str);
				}
			}
		}
		bRet= true;
	}
    if(m_pWaitAreaList->IfUsedThisProcssor(_procID,_pTerm,_stringVec))
	{
		for( int i=0; i<m_pWaitAreaList->getCount(); ++i )
		{
			ProcessorID* pID =m_pWaitAreaList->getItem( i );
			
			if( _procID.idFits( *pID ) )
			{
				CString str=pID->GetIDString();
				str +=" : ";
				str += "As WaitAreaList of ";
				str += _entryID.GetIDString();
				_stringVec.push_back(str);
			}
			else if( pID->idFits( _procID ) && !pID->isBlank() )
			{
				if(!_pTerm->HasBrother(_procID,*pID))
				{
					CString str=pID->GetIDString();
					str +=" : ";
					str += "As WaitAreaList of ";
					str += _entryID.GetIDString();
					_stringVec.push_back(str);
				}
			}
		}
		bRet= true;
	}
	if( m_pLinkedDestList->IfUsedThisProcssor(_procID,_pTerm,_stringVec))
	{
		for( int i=0; i<m_pLinkedDestList->getCount(); ++i )
		{
			ProcessorID* pID =m_pLinkedDestList->getItem( i );
			
			if( _procID.idFits( *pID ) )
			{
				CString str=pID->GetIDString();
				str +=" : ";
				str += "As LinkedDestList of ";
				str += _entryID.GetIDString();
				_stringVec.push_back(str);
			}
			else if( pID->idFits( _procID ) && !pID->isBlank() )
			{
				if(!_pTerm->HasBrother(_procID,*pID))
				{
					CString str=pID->GetIDString();
					str +=" : ";
					str += "As LinkedDestList of ";
					str += _entryID.GetIDString();
					_stringVec.push_back(str);
				}
			}
		}
		bRet= true;
	}
	return bRet;
}

void MiscData::SetQueueCapacity(long _iNewCapacity)
{
	m_lQueueCapacity=_iNewCapacity;
}

long MiscData::GetQueueCapacity() const
{
	return m_lQueueCapacity;
		
}

void MiscData::SetTerminateTime(long _iNewTerminateTime)
{
	m_lTerminateTime = _iNewTerminateTime;
}

long MiscData::GetTerminateTime() const
{
	return m_lTerminateTime;
}

void MiscData::SetCyclicFreq( int _nCyclicFreq )
{
	m_nCyclicFreq = _nCyclicFreq;	
}


int MiscData::GetCyclicFreq() const
{
	return m_nCyclicFreq;
}

void MiscData::readObsoleteMiscData34( ArctermFile&p_file )
{
	p_file.getInteger( m_nCyclicFreq );
	int nCapaAttrCount;
	p_file.getInteger( nCapaAttrCount);
	for (int i=0; i<nCapaAttrCount; i++)
	{
		CapacityAttribute* pCapaAttr = new CapacityAttribute;
		p_file.getInteger(pCapaAttr->m_nMobElementIndex);
		p_file.getInteger(pCapaAttr->m_nCapacity);
		m_pCapacityAttributes->AddCapacityAttribute(pCapaAttr);
		if(p_file.getVersion() > 3.2)
		{
			TCHAR name[1024] = {0} ;
			int version = 0 ;
			p_file.getInteger(version) ;
			pCapaAttr->m_Type = (CapacityAttributes::CapacityType)version ;
			p_file.getField(name ,1024) ;
			pCapaAttr->m_Name = name ;
			//p_file.getLine() ;
			int size = 0 ;
			p_file.getInteger(size) ;
			//p_file.getLine() ;
			CString strname ;
			for (int i = 0 ;i < size ;i++)
			{
				p_file.getField(name ,1024) ;
				pCapaAttr->AddCombinationContainTypeString(CString(name)) ;
			}
		}
		p_file.getLine() ;
	}
	p_file.getInteger( m_lTerminateTime);

	char disallowGroup;
	char visitors;
	char cWaitInQueue;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;

	p_file.getChar (visitors);
	visitorFlag = (visitors == 'Y');

	p_file.getChar(cWaitInQueue);
	m_bWaitInQueue = cWaitInQueue == 'N' ? false : true;

	assert( m_pInTerm );
	gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);

	char cUseDependentProc;
	p_file.getChar( cUseDependentProc );
	m_bUseDependentProc = cUseDependentProc == 'Y' ? true : false;
	m_pDependentProcList->readIDList(p_file, m_pInTerm);

	readSpecificData (p_file);

	p_file.writeInt(m_nGroupSerTimeModIndex);
	p_file.writeInt(m_nGroupSerTimeModSum);
	
}


void MiscData::readParkingLotBehaviorDB(int p_parentID)
{
	//read ParkingLot's behaviors from database
	readCurbSideListDB(p_parentID);
}
void MiscData::readCurbSideListDB(int p_parentID)
{
	//read curbSides whose parentID is p_parentID from database 
/*
	CString strSQL;
	strSQL.Format("select * from IN_LandSide_ParkingLots_Constrained");
	int i=0;
	for(i;i<(int)m_pParkingLotList.size();i++)
	{
		CParkingLotBehavior *tmpPax=m_pParkingLotList.getItem(i);
		delete tmpPax;
		tmpPax=NULL;
	}
	for(i=0;i<(int)m_pDeleteParkingLotList.size();i++)
	{
		delete m_pDeleteParkingLotList.at(i);
		m_pDeleteParkingLotList.at(i)=NULL;
	}
	m_pParkingLotList.clear();
	m_pDeleteParkingLotList.clear();
	long lCount=0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,lCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CParkingLotBehavior *tmpItem=new CParkingLotBehavior();
		tmpItem->initDataFromDB(adoRecordset);		
		tmpItem->readData();
		m_pParkingLotList.addItem(tmpItem);
		adoRecordset.MoveNextData();
	}*/

}
void MiscData::saveDataToDB(int parentID)
{
	saveCurbSideListToDB(parentID);
}
void MiscData::readDataFromDB(int parentID)
{
	readCurbSideListFromDB(parentID);
}
void MiscData::saveCurbSideListToDB(int parentID)
{
/*
	try
	{
		CADODatabase::BeginTransaction() ;
		CString strSQL;
		if (m_nID < 0)
		{
			GetInsertSQL(nParentID,strSQL);

			if (strSQL.IsEmpty())
				return;

			m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		}
		else
		{
			GetUpdateSQL(strSQL);
			if (strSQL.IsEmpty())
				return;

			CADODatabase::ExecuteSQLStatement(strSQL);
		}
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
*/
}
void MiscData::readCurbSideListFromDB(int parentID)
{

}
void MiscData::addCurbSide(const ALTObjectID &p_item)
{
	curbSideList->Add(p_item);
	curbSideListAdd->Add(p_item);
}
void MiscData::delCurbSide(int nIndex)
{
	curbSideListDelete->push_back(curbSideList->at(nIndex));
	curbSideList->erase(curbSideList->begin()+nIndex);
}
int MiscData::getCurbSideCount()
{
	return (int)curbSideList->size();
}
bool MiscData::existCurbSide(const ALTObjectID &p_item)
{
	for (int i=0;(size_t)i<curbSideList->size();i++)
	{
		if (p_item==curbSideList->at(i))
		{
			return true;
		}
	}
	return false;
}
ALTObjectID &MiscData::getCurbSide(int nIndex)const
{
	return curbSideList->at(nIndex);
}

void MiscData::readObsoleteMiscData35( ArctermFile&p_file )
{
	p_file.getInteger( m_nCyclicFreq );
	p_file.getInteger(m_lQueueCapacity);
	int nCapaAttrCount;
	p_file.getInteger( nCapaAttrCount);
	for (int i=0; i<nCapaAttrCount; i++)
	{
		CapacityAttribute* pCapaAttr = new CapacityAttribute;
		p_file.getInteger(pCapaAttr->m_nMobElementIndex);
		p_file.getInteger(pCapaAttr->m_nCapacity);
		m_pCapacityAttributes->AddCapacityAttribute(pCapaAttr);
		if(p_file.getVersion() > 3.2)
		{
			TCHAR name[1024] = {0} ;
			int version = 0 ;
			p_file.getInteger(version) ;
			pCapaAttr->m_Type = (CapacityAttributes::CapacityType)version ;
			p_file.getField(name ,1024) ;
			pCapaAttr->m_Name = name ;
			//p_file.getLine() ;
			int size = 0 ;
			p_file.getInteger(size) ;
			//p_file.getLine() ;
			CString strname ;
			for (int i = 0 ;i < size ;i++)
			{
				p_file.getField(name ,1024) ;
				pCapaAttr->AddCombinationContainTypeString(CString(name)) ;
			}
		}
		p_file.getLine() ;
	}
	p_file.getInteger( m_lTerminateTime);

	char disallowGroup;
	char visitors;
	char cWaitInQueue;
	p_file.getChar( disallowGroup );
	m_bDisallowGroup = disallowGroup == 'Y' ? true : false;

	p_file.getChar (visitors);
	visitorFlag = (visitors == 'Y');

	p_file.getChar(cWaitInQueue);
	m_bWaitInQueue = cWaitInQueue == 'N' ? false : true;

	assert( m_pInTerm );
	gateList->readIDList (p_file, m_pInTerm );
	m_pWaitAreaList->readIDList (p_file, m_pInTerm );
	m_pLinkedDestList->readIDList(p_file,  m_pInTerm);

	char cUseDependentProc;
	p_file.getChar( cUseDependentProc );
	m_bUseDependentProc = cUseDependentProc == 'Y' ? true : false;
	m_pDependentProcList->readIDList(p_file, m_pInTerm);

	readSpecificData (p_file);
	p_file.getInteger(m_nGroupSerTimeModIndex);
	p_file.getInteger(m_nGroupSerTimeModSum);
}
void MiscDependentData::readSpecificData (ArctermFile& p_file )
{
	assert( m_pInTerm );
	bool bWithOneToOne = true;
	// only when version >=2.3,shall read one2oneflag
	if( p_file.getVersion() < (float)2.3 )
		bWithOneToOne = false;
	dependentList->readIDList (p_file, m_pInTerm , bWithOneToOne );
}
bool MiscDependentData::IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID)const
{
	bool bRet = false;
	if( dependentList->IfUsedThisProcssor( _procID,_pTerm,_stringVec ) )
	{
		for( int i=0; i<dependentList->getCount(); ++i )
		{
			ProcessorID* pID =dependentList->getItem( i );
			
			if( _procID.idFits( *pID ) )
			{
				CString str=pID->GetIDString();
				str +=" : ";
				str += "As depandentList of ";
				str += _entryID.GetIDString();
				_stringVec.push_back(str);
			}
			else if( pID->idFits( _procID ) && !pID->isBlank() )
			{
				if(!_pTerm->HasBrother(_procID,*pID))
				{
					CString str=pID->GetIDString();
					str +=" : ";
					str += "As depandentList of ";
					str += _entryID.GetIDString();
					_stringVec.push_back(str);
				}
			}
		}
		bRet= true;
	}
	if(MiscData::IfUsedThisProcssor( _procID,_pTerm,_stringVec,_entryID ))
	{
		bRet =true;
	}
	return bRet;
}
void MiscDepSinkData::readSpecificData (ArctermFile& p_file)
{
	assert( m_pInTerm );
	bool bWithOneToOne = true;
	// only when version >=2.3,shall read one2oneflag
	if( p_file.getVersion() < (float)2.3 )
		bWithOneToOne = false;
	dependentList->readIDList (p_file, m_pInTerm , bWithOneToOne );
}

void MiscDependentData::writeSpecificData (ArctermFile& p_file) const
{
    dependentList->writeIDList (p_file);
}

void MiscGateData::readSpecificData (ArctermFile& p_file)
{
	assert( m_pInTerm );
	bool bWithOneToOne = true;
	// only when version >=2.3,shall read one2oneflag
	if( p_file.getVersion() < (float)2.3 )
		bWithOneToOne = false;
    holdAreaList->readIDList (p_file, m_pInTerm, bWithOneToOne);

	if (p_file.getVersion() >(float)3.3)
		m_pBridgeConnectorList->readIDList(p_file,m_pInTerm,bWithOneToOne);
}

void MiscGateData::writeSpecificData (ArctermFile& p_file) const
{
    holdAreaList->writeIDList (p_file);

	if (p_file.getVersion() >(float)3.3)
		m_pBridgeConnectorList->writeIDList (p_file);
}

bool MiscGateData::IfUsedThisProcssor( const ProcessorID& _procID,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID )const
{
	bool bret =false;
	if( holdAreaList->IfUsedThisProcssor( _procID,_pTerm,_stringVec ) )
	{
		for( int i=0; i<holdAreaList->getCount(); ++i )
		{
			ProcessorID* pID =holdAreaList->getItem( i );
			
			if( _procID.idFits( *pID ) )
			{
				CString str=pID->GetIDString();
				str +=" : ";
				str += "As holdAreaList of ";
				str += _entryID.GetIDString();
				_stringVec.push_back(str);
			}
			else if( pID->idFits( _procID ) && !pID->isBlank() )
			{
				if(!_pTerm->HasBrother(_procID,*pID))
				{
					CString str=pID->GetIDString();
					str +=" : ";
					str += "As holdAreaList of ";
					str += _entryID.GetIDString();
					_stringVec.push_back(str);
				}
			}
		}
		bret = true;
	}
    if(MiscData::IfUsedThisProcssor( _procID ,_pTerm,_stringVec,_entryID))
	{
		bret =true;
	}
	return bret;
}
	
void MiscFloorChangeData::readSpecificData (ArctermFile& p_file)
{
	assert( m_pInTerm );
	bool bWithOneToOne = true;
	// only when version >=2.3,shall read one2oneflag
	if( p_file.getVersion() < (float)2.3 )
		bWithOneToOne = false;
    connectionList->readIDList (p_file, m_pInTerm, bWithOneToOne);
}

void MiscFloorChangeData::writeSpecificData (ArctermFile& p_file) const
{
    connectionList->writeIDList (p_file);
}
bool MiscFloorChangeData::IfUsedThisProcssor( const ProcessorID& _procID,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID )const
{
	bool bret = false;
	if( connectionList->IfUsedThisProcssor( _procID ,_pTerm,_stringVec) )
	{
		for( int i=0; i<connectionList->getCount(); ++i )
		{
			ProcessorID* pID =connectionList->getItem( i );
			
			if( _procID.idFits( *pID ) )
			{
				CString str=pID->GetIDString();
				str +=" : ";
				str += "As connectionList of ";
				str += _entryID.GetIDString();
				_stringVec.push_back(str);
			}
			else if( pID->idFits( _procID ) && !pID->isBlank() )
			{
				if(!_pTerm->HasBrother(_procID,*pID))
				{
					CString str=pID->GetIDString();
					str +=" : ";
					str += "As connectionList of ";
					str += _entryID.GetIDString();
					_stringVec.push_back(str);
				}
			}
		}
		bret= true;
	}
    if(MiscData::IfUsedThisProcssor( _procID,_pTerm,_stringVec ,_entryID))
	{
		bret = true;
	}
	return bret;
}


MiscHoldAreaData::MiscHoldAreaData() 
{
	sourceProcList = new ProcessorIDList;
	sourceProcList->ownsElements( 1 );
	destProcList = new ProcessorIDList; 
	destProcList->ownsElements( 1 );
	radius = 5.0;
	m_nStageNumber = 0;
	m_bApplyServiceTimeAfterGreeting = false;

	m_bCorralService = false;
	//seconds
	m_nTimeToService = 60;

}
void MiscHoldAreaData::readSpecificData (ArctermFile& p_file)
{
    p_file.getFloat (radius);
	assert( m_pInTerm );
	bool bWithOneToOne = true;
	float fFileVersion = p_file.getVersion();
	// only when version >=2.3,shall read one2oneflag
	if( fFileVersion < (float)2.3 )
		bWithOneToOne = false;
    sourceProcList->readIDList (p_file, m_pInTerm ,bWithOneToOne);
    destProcList->readIDList (p_file, m_pInTerm,bWithOneToOne );
	if( fFileVersion > 2.3 )
		p_file.getInteger( m_nStageNumber );

	if( fFileVersion > 3.0 )
	{
		int nValue = 0;
		p_file.getInteger( nValue );  // look bad, but just don't want change the version again.
		m_bApplyServiceTimeAfterGreeting = ( nValue == 1 ? true : false );
	}

	if (fFileVersion > 3.5)
	{
		int nValue = 0;
		p_file.getInteger(nValue);
		m_bCorralService = (nValue == 1 ? true : false);

		p_file.getInteger(m_nTimeToService);
		if(fFileVersion > 3.6)//3.7
		{
			//3.7 or later
		}
		else
		{
			//3.6, 3.5 ..earlier
			m_nTimeToService = 60;
		}
	}

	//if( m_nStageNumber <= 0 )
	//{
	//	if( gateList && gateList->getCount() > 0 )
	//	{
	//		m_nStageNumber = 1;
	//	}
	//}
}

void MiscHoldAreaData::writeSpecificData (ArctermFile& p_file) const
{
    p_file.writeFloat (radius, 2);
    sourceProcList->writeIDList (p_file);
    destProcList->writeIDList (p_file);
	p_file.writeInt( m_nStageNumber );
	p_file.writeInt( m_bApplyServiceTimeAfterGreeting ? 1 : 0 );

	p_file.writeInt(m_bCorralService ? 1 : 0);
	p_file.writeInt(m_nTimeToService);
}
bool MiscHoldAreaData::IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID&_entryID)const
{
	bool bret = false;
	if( sourceProcList->IfUsedThisProcssor( _procID,_pTerm,_stringVec ) || destProcList->IfUsedThisProcssor( _procID,_pTerm,_stringVec ) )
	{
		for( int i=0; i<sourceProcList->getCount(); ++i )
		{
			ProcessorID* pID =sourceProcList->getItem( i );
			
			if( _procID.idFits( *pID ) )
			{
				CString str=pID->GetIDString();
				str +=" : ";
				str += "As sourceProcList of ";
				str += _entryID.GetIDString();
				_stringVec.push_back(str);
			}
			else if( pID->idFits( _procID ) && !pID->isBlank() )
			{
				if(!_pTerm->HasBrother(_procID,*pID))
				{
					CString str=pID->GetIDString();
					str +=" : ";
					str += "As sourceProcList of ";
					str += _entryID.GetIDString();
					_stringVec.push_back(str);
				}
			}
		}
			for(  i=0; i<destProcList->getCount(); ++i )
			{
				ProcessorID* pID =destProcList->getItem( i );
				
				if( _procID.idFits( *pID ) )
				{
					CString str=pID->GetIDString();
					str +=" : ";
					str += "As destProcList of ";
					str += _entryID.GetIDString();
					_stringVec.push_back(str);
				}
				else if( pID->idFits( _procID ) && !pID->isBlank() )
				{
					if(!_pTerm->HasBrother(_procID,*pID))
					{
						CString str=pID->GetIDString();
						str +=" : ";
						str += "As destProcList of ";
						str += _entryID.GetIDString();
						_stringVec.push_back(str);
					}
				}
			}
		bret = true;
	}
	if(MiscData::IfUsedThisProcssor( _procID,_pTerm,_stringVec,_entryID ))
	{
		bret = false;
	}
	return bret;
}

MiscBaggageData::MiscBaggageData() 
{ 
	bypassProcessor = new MiscProcessorIDList; 
	m_pHoldAreaList = new MiscProcessorIDList;
	bypassProcessor->ownsElements( 1 );
	capacity = 1000; 
}

void MiscBaggageData::readSpecificData (ArctermFile& p_file)
{
    p_file.getInteger (capacity);
	assert( m_pInTerm );
	bool bWithOneToOne = true;
	// only when version >=2.3,shall read one2oneflag
	if( p_file.getVersion() < (float)2.3 )
		bWithOneToOne = false;
    bypassProcessor->readIDList (p_file, m_pInTerm,bWithOneToOne );
	if (p_file.getVersion() > (float)2.7 ) 
	{
		m_pHoldAreaList->readIDList(p_file, m_pInTerm,bWithOneToOne);
	}
}

void MiscBaggageData::writeSpecificData (ArctermFile& p_file) const
{
    p_file.writeInt (capacity);
    bypassProcessor->writeIDList (p_file);
	m_pHoldAreaList->writeIDList(p_file);	
}

bool MiscBaggageData::IfUsedThisProcssor( const ProcessorID& _procID,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID )const
{
	bool bret = false;
	if( bypassProcessor->IfUsedThisProcssor( _procID,_pTerm,_stringVec ) )
	{
		for( int i=0; i<bypassProcessor->getCount(); ++i )
		{
			ProcessorID* pID =bypassProcessor->getItem( i );
			
			if( _procID.idFits( *pID ) )
			{
				CString str=pID->GetIDString();
				str +=" : ";
				str += "As bypass of ";
				str += _entryID.GetIDString();
				_stringVec.push_back(str);
			}
			else if( pID->idFits( _procID ) && !pID->isBlank() )
			{
				if(!_pTerm->HasBrother(_procID,*pID))
				{
					CString str=pID->GetIDString();
					str +=" : ";
					str += "As bypass of ";
					str += _entryID.GetIDString();
					_stringVec.push_back(str);
				}
			}
		}
		bret = true;
	}
	if(MiscData::IfUsedThisProcssor( _procID,_pTerm,_stringVec,_entryID ))
	{
		bret = true;
	}
	return bret;
}

void MiscIntegratedStation::readSpecificData (ArctermFile& p_file)
{
	m_listStation.readIDList(p_file,m_pInTerm,false);
}

void MiscIntegratedStation::writeSpecificData (ArctermFile& p_file) const
{
	m_listStation.writeIDList (p_file);
}

void MiscElevatorData::readSpecificData (ArctermFile& p_file)
{
		p_file.getInteger (m_nCapacity);
		p_file.getFloat(m_dSpeed[0]);		
		p_file.getFloat(m_dSpeed[1]);
		p_file.getFloat(m_dSpeed[2]);
		int nCount;
		p_file.getInteger(nCount);
		for(int i=0;i<nCount;i++)
		{
			BOOL bValue;
			p_file.getInteger(bValue);
			m_vbStopAtFloor.push_back(bValue);
		}
		
}

void MiscElevatorData::writeSpecificData (ArctermFile& p_file) const
{
	p_file.writeInt (m_nCapacity);
	p_file.writeDouble(m_dSpeed[0]);		
	p_file.writeDouble(m_dSpeed[1]);
	p_file.writeDouble(m_dSpeed[2]);
	int nCount=m_vbStopAtFloor.size();
	p_file.writeInt(nCount);
	for(unsigned i=0;i<m_vbStopAtFloor.size();i++)
		p_file.writeInt(m_vbStopAtFloor[i]);

}

const double MiscConveyorData::DefaultSpeed = 100.0;

MiscConveyorData::MiscConveyorData():m_enumSubType( SIMPLE_CONVEYOR )
{
	m_enumSubType = SIMPLE_CONVEYOR;
	m_dSpeed = 100.0;
	
	m_bPusherReleasedByFull = false;
	m_bPusherReleasedByNextAvail = false;
	m_bPusherReleasedByMaxTime = false;
	m_bPusherReleasedBySchedPick = false;
	m_bReversible = false;
	m_lQueueCapacity = -1;
    m_pExitBeltList = new MiscProcessorIDList; m_pExitBeltList->ownsElements( 1 );
	m_lTerminateTime = -1;
	
}
MiscConveyorData::~MiscConveyorData()
{

}


void MiscConveyorData::readSpecificData (ArctermFile& p_file)
{
	short nVal;
	p_file.getInteger( nVal );
	m_enumSubType = (enum SUBCONVEYORTYPE)nVal;
	p_file.getTime( m_serviceTime, TRUE );
	p_file.getTime( m_randomTime, TRUE );
	p_file.getTime( m_scheduleTime, TRUE );
	p_file.getFloat( m_dSpeed );		
	char c;
	p_file.getChar( c );
	if( c == 'T' )
		m_bPusherReleasedByFull = true;
	else
		m_bPusherReleasedByFull = false;
	p_file.getChar( c );
	if( c == 'T' )
		m_bPusherReleasedByNextAvail = true;
	else
		m_bPusherReleasedByNextAvail = false;
	p_file.getChar( c );
	if( c == 'T' )
		m_bPusherReleasedByMaxTime = true;
	else
		m_bPusherReleasedByMaxTime = false;
	p_file.getChar( c );
	if( c == 'T' )
		m_bPusherReleasedBySchedPick = true;
	else
		m_bPusherReleasedBySchedPick = false;
	p_file.getChar( c );
	if( c == 'T' )
		m_bReversible = true;
	else
		m_bReversible = false;
	m_pExitBeltList->readIDList(p_file,m_pInTerm);
}


void MiscConveyorData::writeSpecificData (ArctermFile& p_file) const
{
	p_file.writeInt( (int)m_enumSubType );
	p_file.writeTime( m_serviceTime, TRUE );
	p_file.writeTime( m_randomTime, TRUE );
	p_file.writeTime( m_scheduleTime, TRUE );
	p_file.writeDouble( m_dSpeed );
	p_file.writeChar( m_bPusherReleasedByFull? 'T' : 'F' );
	p_file.writeChar( m_bPusherReleasedByNextAvail? 'T' : 'F' );
	p_file.writeChar( m_bPusherReleasedByMaxTime? 'T' : 'F' );
	p_file.writeChar( m_bPusherReleasedBySchedPick? 'T' : 'F' );
	p_file.writeChar( m_bReversible ? 'T' : 'F' );

	m_pExitBeltList->writeIDList(p_file);
}

bool MiscConveyorData::IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID ) const
{
	bool bret = false;
	if( m_pExitBeltList->IfUsedThisProcssor( _procID,_pTerm,_stringVec ) )
	{
		for( int i=0; i<m_pExitBeltList->getCount(); ++i )
		{
			ProcessorID* pID =m_pExitBeltList->getItem( i );

			if( _procID.idFits( *pID ) )
			{
				CString str=pID->GetIDString();
				str +=" : ";
				str += "As exit belt of ";
				str += _entryID.GetIDString();
				_stringVec.push_back(str);
			}
			else if( pID->idFits( _procID ) && !pID->isBlank() )
			{
				if(!_pTerm->HasBrother(_procID,*pID))
				{
					CString str=pID->GetIDString();
					str +=" : ";
					str += "As exit belt of ";
					str += _entryID.GetIDString();
					_stringVec.push_back(str);
				}
			}
		}
		bret = true;
	}
	if(MiscData::IfUsedThisProcssor( _procID,_pTerm,_stringVec,_entryID ))
	{
		bret = true;
	}
	return bret;
}

void MiscProcessorIDListWithLevel::readObsoleteIDList24 (ArctermFile& p_file, InputTerminal* _pInTerm,bool _bWithOneToOne )
{
	clear();
    char *str1, *str2, *str3, str[1024];
    p_file.getField (str, 1024);
    if (!str[0])
        return;
	
    str2 = str - 1;
    MiscProcessorIDWithLevel *id;
	int nLevel = 1;
    while (str2)
    {
        str1 = str2 + 1;
        str2 = strstr (str1, ";");
        if (str2)
            str2[0] = '\0';
		
        id = new MiscProcessorIDWithLevel;
		id->SetStrDict( _pInTerm->inStrDict );
		if( _bWithOneToOne )
		{
			str3 = strstr( str1, "@" );
			if( str3 )
				str3[0] = '\0';
			
			id->setID (str1);
			id->SetLevel( nLevel++ );
		}
		else
		{	
			id->setID( str1 );
			id->SetLevel( nLevel++ );
		}
        addItem (id);
    }
}
//////////////////////////////////////////////////////////////////////////
void MiscProcessorIDListWithLevel::readIDList (ArctermFile& p_file, InputTerminal* _pInTerm,bool _bWithOneToOne )
{
	if( p_file.getVersion() < 2.5 )
	{
		readObsoleteIDList24(p_file, _pInTerm,_bWithOneToOne);
		return;
	}
	clear();
    char *str1, *str2,*str3,*str4, str[1024];
	char cLevel[10]="";
	int nLevel=0;
    p_file.getField (str, 1024);
    if (!str[0])
        return;
	
    str2 = str - 1;
    MiscProcessorIDWithLevel *id;
    while (str2)
    {
        str1 = str2 + 1;
        str2 = strstr (str1, ";");
		str3 = str2 + 1;
        if (str2)
            str2[0] = '\0';
		str4 = strstr(str3,";");
		str2=str4;
		if (str4)
		{
			str4[0]='\0';
		}		
		strcpy(cLevel,str3);
		nLevel=atoi(cLevel);
		
        id = new MiscProcessorIDWithLevel;
		id->SetStrDict( _pInTerm->inStrDict );
        id->setID (str1);
		id->SetLevel(nLevel);
        addItem (id);
    }
}


void MiscProcessorIDListWithLevel::writeIDList (ArctermFile& p_file) const
{
    char str[1024] = "";
	char cLevel[10]="";
	MiscProcessorIDWithLevel * pProcWithLevel=NULL;
    if (getCount())
	{
        (getItem (0))->printID (str);
		pProcWithLevel=(MiscProcessorIDWithLevel *)getItem(0);
		
    	int nLevel=((MiscProcessorIDWithLevel *)(getItem (0)))->GetLevel();		
	    itoa(nLevel,cLevel,10);
		strcat(str,";");
		strcat(str,cLevel);
	}

    for (int i = 1; i < getCount(); i++)
    {
        strcat (str, ";");
		int nLevel=((MiscProcessorIDWithLevel *)(getItem (i)))->GetLevel();		
		itoa(nLevel,cLevel,10);
        (getItem (i))->printID (str + strlen (str));
		strcat(str,";");		
		strcat(str,cLevel);				
    }
    p_file.writeField (str);
}

void MiscEscalatorData::readSpecificData(ArctermFile& p_file)
{
	p_file.getInteger (m_nCapacity);
	p_file.getFloat(m_dMovingSpeed);		
}

void MiscEscalatorData::writeSpecificData(ArctermFile& p_file) const
{
	p_file.writeInt ( m_nCapacity );
	p_file.writeDouble( m_dMovingSpeed );	
}
//////////////////////////////////////////////////////////////////////////
MiscBillboardData::MiscBillboardData()
{
//	m_dBillBoardCoefficient = 0.500;
	m_nTimeRemaining = 30;
	m_nTimeStop = 30;
	m_pBillBoardLinkedProcList = new MiscProcessorIDList;
}
MiscBillboardData::MiscBillboardData(const MiscBillboardData& anothermiscData)
					:MiscData(anothermiscData)
{
//	m_dBillBoardCoefficient = anothermiscData.m_dBillBoardCoefficient;
	m_nTimeRemaining = anothermiscData.m_nTimeRemaining;
	m_nTimeStop = anothermiscData.m_nTimeStop;
	*m_pBillBoardLinkedProcList = *(anothermiscData.m_pBillBoardLinkedProcList);
}

MiscBillboardData::~MiscBillboardData()
{
	if (m_pBillBoardLinkedProcList) 
		delete m_pBillBoardLinkedProcList;
}
void MiscBillboardData::readSpecificData(ArctermFile& p_file)
{
//	p_file.getFloat(m_dBillBoardCoefficient);
	p_file.getInteger(m_nTimeRemaining);
	p_file.getInteger(m_nTimeStop);
	assert( m_pInTerm );
	m_pBillBoardLinkedProcList->readIDList(p_file,m_pInTerm);
	
	int nValue = 0;
	p_file.getInteger(nValue);
	m_defaultPaxTypeCoefficient.setPropernsityCoefficient(nValue);

	nValue = 0;
	p_file.getInteger(nValue);
	m_defaultPaxTypeCoefficient.setBuyCoefficient(nValue);
	
	int nCount =0;
	p_file.getInteger(nCount);
	for (int i =0; i< nCount; i++)
	{
		CPaxTypeCoefficientItem item;
		
		//pax type
		CMobileElemConstraint paxType(m_pInTerm);
		//paxType.SetInputTerminal( m_pInTerm );
		paxType.readConstraint( p_file );
		//propensity coefficient

		int nPropensityCoefficient =0;
		int nBuyCoefficient =0;

		p_file.getInteger(nPropensityCoefficient);
		p_file.getInteger(nBuyCoefficient);

		item.setPaxType(paxType);
		item.setPropernsityCoefficient(nPropensityCoefficient);
		item.setBuyCoefficient(nBuyCoefficient);

		m_vPaxTypeCoefficient.push_back(item);
	}

}
void MiscBillboardData::writeSpecificData(ArctermFile& p_file) const
{
//	p_file.writeDouble(m_dBillBoardCoefficient);
	p_file.writeInt(m_nTimeRemaining);
	p_file.writeInt(m_nTimeStop);
	m_pBillBoardLinkedProcList->writeIDList(p_file);

	//default
	p_file.writeInt(m_defaultPaxTypeCoefficient.getPropensityCoefficient());
	p_file.writeInt(m_defaultPaxTypeCoefficient.getBuyCoefficient());

	int nCount = static_cast<int>(m_vPaxTypeCoefficient.size());
	
	p_file.writeInt(nCount);
	for (int i=0; i< nCount; i++)
	{	
		char szBuf[2560];
		m_vPaxTypeCoefficient[i].getPaxType().WriteSyntaxStringWithVersion( szBuf );
		p_file.writeField( szBuf );

		p_file.writeInt(m_vPaxTypeCoefficient[i].getPropensityCoefficient());
		p_file.writeInt(m_vPaxTypeCoefficient[i].getBuyCoefficient());
	}

}

bool MiscBillboardData::FindCoefficient(const CMobileElemConstraint& paxType,int& nPropensityCoefficient,int& nBuyCoefficient )
{
	// now check which pax type is match

	int nItemCount =static_cast<int>(m_vPaxTypeCoefficient.size());
	int nCritCount = -1;
	int nHitIndex = -1;
	for( int m=0; m<nItemCount; m++ )
	{
		CPaxTypeCoefficientItem item = m_vPaxTypeCoefficient[m];
		CString strPaxType;
		paxType.screenPrint(strPaxType);
		if( item.getPaxType().fits( paxType ) )
		{
			int n = item.getPaxType().getCriteriaCount();
			if( n > nCritCount )
			{
				nCritCount = n;
				nHitIndex = m;
			}
		}
	}
	if( nHitIndex != -1 )
	{
		nPropensityCoefficient = m_vPaxTypeCoefficient[nHitIndex].getPropensityCoefficient();
		nBuyCoefficient = m_vPaxTypeCoefficient[nHitIndex].getBuyCoefficient();
		return true;
	}

    nPropensityCoefficient = m_defaultPaxTypeCoefficient.getPropensityCoefficient();
	nBuyCoefficient = m_defaultPaxTypeCoefficient.getBuyCoefficient();
	return true;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
MiscBridgeConnectorData::MiscBridgeConnectorData()
{
	m_pStandLinkedProcList = new MiscProcessorIDList();
}

MiscBridgeConnectorData::MiscBridgeConnectorData(const MiscBridgeConnectorData& anothermiscData)
{
	*m_pStandLinkedProcList = *(anothermiscData.m_pStandLinkedProcList);
}

MiscBridgeConnectorData::~MiscBridgeConnectorData()
{
	if (m_pStandLinkedProcList)
	{
		delete m_pStandLinkedProcList;
		m_pStandLinkedProcList = NULL;
	}
}

void MiscBridgeConnectorData::readSpecificData(ArctermFile& p_file)
{
	m_pStandLinkedProcList->readIDList(p_file,m_pInTerm);
}

void MiscBridgeConnectorData::writeSpecificData(ArctermFile& p_file)const
{
	m_pStandLinkedProcList->writeIDList(p_file);
}



//////////////////////////////////////////////////////////////////////////
//MiscRetailProcData
MiscRetailProcData::MiscRetailProcData()
{
	//m_checkoutProcList.initDefault(m_pInTerm);
	//m_bypassProcList.initDefault(m_pInTerm);
}

MiscRetailProcData::MiscRetailProcData( const MiscRetailProcData& anothermiscData )
{
	m_vConditions	= anothermiscData.m_vConditions;
	m_bypassProcList	= anothermiscData.m_bypassProcList;
	m_checkoutProcList	= anothermiscData.m_checkoutProcList;
}

MiscRetailProcData::~MiscRetailProcData()
{

}

void MiscRetailProcData::readSpecificData( ArctermFile& p_file )
{

	m_bypassProcList.readIDList(p_file, m_pInTerm);
	m_checkoutProcList.readIDList(p_file,m_pInTerm);
	p_file.getLine();

	int nCount;
	p_file.getInteger(nCount);
	for (int nItem = 0; nItem < nCount; ++ nItem )
	{
		PaxSkipCondition *cond = new PaxSkipCondition;
		cond->m_paxType.SetInputTerminal(m_pInTerm);
		cond->m_paxType.readConstraint(p_file);
		int nLimit;
		p_file.getInteger(nLimit);
		if(nLimit> 0)
			cond->setTimeLimit(true);
		else
			cond->setTimeLimit(false);
		p_file.getInteger(cond->m_nMins);

		p_file.getInteger(nLimit);
		if(nLimit> 0)
			cond->setQLimit(true);
		else
			cond->setQLimit(false);

		p_file.getInteger(cond->m_nQlenth);

		m_vConditions.push_back(cond);
	}
}

void MiscRetailProcData::writeSpecificData( ArctermFile& p_file ) const
{
	m_bypassProcList.writeIDList(p_file);
	m_checkoutProcList.writeIDList(p_file);

	p_file.writeLine();

	int nCount = static_cast<int>(m_vConditions.size());
	p_file.writeInt(nCount);
	for (int nItem = 0; nItem < nCount; ++ nItem )
	{
		const PaxSkipCondition* cond = m_vConditions[nItem];
		
		cond->m_paxType.writeConstraint(p_file);

		if(cond->hasTimeLimit())
			p_file.writeInt(1);
		else
			p_file.writeInt(0);

		p_file.writeInt(cond->m_nMins);

		if(cond->hasQLimit())
			p_file.writeInt(1);
		else
			p_file.writeInt(0);

		p_file.writeInt(cond->m_nQlenth);
	}

}

bool MiscRetailProcData::PaxSkipVacant( const CMobileElemConstraint& type,int nLength,const ElapsedTime& eTime ) const
{
	int nCount = static_cast<int>(m_vConditions.size());
	for (int i = 0; i < nCount; i++)
	{
		const PaxSkipCondition* cond = m_vConditions.at(i);

		if (!cond->getConstraint().fits(type))
			continue;

		if (cond->hasTimeLimit())
		{
			long lMins = cond->getMins();
			ElapsedTime eMins;
			eMins.setPrecisely(lMins*60l*100l);
			if (eTime <= eMins)
				return false;
		}

		if (cond->hasQLimit())
		{
			if (nLength >= cond->getQlenth())
				return false;
		}

		return true;
	}

	return true;
}

bool MiscRetailProcData::PaxQueueLenghtVacant( const CMobileElemConstraint& type,int nLength ) const
{
	int nCount = static_cast<int>(m_vConditions.size());
	for (int i = 0; i < nCount; i++)
	{
		const PaxSkipCondition* cond = m_vConditions.at(i);

		if (!cond->getConstraint().fits(type))
			continue;

		if (cond->hasQLimit())
		{
			if (nLength >= cond->getQlenth())
				return false;
		}

		return true;
	}

	return true;
}

void MiscRetailProcData::SortSkipCondition()
{
	std::sort(m_vConditions.begin(),m_vConditions.end(),CompareSkipCondition);
}

bool MiscRetailProcData::CompareSkipCondition( const PaxSkipCondition*pC1, const PaxSkipCondition* pC2 )
{
	ASSERT(pC2 != NULL&& pC1 != NULL);
	if(pC1==NULL || pC2 == NULL)
		return false;

	if(pC1->getConstraint().fits(pC2->getConstraint()))
		return false;

	return true;
}
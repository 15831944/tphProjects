// Floors.cpp: implementation of the CFloors class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Floors.h"
#include "Floor2.h"
#include "common\projectmanager.h"
#include "common\exeption.h"
#include "common\UndoManager.h"
#include "assert.h"
#include "FloorMapMatch.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
///2.7: add guid for floor 
//////////////////////////////////////////////////////////////////////////
CFloors::CFloors()
	:DataSet( FloorsFile, 2.8f )
{
	m_bReadOnly = false;
}

CFloors::CFloors( int szType)
:DataSet( szType, 2.8f )
{
	m_bReadOnly = false;
}

CFloors::~CFloors()
{
	clear();
}


void CFloors::clear()
{
	int nCount = m_vFloors.size();
	for( int i=0; i<nCount; i++ )
		delete m_vFloors[i];

	m_vFloors.clear();
}

// exception: FileVersionTooNewError
void CFloors::loadDataSet (const CString& _pProjPath)
{
    clear();

    char filename[_MAX_PATH];
    PROJMANAGER->getFileName (_pProjPath, fileType, filename);

    ArctermFile file;
    try { file.openFile (filename, READ); }
    catch (FileOpenError *exception)
    {
		delete exception;
        initDefaultValues();
        saveDataSet(_pProjPath, false);
        return;
    }

	float fVersionInFile = file.getVersion();

	if( fVersionInFile < m_fVersion || fVersionInFile == 21 )
    {
        readObsoleteData( file );
        file.closeIn();
		readLayerInfo(_pProjPath);
		readVRInfo(_pProjPath);
        saveDataSet(_pProjPath, false);
    }
    else if( fVersionInFile > m_fVersion )
	{
		// should stop read the file.
		file.closeIn();
		throw new FileVersionTooNewError( filename );		
	}
	else
    {
        readData (file);
        file.closeIn();
		readLayerInfo(_pProjPath);
		readVRInfo(_pProjPath);
    }
	
	//saveSet

		file.openFile (filename, WRITE, m_fVersion);
        file.writeField (getTitle());
        file.writeLine();
        file.writeField (getHeaders());
        file.writeLine();
        writeData (file);
        file.endFile();
	    writeLayerInfo(_pProjPath);
	    writeVRInfo(_pProjPath);

	
}

void CFloors::saveDataSet (const CString& _pProjPath, bool _bUndo) const
{
	//if( _bUndo )
	//{
	//	CUndoManager* undoMan = CUndoManager::GetInStance( _pProjPath );
	//	if( !undoMan->AddNewUndoProject() )
	//		AfxMessageBox( "Can not create UNDO folder, UNDO did not proceeded", MB_OK|MB_ICONWARNING );

	//}


	// similar to "/INPUT/Floors.txt"
    char filename[_MAX_PATH];
    PROJMANAGER->getFileName (_pProjPath, fileType, filename);

	// write file "/INPUT/Floors.txt"
    ArctermFile file;
    file.openFile (filename, WRITE, m_fVersion);
    file.writeField (getTitle());
    file.writeLine();
    file.writeField (getHeaders());
    file.writeLine();
    writeData (file);
    file.endFile();
    
	// 
	writeLayerInfo(_pProjPath);
	writeVRInfo(_pProjPath);

	// make an UNDO from current project
	if(CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE)
		CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE->DoAutoSave() ;

}

void CFloors::writeLayerInfo (const CString& _pProjPath) const
{
	char fileprefix[_MAX_PATH];
	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, LayerInfoFile, fileprefix);
	int nCount = m_vFloors.size();
	for( int i=0; i<nCount; i++ ) //for each floor
	{
		CFloor2* pFloor = m_vFloors[i];
		if( !pFloor )
			return;

		sprintf(filename, "%s%d.txt", fileprefix, i);
		ArctermFile file;
		file.openFile (filename, WRITE, m_fVersion);
		file.writeField ("LayerInfo File - Do not Edit!"); file.writeLine();
		file.writeField ("."); file.writeLine();
		int nLayerCount = pFloor->GetFloorLayers()->size();
		file.writeInt(nLayerCount); file.writeLine();
		for(int j=0; j<nLayerCount; j++) {
			file.writeInt((long)pFloor->GetFloorLayers()->at(j)->cColor);
			file.writeInt((int)pFloor->GetFloorLayers()->at(j)->bIsOn);
			file.writeLine();
		}

		file.endFile();
	}
}

void CFloors::readLayerInfo (const CString& _pProjPath)
{
	char fileprefix[_MAX_PATH];
	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, LayerInfoFile, fileprefix);
	int nCount = m_vFloors.size();
	for( int i=0; i<nCount; i++ ) //for each floor
	{
		CFloor2* pFloor = m_vFloors[i];
		if( !pFloor )
			return;

		sprintf(filename, "%s%d.txt", fileprefix, i);
		ArctermFile file;
		try { 
			file.openFile (filename, READ);
		}
		catch (FileOpenError *exception) {
			delete exception;
			return;
		}
		file.getLine();
		int nLayerCount;
		file.getInteger(nLayerCount);
		CCADFileContent::CFloorLayerList* pLayers = pFloor->GetFloorLayers();
		ASSERT(pLayers->size() == 0);
		pLayers->reserve(nLayerCount);
		long l;
		int n;
		for(int j=0; j<nLayerCount; j++) {
			file.getLine();
			CCADLayer* pLayer = new CCADLayer();
			file.getInteger(l);
			pLayer->cColor = (COLORREF)l;
			file.getInteger(n);
			pLayer->bIsOn = (BOOL) n;
			pLayer->bIsMonochrome = pFloor->IsMonochrome();
			pLayer->cMonochromeColor = pFloor->GetMonochromeColor();
			pLayers->push_back(pLayer);
		}

		file.closeIn();
	}
}

void CFloors::readObsoleteData(ArctermFile& p_file)
{
	if(p_file.getVersion() < 2.3f) {
		readObsoleteOld(p_file);
	}
	else if(p_file.getVersion() == 2.3f) {
		readObsolete23(p_file);
	}
	else if(p_file.getVersion() == 2.4f) {
		readObsolete24(p_file);
	}
	else if (p_file.getVersion() == 2.5f)
	{
		 readObsolete25( p_file );
	}
	else if(p_file.getVersion() == 2.6f)
	{
		readObsolete26( p_file );
	}
	else if (p_file.getVersion() == 2.7f)
	{
		readObsolete27( p_file );
	}
	else {
		ASSERT(FALSE);
	}
}

void CFloors::readData(ArctermFile& p_file)
{
	p_file.getLine();

    while (!p_file.isBlank ())
	{
		//get level
		int lvl;
		p_file.getInteger(lvl);
		CFloor2* pFloor = new CFloor2(lvl);
		int LinkIndex;
		p_file.getInteger(LinkIndex);
		pFloor->linkToTerminalFloor(LinkIndex);
		//get uuid
		char uid[256];
		if( p_file.isBlankField() || !p_file.getShortField (uid, 256) ){ ASSERT(FALSE);	}
		else
		{
			pFloor->SetUID(uid);
		}
		if( AirsideFloorsFile == fileType )
			pFloor->IsGridVisible(FALSE);// The airside default have not grid.

		//get name
	    char name[256];
		if( p_file.isBlankField() || !p_file.getShortField (name, 256) )
		{
	        throw new StringError ("Invalid Floor Name");
		}
		pFloor->FloorName( CString( name ) );

		// grid
		CGrid aGrid;
		long lVal;
		p_file.getInteger( lVal );
		aGrid.cAxesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cLinesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cSubdivsColor = lVal;
		p_file.getFloat( aGrid.dSizeX );
		p_file.getFloat( aGrid.dSizeY );
		p_file.getFloat( aGrid.dLinesEvery );
		int nVal;
		p_file.getInteger( nVal );
		aGrid.nSubdivs = nVal;
		p_file.getInteger( nVal );
		aGrid.bVisibility = nVal;
		pFloor->SetGrid( aGrid );


		// cad info
		p_file.getShortField (name, 256);

        CString sFileName = name;
		if(sFileName.CompareNoCase(_T("No map selected")) != 0)// haded path before
		{
			CString sPathName;

			p_file.getShortField (name, 256);

			sPathName = name;
		    CFileFind fFind;
			if(!fFind.FindFile(name)) //can not find the file
			{//
				char sTemp[4];
				ZeroMemory(sTemp,4);
				_itoa(lvl,sTemp,10);
				
				CString sMessage;
				sMessage.Format("\"%s\"%s\n\n\"%s\"\n\n%s",sFileName.GetBuffer(0),"     Does Not Exist in:",sPathName.GetBuffer(0),"Please Reset Path or Skip!");
	            
				CFloorMapMatch floorDialog;
				floorDialog.m_path = sPathName;
				floorDialog.m_edit = sPathName;
				floorDialog.m_name = sFileName;
				floorDialog.m_static = sMessage;
			//	floorDialog.GetDlgItem(IDC_STATIC_TEXT)->SetWindowText(sMessage.GetBuffer(0));	
				if(floorDialog.DoModal() ==IDOK)
				{
					//UpdateData(true);
					sPathName = floorDialog.m_path;
				}
			
			}//

            pFloor->MapFileName(sFileName);	
			pFloor->MapPathName(sPathName);
		
		}
		else
		{
			pFloor->MapFileName( CString( name ) );		
			p_file.getShortField (name, 256);
			pFloor->MapPathName( CString( name ) );
		}
		//pFloor->MapFileName( CString( name ) );
		//p_file.getShortField (name, 256);
		//pFloor->MapPathName( CString( name ) );
        
		double dVal1;
		p_file.getFloat( dVal1 );
		double dVal2;
		p_file.getFloat( dVal2 );
		ARCVector2 v2D( dVal1, dVal2 );
		pFloor->MapOffset( v2D );
		p_file.getFloat( dVal1 );
		pFloor->MapRotation( dVal1 );
		p_file.getFloat( dVal1 );
		pFloor->MapScale( dVal1 );
		
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		ARCVector2 v2D1( dVal1, dVal2 );
		pFloor->SetFloorOffset( v2D1 );

		p_file.getFloat( dVal1 );
		pFloor->Altitude( dVal1 );

		p_file.getInteger( nVal );
		pFloor->IsVisible( nVal );
		p_file.getInteger( nVal );
		pFloor->IsActive( nVal );
		p_file.getInteger( nVal );
		pFloor->IsMonochrome( nVal );
		p_file.getInteger( lVal );
		pFloor->SetMonochromeColor((COLORREF) lVal);

		p_file.getInteger( nVal );
		pFloor->UseMarker( nVal );
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		v2D1[0]=dVal1; v2D1[1]=dVal2;
		pFloor->SetMarkerLocation( v2D1 );
        
	//	pFloor->ReadMarkerLocation2();
		p_file.getFloat(dVal1);
		pFloor->RealAltitude(dVal1);

		p_file.getFloat(dVal1);
		pFloor->Thickness(dVal1);

		p_file.getInteger( nVal );
		pFloor->IsOpaque( nVal );
		p_file.getInteger( nVal );
		pFloor->IsShowMap( nVal );

		pFloor->UseLayerInfoFiles(TRUE);

		//read picture info
		{
			char strBuffer[MAX_PATH];
			CPictureInfo& picInfo = pFloor->m_picInfo;
			p_file.getShortField(strBuffer,MAX_PATH);
			picInfo.sFileName = strBuffer;

			p_file.getShortField(strBuffer,MAX_PATH);
			picInfo.sPathName = strBuffer;

			p_file.getFloat(picInfo.vSize[VX]);
			p_file.getFloat(picInfo.vSize[VY]);

			Point head, tail;
			double x, y, z=0.0;
			p_file.getFloat(x);
			p_file.getFloat(y);
			head.setPoint( x, y, z);

			p_file.getFloat(x);
			p_file.getFloat(y);
			tail.setPoint( x, y, z);

			Point ptList[2];
			ptList[0] = head;
			ptList[1] = tail;
			picInfo.refLine.init( 2, ptList);

			p_file.getFloat(picInfo.vOffset[VX]);
			p_file.getFloat(picInfo.vOffset[VY]);

			p_file.getFloat(picInfo.dScale);
			p_file.getFloat(picInfo.dRotation);

			int nShow = 0;
			p_file.getInteger(nShow);
			picInfo.bShow = nShow ? true:false;
		}
		

		m_vFloors.push_back( pFloor );
		p_file.getLine();
	}
}

void CFloors::writeData(ArctermFile& p_file) const
{
	int nCount = m_vFloors.size();
	for( int i=0; i<nCount; i++ ) //for each floor
	{
		CFloor2* pFloor = m_vFloors[i];
		if( !pFloor )
			return;


		p_file.writeInt( pFloor->Level() );
		p_file.writeInt(pFloor->linkToTerminalFloor());
		p_file.writeField(pFloor->GetUID());

		p_file.writeField( pFloor->FloorName() );

		// grid
		CGrid* pGrid = pFloor->GetGrid();
		p_file.writeInt( (long)pGrid->cAxesColor );
		p_file.writeInt( (long)pGrid->cLinesColor );
		p_file.writeInt( (long)pGrid->cSubdivsColor );
		p_file.writeFloat( (float)pGrid->dSizeX );
		p_file.writeFloat( (float)pGrid->dSizeY );
		p_file.writeFloat( (float)pGrid->dLinesEvery );
		p_file.writeInt( pGrid->nSubdivs );
		p_file.writeInt( pGrid->bVisibility );

		// cad info
		p_file.writeField( pFloor->MapFileName() );
		p_file.writeField( pFloor->MapPathName() );
		ARCVector2 v2D = pFloor->MapOffset();
		p_file.writeFloat( (float)v2D[0] );
		p_file.writeFloat( (float)v2D[1] );
		p_file.writeFloat( (float)pFloor->MapRotation() );
		p_file.writeFloat( (float)pFloor->MapScale() );

		v2D = pFloor->GetFloorOffset();
		p_file.writeFloat( (float)v2D[0] );
		p_file.writeFloat( (float)v2D[1] );
		p_file.writeFloat( (float)pFloor->Altitude() );
		p_file.writeInt( pFloor->IsVisible() );
		p_file.writeInt( pFloor->IsActive() );
		p_file.writeInt( pFloor->IsMonochrome() );
		p_file.writeInt( (long) pFloor->GetMonochromeColor() );

		p_file.writeInt( pFloor->UseMarker() );
		pFloor->GetMarkerLocation(v2D);
		p_file.writeFloat( (float)v2D[0] );
		p_file.writeFloat( (float)v2D[1] );

      //  pFloor->SaveMarkerLocation2() ;

		p_file.writeFloat( (float)pFloor->RealAltitude() );
	
		p_file.writeFloat((float)pFloor->Thickness());
	
		p_file.writeInt( pFloor->IsOpaque() );
		p_file.writeInt( pFloor->IsShowMap() );

		//write picture info
		{
			CPictureInfo& picinfo = pFloor->m_picInfo;
			p_file.writeField(picinfo.sFileName);
			p_file.writeField(picinfo.sPathName);

			p_file.writeFloat((float)picinfo.vSize[VX]);
			p_file.writeFloat((float)picinfo.vSize[VY]);

			if (picinfo.refLine.getCount() < 2)
			{
				p_file.writeFloat(0.0);
				p_file.writeFloat(0.0);
				p_file.writeFloat(0.0);
				p_file.writeFloat(0.0);
			}
			else
			{
				p_file.writeFloat((float)picinfo.refLine[0].getX());
				p_file.writeFloat((float)picinfo.refLine[0].getY());

				p_file.writeFloat((float)picinfo.refLine[1].getX());
				p_file.writeFloat((float)picinfo.refLine[1].getY());
			}

			p_file.writeFloat((float)picinfo.vOffset[VX]);
			p_file.writeFloat((float)picinfo.vOffset[VY]);

			p_file.writeFloat((float)picinfo.dScale);
			p_file.writeFloat((float)picinfo.dRotation);

			p_file.writeInt(picinfo.bShow?1:0);
		}

		p_file.writeLine();
	}
}

void CFloors::writeVRInfo(const CString& _pProjPath) const
{
	char fileprefix[_MAX_PATH];
	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, VRInfoFile, fileprefix,fileType>AirPortInfoFile?2:1);
	int nCount = m_vFloors.size();
	for( int i=0; i<nCount; i++ ) //for each floor
	{
		CFloor2* pFloor = m_vFloors[i];
		if( !pFloor )
			return;

		sprintf(filename, "%s%d.txt", fileprefix, i);
		ArctermFile file;
		file.openFile (filename, WRITE, m_fVersion);
		file.writeField ("Visual Region File - Do not Edit!"); file.writeLine();
		file.writeField ("."); file.writeLine();

		file.writeInt((long) pFloor->UseVisibleRegions());

		int nVRCount = pFloor->GetVisibleRegionVectorPtr()->size();
		int nInVRCount = pFloor->GetInVisibleRegionVectorPtr()->size();

		file.writeInt(nVRCount); file.writeInt(nInVRCount);file.writeLine();
		for(int j=0; j<nVRCount; j++) {
			file.writeField(pFloor->GetVisibleRegionVectorPtr()->at(j)->sName);
			int nPtCount = pFloor->GetVisibleRegionVectorPtr()->at(j)->polygon.getCount();
			file.writeInt((int) nPtCount);
			file.writeLine();
			for(int k=0; k<nPtCount; k++) {
				file.writeFloat((float) pFloor->GetVisibleRegionVectorPtr()->at(j)->polygon.getPoint(k).getX());
				file.writeFloat((float) pFloor->GetVisibleRegionVectorPtr()->at(j)->polygon.getPoint(k).getY());
				file.writeLine();
			}
		}
		//Ben
		//Ben	
		for(int j=0; j<nInVRCount; j++) {
			file.writeField(pFloor->GetInVisibleRegionVectorPtr()->at(j)->sName);
			int nPtCount = pFloor->GetInVisibleRegionVectorPtr()->at(j)->polygon.getCount();
			file.writeInt((int) nPtCount);
			file.writeLine();
			for(int k=0; k<nPtCount; k++) {
				file.writeFloat((float) pFloor->GetInVisibleRegionVectorPtr()->at(j)->polygon.getPoint(k).getX());
				file.writeFloat((float) pFloor->GetInVisibleRegionVectorPtr()->at(j)->polygon.getPoint(k).getY());
				file.writeLine();
			}
		}
		file.endFile();
	}
}

void CFloors::readVRInfo(const CString& _pProjPath)
{
	char fileprefix[_MAX_PATH];
	char filename[_MAX_PATH];
	char name[256];
	PROJMANAGER->getFileName (_pProjPath, VRInfoFile, fileprefix,fileType>AirPortInfoFile?2:1);
	int nCount = m_vFloors.size();
	for( int i=0; i<nCount; i++ ) //for each floor
	{
		CFloor2* pFloor = m_vFloors[i];
		if( !pFloor )
			return;

		sprintf(filename, "%s%d.txt", fileprefix, i);
		ArctermFile file;
		try { 
			file.openFile (filename, READ);
		}
		catch (FileOpenError *exception) {
			delete exception;
			return;
		}
		file.getLine();
		long l;
		file.getInteger(l);
		pFloor->UseVisibleRegions((BOOL) l);
		int nVRCount;		
		file.getInteger(nVRCount);
		int nInVRCount;
		file.getInteger(nInVRCount);		
		ASSERT(pFloor->GetVisibleRegionVectorPtr()->size() == 0);
		pFloor->GetVisibleRegionVectorPtr()->reserve(nVRCount);
		double a, b;
		int nPtCount;
		for(int j=0; j<nVRCount; j++) {
			file.getLine();
			file.getShortField (name, 256);
			CVisibleRegion* pVR = new CVisibleRegion(name);
			file.getInteger(nPtCount);
			Point* pPtList = new Point[nPtCount];
			for(int k=0; k<nPtCount; k++) {
				file.getLine();
				file.getFloat(a);
				file.getFloat(b);
				pPtList[k].setPoint(a,b,0.0);
			}
			pVR->polygon.init(nPtCount, pPtList);
			delete [] pPtList;
			//pVR->polygon.DividAsConvexPollygons(pVR->vTessPoly);
			pFloor->GetVisibleRegionVectorPtr()->push_back(pVR);
		}
		
		//for Invisible regions //Ben
		ASSERT(pFloor->GetInVisibleRegionVectorPtr()->size() == 0);
		pFloor->GetVisibleRegionVectorPtr()->reserve(nInVRCount);
		for(int j=0; j<nInVRCount; j++) {
			file.getLine();
			file.getShortField (name, 256);
			CVisibleRegion* pVR = new CVisibleRegion(name);
			file.getInteger(nPtCount);
			Point* pPtList = new Point[nPtCount];
			for(int k=0; k<nPtCount; k++) {
				file.getLine();
				file.getFloat(a);
				file.getFloat(b);
				pPtList[k].setPoint(a,b,0.0);
			}
			pVR->polygon.init(nPtCount, pPtList);
			delete [] pPtList;
			//pVR->polygon.DividAsConvexPollygons(pVR->vTessPoly);
			pFloor->GetInVisibleRegionVectorPtr()->push_back(pVR);
		}


		file.closeIn();
	}
}

void CFloors::readObsoleteOld(ArctermFile& p_file)
{
	p_file.getLine();

    while (!p_file.isBlank ())
	{
		int lvl;
		p_file.getInteger(lvl);
		CFloor2* pFloor = new CFloor2(lvl);
	    char name[256];
		if( p_file.isBlankField() || !p_file.getShortField (name, 256) )
		{
	        throw new StringError ("Invalid Floor Name");
		}
		pFloor->FloorName( CString( name ) );

		// grid
		CGrid aGrid;
		long lVal;
		p_file.getInteger( lVal );
		aGrid.cAxesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cLinesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cSubdivsColor = lVal;
		p_file.getFloat( aGrid.dSizeX );
		p_file.getFloat( aGrid.dLinesEvery );
		int nVal;
		p_file.getInteger( nVal );
		aGrid.nSubdivs = nVal;
		p_file.getInteger( nVal );
		aGrid.bVisibility = nVal;
		aGrid.dSizeY = aGrid.dSizeX;
		pFloor->SetGrid( aGrid );


		// cad info
		p_file.getShortField (name, 256);
		pFloor->MapFileName( CString( name ) );
		p_file.getShortField (name, 256);
		pFloor->MapPathName( CString( name ) );

		double dVal1;
		p_file.getFloat( dVal1 );
		double dVal2;
		p_file.getFloat( dVal2 );
		ARCVector2 v2D( dVal1, dVal2 );
		pFloor->MapOffset( v2D );
		p_file.getFloat( dVal1 );
		pFloor->MapRotation( dVal1 );
		p_file.getFloat( dVal1 );
		pFloor->MapScale( dVal1 );
		
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		ARCVector2 v2D1( dVal1, dVal2 );
		pFloor->SetFloorOffset( v2D1 );

		p_file.getFloat( dVal1 );
		pFloor->Altitude( dVal1 );

		p_file.getInteger( nVal );
		pFloor->IsVisible( nVal );
		p_file.getInteger( nVal );
		pFloor->IsActive( nVal );
		p_file.getInteger( nVal );
		pFloor->IsMonochrome( nVal );
		p_file.getInteger( lVal );
		pFloor->SetMonochromeColor((COLORREF) lVal);


		pFloor->UseLayerInfoFiles(TRUE);

		m_vFloors.push_back( pFloor );
		p_file.getLine();
	}
}



void CFloors::readObsolete23(ArctermFile& p_file)
{
	p_file.getLine();

    while (!p_file.isBlank ())
	{
		int lvl;
		p_file.getInteger(lvl);
		CFloor2* pFloor = new CFloor2(lvl);
	    char name[256]="";
		if( p_file.isBlankField() || !p_file.getShortField (name, 256) )
		{
	        throw new StringError ("Invalid Floor Name");
		}
		pFloor->FloorName( CString( name ) );

		// grid
		CGrid aGrid;
		long lVal;
		p_file.getInteger( lVal );
		aGrid.cAxesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cLinesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cSubdivsColor = lVal;
		p_file.getFloat( aGrid.dSizeX );
		p_file.getFloat( aGrid.dSizeY );
		p_file.getFloat( aGrid.dLinesEvery );
		int nVal;
		p_file.getInteger( nVal );
		aGrid.nSubdivs = nVal;
		p_file.getInteger( nVal );
		aGrid.bVisibility = nVal;
		pFloor->SetGrid( aGrid );


		// cad info
		p_file.getShortField (name, 256);
		pFloor->MapFileName( CString( name ) );
		p_file.getShortField (name, 256);
		pFloor->MapPathName( CString( name ) );

		double dVal1;
		p_file.getFloat( dVal1 );
		double dVal2;
		p_file.getFloat( dVal2 );
		ARCVector2 v2D( dVal1, dVal2 );
		pFloor->MapOffset( v2D );
		p_file.getFloat( dVal1 );
		pFloor->MapRotation( dVal1 );
		p_file.getFloat( dVal1 );
		pFloor->MapScale( dVal1 );
		
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		ARCVector2 v2D1( dVal1, dVal2 );
		pFloor->SetFloorOffset( v2D1 );

		p_file.getFloat( dVal1 );
		pFloor->Altitude( dVal1 );

		p_file.getInteger( nVal );
		pFloor->IsVisible( nVal );
		p_file.getInteger( nVal );
		pFloor->IsActive( nVal );
		p_file.getInteger( nVal );
		pFloor->IsMonochrome( nVal );
		p_file.getInteger( lVal );
		pFloor->SetMonochromeColor((COLORREF) lVal);

		p_file.getInteger( nVal );
		pFloor->UseMarker( nVal );
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		v2D1[0]=dVal1; v2D1[1]=dVal2;
		pFloor->SetMarkerLocation( v2D1 );
			

		pFloor->UseLayerInfoFiles(TRUE);

		m_vFloors.push_back( pFloor );
		p_file.getLine();
	}
}

void CFloors::readObsolete24(ArctermFile& p_file)
{
	p_file.getLine();

    while (!p_file.isBlank ())
	{
		int lvl;
		p_file.getInteger(lvl);
		CFloor2* pFloor = new CFloor2(lvl);
	    char name[256];
		if( p_file.isBlankField() || !p_file.getShortField (name, 256) )
		{
	        throw new StringError ("Invalid Floor Name");
		}
		pFloor->FloorName( CString( name ) );

		// grid
		CGrid aGrid;
		long lVal;
		p_file.getInteger( lVal );
		aGrid.cAxesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cLinesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cSubdivsColor = lVal;
		p_file.getFloat( aGrid.dSizeX );
		p_file.getFloat( aGrid.dSizeY );
		p_file.getFloat( aGrid.dLinesEvery );
		int nVal;
		p_file.getInteger( nVal );
		aGrid.nSubdivs = nVal;
		p_file.getInteger( nVal );
		aGrid.bVisibility = nVal;
		pFloor->SetGrid( aGrid );


		// cad info
		p_file.getShortField (name, 256);
		pFloor->MapFileName( CString( name ) );
		p_file.getShortField (name, 256);
		pFloor->MapPathName( CString( name ) );

		double dVal1;
		p_file.getFloat( dVal1 );
		double dVal2;
		p_file.getFloat( dVal2 );
		ARCVector2 v2D( dVal1, dVal2 );
		pFloor->MapOffset( v2D );
		p_file.getFloat( dVal1 );
		pFloor->MapRotation( dVal1 );
		p_file.getFloat( dVal1 );
		pFloor->MapScale( dVal1 );
		
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		ARCVector2 v2D1( dVal1, dVal2 );
		pFloor->SetFloorOffset( v2D1 );

		p_file.getFloat( dVal1 );
		pFloor->Altitude( dVal1 );

		p_file.getInteger( nVal );
		pFloor->IsVisible( nVal );
		p_file.getInteger( nVal );
		pFloor->IsActive( nVal );
		p_file.getInteger( nVal );
		pFloor->IsMonochrome( nVal );
		p_file.getInteger( lVal );
		pFloor->SetMonochromeColor((COLORREF) lVal);

		p_file.getInteger( nVal );
		pFloor->UseMarker( nVal );
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		v2D1[0]=dVal1; v2D1[1]=dVal2;
		pFloor->SetMarkerLocation( v2D1 );

		p_file.getFloat(dVal1);
		pFloor->RealAltitude(dVal1);
		p_file.getInteger( nVal );
		pFloor->IsOpaque( nVal );
			

		pFloor->UseLayerInfoFiles(TRUE);

		m_vFloors.push_back( pFloor );
		p_file.getLine();
	}
}


void CFloors::readObsolete25(ArctermFile& p_file)
{
	p_file.getLine();

    while (!p_file.isBlank ())
	{
		int lvl;
		p_file.getInteger(lvl);
		CFloor2* pFloor = new CFloor2(lvl);
		if( AirsideFloorsFile == fileType )
			pFloor->IsGridVisible(FALSE);// The airside default have not grid.

	    char name[256];
		if( p_file.isBlankField() || !p_file.getShortField (name, 256) )
		{
	        throw new StringError ("Invalid Floor Name");
		}
		pFloor->FloorName( CString( name ) );

		// grid
		CGrid aGrid;
		long lVal;
		p_file.getInteger( lVal );
		aGrid.cAxesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cLinesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cSubdivsColor = lVal;
		p_file.getFloat( aGrid.dSizeX );
		p_file.getFloat( aGrid.dSizeY );
		p_file.getFloat( aGrid.dLinesEvery );
		int nVal;
		p_file.getInteger( nVal );
		aGrid.nSubdivs = nVal;
		p_file.getInteger( nVal );
		aGrid.bVisibility = nVal;
		pFloor->SetGrid( aGrid );


		// cad info
		p_file.getShortField (name, 256);

        CString sFileName = name;
		if(sFileName.CompareNoCase(_T("No map selected")) != 0)// haded path before
		{
			CString sPathName;

			p_file.getShortField (name, 256);

			sPathName = name;
		    CFileFind fFind;
			if(!fFind.FindFile(name)) //can not find the file
			{//
				char sTemp[4];
				ZeroMemory(sTemp,4);
				_itoa(lvl,sTemp,10);
				
				CString sMessage;
				sMessage.Format("\"%s\"%s\n\n\"%s\"\n\n%s",sFileName.GetBuffer(0),"     Does Not Exist in:",sPathName.GetBuffer(0),"Please Reset Path or Skip!");
	            
				CFloorMapMatch floorDialog;
				floorDialog.m_path = sPathName;
				floorDialog.m_edit = sPathName;
				floorDialog.m_name = sFileName;
				floorDialog.m_static = sMessage;
			//	floorDialog.GetDlgItem(IDC_STATIC_TEXT)->SetWindowText(sMessage.GetBuffer(0));	
				if(floorDialog.DoModal() ==IDOK)
				{
					//UpdateData(true);
					sPathName = floorDialog.m_path;
				}
			
			}//

            pFloor->MapFileName(sFileName);	
			pFloor->MapPathName(sPathName);
		
		}
		else
		{
			pFloor->MapFileName( CString( name ) );		
			p_file.getShortField (name, 256);
			pFloor->MapPathName( CString( name ) );
		}
		//pFloor->MapFileName( CString( name ) );
		//p_file.getShortField (name, 256);
		//pFloor->MapPathName( CString( name ) );
        
		double dVal1;
		p_file.getFloat( dVal1 );
		double dVal2;
		p_file.getFloat( dVal2 );
		ARCVector2 v2D( dVal1, dVal2 );
		pFloor->MapOffset( v2D );
		p_file.getFloat( dVal1 );
		pFloor->MapRotation( dVal1 );
		p_file.getFloat( dVal1 );
		pFloor->MapScale( dVal1 );
		
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		ARCVector2 v2D1( dVal1, dVal2 );
		pFloor->SetFloorOffset( v2D1 );

		p_file.getFloat( dVal1 );
		pFloor->Altitude( dVal1 );

		p_file.getInteger( nVal );
		pFloor->IsVisible( nVal );
		p_file.getInteger( nVal );
		pFloor->IsActive( nVal );
		p_file.getInteger( nVal );
		pFloor->IsMonochrome( nVal );
		p_file.getInteger( lVal );
		pFloor->SetMonochromeColor((COLORREF) lVal);

		p_file.getInteger( nVal );
		pFloor->UseMarker( nVal );
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		v2D1[0]=dVal1; v2D1[1]=dVal2;
		pFloor->SetMarkerLocation( v2D1 );

		p_file.getFloat(dVal1);
		pFloor->RealAltitude(dVal1);

		p_file.getInteger( nVal );
		pFloor->IsOpaque( nVal );
		p_file.getInteger( nVal );
		pFloor->IsShowMap( nVal );

		pFloor->UseLayerInfoFiles(TRUE);

		m_vFloors.push_back( pFloor );
		p_file.getLine();
	}
}


// give logic altitude, get real altitude
double CFloors::getRealAltitude( double _dLogicAltitude ) const
{
	
	int _iFloor = static_cast<int>(_dLogicAltitude / SCALE_FACTOR);
	int _iUpFloor = static_cast<int>( min( _iFloor+1, static_cast<int>(m_vFloors.size())-1 ) );
	

	double _loweflooralt = m_vFloors[_iFloor]->RealAltitude();
	double _diffalt =m_vFloors[_iUpFloor]->RealAltitude();

	double _dAltitude = _diffalt * (_dLogicAltitude / SCALE_FACTOR - _iFloor ) + _loweflooralt;
		
	return _dAltitude;
}

// give real altitude, get logic altitude
double CFloors::getLogicAltitude( double _dRealAltitude ) const
{
	int i=0;
	for( ; i< static_cast<int>(m_vFloors.size()); i++ )
	{
		if(  m_vFloors[i]->RealAltitude() >= _dRealAltitude )
			break;
	}

	if( i == m_vFloors.size() )
		return (i-1) * SCALE_FACTOR;
	
	if( i == 0 )
		return 0.0;

	double _dFloorHight = m_vFloors[i]->RealAltitude() - m_vFloors[i-1]->RealAltitude();
	if( _dFloorHight == 0.0 )	
		return i*SCALE_FACTOR;

	double _dDeta = ( _dRealAltitude - m_vFloors[i-1 ]->RealAltitude() ) / _dFloorHight  * SCALE_FACTOR;

	double _dLogicHigh = (i-1)*SCALE_FACTOR + _dDeta;

	return _dLogicHigh;
}
CFloor2* CFloors::GetActiveFloor()
{
	for(size_t i=0;i<m_vFloors.size();++i)	
	{
		CFloor2* pFloor = GetFloor2(i);
		if( pFloor->IsActive() )
			return pFloor;
	}
	return ActiveFloor(0);	
}

int CFloors::GetActiveFloorLevel()
{
	for(size_t i=0;i<m_vFloors.size();++i)
	{
		if( m_vFloors[i]->IsActive())return i;
	}
	ActiveFloor(0);
	return 0;
}
CFloor2* CFloors::ActiveFloor(int nlevel)
{
	ASSERT( nlevel <(int) m_vFloors.size() );
	if(nlevel>= (int)m_vFloors.size()) return NULL;
	
	for(size_t i=0;i<m_vFloors.size();++i)	
	{
		m_vFloors[i]->IsActive(FALSE);
	}
	m_vFloors[nlevel]->IsActive(TRUE);
	return m_vFloors[nlevel];
}
double CFloors::getVisibleAltitude(double floorIndex)const
{
	int _ifloor = (int)(floorIndex/SCALE_FACTOR);
	/*if(_ifloor<0 || _ifloor>m_vFloors.size() )
	{
		_ifloor = 0;
	}
	*/
	int _iUpfloor = _ifloor +1;
	double dAlt, dUpAlt;
	
	if( _ifloor < (int)m_vFloors.size() && _ifloor >=0 ){
		dAlt = m_vFloors[_ifloor]->Altitude();
	}else{
		if(_ifloor<0)dAlt = 0;
		else dAlt = m_vFloors[m_vFloors.size()-1]->Altitude();
	}

	if(_iUpfloor < (int)m_vFloors.size() && _iUpfloor >=0 ){
		dUpAlt = m_vFloors[_iUpfloor]->Altitude();
	}else{
		dUpAlt = dAlt + 10*SCALE_FACTOR;
	}
	
	dAlt = (floorIndex/SCALE_FACTOR - _ifloor)*(dUpAlt-dAlt) + dAlt;
	return dAlt;
}

double CFloors::getVisibleFloorAltitude(double visibleAlitude)const
{
	size_t i=0;
	for(;i<m_vFloors.size();i++)
	{
		if(visibleAlitude < m_vFloors[i]->Altitude())
			break;
	}
	if(i< 1)return 0;
	double diffAlt;
	if(i > m_vFloors.size()-1 || m_vFloors.size() < 2)
	{
		diffAlt = 10 * SCALE_FACTOR;
	}
	else 
	{
		diffAlt = m_vFloors[i]->Altitude() - m_vFloors[i-1]->Altitude();
	}

	return SCALE_FACTOR*( i-1 + (visibleAlitude - m_vFloors[i-1]->Altitude())/diffAlt );

}
double CFloors::getLevelAltitude(int _ifloor)const
{
	if( _ifloor < 0 && _ifloor >= (int)m_vFloors.size() )
		_ifloor = 0;

	return m_vFloors[_ifloor]->Altitude();
}

double CFloors::getLeavlRealAttitude(int _ifloor)const
{
	if( _ifloor < 0 && _ifloor >= (int)m_vFloors.size() )
		_ifloor = 0;

	return m_vFloors[_ifloor]->RealAltitude();
}

CFloor2 * CFloors::GetFloor2( int idx )
{
	//ASSERT( idx < GetCount() );
	if(idx < GetCount() && idx>=0 )
		return m_vFloors.at(idx);
	return NULL;
}

const CFloor2* CFloors::GetFloor2( int idx ) const
{
	//ASSERT( idx < GetCount() );
	if(idx < GetCount()&& idx>=0)
		return m_vFloors.at(idx);
	return NULL;
}

Path CFloors::getRealAttitudePath(const Path& path)
{
	Path returnPath;
	returnPath.init(path.getCount(),path.getPointList());

	for (int i = 0; i < path.getCount(); i++)
	{
		Point& point = path.getPoint(i);
		double dRealAttitude = getRealAltitude(point.getZ());
		returnPath.getPointList()[i].setZ(dRealAttitude);
	}
	return returnPath;
}

CRenderFloor * CFloors::GetFloor( int idx )
{
	return GetFloor2(idx);
}

const CRenderFloor* CFloors::GetFloor( int idx ) const
{
	return GetFloor2(idx);
}


void CFloors::RemoveFloor( int nLevel, bool nodelete/*= true*/ )
{
	CFloor2* pFloor = GetFloor2(nLevel);
	if(pFloor)
	{
		m_vFloors.erase(m_vFloors.begin() + nLevel);
		for(int i=nLevel; i<static_cast<int>(m_vFloors.size()); i++) {
			m_vFloors[i]->Level(m_vFloors[i]->Level()-1);
		}
	}
	if(!nodelete)
		delete pFloor;
}

void CFloors::AddFloor( CFloor2* pFloor )
{
	m_vFloors.push_back(pFloor);
}

CFloor2 * CFloors::NewFloor()
{
	CFloor2* pFloor = new CFloor2(GetCount());
	AddFloor(pFloor);
	return pFloor;
}

void CFloors::readObsolete26( ArctermFile& p_file )
{
	p_file.getLine();

	while (!p_file.isBlank ())
	{
		int lvl;
		p_file.getInteger(lvl);
		CFloor2* pFloor = new CFloor2(lvl);
		if( AirsideFloorsFile == fileType )
			pFloor->IsGridVisible(FALSE);// The airside default have not grid.

		char name[256];
		if( p_file.isBlankField() || !p_file.getShortField (name, 256) )
		{
			throw new StringError ("Invalid Floor Name");
		}
		pFloor->FloorName( CString( name ) );

		// grid
		CGrid aGrid;
		long lVal;
		p_file.getInteger( lVal );
		aGrid.cAxesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cLinesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cSubdivsColor = lVal;
		p_file.getFloat( aGrid.dSizeX );
		p_file.getFloat( aGrid.dSizeY );
		p_file.getFloat( aGrid.dLinesEvery );
		int nVal;
		p_file.getInteger( nVal );
		aGrid.nSubdivs = nVal;
		p_file.getInteger( nVal );
		aGrid.bVisibility = nVal;
		pFloor->SetGrid( aGrid );


		// cad info
		p_file.getShortField (name, 256);

		CString sFileName = name;
		if(sFileName.CompareNoCase(_T("No map selected")) != 0)// haded path before
		{
			CString sPathName;

			p_file.getShortField (name, 256);

			sPathName = name;
			CFileFind fFind;
			if(!fFind.FindFile(name)) //can not find the file
			{//
				char sTemp[4];
				ZeroMemory(sTemp,4);
				_itoa(lvl,sTemp,10);

				CString sMessage;
				sMessage.Format("\"%s\"%s\n\n\"%s\"\n\n%s",sFileName.GetBuffer(0),"     Does Not Exist in:",sPathName.GetBuffer(0),"Please Reset Path or Skip!");

				CFloorMapMatch floorDialog;
				floorDialog.m_path = sPathName;
				floorDialog.m_edit = sPathName;
				floorDialog.m_name = sFileName;
				floorDialog.m_static = sMessage;
				//	floorDialog.GetDlgItem(IDC_STATIC_TEXT)->SetWindowText(sMessage.GetBuffer(0));	
				if(floorDialog.DoModal() ==IDOK)
				{
					//UpdateData(true);
					sPathName = floorDialog.m_path;
				}

			}//

			pFloor->MapFileName(sFileName);	
			pFloor->MapPathName(sPathName);

		}
		else
		{
			pFloor->MapFileName( CString( name ) );		
			p_file.getShortField (name, 256);
			pFloor->MapPathName( CString( name ) );
		}
		//pFloor->MapFileName( CString( name ) );
		//p_file.getShortField (name, 256);
		//pFloor->MapPathName( CString( name ) );

		double dVal1;
		p_file.getFloat( dVal1 );
		double dVal2;
		p_file.getFloat( dVal2 );
		ARCVector2 v2D( dVal1, dVal2 );
		pFloor->MapOffset( v2D );
		p_file.getFloat( dVal1 );
		pFloor->MapRotation( dVal1 );
		p_file.getFloat( dVal1 );
		pFloor->MapScale( dVal1 );

		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		ARCVector2 v2D1( dVal1, dVal2 );
		pFloor->SetFloorOffset( v2D1 );

		p_file.getFloat( dVal1 );
		pFloor->Altitude( dVal1 );

		p_file.getInteger( nVal );
		pFloor->IsVisible( nVal );
		p_file.getInteger( nVal );
		pFloor->IsActive( nVal );
		p_file.getInteger( nVal );
		pFloor->IsMonochrome( nVal );
		p_file.getInteger( lVal );
		pFloor->SetMonochromeColor((COLORREF) lVal);

		p_file.getInteger( nVal );
		pFloor->UseMarker( nVal );
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		v2D1[0]=dVal1; v2D1[1]=dVal2;
		pFloor->SetMarkerLocation( v2D1 );

		//	pFloor->ReadMarkerLocation2();
		p_file.getFloat(dVal1);
		pFloor->RealAltitude(dVal1);

		p_file.getFloat(dVal1);
		pFloor->Thickness(dVal1);

		p_file.getInteger( nVal );
		pFloor->IsOpaque( nVal );
		p_file.getInteger( nVal );
		pFloor->IsShowMap( nVal );

		pFloor->UseLayerInfoFiles(TRUE);

		m_vFloors.push_back( pFloor );
		p_file.getLine();
	}
}

void CFloors::readObsolete27( ArctermFile& p_file )
{
	p_file.getLine();

	while (!p_file.isBlank ())
	{
		//get level
		int lvl;
		p_file.getInteger(lvl);
		CFloor2* pFloor = new CFloor2(lvl);

		//get uuid
		char uid[256];
		if( p_file.isBlankField() || !p_file.getShortField (uid, 256) ){ ASSERT(FALSE);	}
		else
		{
			pFloor->SetUID(uid);
		}
		if( AirsideFloorsFile == fileType )
			pFloor->IsGridVisible(FALSE);// The airside default have not grid.

		//get name
		char name[256];
		if( p_file.isBlankField() || !p_file.getShortField (name, 256) )
		{
			throw new StringError ("Invalid Floor Name");
		}
		pFloor->FloorName( CString( name ) );

		// grid
		CGrid aGrid;
		long lVal;
		p_file.getInteger( lVal );
		aGrid.cAxesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cLinesColor = lVal;
		p_file.getInteger( lVal );
		aGrid.cSubdivsColor = lVal;
		p_file.getFloat( aGrid.dSizeX );
		p_file.getFloat( aGrid.dSizeY );
		p_file.getFloat( aGrid.dLinesEvery );
		int nVal;
		p_file.getInteger( nVal );
		aGrid.nSubdivs = nVal;
		p_file.getInteger( nVal );
		aGrid.bVisibility = nVal;
		pFloor->SetGrid( aGrid );


		// cad info
		p_file.getShortField (name, 256);

		CString sFileName = name;
		if(sFileName.CompareNoCase(_T("No map selected")) != 0)// haded path before
		{
			CString sPathName;

			p_file.getShortField (name, 256);

			sPathName = name;
			CFileFind fFind;
			if(!fFind.FindFile(name)) //can not find the file
			{//
				char sTemp[4];
				ZeroMemory(sTemp,4);
				_itoa(lvl,sTemp,10);

				CString sMessage;
				sMessage.Format("\"%s\"%s\n\n\"%s\"\n\n%s",sFileName.GetBuffer(0),"     Does Not Exist in:",sPathName.GetBuffer(0),"Please Reset Path or Skip!");

				CFloorMapMatch floorDialog;
				floorDialog.m_path = sPathName;
				floorDialog.m_edit = sPathName;
				floorDialog.m_name = sFileName;
				floorDialog.m_static = sMessage;
				//	floorDialog.GetDlgItem(IDC_STATIC_TEXT)->SetWindowText(sMessage.GetBuffer(0));	
				if(floorDialog.DoModal() ==IDOK)
				{
					//UpdateData(true);
					sPathName = floorDialog.m_path;
				}

			}//

			pFloor->MapFileName(sFileName);	
			pFloor->MapPathName(sPathName);

		}
		else
		{
			pFloor->MapFileName( CString( name ) );		
			p_file.getShortField (name, 256);
			pFloor->MapPathName( CString( name ) );
		}
		//pFloor->MapFileName( CString( name ) );
		//p_file.getShortField (name, 256);
		//pFloor->MapPathName( CString( name ) );

		double dVal1;
		p_file.getFloat( dVal1 );
		double dVal2;
		p_file.getFloat( dVal2 );
		ARCVector2 v2D( dVal1, dVal2 );
		pFloor->MapOffset( v2D );
		p_file.getFloat( dVal1 );
		pFloor->MapRotation( dVal1 );
		p_file.getFloat( dVal1 );
		pFloor->MapScale( dVal1 );

		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		ARCVector2 v2D1( dVal1, dVal2 );
		pFloor->SetFloorOffset( v2D1 );

		p_file.getFloat( dVal1 );
		pFloor->Altitude( dVal1 );

		p_file.getInteger( nVal );
		pFloor->IsVisible( nVal );
		p_file.getInteger( nVal );
		pFloor->IsActive( nVal );
		p_file.getInteger( nVal );
		pFloor->IsMonochrome( nVal );
		p_file.getInteger( lVal );
		pFloor->SetMonochromeColor((COLORREF) lVal);

		p_file.getInteger( nVal );
		pFloor->UseMarker( nVal );
		p_file.getFloat( dVal1 );
		p_file.getFloat( dVal2 );
		v2D1[0]=dVal1; v2D1[1]=dVal2;
		pFloor->SetMarkerLocation( v2D1 );

		//	pFloor->ReadMarkerLocation2();
		p_file.getFloat(dVal1);
		pFloor->RealAltitude(dVal1);

		p_file.getFloat(dVal1);
		pFloor->Thickness(dVal1);

		p_file.getInteger( nVal );
		pFloor->IsOpaque( nVal );
		p_file.getInteger( nVal );
		pFloor->IsShowMap( nVal );

		pFloor->UseLayerInfoFiles(TRUE);

		//read picture info
		{
			char strBuffer[MAX_PATH];
			CPictureInfo& picInfo = pFloor->m_picInfo;
			p_file.getShortField(strBuffer,MAX_PATH);
			picInfo.sFileName = strBuffer;

			p_file.getShortField(strBuffer,MAX_PATH);
			picInfo.sPathName = strBuffer;

			p_file.getFloat(picInfo.vSize[VX]);
			p_file.getFloat(picInfo.vSize[VY]);

			Point head, tail;
			double x, y, z=0.0;
			p_file.getFloat(x);
			p_file.getFloat(y);
			head.setPoint( x, y, z);

			p_file.getFloat(x);
			p_file.getFloat(y);
			tail.setPoint( x, y, z);

			Point ptList[2];
			ptList[0] = head;
			ptList[1] = tail;
			picInfo.refLine.init( 2, ptList);

			p_file.getFloat(picInfo.vOffset[VX]);
			p_file.getFloat(picInfo.vOffset[VY]);

			p_file.getFloat(picInfo.dScale);
			p_file.getFloat(picInfo.dRotation);

			int nShow = 0;
			p_file.getInteger(nShow);
			picInfo.bShow = nShow ? true:false;
		}


		m_vFloors.push_back( pFloor );
		p_file.getLine();
	}
}

//////////////////////////////////////////////////////////////////////////
CLandsideFloors::CLandsideFloors():CFloors(LandsideFloorsFile)
{

}

void CLandsideFloors::loadDataSet( const CString& _pProjPath )
{
	CFloors::loadDataSet(_pProjPath);

	if(GetCount()==0)
	{
		CFloor2* pFloor = new CFloor2(0,"BaseLevel");
		m_vFloors.push_back(pFloor);
		saveDataSet(_pProjPath,false);
	}
}

CFloor2* CLandsideFloors::NewFloor()
{
	CFloor2* pFloor = __super::NewFloor();

	CString sname = CRenderFloor::GetDefaultLevelName(pFloor->Level(),"LEVEL");
	pFloor->FloorName(sname);
	return pFloor;
}

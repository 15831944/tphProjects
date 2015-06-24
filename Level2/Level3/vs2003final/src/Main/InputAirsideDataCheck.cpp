#include "StdAfx.h"
#include ".\inputairsidedatacheck.h"
#include "AirsideGround.h"
#include "FloorMapMatch.h"
#include "InputAirside/InputAirside.h"
#include ".\AirsideLevelList.h"

InputAirsideDataCheck::InputAirsideDataCheck( int nPrjID )
{
	m_nPrjId = nPrjID;
}

void InputAirsideDataCheck::CheckFloorPictureAndDwg(int nLevelID)
{	
	CAirsideGround asGround;
	asGround.ReadCADInfo(nLevelID);

	//check cad
	CString sFileName = asGround.MapFileName();
	if(sFileName.CompareNoCase(_T("No map selected")) != 0)// haded path before
	{
		CString sPathName = asGround.MapPathName();

		
		CFileFind fFind;
		if(!fFind.FindFile(sPathName)) //can not find the file
		{//		

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
		asGround.MapFileName(sFileName);	
		asGround.MapPathName(sPathName);
		asGround.UpdateCADInfo(nLevelID);
	}
	
	//check picture
	asGround.ReadOverlayInfo(nLevelID);
	CString sPathName  = asGround.GetPictureFileName();
	
    if(sPathName.CompareNoCase(_T("")) != 0)
	{		
		CFileFind fFind;
		if(!fFind.FindFile(sPathName)) //can not find the file
		{//		
			CString sFileName = sPathName.Right(sPathName.GetLength()   -   sPathName.ReverseFind('\\')   -   1);
			CString sMessage;
			sMessage.Format("\"%s\"%s\n\n\"%s\"\n\n%s",sFileName.GetBuffer(0),"     Does Not Exist In",sPathName.GetBuffer(0),"Please Reset Path or Skip!");

			CFloorMapMatch floorDialog;
			floorDialog.m_path = sPathName;
			floorDialog.m_edit = sPathName;
			floorDialog.m_name = sFileName;
			floorDialog.m_static = sMessage;
			floorDialog.m_filetype = _T("Floor Picture File (bmp;jpg;png;tif;tiff)");
			
			if(floorDialog.DoModal() ==IDOK)
			{
				//UpdateData(true);
				sPathName = floorDialog.m_path;
			}
		}//		
		asGround.SetPictureFilePath(sPathName);
		asGround.SetPictureFileName(sPathName);
		asGround.UpdateOverlayInfo(nLevelID);
	}

}

void InputAirsideDataCheck::DoCheck()
{
	//check ground 	
	std::vector<int> vAirportIDs;
	InputAirside::GetAirportList(m_nPrjId, vAirportIDs);
	for(int i=0;i<(int) vAirportIDs.size(); ++i)
	{
		CAirsideLevelList levelList(vAirportIDs[i]);
		levelList.ReadLevelList();

		for(int j=0;j<levelList.GetCount();++j){
			int nLevelID = levelList.GetItem(j).nLevelID;
			CheckFloorPictureAndDwg(nLevelID);
		}
	}
	
}
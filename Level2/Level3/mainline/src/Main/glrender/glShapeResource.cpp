#include "StdAfx.h"
#include ".\glshaperesource.h"
#include <cmath>

const static CString strShapeDataFile = "Shapes.data" ;

CglShapeResource::CglShapeResource(){}

CglShapeResource::CglShapeResource(const CString& foldpath){
	m_strpath = foldpath;	
}

CglShapeResource::~CglShapeResource(void)
{
}


CglShape * CglShapeResource::GetShape(const CString& strID){
	std::map<CString, ref_ptr<CglShape> >::iterator itr;
	for(itr = m_vResideShapes.begin();itr!= m_vResideShapes.end();itr++)
	{
		if((*itr).first.CompareNoCase(strID) == 0)
		{
			return (*itr).second.get();
		}
	}
	return NULL;
}

CglShape * CglShapeResource::NewShapeAndReside(const CString& filename,const CString& strID){
	CglShape * ret = NULL;
	if(ret = GetShape(strID)) return ret;
	if(ret = NewShape(filename)){
		m_vResideShapes[strID] = ret;
		return ret;
	}
	return NULL;
}

CglShape * CglShapeResource::NewShape(const CString& filename){
	CglShape * ret  = new CglShape;
	ret->m_strFileName = filename;
	return ret;
}



BOOL CglShapeResource::Load(bool bFlightShape){
	CFile * pFile = NULL;
	CString sFileName = m_strpath + _T("\\") + strShapeDataFile;
	if(bFlightShape)
	{
		sFileName  = m_strpath + strShapeDataFile;
		try{
			pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
			CArchive ar(pFile, CArchive::load);
			const static int strbufsize = 512;
			char line[strbufsize];
			//skip a line;
			ar.ReadString(line,strbufsize);
			//read head
			ar.ReadString(line,strbufsize);
			if(_stricmp(line,"Shapes Database")==0){
				//skip the column name;
				ar.ReadString(line,strbufsize);
				//read the values
				//
				
				while(*(ar.ReadString(line, 512)) != '\0')
				{				
					CString name, filename;
					char* b = line;
					char* p = NULL;
					int c = 1;
					while((p = strchr(b, ',')) != NULL)
					{
						*p = '\0';
						switch(c)
						{
						case 1: //name
							name = _T(b);
							break;
						case 2: //texture file
							filename = _T(b);
							break;					
						default:
							break;
						}
						b = ++p;
						c++;
					}
					if(b!=NULL) // the last column did not have a comma after it
					{
						switch(c)
						{
						case 1: //name
							name = _T(b);
							break;
						case 2: //texture file
							filename = _T(b);
						default:
							break;
						}
					}
					NewFlightShapeAndReside(m_strpath + filename,name);
					
				}

			}
		}

		catch(CException * e){
			AfxMessageBox("Error reading Texture DB file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
			e->Delete();
			if(pFile != NULL)
				delete pFile;
			return FALSE;
		}
	}

	else
	{
		try{
			pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
			CArchive ar(pFile, CArchive::load);
			const static int strbufsize = 512;
			char line[strbufsize];
			//skip a line;
			ar.ReadString(line,strbufsize);
			//read head
			ar.ReadString(line,strbufsize);
			if(_stricmp(line,"Shapes Database")==0){
				//skip the column name;
				ar.ReadString(line,strbufsize);
				//read the values
				//
				CString sPath = m_strpath + "\\";
				while(*(ar.ReadString(line, 512)) != '\0')
				{				
					CString name, filename;
					char* b = line;
					char* p = NULL;
					int c = 1;
					while((p = strchr(b, ',')) != NULL)
					{
						*p = '\0';
						switch(c)
						{
						case 1: //name
							name = _T(b);
							break;
						case 2: //texture file
							filename = _T(b);
							break;					
						default:
							break;
						}
						b = ++p;
						c++;
					}
					if(b!=NULL) // the last column did not have a comma after it
					{
						switch(c)
						{
						case 1: //name
							name = _T(b);
							break;
						case 2: //texture file
							filename = _T(b);
						default:
							break;
						}
					}
					NewShapeAndReside(sPath + filename,name);

				}

			}
		}

		catch(CException * e){
			AfxMessageBox("Error reading Texture DB file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
			e->Delete();
			if(pFile != NULL)
				delete pFile;
			return FALSE;
		}
	}
    
	for(ShapeMap::iterator itr = m_vResideShapes.begin();itr!= m_vResideShapes.end();itr++){
		(*itr).second->LoadModel();
	}

	return TRUE;
}

void CglShapeResource::removeShape(const CString& strID){
	m_vResideShapes.erase(strID);
}

FlightShape * CglShapeResource::NewFlightShape(const CString& filename){
	FlightShape * ret = new FlightShape;
	ret->m_strFileName = filename;
	return ret;
}
FlightShape * CglShapeResource::NewFlightShapeAndReside(const CString& filename, const CString& strID){
	FlightShape * ret = NULL;
	if(ret = (FlightShape *)GetShape(strID)) return ret;
	if(ret = NewFlightShape(filename)){
		m_vResideShapes[strID] = ret;
		return ret;
	}
	return NULL;
}
void CglShapeResource::SetResourcePath(const CString& foldpath){
	m_strpath  = foldpath;
}

void CglShapeResource::GenDisplayList()
{
	for(ShapeMap::iterator itr = m_vResideShapes.begin();itr!= m_vResideShapes.end();itr++){
		(*itr).second->display();
	}
}
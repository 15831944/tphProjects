 #include "stdafx.h"
#include "UserShapeBarManager.h"
#include "ShapesManager.h"
#include "TermPlan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CUserShapeBar::CUserShapeBar():
m_barName(_T("")),
m_barLocation(_T(""))
{
    m_vUserShapes.clear();
}

CUserShapeBar::~CUserShapeBar()
{

}

void CUserShapeBar::readData( ArctermFile& p_file )
{
    char buf[256] = {0};
	CString str;
    int nShapeCount = 0;
    p_file.getLine();
    p_file.getInteger(nShapeCount);
    for(int j=0; j<nShapeCount; j++)
    {
        p_file.getLine();
        p_file.getField(buf, 255); // "Shape"
        p_file.getField(buf, 255);
		str.Format("%s",buf);
		str.MakeLower();
		str.SetAt(0,str.GetAt(0)-32);
		CShape* pNewShape = SHAPESMANAGER->GetShapeByName(str);

		p_file.getField(buf, 255);
		pNewShape->ImageFileName(buf);

		p_file.getField(buf, 255);
		pNewShape->ShapeFileName(buf);

		p_file.getField(buf, 255);
		str.Format("%s",buf);
		str.MakeLower();
		double dScale = UnitToScale(str);
		pNewShape->SetScale(dScale);

		m_vUserShapes.push_back(pNewShape);
		
    }
}

void CUserShapeBar::writeData( ArctermFile& p_file ) const
{
    int nShapeCount = (int)m_vUserShapes.size();
    p_file.writeInt(nShapeCount);
    p_file.writeLine();
    for(int i=0; i<nShapeCount; i++)
    {
        CShape* pShape = m_vUserShapes.at(i);
        p_file.writeField("Shape");
        CString str = pShape->Name();
        p_file.writeField(str.GetBuffer());
        str = pShape->ImageFileName();
        p_file.writeField(str.GetBuffer());
        str = pShape->ShapeFileName();
        p_file.writeField(str.GetBuffer());
		str = ScaleToUnit(pShape->GetScale());
		p_file.writeField(str.GetBuffer());
        p_file.writeLine();
    }
}

BOOL CUserShapeBar::DeleteShape(CShape* pShape)
{
    ASSERT(pShape);
    int nCount = (int)m_vUserShapes.size();
    for(int i=0; i<nCount; i++)
    {
        if(m_vUserShapes.at(i)== pShape)
        {	
            m_vUserShapes.erase(m_vUserShapes.begin() + i);
            return TRUE;
        }
    }
    return FALSE;
}

void CUserShapeBar::AddShape( CShape* pShape )
{
    m_vUserShapes.push_back(pShape);
}

BOOL CUserShapeBar::IsShapeExist( CString str )
{
    int nCount = (int)m_vUserShapes.size();
    for(int i=0; i<nCount; i++)
    {
        if(str.CompareNoCase(m_vUserShapes.at(i)->Name()) == 0)
            return TRUE;
    }
    return FALSE;
}

int CUserShapeBar::GetShapeIndex( CString str )
{
	int nCount = (int)m_vUserShapes.size();
	for(int i=0; i<nCount; i++)
	{
		if(str.CompareNoCase(m_vUserShapes.at(i)->Name()) == 0)
			return i;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////


CUserShapeBarManager::CUserShapeBarManager() : DataSet(UserShapeBar, 2.3f)
{
    m_vUserBars.clear();
}

CUserShapeBarManager::~CUserShapeBarManager()
{
}

// read user defined bars from file
void CUserShapeBarManager::readData(ArctermFile& p_file)
{
    int nBarCount = 0;
    p_file.getLine();
    p_file.getInteger(nBarCount);
    for(int i=0; i<nBarCount; i++)
    {
        char buf[256] = {0};
		CString str;
        p_file.getLine();
        p_file.getField(buf, 255); // "Bar"
        p_file.getField(buf, 255);
		str.Format("%s",buf);
		str.MakeLower();
		str.SetAt(0,str.GetAt(0)-32);
		CUserShapeBar* pUserBar = new CUserShapeBar;
		pUserBar->SetBarName(str);

		p_file.getField(buf, 255);
		pUserBar->SetBarLocation(buf);
		pUserBar->readData(p_file);
		m_vUserBars.push_back(pUserBar);
    }
}

// write user defined shape bars to file.
void CUserShapeBarManager::writeData(ArctermFile& p_file) const
{
    int nBarCount = (int)m_vUserBars.size();
    p_file.writeInt(nBarCount);
    p_file.writeLine();

    for(int i=0; i<nBarCount; i++)
    {
        CUserShapeBar* pUserBar = m_vUserBars.at(i);
        p_file.writeField("Bar");
        p_file.writeField(pUserBar->GetBarName().GetBuffer());
        p_file.writeField(pUserBar->GetBarLocation());
        p_file.writeLine();

        pUserBar->writeData(p_file);
    }
}

BOOL CUserShapeBarManager::IsUserShapeBarExist(CString strBarName)
{
    int nBarCount = (int)m_vUserBars.size();
    for(int i=0; i<nBarCount; i++)
    {
        if(strBarName.CompareNoCase(m_vUserBars.at(i)->GetBarName()) == 0)
            return TRUE;
    }
    return FALSE;
}

// if a shape with name strNewShape exists, return true.
BOOL CUserShapeBarManager::IsUserShapeExist(CString strShape)
{
	if(SHAPESMANAGER->IsShapeExist(strShape))
		return TRUE;

	int nCount = (int)m_vUserBars.size();
	for(int i=0; i<nCount; i++)
    {
        if(m_vUserBars.at(i)->IsShapeExist(strShape))
            return TRUE;
    }
    return FALSE;
}

CUserShapeBar* CUserShapeBarManager::FindUserBarByName(CString strBarName)
{
    int nBarCount = (int)m_vUserBars.size();
    for(int i=0; i<nBarCount; i++)
    {
        if(strBarName.CompareNoCase(m_vUserBars.at(i)->GetBarName()) == 0)
            return m_vUserBars.at(i);
    }
    return NULL;
}

CUserShapeBar* CUserShapeBarManager::GetUserBarByIndex(int nIndex)
{
    if(0 <= nIndex && nIndex < (int)m_vUserBars.size())
        return m_vUserBars.at(nIndex);
    else
        return NULL;
}

CShape* CUserShapeBarManager::FindShapeByName(const CString& str)
{
	int nBarCount = (int)m_vUserBars.size();
	for(int i=0; i<nBarCount; i++)
	{ 
		CShape::CShapeList*	pShapeList = m_vUserBars.at(i)->GetUserShapeList();
		int nShapeCount = (int)pShapeList->size();
		for(int j=0; j<nShapeCount; j++)
			if(!str.Compare(pShapeList->at(j)->Name()))
				return pShapeList->at(j);
	}
    return NULL;
}

// if a shape with name strOldName exists, call this for an available shape name.
CString CUserShapeBarManager::GetAnAvailableShapeName(CString strOldName)
{
    CString strNewName;
    for(int i=1; ; i++)
    {
        strNewName.Format("%s_%d", strOldName, i);
        if(IsUserShapeExist(strNewName))
            continue;
        else
            break;
    }
    return strNewName;
}

BOOL CUserShapeBarManager::DeleteUserShapeBar(CUserShapeBar* pUserBar)
{
    ASSERT(pUserBar);
    int nBarCount = (int)m_vUserBars.size();
    for(int i=0; i<nBarCount; i++)
    {
        if(m_vUserBars.at(i) == pUserBar)
        {
			
			for (int j = pUserBar->GetShapeCount()-1; j>=0; j--)
			{
				CShape* pShape = pUserBar->GetShapeByIndex(j);
				pUserBar->DeleteShape(pShape);
			}

            delete m_vUserBars.at(i);
            m_vUserBars.erase(m_vUserBars.begin() + i);

            return TRUE;
        }
    }
    return FALSE;
}

void CUserShapeBarManager::DeleteAllUserShapeBar()
{
    int nBarCount = (int)m_vUserBars.size();
    for(int i=0; i<nBarCount; i++)
    {
        delete m_vUserBars.at(i);
        m_vUserBars.erase(m_vUserBars.begin() + i);
    }
    return;
}

void CUserShapeBarManager::clear()
{
    DeleteAllUserShapeBar();
}

void CUserShapeBarManager::AddUserBar(CUserShapeBar* pUserBar)
{
    m_vUserBars.push_back(pUserBar);
}


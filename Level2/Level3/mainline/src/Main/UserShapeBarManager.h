#pragma once

#include "ShapesListBox.h"
#include "Common\DATASET.H"

#define SOURCEUNITS_COUNT 9
const CString NAMES[] = {
	"inches", "feet", "millimeters", "centimeters", "meters", "kilometers", "nautical miles", "stat. miles", "yards"
};
const double UINTS[] = {
	2.54, 30.48, 0.10, 1.0, 100.0, 100000.0, 185200.0, 160934.4, 91.44
};
static double UnitToScale(CString sUnit)
{
	for(int i=0;i<SOURCEUNITS_COUNT;i++)
	{
		if(!sUnit.Compare(NAMES[i]))
			return UINTS[i];
	}
	return 1.0;
};

static CString ScaleToUnit(double dScale)
{
	for(int i=0;i<SOURCEUNITS_COUNT;i++)
	{
		if(dScale == UINTS[i])
			return NAMES[i];
	}
	return "centimeters";
};

class CUserShapeBar
{
public:
    CUserShapeBar();
    ~CUserShapeBar();
public:
	void readData(ArctermFile& p_file);
	void writeData(ArctermFile& p_file) const;

    CString GetBarName() const { return m_barName; }
    void SetBarName(CString val) { m_barName = val; }

    CString GetBarLocation() const { return m_barLocation; }
    void SetBarLocation(CString val) { m_barLocation = val; }

    CShape::CShapeList* GetUserShapeList() { return &m_vUserShapes; }
    int GetShapeCount(){ return (int)m_vUserShapes.size(); }
    CShape* GetShapeByIndex(int nIdx){ return m_vUserShapes.at(nIdx); }

    BOOL IsShapeExist(CString str);
	int GetShapeIndex(CString str);
    BOOL DeleteShape(CShape* pShape);

    void AddShape(CShape* pShape);

    CUserShapeBar& operator=(const CUserShapeBar& other)
    {
        m_barName = other.m_barName;
        m_barLocation = other.m_barLocation;
        m_vUserShapes = other.m_vUserShapes;
        return *this;
    }

private:
    CString m_barName;
    CString m_barLocation;
    CShape::CShapeList m_vUserShapes;
};

class CUserShapeBarManager : public DataSet
{
public:
    CUserShapeBarManager();
    virtual ~CUserShapeBarManager();

public:

    virtual void readData(ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file) const;
    virtual void clear();

    virtual const char *getTitle (void) const
    { return "User Defined Shape Bar"; }
    virtual const char *getHeaders (void) const
    { return "Name, Location, Shape Name, Shape Picture, Shape Model ,Unit"; }

    BOOL IsUserShapeBarExist(CString strNewBar);
    BOOL IsUserShapeExist(CString strNewShape);
    CUserShapeBar* FindUserBarByName(CString strBarName);
    CUserShapeBar* GetUserBarByIndex(int nIndex);
    CShape* FindShapeByName(const CString& str);

    // if a shape with name strOldName exists, call this for an available shape name.
    CString GetAnAvailableShapeName(CString strOldName);

    int GetUserBarCount(){ return (int)m_vUserBars.size(); }
    void AddUserBar(CUserShapeBar* pUserBar);
    BOOL DeleteUserShapeBar(CUserShapeBar* pUserBar);
    void DeleteAllUserShapeBar();
protected:
    std::vector<CUserShapeBar*> m_vUserBars;
};


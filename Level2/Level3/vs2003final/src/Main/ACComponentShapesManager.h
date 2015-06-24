#pragma once


#define ACCOMPONENTSHAPESMANAGER          CACComponentShapesManager::GetInstance()


class CAirportDatabase;
class CAircraftComponentModelDatabase;
class CACComponentShapesBar;

class CACComponentShapesManager
{
protected:
	CACComponentShapesManager(void);
	~CACComponentShapesManager(void);

public:
	static CACComponentShapesManager& GetInstance();

	void SetShapesBar(CACComponentShapesBar* shapebar) { m_pShapesBar = shapebar; }
// 	CACComponentShapesBar* GetShapesBar() const { return m_pShapesBar; }
	void ShowShapesBar(BOOL bShow) const;
	BOOL IsShapesBarShow() const;

	BOOL Reload(CAircraftComponentModelDatabase* pDB);

private:
	CACComponentShapesBar* m_pShapesBar;
};

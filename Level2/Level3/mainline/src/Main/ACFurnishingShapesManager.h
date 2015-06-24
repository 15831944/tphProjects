#pragma once


#define ACFURNISHINGSHAPESMANAGER          CACFurnishingShapesManager::GetInstance()


class CAirportDatabase;
class CAircraftFurnishingSectionManager;
class CACFurnishingShapesBar;

class CACFurnishingShapesManager
{
protected:
	CACFurnishingShapesManager(void);
	~CACFurnishingShapesManager(void);

public:
	static CACFurnishingShapesManager& GetInstance();

	void SetShapesBar(CACFurnishingShapesBar* shapebar) { m_pShapesBar = shapebar; }
	void ShowShapesBar(BOOL bShow) const;
	BOOL IsShapesBarShow() const;

	BOOL Reload(CAircraftFurnishingSectionManager* pManager);

private:
	CACFurnishingShapesBar* m_pShapesBar;
};

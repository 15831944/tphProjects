#pragma once

class ARCportDatabaseManager
{
public:
	ARCportDatabaseManager();
	virtual ~ARCportDatabaseManager();
public:
	virtual bool loadData();
	virtual bool saveData();
	virtual bool loadPreVersionData();
protected:
private:
};
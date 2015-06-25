#pragma once

class SyncDateType
{
public:

	SyncDateType(void)
	{
		m_bNeedSync = true;
	}

	~SyncDateType(void)
	{
	}

	virtual void Update(){ m_bNeedSync = true; }
	virtual void InValidate(bool b = true){ 
		if(b)
		{
			m_bNeedSync = true;
		}
	}
	virtual bool IsNeedSync(){ return m_bNeedSync; }
	virtual void DoSync(){ m_bNeedSync = false; }
protected:
	bool m_bNeedSync;
};

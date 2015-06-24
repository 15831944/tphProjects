// HistogramManager.h: interface for the CHistogramManager class.
#pragma once
#include "common\DataSet.h"

class CHistogramManager : public DataSet
{
public:
	CHistogramManager();
	virtual ~CHistogramManager();

	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char* getTitle() const{return "Histogram Data";}
	virtual const char* getHeaders() const{return "Value";}

	std::vector<double>* GetValueVectorPtr() { return m_pvdbValue; }
	void SetValueVectorPtr(std::vector<double>* pvdbValue) { m_pvdbValue = pvdbValue; }

protected:
	std::vector<double>* m_pvdbValue;	
};

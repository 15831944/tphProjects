#pragma once

#include "Common/elaptime.h"
#include "CommonData/procid.h"
#include "common/csv_file.h"
#include "Common/ALTObjectID.h"


namespace AODB
{
	class ConvertParameter;
	class MapField
	{
	public:
		MapField();
		~MapField();

	public:
		virtual void GetValue(CSVFile& csvFile);
		virtual void Convert( ConvertParameter& pParameter ) = 0;

		//check the filed is valid or not
		virtual bool IsValid() const;
		virtual CString GetMapValue() const;

		CString GetErroDesc();	
		virtual void SetValue(const CString& strValue,ConvertParameter& pParameter);
		
		bool IsEmpty() const;
		CString GetOriginal()const;
	protected:
		//original value
		CString m_strOrg;

		//the value after mapped
		CString m_strMap;

		//the message of the field
		CString m_strMessage;

		bool m_bValid;
	protected:
		void setValid(bool bValid);

	private:
	};

	class LimitMapField : public MapField
	{
	public:
		LimitMapField(const CString& strField, int nChar, bool bAllowEmpty)
		{
			m_strField = strField;
			m_nChar = nChar;
			m_bAllowEmpty = bAllowEmpty;
		}
	public:
		void Convert( ConvertParameter& pParameter )
		{
			setValid(false);

			m_strOrg.Trim();

			if (m_strOrg.IsEmpty() )// can not be NULL
			{
				if(!m_bAllowEmpty)
				{
					m_strMessage.Format(_T("%s cannot be empty."),m_strField);
					return;
				}
				else
				{
					m_strMap = m_strOrg;
					setValid(true);
					return;
				}
			}
			if(m_strOrg.GetLength() > m_nChar)
			{
				m_strMessage.Format(_T("%s Cannot more than %d charactor."),m_strOrg,m_nChar);
				return;
			}
			m_strMap = m_strOrg;
			setValid(true);
		}


	protected:
		CString m_strField;
		int m_nChar;
		bool m_bAllowEmpty;


	};
	class AirlineMapField: public LimitMapField
	{
	public:
		AirlineMapField():
		  LimitMapField(_T("Airline"),3,false)
		{

		}


	};
	class FlightIDMapField: public LimitMapField
	{
	public:
		FlightIDMapField()
			:LimitMapField(_T("Flight Number"),4, true)
		{

		}

	};

	class AirportMapField :public LimitMapField
	{
	public:
		AirportMapField()
			:LimitMapField(_T("Airport"),3,true)
		{

		}
	};
	class ACTypeMapField: public LimitMapField
	{
	public:
		ACTypeMapField()
			:LimitMapField(_T("Aircraft Type"),10,false)
		{

		}
	protected:
	private:
	};

	class DoubleMapField :public  MapField
	{
	public:
		DoubleMapField()
		{
			m_dValue = 0.0;
		}

	public:
		double getDouble() const { return m_dValue;}
		CString GetMapValue() const
		{
			CString strValue;
			strValue.Format(_T("%.2f"),m_dValue);
			return strValue;
		}
		void Convert( ConvertParameter& pParameter )
		{
			m_strOrg.Trim();
			if(!IsEmpty())
			{
				m_dValue = ::atof(m_strOrg.GetBuffer());
			}
			setValid(true);
		}

	protected:
		double m_dValue;
	};
	class IntMapField :public  MapField
	{
	public:
		IntMapField()
		{
			m_nValue = 0;
		}

	public:
		int getValue() const { return m_nValue;}
		CString GetMapValue() const
		{
			CString strValue;
			strValue.Format(_T("%d"),m_nValue);
			return strValue;
		}
		void Convert( ConvertParameter& pParameter )
		{
			m_strOrg.Trim();
			if(!IsEmpty())
			{
				m_nValue = ::atoi(m_strOrg.GetBuffer());
			}
			setValid(true);
		}

	protected:
		int m_nValue;
	};



	class ProcMapField: public MapField
	{
	public:
		ProcMapField();
		~ProcMapField();

	public:

		//void Convert( ConvertParameter& pParameter);

		ProcessorID getProcID() const{return m_procID;}

		virtual CString GetMapValue() const;

	protected:
		ProcessorID m_procID;

	};

	class RosterProcMapField: public MapField
	{
	public:
		RosterProcMapField(){}
		~RosterProcMapField(){}

	public:
		void Convert( ConvertParameter& pParameter);

		
	protected:
		//strtext, the name need to convert
		//strProcName, the available processor name, if return true
		//vGateProc, gate names are available
		//vStand, stand names available

		bool Convert(ConvertParameter& pParameter, std::vector<CString> vText, 
			CString& strProcName , 
			std::vector<ProcessorID >&  vAvailableProc,
			std::vector<ALTObjectID>& vStand, 
			CString& strErrorMsg ) const;
		
		bool IsAValidStand( const ALTObjectID& _objID, ConvertParameter& pParameter ) const;
	};

	class GateMapField : public ProcMapField
	{
	public:
		GateMapField();
		~GateMapField();
	public:
		//void Convert( ConvertParameter& pParameter );
		bool IsValid()const;

	};
	class ArrGateMapField : public GateMapField
	{
	public:
		ArrGateMapField(){}
		~ArrGateMapField(){}
	public:
		void Convert( ConvertParameter& pParameter );
	};

	class DepGateMapField : public GateMapField
	{
	public:
		DepGateMapField(){}
		~DepGateMapField(){}
	public:
		void Convert( ConvertParameter& pParameter );
	};


	class BaggageDeviceMapField : public ProcMapField
	{
	public:
		BaggageDeviceMapField();
		~BaggageDeviceMapField();
	public:
		void Convert(ConvertParameter& pParameter);
		bool IsValid()const;
	private:
	};

	class StandMapField : public MapField
	{
	public:
		StandMapField();
		~StandMapField();


	public:

		virtual CString GetMapValue()const;
		void Convert( ConvertParameter& pParameter );
		bool IsAValidStand( const ALTObjectID& _objID, ConvertParameter& pParameter ) const;

		ALTObjectID getObjectID() const {return m_standID;}
	protected:
		ALTObjectID m_standID;
	};

	class DateMapField :public MapField
	{
	public:
		DateMapField();
		~DateMapField();

	public:


		virtual void GetValue(CSVFile& csvFile);
		COleDateTime getDate() const;

		void Convert( ConvertParameter& pParameter );
		virtual CString GetMapValue() const;
		virtual void SetValue(const CString& strValue, ConvertParameter& pParameter );

		int GetDay() const{return m_nDayIndex;}

	protected:
		COleDateTime m_oleDate;
		
		//after convert to day index
		//DAY x
		int m_nDayIndex;
	private:
	};

	class TimeMapField:public MapField
	{
	public:
		TimeMapField();
		~TimeMapField();

	protected:
		ElapsedTime m_eTime;

	public:
		virtual void GetValue(CSVFile& csvFile);

	public:
		void Convert(ConvertParameter& pParameter);

		virtual CString GetMapValue() const;

		ElapsedTime GetTime() const{return m_eTime;}
	};


	class DateTimeMapField : public MapField
	{
	public:
		DateTimeMapField();
		~DateTimeMapField();

	public:

		virtual void SetValue(const CString& strValue,ConvertParameter& pParameter);
		virtual void GetValue(CSVFile& csvFile);

		COleDateTime getDateTime() const;

		void Convert( ConvertParameter& pParameter );
		virtual CString GetMapValue() const;

		ElapsedTime getElapsedTime() const;

	protected:
		COleDateTime m_oleDateTime;
		int m_nDayIndex;
		ElapsedTime m_eTime;

	};








class BaseDataItem
{
public:
	BaseDataItem(void);
	virtual ~BaseDataItem(void);

public:
	//return the string of current field
	CString GetString(CSVFile& csvFile) const;

	bool IsSuccess() const;
	void Success(bool bSucess);

protected:
	
	bool m_bSuccess;

};
}
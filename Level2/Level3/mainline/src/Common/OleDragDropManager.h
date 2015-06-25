#pragma once

#define INVALID_CLIPBOARD_FORMAT     0

// class to store raw data
class RawDragDropDataItem
{
public:
	RawDragDropDataItem();
	RawDragDropDataItem(const RawDragDropDataItem& rhs);

	~RawDragDropDataItem();

	RawDragDropDataItem& operator=(const RawDragDropDataItem& rhs);

	int GetSize() const;
	void* GetData() const;

	// method provided only for OleDragDropManager
	// client should not use it
	void SetData(const void* pData, int nSize);
private:
	int m_nSize;
	char* m_pData;
};

// add type information and guard magic items
class DragDropDataItem
{
public:
	// before use this, define your new type of drag drop item
	enum DragDropDataType
	{
		Type_Invalid = 0,

		// terminal
		Type_TerminalShape,
		Type_TerminalShapePF = Type_TerminalShape, // be the same

		//

		// onboard
		Type_AircraftFurnishing,
		Type_AircraftComponentModel,
	};

	DragDropDataItem(DragDropDataType eDataType = Type_Invalid);

	DragDropDataType GetDataType() const { return m_eDataType; }
	// for data access
	int GetCount() const { return (int)m_dataList.size(); }
	RawDragDropDataItem& GetItem(int nIndex) { return m_dataList[nIndex]; }
	const RawDragDropDataItem& GetItem(int nIndex) const { return m_dataList[nIndex]; }

	void AddDragDropData(const void* pData, int nSize);
	void AddDragDropString(LPCTSTR lpStr);

	//------------------------------------------------------------
	// for OleDragDropManager only, normal code should not access these methods
	int GetBufferSize() const;
	void FillBuffer(void* pBuffer) const;
	bool AnalyzeBuffer(const void* pBuffer);
	//------------------------------------------------------------
private:
	DragDropDataType m_eDataType;

	typedef std::vector<RawDragDropDataItem> DragDropDataList;
	DragDropDataList m_dataList;
};


class OleDragDropManager
{
private:
	OleDragDropManager(void);
	~OleDragDropManager(void);

	UINT m_nRegisterClipboardFormat;
public:

	static OleDragDropManager& GetInstance();
	void SetRegisterClipboardFormat(UINT nFormat);
	// 1. add one enumeration value to DragDropDataItem::DragDropDataType
	// 2. set data here

	// for single piece of data
	void SetDragDropData(DragDropDataItem::DragDropDataType dataType, const void* pData, int nSize);
	// for multi pieces of data, construct DragDropDataItem by yourself
	void SetDragDropData(DragDropDataItem dataItem);
	// analyze drag drop data
	bool GetDragDropData(DragDropDataItem& dataItem, COleDataObject* pDataObject);

	// for compatible of old version
	// latter code should use SetDragDropData & GetDragDropData
	__declspec(deprecated) void SetRawDragDropData(const void* pData, int nSize);
	__declspec(deprecated) bool GetRawDragDropData(RawDragDropDataItem& dataItem, COleDataObject* pDataObject);
};

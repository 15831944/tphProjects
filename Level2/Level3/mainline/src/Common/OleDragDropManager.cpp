#include "StdAfx.h"
#include ".\oledragdropmanager.h"

#include <afxole.h>

#define DRAGDROP_USE_MAGIC

#ifdef DRAGDROP_USE_MAGIC
// magic number is the value we store as data header
// which will be checked when reading data
// better to keep the highest bit set as 1
#define DRAGDROP_DATA_MAGIC     0xf1732ad9
#endif


#define CHECK_DATA_NONZERO1(nData)   \
	if (0 == nData)                  \
	{                                \
		ASSERT(FALSE);               \
		return;                      \
	}

#define CHECK_DATA_NONZERO2(nData)   \
	if (0 == nData)                  \
	{                                \
		ASSERT(FALSE);               \
		return false;                \
	}

RawDragDropDataItem::RawDragDropDataItem()
	: m_nSize(0)
	, m_pData(NULL)
{

}

RawDragDropDataItem::RawDragDropDataItem( const RawDragDropDataItem& rhs )
	: m_nSize(0)
	, m_pData(NULL)
{
	SetData(rhs.GetData(), rhs.GetSize());
}
RawDragDropDataItem::~RawDragDropDataItem()
{
	if (m_pData)
	{
		delete[] m_pData;
	}
}

RawDragDropDataItem& RawDragDropDataItem::operator=( const RawDragDropDataItem& rhs )
{
	SetData(rhs.GetData(), rhs.GetSize());
	return *this;
}

int RawDragDropDataItem::GetSize() const
{
	return m_nSize;
}

void* RawDragDropDataItem::GetData() const
{
	return m_pData;
}

void RawDragDropDataItem::SetData( const void* pData, int nSize )
{
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
		m_nSize = 0;
	}
	if (pData)
	{
		m_pData = new char[nSize];
		if (m_pData)
		{
			memcpy(m_pData, pData, nSize);
			m_nSize = nSize;
		}
	}
}

DragDropDataItem::DragDropDataItem( DragDropDataType eDataType /*= Type_Invalid*/ )
	: m_eDataType(eDataType)
{

}

int DragDropDataItem::GetBufferSize() const
{
	int nTotal = 0;
	int nCount = GetCount();
	for(int i=0;i<nCount;i++)
	{
		nTotal += GetItem(i).GetSize();
	}

	return
#ifdef DRAGDROP_USE_MAGIC
		sizeof(int) +				// the magic
#endif
		sizeof(m_eDataType) +		// the data type
		sizeof(int) +				// the count of data items
		sizeof(int)*nCount +		// space for data item length
		nTotal;						// space for data item
}

void DragDropDataItem::FillBuffer(void* pBuffer) const
{
	char* buf = (char*)pBuffer;

#ifdef DRAGDROP_USE_MAGIC
	*((int*)buf) = DRAGDROP_DATA_MAGIC;
	buf += sizeof(int);
#endif

	*((DragDropDataType*)buf) = m_eDataType;
	buf += sizeof(m_eDataType);

	*((int*)buf) = GetCount();
	buf += sizeof(int);

	int nCount = GetCount();
	for(int i=0;i<nCount;i++)
	{
		int nSize = GetItem(i).GetSize();
		*((int*)buf) = nSize;
		buf += sizeof(int);
		memcpy(buf, GetItem(i).GetData(), nSize);
		buf += nSize;
	}
}

bool DragDropDataItem::AnalyzeBuffer(const void* pBuffer)
{
	const char* buf = (const char*)pBuffer;

#ifdef DRAGDROP_USE_MAGIC
	int nMagic = *((const int*)buf);
	if (DRAGDROP_DATA_MAGIC != nMagic)
		return false;
#endif
	buf += sizeof(int);

	m_eDataType = *((const DragDropDataType*)buf);
	buf += sizeof(m_eDataType);

	int nCount = *((const int*)buf);
	buf += sizeof(int);

	for(int i=0;i<nCount;i++)
	{
		int nSize = *((const int*)buf);
		buf += sizeof(int);

		AddDragDropData(buf, nSize);
		buf += nSize;
	}
	return true;
}

void DragDropDataItem::AddDragDropData( const void* pData, int nSize )
{
	m_dataList.push_back(RawDragDropDataItem());
	RawDragDropDataItem& dataItem = m_dataList.back();
	dataItem.SetData(pData, nSize);
}
void DragDropDataItem::AddDragDropString(LPCTSTR lpStr)
{
	if (NULL == lpStr)
	{
		ASSERT(FALSE);
		lpStr = _T("");
	}
	DragDropDataItem::AddDragDropData(lpStr, sizeof(TCHAR) * (_tcslen(lpStr) + 1));
}


OleDragDropManager::OleDragDropManager(void)
	: m_nRegisterClipboardFormat(INVALID_CLIPBOARD_FORMAT)
{
}

OleDragDropManager::~OleDragDropManager(void)
{
}

void OleDragDropManager::SetDragDropData( DragDropDataItem::DragDropDataType dataType, const void* pData, int nSize )
{
	DragDropDataItem dataItem(dataType);
	dataItem.AddDragDropData(pData, nSize);

	SetDragDropData(dataItem);
}

void OleDragDropManager::SetDragDropData( DragDropDataItem dataItem )
{
	CHECK_DATA_NONZERO1(m_nRegisterClipboardFormat);

	HANDLE hData = ::GlobalAlloc(GMEM_MOVEABLE, dataItem.GetBufferSize());
	void* pBuffer = (void*)::GlobalLock(hData);
	dataItem.FillBuffer(pBuffer);
	::GlobalUnlock(hData);

	COleDataSource ods;
	ods.CacheGlobalData(m_nRegisterClipboardFormat, hData);

	DROPEFFECT de = ods.DoDragDrop(DROPEFFECT_COPY);
}


bool OleDragDropManager::GetDragDropData( DragDropDataItem& dataItem, COleDataObject* pDataObject )
{
	CHECK_DATA_NONZERO2(m_nRegisterClipboardFormat);

	HGLOBAL hData = pDataObject->GetGlobalData(m_nRegisterClipboardFormat);
	if(NULL == hData)
		return false;

	void* pBuffer = (void*)::GlobalLock(hData);
	bool bAnalyzeOK = dataItem.AnalyzeBuffer(pBuffer);
	::GlobalUnlock(hData);

	return bAnalyzeOK;
}


void OleDragDropManager::SetRawDragDropData( const void* pData, int nSize )
{
	CHECK_DATA_NONZERO1(m_nRegisterClipboardFormat);

	HANDLE hData = ::GlobalAlloc(GMEM_MOVEABLE, nSize);
	void* pBuffer = (void*)::GlobalLock(hData);
	memcpy(pBuffer, pData, nSize);
	::GlobalUnlock(hData);

	COleDataSource ods;
	ods.CacheGlobalData(m_nRegisterClipboardFormat, hData);

	DROPEFFECT de = ods.DoDragDrop(DROPEFFECT_COPY);
}

bool OleDragDropManager::GetRawDragDropData( RawDragDropDataItem& dataItem, COleDataObject* pDataObject )
{
	CHECK_DATA_NONZERO2(m_nRegisterClipboardFormat);

	HGLOBAL hData = pDataObject->GetGlobalData(m_nRegisterClipboardFormat);
	if(NULL == hData)
		return false;

	void* pBuffer = (void*)::GlobalLock(hData);
	dataItem.SetData(pBuffer, ::GlobalSize(hData));
	::GlobalUnlock(hData);

	return true;
}

OleDragDropManager& OleDragDropManager::GetInstance()
{
	static OleDragDropManager oleDDManager;
	return oleDDManager;
}

void OleDragDropManager::SetRegisterClipboardFormat( UINT nFormat )
{
	m_nRegisterClipboardFormat = nFormat;
}
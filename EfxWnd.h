#pragma once
#include "CDITGExpertCtl.h"
#include "EfxMfc.h"


class CEfxMfcApp;

class ReqIdTable
{
public:
	ReqIdTable(LONG nTableSize = 50) : m_tableSize(nTableSize), m_curPos(0), m_pTable(nullptr) {
		m_pTable = new Item[m_tableSize];
		for (LONG i = 0; i < m_tableSize; i++)
			m_pTable[i].reqId = 0;
	}
	~ReqIdTable() {
		if (m_pTable) {
			delete [] m_pTable;
			m_pTable = nullptr;
		}
	}

	typedef	struct {
		int		reqId;
		char	szQryName[16];
		char	szParam[256];
	} Item;

	void SetItem(int reqId, LPCSTR lpszQryName, LPCSTR lpszParam) {
		LONG pos = InterlockedExchangeAdd(&m_curPos, 1) % m_tableSize;
		auto& item = m_pTable[pos];
		item.reqId = reqId;
		strcpy_s(item.szQryName, sizeof(item.szQryName), lpszQryName);
		if (lpszParam)
			strcpy_s(item.szParam, sizeof(item.szParam), lpszParam);
		else
			item.szParam[0] = '\0';
	}

	Item* GetItem(int reqId) {
		LONG pos = m_curPos % m_tableSize;
		for (LONG i = 0; i < m_tableSize; i++) {
			if (--pos < 0)
				pos = m_tableSize - 1;
			if (m_pTable[pos].reqId == reqId)
				return &m_pTable[pos];
		}
		return nullptr;
	}

private:
	LONG	m_tableSize;
	LONG	m_curPos;
	Item*	m_pTable;
};


// CEfxWnd

class CEfxWnd : public CDITGExpertCtl
{
	DECLARE_DYNAMIC(CEfxWnd)

public:
	CEfxWnd();
	virtual ~CEfxWnd();
	BOOL Create(CWnd* pParentWnd, UINT uID, efxEventCB fnRecvData = nullptr, efxEventCB fnRecvRealData = nullptr,
				efxEventCB fnRecvError = nullptr, efxSysMsgCB fnSysMsg = nullptr);
	AFX_INLINE ReqIdTable& GetReqIdTbl() { return m_reqIdTbl; }
	AFX_INLINE void SetRealQryName(LPCSTR lpszQryName) { strcpy_s(m_szRealQryName, sizeof(m_szRealQryName), lpszQryName); }
	AFX_INLINE LPCSTR GetRealQryName() const { return m_szRealQryName; }

private:
	UINT				m_uID;
	efxEventCB			m_fnRecvData;
	efxEventCB			m_fnRecvRealData;
	efxEventCB			m_fnError;
	efxSysMsgCB			m_fnSysMsg;

	char				m_szRealQryName[16];
	static ReqIdTable	m_reqIdTbl;

	friend class CEfxMfcApp;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_EVENTSINK_MAP()
	void OnReceiveData();
	void OnReceiveRealData();
	void OnReceiveError();
	void OnReceiveSysMessage(short nSysMsg);
};

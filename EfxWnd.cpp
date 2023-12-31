// EfxWnd.cpp : implementation file
//

#include "pch.h"
#include "EfxWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEfxWnd

ReqIdTable CEfxWnd::m_reqIdTbl;

IMPLEMENT_DYNAMIC(CEfxWnd, CDITGExpertCtl)

CEfxWnd::CEfxWnd() : m_uID(0), m_szRealQryName{0}, m_fnRecvData(nullptr), m_fnRecvRealData(nullptr), m_fnError(nullptr), m_fnSysMsg(nullptr)
{
}

CEfxWnd::~CEfxWnd()
{
}


BEGIN_MESSAGE_MAP(CEfxWnd, CDITGExpertCtl)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CEfxWnd message handlers

BOOL CEfxWnd::Create(CWnd* pParentWnd, UINT uID, efxEventCB fnRecvData, efxEventCB fnRecvRealData, efxEventCB fnRecvError, efxSysMsgCB fnSysMsg)
{
	m_uID = uID;
	m_fnRecvData = fnRecvData;
	m_fnRecvRealData = fnRecvRealData;
	m_fnError = fnRecvError;
	m_fnSysMsg = fnSysMsg;
	return CreateControl(GetClsid(), NULL, WS_CHILDWINDOW, CRect(0, 0, 10, 10), pParentWnd, IDC_ITGEXPERTCTLCTRL1);
}

int CEfxWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDITGExpertCtl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


BEGIN_EVENTSINK_MAP(CEfxWnd, CDITGExpertCtl)
	ON_EVENT(CEfxWnd, IDC_ITGEXPERTCTLCTRL1, 1, CEfxWnd::OnReceiveData, VTS_NONE)
	ON_EVENT(CEfxWnd, IDC_ITGEXPERTCTLCTRL1, 2, CEfxWnd::OnReceiveRealData, VTS_NONE)
	ON_EVENT(CEfxWnd, IDC_ITGEXPERTCTLCTRL1, 3, CEfxWnd::OnReceiveError, VTS_NONE)
	ON_EVENT(CEfxWnd, IDC_ITGEXPERTCTLCTRL1, 4, CEfxWnd::OnReceiveSysMessage, VTS_I2)
END_EVENTSINK_MAP()

void CEfxWnd::OnReceiveData()
{
	if (m_fnRecvData) {
		int nReqID = GetRecvRqID();
		auto* pItem = m_reqIdTbl.GetItem(nReqID);
		if (pItem) {
			//printf("szParam: \"%s\" \"%s\"\n", pItem->szQryName, pItem->szParam);
			m_fnRecvData(this, m_uID, pItem->szQryName, pItem->szParam);
		} else
			m_fnRecvData(this, m_uID, "", nullptr);
	}
}

void CEfxWnd::OnReceiveRealData()
{
	if (m_fnRecvRealData)
		m_fnRecvRealData(this, m_uID, m_szRealQryName, nullptr);
}

void CEfxWnd::OnReceiveError()
{
	if (m_fnError) {
		CString strCode, strMsg;
		GetReqMsgCode(strCode);
		GetReqMessage(strMsg);
		char szCode[100], szMsg[256];
		WideCharToUtf8(szCode, sizeof(szCode), strCode);
		WideCharToUtf8(szMsg, sizeof(szMsg), strMsg);
		m_fnError(this, m_uID, szCode, szMsg);
	}
}

void CEfxWnd::OnReceiveSysMessage(short nSysMsg)
{
	// nSysMsg = 1: 메인 시작, 2: 메인 종료, 3: 메인 재접속
	if (m_fnSysMsg)
		m_fnSysMsg(this, m_uID, nSysMsg);
}

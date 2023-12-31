// EfxMfc.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "resource.h"		// main symbols
#include "EfxWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma data_seg(".share")
HANDLE		g_hMutex = NULL;
LONG64		g_syncTime = 0;
LONG		g_refCount = 0;
#pragma data_seg()

#pragma comment(linker, "/SECTION:.share,RWS")

static LPCWSTR g_wszMutexName = L"Global\\EfxMfc.Synchronize";
HANDLE  g_hOpenMutex = NULL;


typedef	CMap<CStringA, LPCSTR, efxStockMasterItem*, efxStockMasterItem*>	CEfxStockMasterMap;
typedef	CMap<CStringW, LPCWSTR, efxStockMasterItem*, efxStockMasterItem*>	CEfxStockMasterMapW;
typedef	CMap<UINT, UINT, CEfxWnd*, CEfxWnd*>	CEfxWndMap;


// CEfxMfcApp
// See EfxMfc.cpp for the implementation of this class
//

class CEfxMfcApp : public CWinApp
{
public:
	CEfxMfcApp();

	AFX_INLINE CEfxWnd* GetEfxWnd() const { return m_pEfxWnd; }

	void SetSysMsgCallback(efxSysMsgCB fnSysMsg);
	bool LoadStockMaster();
	AFX_INLINE efxStockMasterItem* FindStockMaster(LPCSTR lpszStockCode) {
		return m_mapStockMstCode[lpszStockCode];
	}
	AFX_INLINE efxStockMasterItem* FindStockMasterByName(LPCSTR lpszStockName) {
		WCHAR wszStockName[41] = { 0 };
		Utf8ToWideChar(wszStockName, 41, lpszStockName);
		return m_mapStockMstName[wszStockName];
	}
	AFX_INLINE efxStockMasterItem* GetStockMasterAt(int seqNo) {
		if (seqNo < 0 || seqNo >= m_nStockMasterSize)
			return nullptr;
		return &m_arStockMaster.GetAt(seqNo);
	}

	void InsertEfxWnd(UINT uID, CEfxWnd* pEfxWnd);
	void OnEfxExit();

	static HANDLE LaunchApp(LPCWSTR lpszFilePath, LPCWSTR lpszArgs, BOOL bWaitInputIdle, BOOL bWaitTerminate, int nShowFlag = SW_SHOW);
	static int ExecCmd(CStringW& strResult, LPCWSTR lpszCmdArg, DWORD dwWait = INFINITE);

	static UINT		m_efxSeqNo;
	CString			m_strEfxDir;

private:
	CEfxWnd*		m_pEfxWnd;		// default CEfxWnd
	CEfxWndMap		m_mapEfxWnd;
	CWnd			m_wndEfxMain;
	CArray<efxStockMasterItem>	m_arStockMaster;
	int							m_nStockMasterSize;
	CEfxStockMasterMap			m_mapStockMstCode;
	CEfxStockMasterMapW			m_mapStockMstName;

	// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CEfxMfcApp, CWinApp)
END_MESSAGE_MAP()


UINT CEfxMfcApp::m_efxSeqNo = 1;

// CEfxMfcApp construction

CEfxMfcApp::CEfxMfcApp() : m_pEfxWnd(nullptr), m_nStockMasterSize(0)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
#ifdef DEBUG
	printf("efxInvestInfo size = %d\n", sizeof(efxInvestInfo));
	printf("efxStockMasterItem size = %d\n", sizeof(efxStockMasterItem));
#endif // DEBUG
}


// The one and only CEfxMfcApp object

CEfxMfcApp theApp;


// CEfxMfcApp initialization

BOOL CEfxMfcApp::InitInstance()
{
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	CoInitialize(NULL);

	WNDCLASS wndcls;
	ZeroMemory(&wndcls, sizeof(WNDCLASS));
	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpfnWndProc = ::DefWindowProc; 
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon = nullptr;
	wndcls.hCursor = nullptr;
	wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = _T("EfxMfc Window Class");
	if (!AfxRegisterClass(&wndcls)) {
		printf("Class Registration Failed!\n");
		return FALSE;
	}

	UINT _seqNo = InterlockedExchangeAdd(&m_efxSeqNo, 1);
	//printf("_seqNo = %d, m_efxSeqNo = %d\n", _seqNo, m_efxSeqNo);
	if (!m_wndEfxMain.CreateEx(0, wndcls.lpszClassName, _T("EfxMfc Main"), WS_POPUP, CRect(0, 0, 0, 0), nullptr, 0)) {
		printf("Create Window Failed!(Err: %x)\n", GetLastError());
		return FALSE;
	}
	m_pMainWnd = &m_wndEfxMain;

	m_pEfxWnd = new CEfxWnd();
	if (!m_pEfxWnd->Create(&m_wndEfxMain, _seqNo))
		return FALSE;
	m_mapEfxWnd.SetAt(_seqNo, m_pEfxWnd);

	return TRUE;
}

int CEfxMfcApp::ExitInstance()
{
	//printf("CEfxMfcApp::ExitInstance()\n");
	if (g_hOpenMutex)
		CloseHandle(g_hOpenMutex);
	if (InterlockedDecrement(&g_refCount) <= 0) {
		CloseHandle(g_hMutex);
		g_hMutex = NULL;
#ifdef DEBUG
		printf("destroy Mutex(%d)\n", g_refCount);
#endif // DEBUG
	}

	return CWinApp::ExitInstance();
}

void CEfxMfcApp::SetSysMsgCallback(efxSysMsgCB fnSysMsg)
{
	if (m_pEfxWnd)
		m_pEfxWnd->m_fnSysMsg = fnSysMsg;
}

#define	EVAL_YN(X, Y)		X = (c_bool)(Y == 'Y')

bool CEfxMfcApp::LoadStockMaster()
{
	CString strFilePath(m_strEfxDir);
	strFilePath += _T("Common\\master\\kospi_code.mst");

	CFile file;
	CFileException ex;
	if (!file.Open(strFilePath, CFile::modeRead | CFile::typeBinary, &ex)) {
		TCHAR szError[256];
		ex.GetErrorMessage(szError, 256);
		_putts(szError);
		return false;
	}

	int sequence = 0;
	m_arStockMaster.SetSize(0, 4000);
	int nRead;
	char szBuff[290] = { 0 };
	CStringA str, strToday;
	CTime now = CTime::GetCurrentTime();
	strToday.Format("%4d%02d%02d", now.GetYear(), now.GetMonth(), now.GetDay());
	CStringW strStockName;
	fast_float::from_chars_result ffResult;
	float fROE = 0;
	efxStockMasterItem item;
	ZeroMemory(&item, sizeof(item));
	item.marketType = efxMarketType::KOSPI;
	while (1) {
		nRead = file.Read(szBuff, 289);
		if (!nRead)
			break;

		item.seqNo = sequence++;
		str.SetString(szBuff, 9);
		str.TrimRight();
		strcpy_s(item.stockCode, sizeof(item.stockCode), (LPCSTR)str);
		strncpy_s(item.stockStdCode, sizeof(item.stockStdCode), &szBuff[9], 12);
		str.SetString(&szBuff[21], 40);
		str.TrimRight();
		strStockName = (LPCWSTR)CA2W(str);
		WideCharToUtf8(item.stockName, sizeof(item.stockName), strStockName);
		str.SetString(&szBuff[79], 1);
		item.kospi200Sector = (char)atoi(str);
		EVAL_YN(item.isKospi100, szBuff[80]);
		EVAL_YN(item.isKospi50, szBuff[81]);
		item.isETF = (szBuff[83] != ' ');
		EVAL_YN(item.isAcquisition, szBuff[90]);
		EVAL_YN(item.investInfo.isSuspension, szBuff[121]);
		EVAL_YN(item.investInfo.isClearanceSale, szBuff[122]);
		EVAL_YN(item.investInfo.isManagement, szBuff[123]);
		str.SetString(&szBuff[124], 2);
		switch (atoi(str)) {
		case 0: item.investInfo.marketWarnCode = efxMarketWarning::None; break;
		case 1: item.investInfo.marketWarnCode = efxMarketWarning::Caution; break;
		case 2: item.investInfo.marketWarnCode = efxMarketWarning::Warning; break;
		case 3: item.investInfo.marketWarnCode = efxMarketWarning::Danger; break;
		default:
			item.investInfo.marketWarnCode = efxMarketWarning::None;
			ASSERT(false);
			break;
		}
		EVAL_YN(item.investInfo.isMarketWarning, szBuff[126]);
		EVAL_YN(item.investInfo.isInsincerityNotice, szBuff[127]);
		EVAL_YN(item.investInfo.isBackdoorListing, szBuff[128]);
		EVAL_YN(item.investInfo.isCreditable, szBuff[138]);
		str.SetString(&szBuff[135], 3);
		item.investInfo.marginRate = (short)atoi(str);
		str.SetString(&szBuff[154], 12);
		item.investInfo.parValue = atoi(str);
		strncpy_s(item.investInfo.listingDate, sizeof(item.investInfo.listingDate), &szBuff[166], 8);
		str.SetString(&szBuff[174], 15);
		item.investInfo.listingCount = _atoi64(str) * 1000;
		item.investInfo.isNewListing = (strToday == item.investInfo.listingDate);
		str.SetString(&szBuff[189], 21);
		item.investInfo.capital = _atoi64(str);
		EVAL_YN(item.investInfo.isShortSelling, szBuff[220]);
		EVAL_YN(item.investInfo.isAbnormalRise, szBuff[221]);
		EVAL_YN(item.isKRX300, szBuff[222]);
		str.SetString(&szBuff[224], 9);
		item.investInfo.sales = atoi(str);
		str.SetString(&szBuff[233], 9);
		item.investInfo.operatingProfit = atoi(str);
		str.SetString(&szBuff[242], 9);
		item.investInfo.ordinaryProfit = atoi(str);
		str.SetString(&szBuff[251], 5);
		item.investInfo.netIncome = atoi(str);
		//str.SetString(&szBuff[256], 9);
		ffResult = fast_float::from_chars(&szBuff[256], &szBuff[256]+9, fROE);
		item.investInfo.ROE = fROE;
		//item.investInfo.ROE = (int)(fROE * 100.0f);
		strncpy_s(item.investInfo.baseYearMonth, sizeof(item.investInfo.baseYearMonth), &szBuff[265], 8);
		str.SetString(&szBuff[273], 9);
		item.investInfo.predayMarketCap = atoi(str);
		EVAL_YN(item.investInfo.isCreditLimtOver, szBuff[285]);
		EVAL_YN(item.investInfo.isLoanable, szBuff[286]);
		EVAL_YN(item.investInfo.isStockLoanable, szBuff[287]);
		m_arStockMaster.Add(item);
		m_mapStockMstCode.SetAt(item.stockCode, &m_arStockMaster.GetAt(item.seqNo));
		m_mapStockMstName.SetAt(strStockName, &m_arStockMaster.GetAt(item.seqNo));
	}
	file.Close();

	strFilePath = m_strEfxDir;
	strFilePath += _T("Common\\master\\kosdaq_code.mst");
	if (!file.Open(strFilePath, CFile::modeRead | CFile::typeBinary, &ex)) {
		TCHAR szError[256];
		ex.GetErrorMessage(szError, 256);
		_putts(szError);
		return false;
	}

	ZeroMemory(&item, sizeof(item));
	item.marketType = efxMarketType::KOSDAQ;
	while (1) {
		nRead = file.Read(szBuff, 283);
		if (!nRead)
			break;

		item.seqNo = sequence++;
		str.SetString(szBuff, 9);
		str.TrimRight();
		strcpy_s(item.stockCode, sizeof(item.stockCode), (LPCSTR)str);
		strncpy_s(item.stockStdCode, sizeof(item.stockStdCode), &szBuff[9], 12);
		str.SetString(&szBuff[21], 40);
		str.TrimRight();
		strStockName = (LPCWSTR)CA2W(str);
		WideCharToUtf8(item.stockName, sizeof(item.stockName), strStockName);
		EVAL_YN(item.isVenture, szBuff[76]);
		EVAL_YN(item.isKRX100, szBuff[78]);
		EVAL_YN(item.isAcquisition, szBuff[83]);
		EVAL_YN(item.investInfo.isSuspension, szBuff[116]);
		EVAL_YN(item.investInfo.isClearanceSale, szBuff[117]);
		EVAL_YN(item.investInfo.isManagement, szBuff[118]);
		str.SetString(&szBuff[119], 2);
		switch (atoi(str)) {
		case 0: item.investInfo.marketWarnCode = efxMarketWarning::None; break;
		case 1: item.investInfo.marketWarnCode = efxMarketWarning::Caution; break;
		case 2: item.investInfo.marketWarnCode = efxMarketWarning::Warning; break;
		case 3: item.investInfo.marketWarnCode = efxMarketWarning::Danger; break;
		default:
			item.investInfo.marketWarnCode = efxMarketWarning::None;
			ASSERT(false);
			break;
		}
		EVAL_YN(item.investInfo.isMarketWarning, szBuff[121]);
		EVAL_YN(item.investInfo.isInsincerityNotice, szBuff[122]);
		EVAL_YN(item.investInfo.isBackdoorListing, szBuff[123]);
		EVAL_YN(item.investInfo.isCreditable, szBuff[133]);
		str.SetString(&szBuff[130], 3);
		item.investInfo.marginRate = (short)atoi(str);
		str.SetString(&szBuff[149], 12);
		item.investInfo.parValue = atoi(str);
		strncpy_s(item.investInfo.listingDate, sizeof(item.investInfo.listingDate), &szBuff[161], 8);
		str.SetString(&szBuff[169], 15);
		item.investInfo.listingCount = _atoi64(str) * 1000;
		item.investInfo.isNewListing = (strToday == item.investInfo.listingDate);
		str.SetString(&szBuff[184], 21);
		item.investInfo.capital = _atoi64(str);
		EVAL_YN(item.investInfo.isShortSelling, szBuff[215]);
		EVAL_YN(item.investInfo.isAbnormalRise, szBuff[216]);
		EVAL_YN(item.isKRX300, szBuff[217]);
		str.SetString(&szBuff[218], 9);
		item.investInfo.sales = atoi(str);
		str.SetString(&szBuff[227], 9);
		item.investInfo.operatingProfit = atoi(str);
		str.SetString(&szBuff[236], 9);
		item.investInfo.ordinaryProfit = atoi(str);
		str.SetString(&szBuff[245], 5);
		item.investInfo.netIncome = atoi(str);
		//str.SetString(&szBuff[250], 9);
		ffResult = fast_float::from_chars(&szBuff[250], &szBuff[250]+9, fROE);
		item.investInfo.ROE = fROE;
		//item.investInfo.ROE = (int)(fROE * 100.0f);
		strncpy_s(item.investInfo.baseYearMonth, sizeof(item.investInfo.baseYearMonth), &szBuff[259], 8);
		str.SetString(&szBuff[267], 9);
		item.investInfo.predayMarketCap = atoi(str);
		EVAL_YN(item.investInfo.isCreditLimtOver, szBuff[279]);
		EVAL_YN(item.investInfo.isLoanable, szBuff[280]);
		EVAL_YN(item.investInfo.isStockLoanable, szBuff[281]);
		m_arStockMaster.Add(item);
		m_mapStockMstCode.SetAt(item.stockCode, &m_arStockMaster.GetAt(item.seqNo));
		m_mapStockMstName.SetAt(strStockName, &m_arStockMaster.GetAt(item.seqNo));
	}
	file.Close();
	m_nStockMasterSize = m_arStockMaster.GetCount();
#ifdef DEBUG
	printf("LoadStockMaster(): Count = %d, memSize = %d\n", sequence, sizeof(item) * m_nStockMasterSize);
#endif // DEBUG

	return true;
}

void CEfxMfcApp::InsertEfxWnd(UINT uID, CEfxWnd* pEfxWnd)
{
	m_mapEfxWnd.SetAt(uID, pEfxWnd);
}

void CEfxMfcApp::OnEfxExit()
{
	if (m_pEfxWnd)
		m_pEfxWnd->OnReceiveSysMessage(4);

	UINT id;
	CEfxWnd* pEfxWnd;
	POSITION pos = m_mapEfxWnd.GetStartPosition();
	while (pos) {
		m_mapEfxWnd.GetNextAssoc(pos, id, pEfxWnd);
		pEfxWnd->DestroyWindow();
		delete pEfxWnd;
	}
	m_mapEfxWnd.RemoveAll();
	m_wndEfxMain.DestroyWindow();
}

HANDLE CEfxMfcApp::LaunchApp(LPCWSTR lpszFilePath, LPCWSTR lpszArgs, BOOL bWaitInputIdle, BOOL bWaitTerminate, int nShowFlag)
{
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = lpszFilePath;
	ShExecInfo.lpParameters = lpszArgs;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = nShowFlag;
	ShExecInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&ShExecInfo))
		return nullptr;

	if (bWaitInputIdle)
		WaitForInputIdle(ShExecInfo.hProcess, INFINITE);

	if (bWaitTerminate)
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	return ShExecInfo.hProcess;
}

int CEfxMfcApp::ExecCmd(CStringW& strResult, LPCWSTR lpszCmdArg, DWORD dwWait)
{
	HANDLE hChildStdoutRd; // Read-side, used in calls to ReadFile() to get child's stdout output.
	HANDLE hChildStdoutWr; // Write-side, given to child process using si struct.

	// Create security attributes to create pipe.
	SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
	saAttr.bInheritHandle = TRUE; // Set the bInheritHandle flag so pipe handles are inherited by child process. Required.
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe to get results from child's stdout.
	// I'll create only 1 because I don't need to pipe to the child's stdin.
	if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
		return -1;

	STARTUPINFO si = { sizeof(STARTUPINFO) }; // specifies startup parameters for child process.
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; // STARTF_USESTDHANDLES is Required.
	si.hStdOutput = hChildStdoutWr; // Requires STARTF_USESTDHANDLES in dwFlags.
	si.hStdError = hChildStdoutWr; // Requires STARTF_USESTDHANDLES in dwFlags.
	// si.hStdInput remains null.
	si.wShowWindow = SW_HIDE; // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.

	TCHAR szCmdLine[MAX_PATH];
	_tcscpy_s(szCmdLine, MAX_PATH, lpszCmdArg);

	PROCESS_INFORMATION pi = { 0 };
	// Create the child process.
	BOOL fSuccess = CreateProcess(
		NULL,
		szCmdLine,			// command line
		NULL,               // process security attributes
		NULL,               // primary thread security attributes
		TRUE,               // TRUE=handles are inherited. Required.
		CREATE_NEW_CONSOLE, // creation flags
		NULL,               // use parent's environment
		NULL,               // use parent's current directory
		&si,                // __in, STARTUPINFO pointer
		&pi);               // __out, receives PROCESS_INFORMATION

	if (!fSuccess)
		return -1;

	// Wait until child processes exit. Don't wait forever.
	WaitForSingleObject(pi.hProcess, dwWait);
	//TerminateProcess(pi.hProcess, 0); // Kill process if it is still running. Tested using cmd "ping blah -n 99"
	DWORD dwExitCode = (DWORD)-1;
	if (!GetExitCodeProcess(pi.hProcess, &dwExitCode))
		dwExitCode = GetLastError();

	// Close the write end of the pipe before reading from the read end of the pipe.
	if (!CloseHandle(hChildStdoutWr))
		return -1;

	// Read output from the child process.
	DWORD dwRead;
	CHAR chBuf[4096];
	for (;;) {
		// Read from pipe that is the standard output for child process.
		if (!ReadFile(hChildStdoutRd, chBuf, 4096, &dwRead, NULL) || dwRead == 0)
			break;

		// Append result to string.
		strResult += CStringW(chBuf, dwRead);
	}

	// Close process and thread handles.
	CloseHandle(hChildStdoutRd);

	// CreateProcess docs specify that these must be closed. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return (int)dwExitCode;
}


// EfxMfc Impl

int efxInit(LPCSTR lpszEFXDir, efxSysMsgCB fnSysMsg)
{
	if (!IsUserAnAdmin()) {
#ifdef DEBUG
		printf("EfxInit: Administrative Privileges Required!\n");
#endif // DEBUG
		return -1;
	}
	if (!lpszEFXDir || !lpszEFXDir[0])
		theApp.m_strEfxDir = _T("C:\\eFriend Expert");
	else {
		WCHAR wszEfxDir[MAX_PATH];
		Utf8ToWideChar(wszEfxDir, _countof(wszEfxDir), lpszEFXDir);
		theApp.m_strEfxDir = wszEfxDir;
	}
	if (!theApp.m_strEfxDir.IsEmpty() && theApp.m_strEfxDir.Right(1) != _T("\\"))
		theApp.m_strEfxDir += _T("\\");
	theApp.SetSysMsgCallback(fnSysMsg);

	if (!g_hMutex) {
		g_hMutex = CreateMutexW(NULL, FALSE, g_wszMutexName);
		DWORD dwErr = GetLastError();
		if (!g_hMutex) {
			if (dwErr != ERROR_ALREADY_EXISTS)
				return (int)dwErr;
		}
	}
	g_hOpenMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, g_wszMutexName);
	if (!g_hOpenMutex)
		return (int)GetLastError();

	InterlockedIncrement(&g_refCount);

	int nRet = 0;
	if (!theApp.LoadStockMaster()) {
#ifdef DEBUG
		printf("EfxInit: LoadStockMaster() error!\n");
#endif // DEBUG
		nRet = ERROR_FILE_NOT_FOUND;
	}

	return nRet;
}

void efxExit(void)
{
	theApp.OnEfxExit();
}

UINT efxSynchronize(UINT uMilliSec)
{
	if (!g_hOpenMutex)
		return 0;

	ULONGLONG waitTime = GetTickCount64();
	WaitForSingleObject(g_hOpenMutex, INFINITE);

	ULONGLONG tickTime = GetTickCount64();
	ULONGLONG elapsedTime = tickTime - InterlockedExchangeAdd64(&g_syncTime, 0);
	if (elapsedTime < uMilliSec) {
		DWORD dwSleep = (DWORD)(uMilliSec - elapsedTime);
		Sleep(dwSleep);
		/*elapsedTime = GetTickCount64() - tickTime;
		if (elapsedTime < uMilliSec) {
			dwSleep = (DWORD)(uMilliSec - elapsedTime);
			Sleep(dwSleep + 1);
		}*/
	}
	tickTime = GetTickCount64();
	InterlockedExchange64(&g_syncTime, tickTime);
	if (g_hOpenMutex)
		ReleaseMutex(g_hOpenMutex);
	return (UINT)(tickTime - waitTime);
}

HANDLE efxLaunchApp(LPCSTR lpszFilePath, LPCSTR lpszArgs, c_bool bWaitInputIdle, c_bool bWaitTerminate, int nShowFlag)
{
	WCHAR wszFilePath[MAX_PATH] = { 0 };
	int nLen = Utf8ToWideChar(wszFilePath, _countof(wszFilePath), lpszFilePath);
	WCHAR wszArgs[MAX_PATH] = { 0 };
	if (lpszArgs)
		nLen = Utf8ToWideChar(wszArgs, _countof(wszArgs), lpszArgs);
	return CEfxMfcApp::LaunchApp(wszFilePath, wszArgs, bWaitInputIdle, bWaitTerminate, nShowFlag);
}

int efxExecCmd(LPSTR lpszResult, int nBuffLen, LPCSTR lpszCmdArg, DWORD dwWait)
{
	WCHAR wszCmd[1024] = { 0 };
	int nLen = Utf8ToWideChar(wszCmd, _countof(wszCmd), lpszCmdArg);
	CStringW strResult;
	int nRet = CEfxMfcApp::ExecCmd(strResult, wszCmd, dwWait);
	return WideCharToUtf8(lpszResult, nBuffLen, strResult);
}

int efxProcessMessage(UINT uMilliSec)
{
	MSG msg;
	int nMsgCount = 0;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			return 0;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		++nMsgCount;
	}
	Sleep(uMilliSec);
	return nMsgCount;
}

static DWORD g_dwMsgLoopThreadId = 0;

void efxMessageLoop(void)
{
	g_dwMsgLoopThreadId = GetCurrentThreadId();

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void efxQuit(int nExitCode)
{
	if (g_dwMsgLoopThreadId != 0)
		PostThreadMessage(g_dwMsgLoopThreadId, WM_QUIT, nExitCode, 0);
	else
		PostQuitMessage(nExitCode);
}

efxStockMasterItem* efxFindStockMaster(LPCSTR lpszStockCode)
{
	return theApp.FindStockMaster(lpszStockCode);
}

efxStockMasterItem* efxGetStockMaster(int seqNo)
{
	return theApp.GetStockMasterAt(seqNo);
}

HANDLE efxNewControl(UINT uCtrlID, efxEventCB fnRecvData, efxEventCB fnRecvRealData, efxEventCB fnError)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UINT _seqNo = InterlockedExchangeAdd(&CEfxMfcApp::m_efxSeqNo, 1);
	auto pEfxWnd = new CEfxWnd();
	if (!pEfxWnd->Create(theApp.GetMainWnd(), uCtrlID, fnRecvData, fnRecvRealData, fnError))
		return nullptr;
	theApp.InsertEfxWnd(_seqNo, pEfxWnd);
	return pEfxWnd;
}

int efxGetAccountCount(void)
{
	auto pEfxWnd = theApp.GetEfxWnd();
	if (pEfxWnd)
		return pEfxWnd->GetAccountCount();
	return 0;
}

int efxGetAccount(int nIdx, LPSTR lpszAccount, int nBuffLen)
{
	auto pEfxWnd = theApp.GetEfxWnd();
	if (!pEfxWnd)
		return 0;
	CString strAccount = pEfxWnd->GetAccount(nIdx);
	return WideCharToUtf8(lpszAccount, nBuffLen, strAccount);
}

int efxGetAccountBrcode(LPCSTR lpszAcntNo, LPSTR lpszBrcode, int nBuffLen)
{
	auto pEfxWnd = theApp.GetEfxWnd();
	if (!pEfxWnd)
		return 0;
	WCHAR wszTemp[32] = { 0 };
	int nLen = Utf8ToWideChar(wszTemp, _countof(wszTemp), lpszAcntNo);
	CString strBrcode = pEfxWnd->GetAccountBrcode(wszTemp);
	return WideCharToUtf8(lpszBrcode, nBuffLen, strBrcode);
}

int efxGetEncryptPassword(LPCSTR lpszPwd, LPSTR lpszEncrypt, int nBuffLen)
{
	auto pEfxWnd = theApp.GetEfxWnd();
	if (!pEfxWnd)
		return 0;
	WCHAR wszTemp[256] = { 0 };
	int nLen = Utf8ToWideChar(wszTemp, _countof(wszTemp), lpszPwd);
	CString strEncrypt = pEfxWnd->GetEncryptPassword(wszTemp);
	return WideCharToUtf8(lpszEncrypt, nBuffLen, strEncrypt);
}

void efxAboutBox(void)
{
	auto pEfxWnd = theApp.GetEfxWnd();
	if (pEfxWnd)
		pEfxWnd->AboutBox();
}

void efxSetSingleData(HANDLE hCtrl, int nIndex, LPCSTR lpszData)
{
	//printf("lpszData: \"%s\"\n", lpszData);
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	wchar_t wszData[256] = { 0 };
	Utf8ToWideChar(wszData, _countof(wszData), lpszData);
	pEfxWnd->SetSingleData(nIndex, wszData);
}

void efxSetSingleDataEx(HANDLE hCtrl, int nBlockIdx, int nFieldIdx, LPCSTR lpszValue)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	wchar_t wszData[256] = { 0 };
	Utf8ToWideChar(wszData, _countof(wszData), lpszValue);
	pEfxWnd->SetSingleDataEx(nBlockIdx, nFieldIdx, wszData);
}

void efxSetMultiData(HANDLE hCtrl, int nRecIdx, int nFieldIdx, LPCSTR lpszValue)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	wchar_t wszData[256] = { 0 };
	Utf8ToWideChar(wszData, _countof(wszData), lpszValue);
	pEfxWnd->SetMultiData(nRecIdx, nFieldIdx, wszData);
}

void efxSetMultiBlockData(HANDLE hCtrl, int nBlockIdx, int nRecIdx, int nFieldIdx, LPCSTR lpszValue)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	wchar_t wszData[256] = { 0 };
	Utf8ToWideChar(wszData, _countof(wszData), lpszValue);
	pEfxWnd->SetMultiBlockData(nBlockIdx, nRecIdx, nFieldIdx, wszData);
}

void efxRequestData(HANDLE hCtrl, LPCSTR lpszQryName, LPCSTR lpszParam)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	wchar_t wszQryName[16] = { 0 };
	Utf8ToWideChar(wszQryName, _countof(wszQryName), lpszQryName);
	pEfxWnd->RequestData(wszQryName);
	int nRqId = pEfxWnd->GetSendRqID();
	pEfxWnd->GetReqIdTbl().SetItem(nRqId, lpszQryName, lpszParam);
}

int efxIsMoreNextData(HANDLE hCtrl)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	return pEfxWnd->IsMoreNextData();
}

void efxRequestNextData(HANDLE hCtrl, LPCSTR lpszQryName)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	wchar_t wszQryName[16] = { 0 };
	Utf8ToWideChar(wszQryName, _countof(wszQryName), lpszQryName);
	pEfxWnd->RequestNextData(wszQryName);
	int nRqId = pEfxWnd->GetSendRqID();
	pEfxWnd->GetReqIdTbl().SetItem(nRqId, lpszQryName, "");
}

void efxRequestRealData(HANDLE hCtrl, LPCSTR lpszQryName, LPCSTR lpszCode)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	wchar_t wszQryName[16] = { 0 }, wszCode[1024] = { 0 };
	Utf8ToWideChar(wszQryName, _countof(wszQryName), lpszQryName);
	Utf8ToWideChar(wszCode, _countof(wszCode), lpszCode);
	pEfxWnd->RequestRealData(wszQryName, wszCode);
	pEfxWnd->SetRealQryName(lpszQryName);
}

void efxUnrequestRealData(HANDLE hCtrl, LPCSTR lpszQryName, LPCSTR lpszCode)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	wchar_t wszQryName[16] = { 0 }, wszCode[1024] = { 0 };
	Utf8ToWideChar(wszQryName, _countof(wszQryName), lpszQryName);
	Utf8ToWideChar(wszCode, _countof(wszCode), lpszCode);
	pEfxWnd->UnrequestRealData(wszQryName, wszCode);
}

void efxUnrequestAllRealData(HANDLE hCtrl)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	pEfxWnd->UnrequestAllRealData();
}

int efxGetSingleFieldCount(HANDLE hCtrl)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	return pEfxWnd->GetSingleFieldCount();
}

int efxGetSingleData(HANDLE hCtrl, LPSTR lpszData, int nBuffLen, int nIndex, int nAttrType)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	pEfxWnd->GetSingleData(strData, nIndex, nAttrType);
	return WideCharToUtf8(lpszData, nBuffLen, strData);
}

int efxGetSingleDataInt(HANDLE hCtrl, int nIndex, int nAttrType)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	pEfxWnd->GetSingleData(strData, nIndex, nAttrType);
	return _ttoi(strData);
}

float efxGetSingleDataFloat(HANDLE hCtrl, int nIndex, int nAttrType)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	pEfxWnd->GetSingleData(strData, nIndex, nAttrType);
	int nDataLen = strData.GetLength();
	float result = 0.0f;
	char szValue[41];
	if (nDataLen >= sizeof(szValue)) {
		strData.Truncate(sizeof(szValue) - 1);
		nDataLen = strData.GetLength();
	}
	int len = WideCharToMultiByte(CP_ACP, 0, strData, nDataLen, NULL, 0, NULL, NULL);
	len = WideCharToMultiByte(CP_ACP, 0, strData, nDataLen, szValue, len, NULL, NULL);
	szValue[len] = '\0';
	auto answer = fast_float::from_chars(szValue, szValue+len, result);
	return result;
}

void efxGetSingleDataFloat2(HANDLE hCtrl, float* pResult, int nIndex, int nAttrType)
{
	*pResult = efxGetSingleDataFloat(hCtrl, nIndex, nAttrType);
}

int efxGetMultiBlockCount(HANDLE hCtrl)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	return pEfxWnd->GetMultiBlockCount();
}

int efxGetMultiRecordCount(HANDLE hCtrl, int nBlockIdx)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	return pEfxWnd->GetMultiRecordCount(nBlockIdx);
}

int efxGetMultiFieldCount(HANDLE hCtrl, int nBlockIdx, int nRecIdx)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	return pEfxWnd->GetMultiFieldCount(nBlockIdx, nRecIdx);
}

int efxGetMultiData(HANDLE hCtrl, LPSTR lpszData, int nBuffLen, int nBlockIdx, int nRecIdx, int nFieldIdx, int nAttrType)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	pEfxWnd->GetMultiData(strData, nBlockIdx, nRecIdx, nFieldIdx, nAttrType);
	return WideCharToUtf8(lpszData, nBuffLen, strData);
}

int efxGetMultiDataInt(HANDLE hCtrl, int nBlockIdx, int nRecIdx, int nFieldIdx, int nAttrType)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	pEfxWnd->GetMultiData(strData, nBlockIdx, nRecIdx, nFieldIdx, nAttrType);
	return _ttoi(strData);
}

float efxGetMultiDataFloat(HANDLE hCtrl, int nBlockIdx, int nRecIdx, int nFieldIdx, int nAttrType)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	pEfxWnd->GetMultiData(strData, nBlockIdx, nRecIdx, nFieldIdx, nAttrType);
	int nDataLen = strData.GetLength();
	float result = 0.0f;
	char szValue[41];
	if (nDataLen >= sizeof(szValue)) {
		strData.Truncate(sizeof(szValue) - 1);
		nDataLen = strData.GetLength();
	}
	int len = WideCharToMultiByte(CP_ACP, 0, strData, nDataLen, NULL, 0, NULL, NULL);
	len = WideCharToMultiByte(CP_ACP, 0, strData, nDataLen, szValue, len, NULL, NULL);
	szValue[len] = '\0';
	auto answer = fast_float::from_chars(szValue, szValue+len, result);
	return result;
}

void efxGetMultiDataFloat2(HANDLE hCtrl, float* pResult, int nBlockIdx, int nRecIdx, int nFieldIdx, int nAttrType)
{
	*pResult = efxGetMultiDataFloat(hCtrl, nBlockIdx, nRecIdx, nFieldIdx, nAttrType);
}

int efxGetReqMsgCode(HANDLE hCtrl, LPSTR lpszMsgCode, int nBuffLen)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	pEfxWnd->GetReqMsgCode(strData);
	return WideCharToUtf8(lpszMsgCode, nBuffLen, strData);
}

int efxGetReqMessage(HANDLE hCtrl, LPSTR lpszMessage, int nBuffLen)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	pEfxWnd->GetReqMessage(strData);
	return WideCharToUtf8(lpszMessage, nBuffLen, strData);
}

int efxGetRtCode(HANDLE hCtrl, LPSTR lpszRtCode, int nBuffLen)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	pEfxWnd->GetRtCode(strData);
	return WideCharToUtf8(lpszRtCode, nBuffLen, strData);
}

int efxGetSendRqID(HANDLE hCtrl)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	return pEfxWnd->GetSendRqID();
}

int efxGetRecvRqID(HANDLE hCtrl)
{
	auto pEfxWnd = reinterpret_cast<CEfxWnd*>(hCtrl);
	CString strData;
	return pEfxWnd->GetRecvRqID();
}

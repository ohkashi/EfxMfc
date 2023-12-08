// EfxMfc.h : main header file for the EfxMfc DLL
//

#pragma once

#ifdef EFXMFC_EXPORTS
#define EFXMFC_API __declspec(dllexport)
#else
#define EFXMFC_API __declspec(dllimport)
#endif


typedef	__int8		c_bool;

enum class efxMarketType { KOSPI, KOSDAQ };
enum class efxMarketWarning { None, Caution, Warning, Danger };

struct efxInvestInfo {
	c_bool	isSuspension;				// �ŷ�����
	c_bool	isClearanceSale;			// �����Ÿ�
	c_bool	isManagement;				// ��������
	efxMarketWarning	marketWarnCode;	// ���� ��� �ڵ�
	c_bool	isMarketWarning;			// ��� ���� ����
	c_bool	isInsincerityNotice;		// �Ҽ��� ���� ����
	c_bool	isBackdoorListing;			// ��ȸ���� ����
	c_bool	isCreditable;				// �ſ��ֹ� ����
	short	marginRate;					// ���ű� ����
	int		parValue;					// �׸鰡
	char	listingDate[9];				// ���� ��¥
	__int64	listingCount;				// ���� �ּ�
	c_bool	isNewListing;				// �űԻ��� ����
	__int64	capital;					// �ں���
	c_bool	isShortSelling;				// ���ŵ��ֹ����� ����
	c_bool	isAbnormalRise;				// �̻�޵����� ����
	int		sales;						// �����
	int		operatingProfit;			// ��������
	int		ordinaryProfit;				// �������
	int		netIncome;					// ��������
	float	ROE;						// �ڱ��ں����ͷ�
	char	baseYearMonth[9];			// ���س��
	UINT	predayMarketCap;			// ���ϱ��� �ð��Ѿ�(����: ���)
	c_bool	isCreditLimtOver;			// ȸ��ſ��ѵ��ʰ� ����
	c_bool	isLoanable;					// �㺸���Ⱑ�� ����
	c_bool	isStockLoanable;			// ���ְ��� ����
};

struct efxStockMasterItem {
	int		seqNo;
	char	stockCode[10];
	char	stockStdCode[13];
	char	stockName[61];				// UTF-8 �ѱ�20��
	efxMarketType	marketType;
	c_bool	isVenture;
	c_bool	isKospi50;
	c_bool	isKospi100;
	char	kospi200Sector;
	c_bool	isKRX100;
	c_bool	isKRX300;
	c_bool	isETF;
	c_bool	isAcquisition;				// ����μ�����ȸ�� ����
	efxInvestInfo	investInfo;
};

typedef void (CALLBACK *efxEventCB)(const HANDLE hCtrl, UINT uCtrlID, LPCSTR lpszQryName, LPCSTR lpszParam);
typedef void (CALLBACK *efxSysMsgCB)(const HANDLE hCtrl, UINT uCtrlID, int nSysMsg);

EXTERN_C {
	EFXMFC_API int efxInit(LPCSTR lpszEFXDir, efxSysMsgCB fnSysMsg);
	EFXMFC_API void efxExit(void);
	EFXMFC_API UINT efxSynchronize(UINT uMilliSec);
	EFXMFC_API HANDLE efxLaunchApp(LPCSTR lpszFilePath, LPCSTR lpszArgs, c_bool bWaitInputIdle, c_bool bWaitTerminate, int nShowFlag);
	EFXMFC_API int efxExecCmd(LPSTR lpszResult, int nBuffLen, LPCSTR lpszCmdArg, DWORD dwWait);
	EFXMFC_API int efxProcessMessage(UINT uMilliSec);
	EFXMFC_API void efxMessageLoop(void);
	EFXMFC_API void efxQuit(int nExitCode);
	EFXMFC_API efxStockMasterItem* efxFindStockMaster(LPCSTR lpszStockCode);
	EFXMFC_API efxStockMasterItem* efxFindStockMasterByName(LPCSTR lpszStockName);
	EFXMFC_API efxStockMasterItem* efxGetStockMaster(int seqNo);

	EFXMFC_API HANDLE efxNewControl(UINT uCtrlID, efxEventCB fnRecvData, efxEventCB fnRecvRealData, efxEventCB fnError);
	EFXMFC_API int efxGetAccountCount(void);
	EFXMFC_API int efxGetAccount(int nIdx, LPSTR lpszAccount, int nBuffLen);
	EFXMFC_API int efxGetAccountBrcode(LPCSTR lpszAcntNo, LPSTR lpszBrcode, int nBuffLen);
	EFXMFC_API int efxGetEncryptPassword(LPCSTR lpszPwd, LPSTR lpszEncrypt, int nBuffLen);
	EFXMFC_API void efxAboutBox(void);

	EFXMFC_API void efxSetSingleData(HANDLE hCtrl, int nIndex, LPCSTR lpszData);
	EFXMFC_API void efxSetSingleDataEx(HANDLE hCtrl, int nBlockIdx, int nFieldIdx, LPCSTR lpszValue);
	EFXMFC_API void efxSetMultiData(HANDLE hCtrl, int nRecIdx, int nFieldIdx, LPCSTR lpszValue);
	EFXMFC_API void efxSetMultiBlockData(HANDLE hCtrl, int nBlockIdx, int nRecIdx, int nFieldIdx, LPCSTR lpszValue);

	EFXMFC_API void efxRequestData(HANDLE hCtrl, LPCSTR lpszQryName, LPCSTR lpszParam);
	EFXMFC_API int efxIsMoreNextData(HANDLE hCtrl);
	EFXMFC_API void efxRequestNextData(HANDLE hCtrl, LPCSTR lpszQryName);
	EFXMFC_API void efxRequestRealData(HANDLE hCtrl, LPCSTR lpszQryName, LPCSTR lpszCode);
	EFXMFC_API void efxUnrequestRealData(HANDLE hCtrl, LPCSTR lpszQryName, LPCSTR lpszCode);
	EFXMFC_API void efxUnrequestAllRealData(HANDLE hCtrl);

	EFXMFC_API int efxGetSingleFieldCount(HANDLE hCtrl);
	EFXMFC_API int efxGetSingleData(HANDLE hCtrl, LPSTR lpszData, int nBuffLen, int nIndex, int nAttrType);
	EFXMFC_API int efxGetSingleDataInt(HANDLE hCtrl, int nIndex, int nAttrType);
	EFXMFC_API float efxGetSingleDataFloat(HANDLE hCtrl, int nIndex, int nAttrType);
	EFXMFC_API void efxGetSingleDataFloat2(HANDLE hCtrl, float* pResult, int nIndex, int nAttrType);

	EFXMFC_API int efxGetMultiBlockCount(HANDLE hCtrl);
	EFXMFC_API int efxGetMultiRecordCount(HANDLE hCtrl, int nBlockIdx);
	EFXMFC_API int efxGetMultiFieldCount(HANDLE hCtrl, int nBlockIdx, int nRecIdx);
	EFXMFC_API int efxGetMultiData(HANDLE hCtrl, LPSTR lpszData, int nBuffLen, int nBlockIdx, int nRecIdx, int nFieldIdx, int nAttrType);
	EFXMFC_API int efxGetMultiDataInt(HANDLE hCtrl, int nBlockIdx, int nRecIdx, int nFieldIdx, int nAttrType);
	EFXMFC_API float efxGetMultiDataFloat(HANDLE hCtrl, int nBlockIdx, int nRecIdx, int nFieldIdx, int nAttrType);
	EFXMFC_API void efxGetMultiDataFloat2(HANDLE hCtrl, float* pResult, int nBlockIdx, int nRecIdx, int nFieldIdx, int nAttrType);

	EFXMFC_API int efxGetReqMsgCode(HANDLE hCtrl, LPSTR lpszMsgCode, int nBuffLen);
	EFXMFC_API int efxGetReqMessage(HANDLE hCtrl, LPSTR lpszMessage, int nBuffLen);
	EFXMFC_API int efxGetRtCode(HANDLE hCtrl, LPSTR lpszRtCode, int nBuffLen);
	EFXMFC_API int efxGetSendRqID(HANDLE hCtrl);
	EFXMFC_API int efxGetRecvRqID(HANDLE hCtrl);
}
// CDITGExpertCtl.h  : Microsoft Visual C++로 만든 ActiveX 컨트롤 래퍼 클래스의 선언입니다.

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDITGExpertCtl

class CDITGExpertCtl : public CWnd
{
protected:
	DECLARE_DYNCREATE(CDITGExpertCtl)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid = { 0x8E39D09, 0x206D, 0x43D1, { 0xAC, 0x78, 0xD1, 0xAE, 0x36, 0x35, 0xA4, 0xE9 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE, BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 특성입니다.
public:

// 작업입니다.
public:

	void SetSingleData(short nFieldIdx, LPCWSTR lpszValue)
	{
		COleVariant vValue(lpszValue);
		static BYTE parms[] = VTS_I2 VTS_VARIANT ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nFieldIdx, &vValue);
	}
	void SetMultiData(short nRecIdx, short nFieldIdx, LPCWSTR lpszValue)
	{
		COleVariant vValue(lpszValue);
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_VARIANT ;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nRecIdx, nFieldIdx, &vValue);
	}
	short GetSingleFieldCount()
	{
		short result;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	short GetMultiBlockCount()
	{
		short result;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	short GetMultiRecordCount(short nBlockIdx)
	{
		short result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_I2, (void*)&result, parms, nBlockIdx);
		return result;
	}
	short GetMultiFieldCount(short nBlockIdx, short nRecIdx)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_I2 ;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_I2, (void*)&result, parms, nBlockIdx, nRecIdx);
		return result;
	}
	CString GetSingleData(short nFieldIdx, short nAttrType = 0)
	{
		COleVariant result;
		static BYTE parms[] = VTS_I2 VTS_I2 ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_VARIANT, result, parms, nFieldIdx, nAttrType);
		return V_BSTR(&result);
	}
	void GetSingleData(CString& strData, short nFieldIdx, short nAttrType = 0)
	{
		static BYTE parms[] = VTS_I2 VTS_I2;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_BSTR, (void*)&strData, parms, nFieldIdx, nAttrType);
	}
	CString GetMultiData(short nBlockIdx, short nRecIdx, short nFieldIdx, short nAttrType = 0)
	{
		COleVariant result;
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_VARIANT, result, parms, nBlockIdx, nRecIdx, nFieldIdx, nAttrType);
		return V_BSTR(&result);
	}
	void GetMultiData(CString& strData, short nBlockIdx, short nRecIdx, short nFieldIdx, short nAttrType = 0)
	{
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 VTS_I2;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_BSTR, (void*)&strData, parms, nBlockIdx, nRecIdx, nFieldIdx, nAttrType);
	}
	CString GetReqMsgCode()
	{
		COleVariant result;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_VARIANT, result, NULL);
		return V_BSTR(&result);
	}
	void GetReqMsgCode(CString& strResult)
	{
		InvokeHelper(0x9, DISPATCH_METHOD, VT_BSTR, (void*)&strResult, NULL);
	}
	CString GetReqMessage()
	{
		COleVariant result;
		InvokeHelper(0xa, DISPATCH_METHOD, VT_VARIANT, result, NULL);
		return V_BSTR(&result);
	}
	void GetReqMessage(CString& strResult)
	{
		InvokeHelper(0xa, DISPATCH_METHOD, VT_BSTR, (void*)&strResult, NULL);
	}
	short RequestData(LPCWSTR lpszQryName)
	{
		short result;
		COleVariant vQryName(lpszQryName);
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0xe, DISPATCH_METHOD, VT_I2, (void*)&result, parms, &vQryName);
		return result;
	}
	short RequestNextData(LPCWSTR lpszQryName)
	{
		short result;
		COleVariant vQryName(lpszQryName);
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0xf, DISPATCH_METHOD, VT_I2, (void*)&result, parms, &vQryName);
		return result;
	}
	short RequestRealData(LPCWSTR lpszQryName, LPCWSTR lpszCode)
	{
		short result;
		COleVariant vQryName(lpszQryName), vCode(lpszCode);
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT ;
		InvokeHelper(0x10, DISPATCH_METHOD, VT_I2, (void*)&result, parms, &vQryName, &vCode);
		return result;
	}
	short UnrequestRealData(LPCWSTR lpszQryName, LPCWSTR lpszCode)
	{
		short result;
		COleVariant vQryName(lpszQryName), vCode(lpszCode);
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT ;
		InvokeHelper(0x11, DISPATCH_METHOD, VT_I2, (void*)&result, parms, &vQryName, &vCode);
		return result;
	}
	short UnrequestAllRealData()
	{
		short result;
		InvokeHelper(0x12, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	void SetMultiBlockData(short nBlockIdx, short nRecIdx, short nFieldIdx, LPCWSTR lpszValue)
	{
		COleVariant vValue(lpszValue);
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 VTS_VARIANT ;
		InvokeHelper(0x13, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nBlockIdx, nRecIdx, nFieldIdx, &vValue);
	}
	signed char IsMoreNextData()
	{
		signed char result;
		InvokeHelper(0x14, DISPATCH_METHOD, VT_I1, (void*)&result, NULL);
		return result;
	}
	short GetAccountCount()
	{
		short result;
		InvokeHelper(0x15, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	CString GetAccount(short nIdx)
	{
		COleVariant result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x16, DISPATCH_METHOD, VT_VARIANT, result, parms, nIdx);
		return V_BSTR(&result);
	}
	CString GetAccountBrcode(LPCWSTR lpszAcntNo)
	{
		COleVariant result, vAcntNo(lpszAcntNo);
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x17, DISPATCH_METHOD, VT_VARIANT, result, parms, &vAcntNo);
		return V_BSTR(&result);
	}
	CString GetEncryptPassword(LPCWSTR lpszPwd)
	{
		COleVariant result, vPwd(lpszPwd);
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x18, DISPATCH_METHOD, VT_VARIANT, result, parms, &vPwd);
		return V_BSTR(&result);
	}
	void SetSingleDataEx(short nBlockIdx, short nFieldIdx, LPCWSTR lpszValue)
	{
		COleVariant vValue(lpszValue);
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_VARIANT ;
		InvokeHelper(0x19, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nBlockIdx, nFieldIdx, &vValue);
	}
	CString GetSingleDataEx(short nBlockIdx, short nFieldIdx, short nAttrType = 0)
	{
		COleVariant result;
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x1a, DISPATCH_METHOD, VT_VARIANT, result, parms, nBlockIdx, nFieldIdx, nAttrType);
		return V_BSTR(&result);
	}
	void GetSingleDataEx(CString& strData, short nBlockIdx, short nFieldIdx, short nAttrType = 0)
	{
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2;
		InvokeHelper(0x1a, DISPATCH_METHOD, VT_BSTR, (void*)&strData, parms, nBlockIdx, nFieldIdx, nAttrType);
	}
	short GetSingleFieldCountEx(short nBlockIdx)
	{
		short result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x1b, DISPATCH_METHOD, VT_I2, (void*)&result, parms, nBlockIdx);
		return result;
	}
	CString GetRtCode()
	{
		COleVariant result;
		InvokeHelper(0x1c, DISPATCH_METHOD, VT_VARIANT, result, NULL);
		return V_BSTR(&result);
	}
	void GetRtCode(CString& strResult)
	{
		InvokeHelper(0x1c, DISPATCH_METHOD, VT_BSTR, (void*)&strResult, NULL);
	}
	CString GetOverSeasStockSise()
	{
		COleVariant result;
		InvokeHelper(0x1d, DISPATCH_METHOD, VT_VARIANT, result, NULL);
		return V_BSTR(&result);
	}
	unsigned char IsMoreNextData2()
	{
		COleVariant result;
		InvokeHelper(0x1e, DISPATCH_METHOD, VT_VARIANT, result, NULL);
		return V_I1(&result);
	}
	INT GetSendRqID()
	{
		COleVariant result;
		InvokeHelper(0x1f, DISPATCH_METHOD, VT_VARIANT, result, NULL);
		return V_INT(&result);
	}
	INT GetRecvRqID()
	{
		COleVariant result;
		InvokeHelper(0x20, DISPATCH_METHOD, VT_VARIANT, result, NULL);
		return V_INT(&result);
	}
	CString _getConnectID()
	{
		CString result;
		GetProperty(0x21, VT_BSTR, (void*)&result);
		return result;
	}
	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	__declspec(property(get = _getConnectID)) CString ConnectID;
};

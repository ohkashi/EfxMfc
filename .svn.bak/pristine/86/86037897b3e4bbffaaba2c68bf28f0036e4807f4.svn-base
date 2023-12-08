extern crate libc;

#[allow(unused_imports)]
use libc::*;
use std::ptr;


#[allow(dead_code)]
pub const REQ_LIMIT_MS: libc::c_uint = 10;  // 초당 조회TR 및 주문TR 제한: 20ms 간 5개 TR 이하 호출

#[repr(C)]
#[derive(Copy, Clone, Debug)]
#[allow(dead_code)]
pub enum MarketType {
    KOSPI = 0,
    KOSDAQ = 1
}

#[repr(C)]
#[derive(Copy, Clone, Debug)]
#[allow(dead_code)]
pub enum MarketWarning {
    None = 0,
    Caution = 1,
    Warning = 2,
    Danger = 3
}

#[repr(C)]
#[derive(Debug)]
pub struct InvestInfo {
    pub is_suspension:      bool,		// 거래정지
    pub is_clearance_sale:  bool,  		// 정리매매
    pub is_management:      bool,		// 관리종목
    pub market_warn_code:   self::MarketWarning,   // 시장 경고 코드
    pub is_market_warning:  bool,		// 경고 예고 여부
    pub is_insincerity_notice:bool, 	// 불성실 공시 여부
    pub is_backdoor_listing:bool,		// 우회상장 여부
    pub is_creditable:      bool,		// 신용주문 가능
    pub margin_rate:        i16,		// 증거금 비율
    pub par_value:          i32,		// 액면가
    pub listing_date:       [u8; 9],	// 상장 날짜
    pub listing_count:      i64,		// 상장 주수
    pub is_new_listing:     bool,		// 신규상장 여부
    pub capital:            i64,		// 자본금
    pub is_short_selling:   bool,		// 공매도주문가능 여부
    pub is_abnormal_rise:   bool,		// 이상급등종목 여부
    pub sales:              i32,		// 매출액
    pub operating_profit:   i32,		// 영업이익
    pub ordinary_profit:    i32,		// 경상이익
    pub net_income:         i32,		// 당기순이익
    pub roe:                f32,		// 자기자본이익률
    pub base_year_month:    [u8; 9],	// 기준년월
    pub preday_market_cap:  u32,		// 전일기준 시가총액(단위: 억원)
    pub is_credit_limt_over:bool,		// 회사신용한도초과 여부
    pub is_loanable:        bool,		// 담보대출가능 여부
    pub is_stock_loanable:  bool,		// 대주가능 여부
}

#[repr(C)]
pub struct StockMasterItem {
    pub seq_no:         libc::c_int,
    pub stock_code:     [u8; 10],
    pub standard_code:  [u8; 13],
    pub stock_name:     [u8; 61],
    pub market_type:    self::MarketType,
    pub is_venture:     bool,
    pub is_kospi50:     bool,
    pub is_kospi100:    bool,
    pub kospi200_sector:i8,             // KOSPI200 섹터업종
    pub is_krx100:      bool,
    pub is_krx300:      bool,
    pub is_etf:         bool,
    pub is_acquisition: bool,           // 기업인수목적회사 여부
    pub invest_info:    self::InvestInfo,
}

#[derive(Clone, Debug)]
#[allow(dead_code)]
pub struct StockMasterInfo {
    pub seq_no:             i32,
    pub stock_code:         String,
    pub stock_name:         String,
    pub market_type:        self::MarketType,
    pub is_venture:         bool,
    pub is_kospi50:         bool,
    pub is_kospi100:        bool,
    pub kospi200_sector:    i8,         // KOSPI200 섹터업종
    pub is_krx100:          bool,
    pub is_krx300:          bool,
    pub is_etf:             bool,
    pub is_acquisition:     bool,       // 기업인수목적회사 여부
    pub is_suspension:      bool,		// 거래정지
    pub is_clearance_sale:  bool,  		// 정리매매
    pub is_management:      bool,		// 관리종목
    pub market_warn_code:   self::MarketWarning,   // 시장 경고 코드
    pub is_market_warning:  bool,		// 경고 예고 여부
    pub is_insincerity_notice:bool, 	// 불성실 공시 여부
    pub is_backdoor_listing:bool,		// 우회상장 여부
    pub is_creditable:      bool,		// 신용주문 가능
    pub margin_rate:        i16,		// 증거금 비율
    pub par_value:          i32,		// 액면가
    pub listing_date:       String, 	// 상장 날짜
    pub listing_count:      i64,		// 상장 주수
    pub is_new_listing:     bool,		// 신규상장 여부
    pub capital:            i64,		// 자본금
    pub is_short_selling:   bool,		// 공매도주문가능 여부
    pub is_abnormal_rise:   bool,		// 이상급등종목 여부
    pub sales:              i32,		// 매출액
    pub operating_profit:   i32,		// 영업이익
    pub ordinary_profit:    i32,		// 경상이익
    pub net_income:         i32,		// 당기순이익
    pub roe:                f32,		// 자기자본이익률
    pub base_year_month:    String, 	// 기준년월
    pub preday_market_cap:  u32,		// 전일기준 시가총액(단위: 억원)
    pub is_credit_limt_over:bool,		// 회사신용한도초과 여부
    pub is_loanable:        bool,		// 담보대출가능 여부
    pub is_stock_loanable:  bool,		// 대주가능 여부
}

impl StockMasterInfo {
    #[allow(dead_code)]
    pub fn new(item:*const self::StockMasterItem) -> StockMasterInfo {
        unsafe {
            let mut data:Vec<u8> = (*item).stock_code.to_vec();
            let mut idx_null = data.iter().position(|&x| x == 0).unwrap();
            data.resize(idx_null, 0);
            let stock_code = String::from_utf8_lossy(&data);
            let mut data2 = (*item).stock_name.to_vec();
            idx_null = data2.iter().position(|&x| x == 0).unwrap();
            data2.resize(idx_null, 0);
            let stock_name = String::from_utf8_lossy(&data2);
            let v_ldate = (*item).invest_info.listing_date.to_vec();
            let ldate = std::str::from_utf8(&v_ldate).unwrap();
            let v_ym = (*item).invest_info.base_year_month.to_vec();
            let base_ym = std::str::from_utf8(&v_ym).unwrap();

            StockMasterInfo {
                seq_no: (*item).seq_no,
                stock_code: String::from(stock_code),
                stock_name: String::from(stock_name),
                market_type: (*item).market_type,
                is_venture: (*item).is_venture,
                is_kospi50: (*item).is_kospi50,
                is_kospi100: (*item).is_kospi100,
                kospi200_sector: (*item).kospi200_sector,
                is_krx100: (*item).is_krx100,
                is_krx300: (*item).is_krx300,
                is_etf: (*item).is_etf,
                is_acquisition: (*item).is_acquisition,
                is_suspension: (*item).invest_info.is_suspension,
                is_clearance_sale: (*item).invest_info.is_clearance_sale,
                is_management: (*item).invest_info.is_management,
                market_warn_code: (*item).invest_info.market_warn_code,
                is_market_warning: (*item).invest_info.is_market_warning,
                is_insincerity_notice:(*item).invest_info.is_insincerity_notice,
                is_backdoor_listing:(*item).invest_info.is_backdoor_listing,
                is_creditable: (*item).invest_info.is_creditable,
                margin_rate: (*item).invest_info.margin_rate,
                par_value: (*item).invest_info.par_value,
                listing_date: String::from(ldate),
                listing_count: (*item).invest_info.listing_count,
                is_new_listing: (*item).invest_info.is_new_listing,
                capital: (*item).invest_info.capital,
                is_short_selling: (*item).invest_info.is_short_selling,
                is_abnormal_rise: (*item).invest_info.is_abnormal_rise,
                sales: (*item).invest_info.sales,
                operating_profit: (*item).invest_info.operating_profit,
                ordinary_profit: (*item).invest_info.ordinary_profit,
                net_income: (*item).invest_info.net_income,
                roe: (*item).invest_info.roe,
                base_year_month: String::from(base_ym),
                preday_market_cap: (*item).invest_info.preday_market_cap,
                is_credit_limt_over:(*item).invest_info.is_credit_limt_over,
                is_loanable: (*item).invest_info.is_loanable,
                is_stock_loanable: (*item).invest_info.is_stock_loanable,
            }
        }
    }
}

pub type EventCallback = unsafe extern "system" fn(*const libc::c_void, libc::c_uint, *const libc::c_char, *const libc::c_char);
pub type SysMsgCallback = unsafe extern "system" fn(*const libc::c_void, libc::c_uint, libc::c_int);

#[link(name = "EfxMfc", kind = "static")]
#[allow(dead_code)]
extern {
    fn efxInit(efxDir:*const u8, sysMsgCB:self::SysMsgCallback) -> libc::c_int;
    fn efxExit();
    fn efxSynchronize(milliSec:libc::c_uint) -> libc::c_uint;
    fn efxLaunchApp(filePath:*const u8, args:*const u8, waitInputIdle:libc::c_char, waitTerminate:libc::c_char, showFlag:libc::c_int) -> *const libc::c_void;
	fn efxExecCmd(result:*mut u8, buffLen:libc::c_int, cmdArg:*const u8, wait_time:libc::c_uint) -> libc::c_int;
    fn efxProcessMessage(milliSec:libc::c_uint) -> libc::c_int;
    fn efxMessageLoop();
    fn efxQuit(exit_code:libc::c_int);
    fn efxFindStockMaster(stock_code:*const u8) -> *const self::StockMasterItem;
    fn efxFindStockMasterByName(stock_name:*const u8) -> *const self::StockMasterItem;
    fn efxGetStockMaster(seq_no:libc::c_int) -> *const self::StockMasterItem;

    fn efxNewControl(id:libc::c_uint, recvData:Option<self::EventCallback>,
        recvRealData:Option<self::EventCallback>, recvError:Option<self::EventCallback>) -> *const libc::c_void;
    fn efxGetAccountCount() -> libc::c_int;
    fn efxGetAccount(idx:libc::c_int, account:*mut u8, buff_len:libc::c_int) -> libc::c_int;
    fn efxGetAccountBrcode(acnt_no:*const u8, brcode:*mut u8, buff_len:libc::c_int) -> libc::c_int;
    fn efxGetEncryptPassword(pwd:*const u8, enc_pwd:*mut u8, buff_len:libc::c_int) -> libc::c_int;
    fn efxAboutBox();

    fn efxSetSingleData(handle:*const libc::c_void, field_idx:libc::c_int, data:*const u8);
    fn efxSetSingleDataEx(handle:*const libc::c_void, block_idx:libc::c_int, field_idx:libc::c_int, data:*const u8);
    fn efxSetMultiData(handle:*const libc::c_void, rec_idx:libc::c_int, field_idx:libc::c_int, data:*const u8);
	fn efxSetMultiBlockData(handle:*const libc::c_void, block_idx:libc::c_int, rec_idx:libc::c_int, field_idx:libc::c_int, data:*const u8);

    fn efxRequestData(handle:*const libc::c_void, qry_name:*const u8, param:*const u8);
    fn efxIsMoreNextData(handle:*const libc::c_void) -> libc::c_int;
    fn efxRequestNextData(handle:*const libc::c_void, qry_name:*const u8);
	fn efxRequestRealData(handle:*const libc::c_void, qry_name:*const u8, code:*const u8);
	fn efxUnrequestRealData(handle:*const libc::c_void, qry_name:*const u8, code:*const u8);
	fn efxUnrequestAllRealData(handle:*const libc::c_void);

    fn efxGetSingleFieldCount(handle:*const libc::c_void) -> libc::c_int;
    fn efxGetSingleData(handle:*const libc::c_void, s_data:*mut u8, buff_len:libc::c_int, field_idx:libc::c_int, attr_type:libc::c_int) -> libc::c_int;
    fn efxGetSingleDataInt(handle:*const libc::c_void, field_idx:libc::c_int, attr_type:libc::c_int) -> libc::c_int;
    fn efxGetSingleDataFloat(handle:*const libc::c_void, field_idx:libc::c_int, attr_type:libc::c_int) -> libc::c_float;

	fn efxGetMultiBlockCount(handle:*const libc::c_void) -> libc::c_int;
	fn efxGetMultiRecordCount(handle:*const libc::c_void, block_idx:libc::c_int) -> libc::c_int;
	fn efxGetMultiFieldCount(handle:*const libc::c_void, block_idx:libc::c_int, rec_idx:libc::c_int) -> libc::c_int;
	fn efxGetMultiData(handle:*const libc::c_void, s_data:*mut u8, buff_len:libc::c_int, block_idx:libc::c_int, rec_idx:libc::c_int, field_idx:libc::c_int, attr_type:libc::c_int) -> libc::c_int;
	fn efxGetMultiDataInt(handle:*const libc::c_void, block_idx:libc::c_int, rec_idx:libc::c_int, field_idx:libc::c_int, attr_type:libc::c_int) -> libc::c_int;
	fn efxGetMultiDataFloat(handle:*const libc::c_void, block_idx:libc::c_int, rec_idx:libc::c_int, field_idx:libc::c_int, attr_type:libc::c_int) -> libc::c_float;

    fn efxGetReqMsgCode(handle:*const libc::c_void, msg_code:*mut u8, buff_len:libc::c_int) -> libc::c_int;
    fn efxGetReqMessage(handle:*const libc::c_void, req_msg:*mut u8, buff_len:libc::c_int) -> libc::c_int;
	fn efxGetRtCode(handle:*const libc::c_void, rt_code:*mut u8, buff_len:libc::c_int) -> libc::c_int;
	fn efxGetSendRqID(handle:*const libc::c_void) -> libc::c_int;
	fn efxGetRecvRqID(handle:*const libc::c_void) -> libc::c_int;
}

pub fn init(efx_dir:Option<&str>, sys_msg_cb:self::SysMsgCallback) -> Result<i32, &str> {
    match unsafe {
        match efx_dir {
            Some(path) => efxInit(path.as_ptr(), sys_msg_cb),
            None => efxInit(ptr::null(), sys_msg_cb)
        }
    } {
        0 => Ok(0),
        ret => match ret {
            -1 => Err("Administrative Privileges Required"),
            1 => Err("Incorrect function"),
            2 => Err("File not found"),
            3 => Err("Path not found"),
            _ => Err("Undefined")
        }
    }
}

pub fn exit() {
    unsafe { efxExit() }
}

#[allow(dead_code)]
pub fn synchronize(milli_sec:u32) -> u32 {
    unsafe { efxSynchronize(milli_sec as libc::c_uint) }
}

#[allow(dead_code)]
pub fn launch_app(file_path:&str, args:&str, wait_input_idle:bool, wait_terminate:bool, show_flag:i32) -> *const libc::c_void {
    let mut spath = file_path.to_string();
    spath.push('\0');
    let mut sarg = args.to_string();
    sarg.push('\0');
    let wait_param1 = match wait_input_idle {
        true => 1,
        _ => 0
    };
    let wait_param2 = match wait_terminate {
        true => 1,
        _ => 0
    };
    unsafe { efxLaunchApp(spath.as_ptr(), sarg.as_ptr(), wait_param1, wait_param2, show_flag) }
}

#[allow(dead_code)]
pub fn exec_cmd(cmd_arg:&str, wait_time:u32) -> String {
    let mut scmd = cmd_arg.to_string();
    scmd.push('\0');
    let mut buff:Vec<u8> = Vec::with_capacity(4096);
    buff.resize(buff.capacity() - 1, 0);
    unsafe {
        let l = efxExecCmd(buff.as_mut_ptr(), buff.len() as libc::c_int, scmd.as_ptr(), wait_time);
        let mut result = String::from_utf8_lossy(&buff).to_string();
        result.truncate(l as usize);
        result
    }
}

#[allow(dead_code)]
pub fn process_msg(milli_sec:u32) -> i32 {
    unsafe { efxProcessMessage(milli_sec as libc::c_uint) }
}

#[allow(dead_code)]
pub fn msg_loop() {
    unsafe { efxMessageLoop() }
}

#[allow(dead_code)]
pub fn quit(exit_code:i32) {
    unsafe { efxQuit(exit_code) }
}

#[allow(dead_code)]
pub fn find_stock_master(stock_code:&str) -> *const self::StockMasterItem {
    let mut scode = stock_code.to_string();
    scode.push('\0');
    unsafe { efxFindStockMaster(scode.as_ptr()) }
}

#[allow(dead_code)]
pub fn find_stock_master_by_name(stock_name:&str) -> *const self::StockMasterItem {
    let mut sname = stock_name.to_string();
    sname.push('\0');
    unsafe { efxFindStockMasterByName(sname.as_ptr()) }
}

#[allow(dead_code)]
pub fn get_stock_master(seq_no:i32) -> *const self::StockMasterItem {
    unsafe { efxGetStockMaster(seq_no) }
}

pub fn new_control(id:u32, recv_data_cb:Option<self::EventCallback>,
    recv_real_data_cb:Option<self::EventCallback>, recv_error_cb:Option<self::EventCallback>) -> *const libc::c_void {
    unsafe { efxNewControl(id, recv_data_cb, recv_real_data_cb, recv_error_cb) }
}

#[allow(dead_code)]
pub fn get_account_count() -> i32 {
    unsafe { efxGetAccountCount() }
}

#[allow(dead_code)]
pub fn get_account(idx:i32) -> String {
    let mut buff:Vec<u8> = Vec::with_capacity(16);
    buff.resize(buff.capacity() - 1, 0);
    unsafe {
        let l = efxGetAccount(idx, buff.as_mut_ptr(), buff.len() as libc::c_int);
        let mut account = String::from_utf8_lossy(&buff).to_string();
        account.truncate(l as usize);
        account
    }
}

#[allow(dead_code)]
pub fn get_account_brcode(acnt_no:&str) -> String {
    let mut buff:Vec<u8> = Vec::with_capacity(16);
    buff.resize(buff.capacity() - 1, 0);
    unsafe {
        let l = efxGetAccountBrcode(acnt_no.as_ptr(), buff.as_mut_ptr(), buff.len() as libc::c_int);
        let mut brcode = String::from_utf8_lossy(&buff).to_string();
        brcode.truncate(l as usize);
        brcode
    }
}

#[allow(dead_code)]
pub fn get_encrypt_password(pwd:&str) -> String {
    let mut spwd = pwd.to_string();
    spwd.push('\0');
    let mut buff:Vec<u8> = Vec::with_capacity(100);
    buff.resize(buff.capacity() - 1, 0);
    unsafe {
        let l = efxGetEncryptPassword(spwd.as_ptr(), buff.as_mut_ptr(), buff.len() as libc::c_int);
        let mut enc_pwd = String::from_utf8_lossy(&buff).to_string();
        enc_pwd.truncate(l as usize);
        enc_pwd
    }
}

#[allow(dead_code)]
pub fn about_box() {
    unsafe { efxAboutBox() }
}

#[allow(dead_code)]
pub fn set_single_data(handle:*const libc::c_void, field_idx:i32, data:&str) {
    let mut sdata = data.to_string();
    //println!("set_single_data({}, {})", field_idx, sdata);
    sdata.push('\0');
    unsafe { efxSetSingleData(handle, field_idx, sdata.as_ptr()) }
}

#[allow(dead_code)]
pub fn set_single_data_ex(handle:*const libc::c_void, block_idx:i32, field_idx:i32, data:&str) {
    let mut sdata = data.to_string();
    sdata.push('\0');
    unsafe { efxSetSingleDataEx(handle, block_idx, field_idx, sdata.as_ptr()) }
}

#[allow(dead_code)]
pub fn set_multi_data(handle:*const libc::c_void, rec_idx:i32, field_idx:i32, data:&str) {
    let mut sdata = data.to_string();
    sdata.push('\0');
    unsafe { efxSetMultiData(handle, rec_idx, field_idx, sdata.as_ptr()) }
}

#[allow(dead_code)]
pub fn set_multi_block_data(handle:*const libc::c_void, block_idx:i32, rec_idx:i32, field_idx:i32, data:&str) {
    let mut sdata = data.to_string();
    sdata.push('\0');
    unsafe { efxSetMultiBlockData(handle, block_idx, rec_idx, field_idx, sdata.as_ptr()) }
}

#[allow(dead_code)]
pub fn request_data(handle:*const libc::c_void, qry_name:&str, param:Option<&str>) {
    let mut qname = qry_name.to_string();
    qname.push('\0');
    let p = match param {
        Some(m) => m,
        None => ""
    };
    let mut s_param = p.to_string();
    s_param.push('\0');
    unsafe { efxRequestData(handle, qname.as_ptr(), s_param.as_ptr()) }
}

#[allow(dead_code)]
pub fn is_more_next_data(handle:*const libc::c_void) -> bool {
    unsafe {
        if efxIsMoreNextData(handle) > 0 { true } else { false }
    }
}

#[allow(dead_code)]
pub fn request_next_data(handle:*const libc::c_void, qry_name:&str) {
    let mut qname = qry_name.to_string();
    qname.push('\0');
    unsafe { efxRequestNextData(handle, qname.as_ptr()) }
}

#[allow(dead_code)]
pub fn request_realdata(handle:*const libc::c_void, qry_name:&str, code:&str) {
    let mut qname = qry_name.to_string();
    qname.push('\0');
    let mut s_code = code.to_string();
    s_code.push('\0');
    unsafe { efxRequestRealData(handle, qname.as_ptr(), s_code.as_ptr()) }
}

#[allow(dead_code)]
pub fn unrequest_realdata(handle:*const libc::c_void, qry_name:&str, code:&str) {
    let mut qname = qry_name.to_string();
    qname.push('\0');
    let mut s_code = code.to_string();
    s_code.push('\0');
    unsafe { efxUnrequestRealData(handle, qname.as_ptr(), s_code.as_ptr()) }
}

#[allow(dead_code)]
pub fn unrequest_all_realdata(handle:*const libc::c_void) {
    unsafe { efxUnrequestAllRealData(handle) }
}

#[allow(dead_code)]
pub fn get_single_field_count(handle:*const libc::c_void) -> i32 {
    unsafe { efxGetSingleFieldCount(handle) }
}

#[allow(dead_code)]
pub fn get_single_data(handle:*const libc::c_void, field_idx:i32, attr_type:i32) -> String {
    let mut buff:Vec<u8> = Vec::with_capacity(256);
    buff.resize(buff.capacity() - 1, 0);
    unsafe {
        let l = efxGetSingleData(handle, buff.as_mut_ptr(), buff.len() as libc::c_int, field_idx, attr_type);
        let mut sdata = String::from_utf8_lossy(&buff).to_string();
        sdata.truncate(l as usize);
        sdata
    }
}

#[allow(dead_code)]
pub fn get_single_data_vec(handle:*const libc::c_void, buff:&mut Vec<u8>, field_idx:i32, attr_type:i32) -> usize {
    unsafe {
        efxGetSingleData(handle, buff.as_mut_ptr(), buff.len() as libc::c_int, field_idx, attr_type) as usize
    }
}

#[allow(dead_code)]
pub fn get_single_data_int(handle:*const libc::c_void, field_idx:libc::c_int, attr_type:libc::c_int) -> i32 {
    unsafe { efxGetSingleDataInt(handle, field_idx, attr_type) }
}

#[allow(dead_code)]
pub fn get_single_data_float(handle:*const libc::c_void, field_idx:libc::c_int, attr_type:libc::c_int) -> f32 {
    unsafe { efxGetSingleDataFloat(handle, field_idx, attr_type) }
}

#[allow(dead_code)]
pub fn get_multi_block_count(handle:*const libc::c_void) -> i32 {
    unsafe { efxGetMultiBlockCount(handle) }
}

#[allow(dead_code)]
pub fn get_multi_record_count(handle:*const libc::c_void, block_idx:i32) -> i32 {
    unsafe { efxGetMultiRecordCount(handle, block_idx) }
}

#[allow(dead_code)]
pub fn get_multi_field_count(handle:*const libc::c_void, block_idx:i32, rec_idx:i32) -> i32 {
    unsafe { efxGetMultiFieldCount(handle, block_idx, rec_idx) }
}

#[allow(dead_code)]
pub fn get_multi_data(handle:*const libc::c_void, block_idx:i32, rec_idx:i32, field_idx:i32, attr_type:i32) -> String {
    let mut buff:Vec<u8> = Vec::with_capacity(256);
    buff.resize(buff.capacity() - 1, 0);
    unsafe {
        let l = efxGetMultiData(handle, buff.as_mut_ptr(), buff.len() as libc::c_int, block_idx, rec_idx, field_idx, attr_type);
        let mut sdata = String::from_utf8_lossy(&buff).to_string();
        sdata.truncate(l as usize);
        sdata
    }
}

#[allow(dead_code)]
pub fn get_multi_data_vec(handle:*const libc::c_void, buff:&mut Vec<u8>, block_idx:i32, rec_idx:i32, field_idx:i32, attr_type:i32) -> usize {
    unsafe {
        efxGetMultiData(handle, buff.as_mut_ptr(), buff.len() as libc::c_int, block_idx, rec_idx, field_idx, attr_type) as usize
    }
}

#[allow(dead_code)]
pub fn get_multi_data_int(handle:*const libc::c_void, block_idx:i32, rec_idx:i32, field_idx:i32, attr_type:i32) -> i32 {
    unsafe { efxGetMultiDataInt(handle, block_idx, rec_idx, field_idx, attr_type) }
}

#[allow(dead_code)]
pub fn get_multi_data_float(handle:*const libc::c_void, block_idx:i32, rec_idx:i32, field_idx:i32, attr_type:i32) -> f32 {
    unsafe { efxGetMultiDataFloat(handle, block_idx, rec_idx, field_idx, attr_type) }
}

#[allow(dead_code)]
pub fn get_req_msg_code(handle:*const libc::c_void) -> String {
    let mut buff:Vec<u8> = Vec::with_capacity(100);
    buff.resize(buff.capacity() - 1, 0);
    unsafe {
        let l = efxGetReqMsgCode(handle, buff.as_mut_ptr(), buff.len() as libc::c_int);
        let mut msg_code = String::from_utf8_lossy(&buff).to_string();
        msg_code.truncate(l as usize);
        msg_code
    }
}

#[allow(dead_code)]
pub fn get_req_message(handle:*const libc::c_void) -> String {
    let mut buff:Vec<u8> = Vec::with_capacity(256);
    buff.resize(buff.capacity() - 1, 0);
    unsafe {
        let l = efxGetReqMessage(handle, buff.as_mut_ptr(), buff.len() as libc::c_int);
        let mut req_msg = String::from_utf8_lossy(&buff).to_string();
        req_msg.truncate(l as usize);
        req_msg
    }
}

#[allow(dead_code)]
pub fn get_rt_code(handle:*const libc::c_void) -> String {
    let mut buff:Vec<u8> = Vec::with_capacity(100);
    buff.resize(buff.capacity() - 1, 0);
    unsafe {
        let l = efxGetRtCode(handle, buff.as_mut_ptr(), buff.len() as libc::c_int);
        let mut rt_code = String::from_utf8_lossy(&buff).to_string();
        rt_code.truncate(l as usize);
        rt_code
    }
}

#[allow(dead_code)]
pub fn get_send_rq_id(handle:*const libc::c_void) -> i32 {
    unsafe { efxGetSendRqID(handle) }
}

#[allow(dead_code)]
pub fn get_recv_rq_id(handle:*const libc::c_void) -> i32 {
    unsafe { efxGetRecvRqID(handle) }
}

#[allow(dead_code)]
pub fn request_account_balance(handle:*const libc::c_void, account:&str, pwd:&str, param:Option<&str>) {
    self::set_single_data(handle, 0, &account[..8]);
    self::set_single_data(handle, 1, &account[8..10]);
    let enc_pwd = self::get_encrypt_password(pwd);
    self::set_single_data(handle, 2, enc_pwd.as_str());
    self::set_single_data(handle, 3, "");
    self::set_single_data(handle, 4, "");
    self::set_single_data(handle, 5, "00");
    self::set_single_data(handle, 6, "N");
    self::request_data(handle, "SCAP", param);
}

//#[macro_export]
pub mod utils {
    #[allow(unused_macros)]
    macro_rules! efx_single_data {
        ($a:expr, $b:expr) => {
            efx::get_single_data($a, $b, 0)
        };
        ($a:expr, $b:expr, $c:expr) => {
            efx::get_single_data($a, $b, $c)
        };
    }
    #[allow(unused_imports)]
    pub(crate) use efx_single_data;
    #[allow(unused_macros)]
    macro_rules! efx_single_int {
        ($a:expr, $b:expr) => {
            efx::get_single_data_int($a, $b, 0)
        };
        ($a:expr, $b:expr, $c:expr) => {
            efx::get_single_data_int($a, $b, $c)
        };
    }
    #[allow(unused_imports)]
    pub(crate) use efx_single_int;
    #[allow(unused_macros)]
    macro_rules! efx_single_float {
        ($a:expr, $b:expr) => {
            efx::get_single_data_float($a, $b, 0)
        };
        ($a:expr, $b:expr, $c:expr) => {
            efx::get_single_data_float($a, $b, $c)
        };
    }
    #[allow(unused_imports)]
    pub(crate) use efx_single_float;
}

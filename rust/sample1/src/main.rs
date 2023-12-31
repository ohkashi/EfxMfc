mod efx;
use efx::utils::*;

use libc::*;
use std::mem;
use std::ffi::CStr;
use num_format::{Locale, ToFormattedString};
use std::time::Instant;
use std::env;
use std::fs;


static mut SIG_RECV_DATA: bool = false;

unsafe extern "system" fn efx_recv_sysmsg(_handle:*const c_void, ctrl_id:c_uint, sys_msg:c_int)
{
    println!("<== OnRecvSysMsg({}, {})", ctrl_id, sys_msg);
}

unsafe extern "system" fn efx_recv_data(handle:*const c_void, ctrl_id:c_uint, qryname:*const libc::c_char, param:*const c_char)
{
    let qry_name = CStr::from_ptr(qryname).to_str().unwrap();
    let stock_code = CStr::from_ptr(param).to_str().unwrap();
    println!("<-- OnRecvData({}, \"{}\", \"{}\"): rq_id = {}", ctrl_id, qry_name, stock_code, efx::get_recv_rq_id(handle));
    println!("ReqMsgCode: {}, ReqMsg: \"{}\", RtCode: {}", efx::get_req_msg_code(handle), efx::get_req_message(handle), efx::get_rt_code(handle));
    match qry_name {
        "SCPC" => {
            let cur_price = efx_single_int!(handle, 1);
            /*let mut buff:Vec<u8> = Vec::with_capacity(41);
            buff.resize(buff.capacity() - 1, 0);
            let len = efx::get_single_data_vec(handle, &mut buff, buff.len(), 1, 0);
            let mut sdata = String::from_utf8_lossy(&buff).to_string();
            sdata.truncate(len);
            let cur_price = sdata.parse::<i32>().unwrap();*/
            let chg_rate = efx_single_float!(handle, 6);

            let mst = efx::find_stock_master(stock_code);
            if !mst.is_null() {
                let info = efx::StockMasterInfo::new(mst);
                println!("{} {} {:?} 현재가: {}원 {:+}%", stock_code, info.stock_name, info.market_type,
                    cur_price.to_formatted_string(&Locale::en), chg_rate);
                println!("seqNo: {}, 신용가능: {}, 증거금 비율: {}, 액면가: {}, 상장주식: {}, 시장경고: {:?}", info.seq_no,
                    info.is_creditable, info.margin_rate, info.par_value, info.listing_count, info.market_warn_code);
                println!("매출액: {}, 영업이익: {}, 당기순이익: {}, ROE: {}, 기준년월: {}, 자본금: {}",
                    info.sales, info.operating_profit, info.net_income, info.roe, info.base_year_month, info.capital);
                //println!("{:?}", info);
            }
        },
        "SCAP" => {
            let cash = efx_single_int!(handle, 7);
            println!("주문가능현금: {}원", cash.to_formatted_string(&Locale::en));
        },
        &_ => {
            println!("엥?");
        }
    };
    println!();
    SIG_RECV_DATA = true;
}

unsafe extern "system" fn efx_recv_realdata(_handle:*const c_void, ctrl_id:c_uint, qryname:*const c_char, _param:*const c_char)
{
    let qry_name = CStr::from_ptr(qryname).to_str().unwrap();
    println!("OnRecvRealData({}, \"{}\")", ctrl_id, qry_name);
}

unsafe extern "system" fn efx_recv_error(handle:*const c_void, ctrl_id:c_uint, mcode:*const c_char, message:*const c_char)
{
    let msg_code = CStr::from_ptr(mcode).to_str().unwrap();
    let msg = CStr::from_ptr(message).to_str().unwrap();
    println!("OnRecvError({}, {}, \"{}\", {})", ctrl_id, msg_code, msg, efx::get_rt_code(handle));
}

fn wait_recv_data() {
    unsafe {
        while !SIG_RECV_DATA {
            efx::process_msg(1);
        }
        SIG_RECV_DATA = false;
    }
}

fn main() {
    println!("Hello, world! {:?}, {:?}", mem::size_of::<efx::InvestInfo>(), mem::size_of::<efx::StockMasterItem>());
    let str_cmd = efx::exec_cmd("ping -n 1 192.168.0.1", u32::MAX);
	println!("efx::exec_cmd: {}", str_cmd);
	//efx::launch_app("C:\\eFriend Expert\\efriendexpert\\efexpertviewer.exe", "", false, false, 5);

    let mut pwd_path = env::current_exe().unwrap();
    pwd_path.set_extension("pwd");
    //println!("pwd path: {}", pwd_path.display());
    let mut password = String::new();
    let f = fs::read_to_string(pwd_path);  // sample1.pwd에서 계좌 암호 읽는다.
    match f {
        Ok(s) => {
            password.push_str(s.trim());
        },
        Err(e)=> {
            println!("{:?}", e);
        }
    };

    let start = Instant::now();
    let _ret = efx::init(None, efx_recv_sysmsg).unwrap();
    println!("\nefxInit() elapsed time: {:?}", start.elapsed());
    let acc_cnt = match efx::get_account_count() {
        0 => {
            println!("계좌도 없는 게 까불어! :(");
            0
        },
        acnt => {
            for i in 0..acnt {
                println!("Account[{}]: {}", i, efx::get_account(i));
            }
            acnt
        }
    };
    println!();

    let efx1 = efx::new_control(1, Some(efx_recv_data), Some(efx_recv_realdata), Some(efx_recv_error));
    if acc_cnt > 0 && password.len() == 4 {
        efx::synchronize(efx::REQ_LIMIT_MS);
        let acc_no = efx::get_account(acc_cnt - 1);
        efx::request_account_balance(efx1, acc_no.as_str(), password.as_str(), None);
        println!("--> RequestData(\"SCAP\")");
        wait_recv_data();
    }

    let stock_code = ["254120", "081000", "005930", "096610", "294140"];
    for scode in stock_code.iter() {
        efx::synchronize(efx::REQ_LIMIT_MS);
        efx::set_single_data(efx1, 0, "J");
        efx::set_single_data(efx1, 1, scode);
        efx::request_data(efx1, "SCPC", Some(scode));
        println!("--> RequestData(\"SCPC\", \"{}\")", scode);
        wait_recv_data();
    }

    //efx::about_box();
    efx::quit(0);

    efx::msg_loop();
    efx::exit();
}

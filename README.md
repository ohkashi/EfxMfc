![true_friend.png](https://github.com/ohkashi/efxMfc-go/blob/main/true_friend.png)

# EfxMfc
eFriend Expert API MFC DLL  
한국투자증권 ITGExpertCtl.ocx를 C interface Regular DLL로 만듦. 

[![Licence](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE)

### Example

Rust 예제 ([main.rs](rust/sample1/src/main.rs))

```rust
mod efx;
use efx::utils::*;

...

fn main() {
    println!("Hello, world! {:?}, {:?}",
        mem::size_of::<efx::InvestInfo>(), mem::size_of::<efx::StockMasterItem>());
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
```

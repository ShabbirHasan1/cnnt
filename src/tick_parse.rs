use std::collections::HashMap;
use std::ffi::CStr;
use std::mem;
use std::mem::MaybeUninit;
use crate::fifo_bindings::{boost_init_ringbuffer, get_md_information, mkt_snd_s, MQ8, parse_symbol, subscribe, unsubscribe};
use crate::TickData;

pub fn wait_to() {
    loop {
        let x = (rtsc::unix_nano() / 1000000) % 1000;
        if x == 250 {
            break;
        }
    }
}


/// 获取基准时间
pub fn get_base_time() -> u32 {
    let x = (rtsc::unix_nano() / 1000000000) as u32 / 86400;
    let base_time = 86400 * x - 3600 * 8;
    base_time
}


/// 初始化订阅函数传入信息进行订阅
pub fn init_subscribe(hfp_base: *mut mkt_snd_s, symbols: Vec<&str>) -> HashMap<u32, u64> {
    let mut stack: HashMap<u32, u64> = HashMap::default();
    for bucket_id in 0..4095 {
        let md_ii = unsafe { *get_md_information(hfp_base, bucket_id) };
        let topic = md_ii.topic_id;
        unsafe {
            let id = CStr::from_ptr(md_ii.instrument_id.as_ptr()).to_str().unwrap();
            if symbols.contains(&id)
            {
                println!("subscribe_with: {}", id);
                subscribe(topic as i16, md_ii.instrument_no);
                let id = parse_symbol(hfp_base, bucket_id);
                stack.insert(bucket_id, id);
            } else {
                unsubscribe(topic as i16, md_ii.instrument_no);
            };
        }
    }
    println!("Finish symbol subscribe mapping:{:?}", stack);
    stack
}


pub fn parse_tick_with_raw_pointer(time_delta: u32, symbol: u64, f: MQ8) -> TickData {
    let last_price = f.last_price as u64;
    let volume = f.volume;
    let turnover = f.turnover as u64;
    let open_interest = f.open_interest as u64;
    let ask = f.ask_price_list;
    let bid = f.bid_price_list;
    let mut ask_price: [MaybeUninit<u64>; 5] = unsafe { MaybeUninit::uninit().assume_init() };
    let mut ask_volume: [MaybeUninit<u32>; 5] = unsafe { MaybeUninit::uninit().assume_init() };
    let mut bid_price: [MaybeUninit<u64>; 5] = unsafe { MaybeUninit::uninit().assume_init() };
    let mut bid_volume: [MaybeUninit<u32>; 5] = unsafe { MaybeUninit::uninit().assume_init() };
    for i in 0..5 {
        ask_price[i].write(ask[i].price as u64);
        ask_volume[i].write(ask[i].volume as u32);
        bid_price[i].write(bid[i].price as u64);
        bid_volume[i].write(bid[i].volume as u32);
    }
    let mill = f.snap_millisec;
    let hour = time_delta / 3600;
    let minute = time_delta % 3600 / 60;
    let second = time_delta % 60;
    TickData {
        symbol,
        volume,
        last_price,
        turnover,
        open_interest,
        bid_price: unsafe { mem::transmute::<_, [u64; 5]>(bid_price) },
        ask_price: unsafe { mem::transmute::<_, [u64; 5]>(ask_price) },
        bid_volume: unsafe { mem::transmute::<_, [u32; 5]>(bid_volume) },
        ask_volume: unsafe { mem::transmute::<_, [u32; 5]>(ask_volume) },
        h: hour as u8,
        m: minute as u8,
        s: second as u8,
        ms: mill,
    }
}

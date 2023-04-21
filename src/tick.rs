use std::collections::HashMap;
pub use crate::fifo_bindings::*;
use crate::tick_parse::{get_base_time, init_subscribe, parse_tick_with_raw_pointer};
use rtsc::unix_nano;

pub struct Mode {
    base_time: u32,
    mq_dup: [Union_MQ_Dup; 8192],
    mq_base: *mut MQ8,
    hfp_ext_base: *mut HFP_Ext,
    hfp_base: *mut mkt_snd_s,
    code_stack: HashMap<u32, u64>,
}

impl Mode {
    pub fn new() -> Mode {
        let ret = unsafe { init_boot_start() };
        println!("{:?}", ret);
        let mut mq_dup = [Union_MQ_Dup::default(); 8192];
        let mq_base = unsafe { get_mq_base() };
        let hfp_base = unsafe { get_hfp_base() };
        let hfp_ext_base = unsafe { get_hfp_ext_base() };
        unsafe { init_ok() };
        let code_stack = init_subscribe(hfp_base, vec!["rb2310"]);

        let base = unsafe { get_base_time() };
        Mode {
            base_time: base,
            mq_dup,
            mq_base,
            hfp_ext_base,
            hfp_base,
            code_stack,
        }
    }

    pub fn loop_tick(&mut self) {
        let mut snap_time_array = [0, 0];
        let mut marked_init = false;
        let mut last_time_qs = 0;
        let mut mq_offset = unsafe {
            boost_init_ringbuffer(self.mq_dup.as_mut_ptr(),
                                  self.mq_base)
        } as usize;
        println!("init mq_offset: {}", mq_offset);
        loop {
            unsafe {
                let u32_ = self.mq_dup[mq_offset].u32_;
                self.mq_dup[mq_offset].u32_ = get_mq_base_time(self.mq_base, mq_offset as u32);
                if self.mq_dup[mq_offset].u32_ > u32_ {
                    let mq_obj = *as_mq_obj(self.mq_base, mq_offset as u32);
                    let snap_time = mq_obj.snap_time;
                    let interval_time = snap_time - self.base_time;
                    let bucket_id = mq_obj.bucket_id();
                    let topic_id = mq_obj.topic_id_idx();
                    let symbol = *(self.code_stack.get(&bucket_id).unwrap_unchecked());
                    let tick_data = parse_tick_with_raw_pointer(interval_time,
                                                                symbol,
                                                                mq_obj);
                    println!("{:?}", tick_data);
                    mq_offset = (mq_offset + 1) % 8192;
                    let idx = if topic_id > 0 { 1 } else { 0 };
                    if snap_time < snap_time_array[idx] {
                        marked_init = true;
                    }
                    snap_time_array[idx] = snap_time;
                    last_time_qs = unix_nano();
                } else {
                    let unix = unix_nano();
                    let time_diff = (unix - last_time_qs as i64) % 500000000;
                    if (100000000 < time_diff) && (time_diff < 400000000) {
                        boost_mdqp_work(false);
                        if marked_init {
                            let mq = mq_offset.clone();
                            mq_offset = unsafe { boost_init_ringbuffer(self.mq_dup.as_mut_ptr(), self.mq_base) } as usize;
                            marked_init = false;
                        }
                    }
                }
            }
        }
    }
}
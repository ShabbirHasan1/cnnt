pub mod tick;
pub mod fifo_bindings;
pub mod tick_parse;

pub use crate::tick::Mode;


#[derive(Clone, Debug, Default, Copy)]
pub struct TickData {
    pub symbol: u64,
    pub volume: u32,
    pub last_price: u64,
    pub turnover: u64,
    pub open_interest: u64,
    pub bid_price: [u64; 5],
    pub ask_price: [u64; 5],
    pub bid_volume: [u32; 5],
    pub ask_volume: [u32; 5],
    pub h: u8,
    pub m: u8,
    pub s: u8,
    pub ms: u16,
}

impl TickData {
    pub const MSG_LEN: usize = 172;

    pub fn bid_price(&self, index: usize) -> u64 {
        unsafe { *self.bid_price.get_unchecked(index) }
    }

    pub fn ask_price(&self, index: usize) -> u64 {
        unsafe { *self.ask_price.get_unchecked(index) }
    }

    pub fn bid_volume(&self, index: usize) -> u32 {
        unsafe { *self.bid_volume.get_unchecked(index) as u32 }
    }
    pub fn ask_volume(&self, index: usize) -> u32 {
        unsafe { *self.ask_volume.get_unchecked(index) as u32 }
    }

    pub fn mid_price(&self) -> f64 {
        (self.ask_price(0) as f64 + self.bid_price(0) as f64) / 2.0
    }

    pub fn time(&self) -> String {
        format!("{}:{}:{}.{}", self.h, self.m, self.s, self.ms)
    }
}

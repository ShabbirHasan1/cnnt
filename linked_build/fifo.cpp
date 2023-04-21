//
// Created by somew on 2023/3/31.
//
#include "fifo.hpp"
#include <cstddef>
#include <cstring>

#define SHFE_TOPICID  1000
#define INE_TOPICID   5000

// 获取最新的行情对象
MQ8 *as_mq_obj(MQ8 *mq_base, uint32_t mq_offset) {
    return mq_base + mq_offset;
}


// 获取行情信息对象
mkt_snd_s *get_md_information(mkt_snd_s *hfp_base, uint32_t bucket_id) {
    mkt_snd_s *md = hfp_base + bucket_id;
    return md;
}

// 直接根据bucket_id解析出对应的u64
uint64_t parse_symbol(mkt_snd_s *hfp_base, uint32_t bucket_id) {
    mkt_snd_s *md = hfp_base + bucket_id;
    uint64_t val;
    memcpy(&val, md->instrument_id, 6);
    return val;
}

auto init_boot_start() {
    BoostArguments args;
    memset(&args, 0, sizeof(struct BoostArguments));
//    memset(&boot, 0, sizeof(struct BOOT));
    args.print_heartbeat = false;
    args.dump_to_pcap = false;
    args.dump_on_exit = false;
    args.debug_log = false;
    args.output_stats = false;
    args.dump_mdqp_csv = false;
    args.dump_packets = 0;
    args.need_crc = false;
    args.subscription_init = 0;
    args.no_mdqp_validation = false;
    // 硬件初始化
    int ret = boost_start(&args, NULL, 0);
    return ret;
}


void init_ok() {
    while (true) {
        boost_mdqp_work(false);
        if (is_mdqp_inited(SHFE_TOPICID)) {
            break;
        }
    }
}

uint32_t get_mq_base_time(MQ8 *mq_base, uint32_t mq_offset) {
    return mq_base[mq_offset].snap_time;
}

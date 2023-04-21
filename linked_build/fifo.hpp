#pragma once

#include <stdint.h>

#define INSTRUMENT_ID_LEN 27
#define PRICE_LEVEL_MAX 5
#define SNAP_DATE_LEN 8
#define SNAP_TIME_LEN 8

#ifndef MQ_ITEMS
#define MQ_ITEMS (8 * 1024)
#endif

#ifndef HFP_ITEMS
#define HFP_ITEMS 4096
#endif

#ifndef MQ_SIZE
#define MQ_SIZE 256
#endif

typedef struct __attribute__((packed)) price_list {
    double price;
    unsigned int volume;
} prc_lst_s;

typedef struct __attribute__((packed)) market_send {
    // 00 - 1f
    char instrument_id[32];                     // static information after MDQP initialization

    // 20 - 3f
    unsigned int instrument_no;                 // static information after MDQP initialization
    unsigned short topic_id;                    // static information after MDQP initialization
    unsigned short reserved1;

    unsigned int volume_multiple;               // static information after MDQP initialization
    unsigned int reserved2;
    char snap_date[SNAP_DATE_LEN + 1];          // static information after MDQP initialization
    char reserved3[7];

    // 40 - 5f
    uint32_t snap_time;                         // don't use in produciton
    char reserved4[8];                          // don't use in produciton
    unsigned int snap_no;                       // don't use in produciton
    unsigned int packet_no;                     // don't use in produciton
    unsigned short snap_millisec;               // don't use in produciton
    unsigned short ip_len;                      // don't use in produciton
    unsigned int reserved5;
    unsigned int reserved6;

    // 60 - 9f
    prc_lst_s bid_price_list[PRICE_LEVEL_MAX];  // don't use in produciton
    unsigned int reserved7;

    // a0 - df
    prc_lst_s ask_price_list[PRICE_LEVEL_MAX];  // don't use in produciton
    unsigned int reserved8;

    // e0 - ff
    double last_price;                          // don't use in produciton
    double turnover;                            // don't use in produciton
    double open_interest;                       // don't use in produciton
    unsigned int volume;                        // don't use in produciton
    unsigned int change_no;                     // don't use in produciton
} mkt_snd_s;

typedef struct struct_snap_market {
    unsigned int change_no;
    unsigned int instrument_no;
    char instrument_id[INSTRUMENT_ID_LEN];
    unsigned short topic_id;
    unsigned int snap_no;
    char snap_date[SNAP_DATE_LEN + 1];
    char snap_time[SNAP_TIME_LEN + 1];
    unsigned short snap_millisec;
    unsigned int packet_no;
    unsigned int volume_multiple;
    double price_tick;
    double codec_price;
    double last_price;
    unsigned int volume;
    double turnover;
    double open_interest;
    prc_lst_s bid_price_list[PRICE_LEVEL_MAX];
    prc_lst_s ask_price_list[PRICE_LEVEL_MAX];
} __attribute__((packed)) snap_market_s;

#pragma pack(1)
typedef struct {
    uint8_t destination[6];
    uint8_t source[6];
    uint16_t frame_type;
} Eth_Frame_Header;

typedef struct {
    uint8_t flag;
    uint8_t tos;
    uint16_t total_len;
    uint16_t identification;
    uint16_t fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t hdr_chk_sum;
    uint32_t source_address;
    uint32_t destination_address;
} IP_Frame_Header;

typedef struct {
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t chk_sum;
} UDP_Frame_Header;

struct BoostArguments {
    // 是否在屏幕上打印输出U50收到的报文数据信息，1：是，0：否
    uint32_t dump_packets;
    // 是否统计CRC
    bool need_crc;

    // 是否打印心跳信息
    bool print_heartbeat;

    //是否保存U50接收报文到pcap文件
    bool dump_to_pcap;

    // 初始订阅设置
    // 0: 订阅所有合约，初始化时所有合约均订阅直至unsubscribe()被调用
    // 1: 取消所有合约，初始化时所有合约均不订阅直至subscribe()被调用
    int subscription_init;

    // 程序退出时是否输出mq/rx的dump文件
    bool dump_on_exit;

    // 输出调试信息到文件中
    bool debug_log;

    // 是否加载instru.init.dat文件，默认不加载
    bool load_init;

    // 是否输出汇总的latency统计值
    bool output_stats;

    // 是否输出mdqp初始化合约信息
    bool dump_mdqp_csv;

    bool no_mdqp_validation;
};

#pragma pack(1)
// typedef struct Struct_MQ
// {
//     uint32_t snap_time;
//     uint16_t instrument_no;
//     uint16_t reserved1;
//     uint32_t port_num : 8;
//     uint32_t ts_cnt : 24;
//     uint32_t snap_no;

//     uint32_t packet_no;
//     uint16_t snap_millisec;
//     uint16_t ip_len;
//     uint32_t ts_cnt_fr;

//     uint16_t bucket_id : 13;
//     uint16_t reserved3 : 1;
//     uint16_t topic_id : 2;
//     uint16_t change_no;
// } MQ;

typedef struct Struct_HFP_Ext {
    double codec_price;
    double price_tick;

    double HighestPrice;       // 最高价。统计本交易日到目前为止的最高成交价。
    double LowestPrice;        // 最低价。统计本交易日到目前为止的最低成交价。
    double OpenPrice;          // 今开盘。
    double ClosePrice;         // 今收盘
    double SettlementPrice;    // 今结算。
    double UpperLimitPrice;    // 涨停板价。
    double LowerLimitPrice;    // 跌停板价。
    double PreSettlementPrice; // 昨结算。
    double PreClosePrice;      // 昨收盘。
    double PreOpenInterest;    // 昨持仓量。本交易日开盘前的该合约总持仓。
    double PreDelta;           // 昨虚实度。
    double CurrDelta;          // 今虚实度。

} HFP_Ext;

typedef struct Struct_MQ8 {
    // 00 - 0f
    uint32_t change_no;
    uint16_t instrument_no;
    uint16_t reserved1;
    uint32_t port_num: 8;
    uint32_t ts_cnt: 24;
    uint32_t snap_no;

    // 10 - 1f
    uint32_t packet_no;
    uint16_t snap_millisec;  // 0 , 500 ms
    uint16_t ip_len;

    uint32_t ts_cnt_fr: 31;
    uint32_t reserved2: 1;
    uint32_t bucket_id: 14;
    uint32_t topic_id_idx: 2;
    uint32_t change_no_short: 16;

    // 20 - 5f
    prc_lst_s bid_price_list[PRICE_LEVEL_MAX];
    unsigned int reserved3;

    // 60 - 9f
    prc_lst_s ask_price_list[PRICE_LEVEL_MAX];
    unsigned int reserved4;

    // a0 - bf
    double last_price;
    double turnover;
    double open_interest;
    uint32_t volume;
    uint32_t snap_time;     // epoch time in second
    // c0 - ff
    uint8_t reserved5[64];
} MQ8;

typedef union Union_MQ_Dup {
    uint32_t u32;
    struct {
        uint16_t bucket_id: 13;
        uint16_t reserved3: 1;
        uint16_t topic_id: 2;
        uint16_t change_no;
    } mq;
} MQ_Dup;

struct StatusExport {
    bool mdqp_inited[2];
    uint32_t mq_offset;
};

#pragma pack()

extern struct BoostArguments args;
extern struct StatusExport *status_export;

uint32_t read_mq_offset();

uint32_t read_rx_offset();

uint32_t read_ts_cnt();

uint32_t read_ts_cnt_fr();

MQ8 *get_mq_base();

mkt_snd_s *get_hfp(int topic_idx, int instrument_no);

volatile mkt_snd_s *get_hfp_base();

HFP_Ext *get_hfp_ext_base();

void boost_mdqp_work(bool mq_stopped);

int boost_start(void *arg, snap_market_s *snap_markets, int snap_count);

uint32_t boost_init_ringbuffer(MQ_Dup *mq_dup, MQ8 *mq_base);

int boost_stop();

void subscribe_all();

void unsubscribe_all();

bool subscribe(int16_t topic_id, uint32_t instrument_no);   // topic_id = 1000 (SHFE), or 5000)
bool unsubscribe(int16_t topic_id, uint32_t instrument_no);

bool subscribe(const char *instrument_id);

bool unsubscribe(const char *instrument_id);

int update_mdqp(snap_market_s *snap_market);  // return 1 : success ;     return 0 : fail
int get_codec_price(int topic_id, uint32_t instrument_no, double &codec_price, double &price_t);

int is_mdqp_inited(uint16_t topic_id);

void printHex(void *data, int len);

uint32_t get_snaptime_with_base(MQ8 *mq_base, uint32_t mq_offset);

// 读取出基本的MQ8对象
MQ8 *as_mq_obj(MQ8 *mq_base, uint32_t mq_offset);

// 硬件初始化
auto init_boot_start();


// 等待基准初始化
void init_ok();

// 传换为u64
uint64_t parse_symbol(mkt_snd_s *hfp_base, uint32_t bucket_id);

// 获取合约基本信息
mkt_snd_s *get_md_information(mkt_snd_s *hfp_base, uint32_t bucket_id);

uint32_t get_mq_base_time(MQ8 *mq_base, uint32_t mq_offset);
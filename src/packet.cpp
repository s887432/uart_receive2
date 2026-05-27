#include "packet.h"
#include <string.h>

void pkt_parser_init(pkt_parser_t *p, pkt_handler_t cb, void *user) {
    p->used = 0;
    p->on_packet = cb;
    p->user = user;
}

static void drop_front(pkt_parser_t *p, size_t n) {
    if (n >= p->used) { p->used = 0; return; }
    memmove(p->buf, p->buf + n, p->used - n);
    p->used -= n;
}

// 從 buf[1] 開始往後找下一個 0x55，沒找到就整個丟掉
static void resync(pkt_parser_t *p) {
    for (size_t i = 1; i < p->used; i++) {
        if (p->buf[i] == PKT_HDR0) { drop_front(p, i); return; }
    }
    p->used = 0;
}

void pkt_parser_feed(pkt_parser_t *p, const uint8_t *data, size_t n) {
    while (n > 0) {
        size_t space = sizeof(p->buf) - p->used;
        size_t copy  = (n < space) ? n : space;
        memcpy(p->buf + p->used, data, copy);
        p->used += copy;
        data    += copy;
        n       -= copy;

        // 嘗試從 buffer 中拉出盡可能多的封包
        for (;;) {
            // 1. 對齊到 0x55
            if (p->used >= 1 && p->buf[0] != PKT_HDR0) { resync(p); continue; }
            if (p->used < 2) break;

            // 2. 第二個 byte 必須是 0xAA，否則丟掉第一個 byte 重找
            if (p->buf[1] != PKT_HDR1) { drop_front(p, 1); resync(p); continue; }

            // 3. 等到 cmd + len 收齊
            if (p->used < 4) break;
            uint8_t len = p->buf[3];
            size_t  total = PKT_OVERHEAD + len;

            // 4. 等整包收齊
            if (p->used < total) break;

            // 5. 驗 checksum
            uint8_t sum = 0;
            for (size_t i = 0; i < total - 1; i++) sum += p->buf[i];
            if (sum != p->buf[total - 1]) {
                // 校驗失敗 → 丟掉 0x55，找下一個可能的起點
                drop_front(p, 1);
                resync(p);
                continue;
            }

            // 6. 成功，回呼
            if (p->on_packet) {
                packet_t pkt;
                pkt.cmd = p->buf[2];
                pkt.len = len;
                if (len) memcpy(pkt.payload, p->buf + 4, len);
                p->on_packet(&pkt, p->user);
            }
            drop_front(p, total);
        }
    }
}
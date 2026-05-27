#ifndef PACKET_H
#define PACKET_H

#include <stddef.h>
#include <stdint.h>

#define PKT_HDR0      0x55
#define PKT_HDR1      0xAA
#define PKT_MAX_PAYLOAD 255
#define PKT_OVERHEAD    5   // hdr(2) + cmd(1) + len(1) + cksum(1)

typedef struct {
    uint8_t cmd;
    uint8_t len;
    uint8_t payload[PKT_MAX_PAYLOAD];
} packet_t;

typedef void (*pkt_handler_t)(const packet_t *pkt, void *user);

typedef struct {
    uint8_t  buf[PKT_OVERHEAD + PKT_MAX_PAYLOAD];
    size_t   used;
    pkt_handler_t on_packet;
    void    *user;
} pkt_parser_t;

void pkt_parser_init(pkt_parser_t *p, pkt_handler_t cb, void *user);
void pkt_parser_feed(pkt_parser_t *p, const uint8_t *data, size_t n);

#endif
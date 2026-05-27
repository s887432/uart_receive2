#include "packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

static int uart_open(const char *dev, speed_t baud) {
    int fd = open(dev, O_RDWR | O_NOCTTY);
    if (fd < 0) { perror("open"); return -1; }

    struct termios tio;
    tcgetattr(fd, &tio);
    cfmakeraw(&tio);
    cfsetispeed(&tio, baud);
    cfsetospeed(&tio, baud);
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_cflag &= ~CRTSCTS;
    tio.c_cc[VMIN]  = 0;
    tio.c_cc[VTIME] = 1;
    tcsetattr(fd, TCSANOW, &tio);
    tcflush(fd, TCIFLUSH);
    return fd;
}

static void on_packet(const packet_t *pkt, void *user) {
    (void)user;
    printf("RX cmd=0x%02X len=%u payload=", pkt->cmd, pkt->len);
    for (int i = 0; i < pkt->len; i++) printf("%02X ", pkt->payload[i]);
    putchar('\n');
}

int main(int argc, char **argv) {
    const char *dev = (argc > 1) ? argv[1] : "/dev/ttyUSB0";
    int fd = uart_open(dev, B115200);
    if (fd < 0) 
		return 1;

    pkt_parser_t parser;
    pkt_parser_init(&parser, on_packet, NULL);

    uint8_t buf[512];
    for (;;) {
        fd_set rfds;
        FD_ZERO(&rfds); FD_SET(fd, &rfds);
        int r = select(fd + 1, &rfds, NULL, NULL, NULL);
        if (r < 0) { if (errno == EINTR) continue; perror("select"); break; }

        if (FD_ISSET(fd, &rfds)) {
            ssize_t n = read(fd, buf, sizeof(buf));
            if (n > 0)       
				pkt_parser_feed(&parser, buf, (size_t)n);
            else if (n == 0) 
				continue;
            else 
			{ 
				if (errno == EINTR) 
					continue; 
				perror("read"); 
				break; 
			}
        }
    }
    close(fd);
    return 0;
}
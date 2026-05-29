#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "libtkb.h"

/* Read exactly size bytes */
static int read_exact_client(netClass_t* cl, unsigned char* data, size_t size) {
    size_t offset = 0;
    while (offset < size) {
        int ret = netclRead(cl, data + offset, size - offset);
        if (ret <= 0) {
            return -1;
        }
        offset += (size_t)ret;
    }
    return 0;
}

/* Write exactly size bytes */
static int write_all_client(netClass_t* cl, const unsigned char* data, size_t size) {
    size_t offset = 0;
    while (offset < size) {
        int ret = netclWrite(cl, data + offset, size - offset);
        if (ret <= 0) {
            return -1;
        }
        offset += (size_t)ret;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    const char* host = "127.0.0.1";
    int port = 40101;

    if (argc >= 2) {
        host = argv[1];
    }
    if (argc >= 3) {
        port = std::atoi(argv[2]);
    }

    const char send_msg[] = "PING";
    const char expect_reply[] = "PONG";
    unsigned char buf[sizeof(expect_reply)] = {0};

    std::printf("client: connecting to %s:%d\n", host, port);

    netClass_t* cl = netclInit(host, port);
    if (!cl) {
        std::fprintf(stderr, "client: netclInit failed\n");
        return 1;
    }

    netclSetTimeOut(cl, 10.0);

    if (write_all_client(cl, (const unsigned char*)send_msg, sizeof(send_msg) - 1) != 0) {
        std::fprintf(stderr, "client: write_all_client failed\n");
        netclEnd(cl);
        return 1;
    }

    std::printf("client: sent [%s]\n", send_msg);

    if (read_exact_client(cl, buf, sizeof(expect_reply) - 1) != 0) {
        std::fprintf(stderr, "client: read_exact_client failed\n");
        netclEnd(cl);
        return 1;
    }

    std::printf("client: received [%s]\n", buf);

    netclEnd(cl);

    if (std::memcmp(buf, expect_reply, sizeof(expect_reply) - 1) != 0) {
        std::fprintf(stderr, "client: unexpected reply\n");
        return 1;
    }

    std::printf("client: success\n");
    return 0;
}


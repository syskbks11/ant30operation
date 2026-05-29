#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "libtkb.h"

/* Read exactly size bytes */
static int read_exact_server(netServerClass_t* sv, unsigned char* data, size_t size) {
    size_t offset = 0;
    while (offset < size) {
        int ret = netsvRead(sv, data + offset, size - offset);
        if (ret <= 0) {
            return -1;
        }
        offset += (size_t)ret;
    }
    return 0;
}

/* Write exactly size bytes */
static int write_all_server(netServerClass_t* sv, const unsigned char* data, size_t size) {
    size_t offset = 0;
    while (offset < size) {
        int ret = netsvWrite(sv, data + offset, size - offset);
        if (ret <= 0) {
            return -1;
        }
        offset += (size_t)ret;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int port = 40101;
    if (argc >= 2) {
        port = std::atoi(argv[1]);
    }

    const char expect_msg[] = "PING";
    const char reply_msg[]  = "PONG";
    unsigned char buf[sizeof(expect_msg)] = {0};

    std::printf("server: starting on port %d\n", port);

    netServerClass_t* sv = netsvInit(port);
    if (!sv) {
        std::fprintf(stderr, "server: netsvInit failed\n");
        return 1;
    }

    // netsvSetTimeOut(sv, 10.0);

    std::printf("server: waiting for client...\n");
    if (netsvWaiting(sv) != 0) {
        std::fprintf(stderr, "server: netsvWaiting failed\n");
        netsvEnd(sv);
        return 1;
    }

    std::printf("server: client connected\n");

    if (read_exact_server(sv, buf, sizeof(expect_msg) - 1) != 0) {
        std::fprintf(stderr, "server: read_exact_server failed\n");
        netsvDisconnect(sv);
        netsvEnd(sv);
        return 1;
    }

    std::printf("server: received [%s]\n", buf);

    if (std::memcmp(buf, expect_msg, sizeof(expect_msg) - 1) != 0) {
        std::fprintf(stderr, "server: unexpected message\n");
        netsvDisconnect(sv);
        netsvEnd(sv);
        return 1;
    }

    if (write_all_server(sv, (const unsigned char*)reply_msg, sizeof(reply_msg) - 1) != 0) {
        std::fprintf(stderr, "server: write_all_server failed\n");
        netsvDisconnect(sv);
        netsvEnd(sv);
        return 1;
    }

    std::printf("server: sent [%s]\n", reply_msg);

    netsvDisconnect(sv);
    netsvEnd(sv);

    std::printf("server: success\n");
    return 0;
}


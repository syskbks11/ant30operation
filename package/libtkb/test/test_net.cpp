#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include "libtkb.h"

// extern "C" {
// #include "libtkb.h"
// }

/* ---------- Compile ---------- */
// g++ -m32 -O -g -Wall -Wno-format-extra-args -pthread \
//     -I./src \
//     test/test_net.cpp \
//     ./export/libtkb.a \
//     -o test/test_net


/* ---------- Utility wrappers ---------- */

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

/* ---------- Test 1: Basic echo ---------- */

struct EchoServerArgs {
    int port;
    int result;
};

static void* echo_server_thread(void* arg) {
    EchoServerArgs* a = (EchoServerArgs*)arg;
    a->result = 1;

    const char req[] = "PING";
    const char rep[] = "PONG";
    unsigned char buf[sizeof(req)] = {0};

    netServerClass_t* sv = netsvInit(a->port);
    if (!sv) {
        std::fprintf(stderr, "echo_server_thread: netsvInit failed\n");
        return NULL;
    }

    netsvSetTimeOut(sv, 5.0);

    if (netsvWaiting(sv) != 0) {
        std::fprintf(stderr, "echo_server_thread: netsvWaiting failed\n");
        netsvEnd(sv);
        return NULL;
    }

    if (read_exact_server(sv, buf, sizeof(req) - 1) != 0) {
        std::fprintf(stderr, "echo_server_thread: read_exact_server failed\n");
        netsvDisconnect(sv);
        netsvEnd(sv);
        return NULL;
    }

    if (std::memcmp(buf, req, sizeof(req) - 1) != 0) {
        std::fprintf(stderr, "echo_server_thread: request mismatch\n");
        netsvDisconnect(sv);
        netsvEnd(sv);
        return NULL;
    }

    if (write_all_server(sv, (const unsigned char*)rep, sizeof(rep) - 1) != 0) {
        std::fprintf(stderr, "echo_server_thread: write_all_server failed\n");
        netsvDisconnect(sv);
        netsvEnd(sv);
        return NULL;
    }

    netsvDisconnect(sv);
    netsvEnd(sv);
    a->result = 0;
    return NULL;
}

static int test_basic_echo(int port) {
    EchoServerArgs args;
    args.port = port;
    args.result = 1;

    pthread_t th;
    if (pthread_create(&th, NULL, echo_server_thread, &args) != 0) {
        std::fprintf(stderr, "test_basic_echo: pthread_create failed\n");
        return 1;
    }

    usleep(200000);

    netClass_t* cl = netclInit("127.0.0.1", port);
    if (!cl) {
        std::fprintf(stderr, "test_basic_echo: netclInit failed\n");
        pthread_join(th, NULL);
        return 1;
    }

    netclSetTimeOut(cl, 5.0);

    const char req[] = "PING";
    const char rep[] = "PONG";
    unsigned char buf[sizeof(rep)] = {0};

    if (write_all_client(cl, (const unsigned char*)req, sizeof(req) - 1) != 0) {
        std::fprintf(stderr, "test_basic_echo: write_all_client failed\n");
        netclEnd(cl);
        pthread_join(th, NULL);
        return 1;
    }

    if (read_exact_client(cl, buf, sizeof(rep) - 1) != 0) {
        std::fprintf(stderr, "test_basic_echo: read_exact_client failed\n");
        netclEnd(cl);
        pthread_join(th, NULL);
        return 1;
    }

    netclEnd(cl);
    pthread_join(th, NULL);

    if (args.result != 0) {
        std::fprintf(stderr, "test_basic_echo: server side failed\n");
        return 1;
    }

    if (std::memcmp(buf, rep, sizeof(rep) - 1) != 0) {
        std::fprintf(stderr, "test_basic_echo: response mismatch\n");
        return 1;
    }

    std::printf("[PASS] test_basic_echo\n");
    return 0;
}

/* ---------- Test 2: Auto reconnect smoke test ---------- */

struct DelayedServerArgs {
    int port;
    int start_delay_msec;
    int result;
};

static void* delayed_server_thread(void* arg) {
    DelayedServerArgs* a = (DelayedServerArgs*)arg;
    a->result = 1;

    usleep((useconds_t)(a->start_delay_msec * 1000));

    const char req[] = "HELLO";
    const char rep[] = "WORLD";
    unsigned char buf[sizeof(req)] = {0};

    netServerClass_t* sv = netsvInit(a->port);
    if (!sv) {
        std::fprintf(stderr, "delayed_server_thread: netsvInit failed\n");
        return NULL;
    }

    netsvSetTimeOut(sv, 5.0);

    if (netsvWaiting(sv) != 0) {
        std::fprintf(stderr, "delayed_server_thread: netsvWaiting failed\n");
        netsvEnd(sv);
        return NULL;
    }

    if (read_exact_server(sv, buf, sizeof(req) - 1) != 0) {
        std::fprintf(stderr, "delayed_server_thread: read_exact_server failed\n");
        netsvDisconnect(sv);
        netsvEnd(sv);
        return NULL;
    }

    if (std::memcmp(buf, req, sizeof(req) - 1) != 0) {
        std::fprintf(stderr, "delayed_server_thread: request mismatch\n");
        netsvDisconnect(sv);
        netsvEnd(sv);
        return NULL;
    }

    if (write_all_server(sv, (const unsigned char*)rep, sizeof(rep) - 1) != 0) {
        std::fprintf(stderr, "delayed_server_thread: write_all_server failed\n");
        netsvDisconnect(sv);
        netsvEnd(sv);
        return NULL;
    }

    netsvDisconnect(sv);
    netsvEnd(sv);
    a->result = 0;
    return NULL;
}

static int test_auto_reconnect(int port) {
    DelayedServerArgs args;
    args.port = port;
    args.start_delay_msec = 700;
    args.result = 1;

    pthread_t th;
    if (pthread_create(&th, NULL, delayed_server_thread, &args) != 0) {
        std::fprintf(stderr, "test_auto_reconnect: pthread_create failed\n");
        return 1;
    }

    /* Start client before server becomes available */
    netClass_t* cl = netclInitWithTimeOut("127.0.0.1", port, 0.2);
    if (!cl) {
        std::fprintf(stderr, "test_auto_reconnect: netclInitWithTimeOut returned NULL\n");
        pthread_join(th, NULL);
        return 1;
    }

    netclSetTimeOut(cl, 1.0);

    const char req[] = "HELLO";
    const char rep[] = "WORLD";
    unsigned char buf[sizeof(rep)] = {0};

    int sent = 0;
    for (int i = 0; i < 30; ++i) {
        if (write_all_client(cl, (const unsigned char*)req, sizeof(req) - 1) == 0) {
            sent = 1;
            break;
        }
        usleep(100000);
    }

    if (!sent) {
        std::fprintf(stderr, "test_auto_reconnect: could not send after retries\n");
        netclEnd(cl);
        pthread_join(th, NULL);
        return 1;
    }

    if (read_exact_client(cl, buf, sizeof(rep) - 1) != 0) {
        std::fprintf(stderr, "test_auto_reconnect: read_exact_client failed\n");
        netclEnd(cl);
        pthread_join(th, NULL);
        return 1;
    }

    netclEnd(cl);
    pthread_join(th, NULL);

    if (args.result != 0) {
        std::fprintf(stderr, "test_auto_reconnect: server side failed\n");
        return 1;
    }

    if (std::memcmp(buf, rep, sizeof(rep) - 1) != 0) {
        std::fprintf(stderr, "test_auto_reconnect: response mismatch\n");
        return 1;
    }

    std::printf("[PASS] test_auto_reconnect\n");
    return 0;
}

/* ---------- Main ---------- */

int main(int argc, char* argv[]) {
    int port1 = 40101;
    int port2 = 40102;

    if (argc >= 2) {
        port1 = std::atoi(argv[1]);
    }
    if (argc >= 3) {
        port2 = std::atoi(argv[2]);
    }

    int failed = 0;

    failed += test_basic_echo(port1);
    failed += test_auto_reconnect(port2);

    if (failed == 0) {
        std::printf("All tests passed.\n");
        return 0;
    }

    std::printf("Some tests failed. failed=%d\n", failed);
    return 1;
}


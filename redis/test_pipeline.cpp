#include <hiredis/hiredis.h>
#include <stdio.h>
#include <stdlib.h>

// #define _DEBUG
#define PIPELINE_CMD_CNT 100

int g_cmd_cnt = 0;
bool g_is_pipeline = true;

int g_port = 6379;
const char* g_host = "127.0.0.1";
const char* g_read_cmd = "get key123";
const char* g_write_cmd = "set key123 value123";

long long usec(void);
void handle_normal_cmds(redisContext* c);
void handle_pipeline_cmds(redisContext* c);

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("./proc [is_pipeline] [cmd_cnt]\n");
        return -1;
    }

    g_cmd_cnt = atoi(argv[2]);
    g_is_pipeline = (atoi(argv[1]) == 1);
    printf("test pipeline: %d, cmd cnt: %d\n", g_is_pipeline, g_cmd_cnt);

    redisContext* c = redisConnect(g_host, g_port);
    if (c == nullptr || c->err != REDIS_OK) {
        redisFree(c);
        printf("redis conn error: can't allocate redis context.\n");
        return -1;
    }

    redisReply* r = (redisReply*)redisCommand(c, g_write_cmd);
    if (r == NULL) {
        printf("write redis key failed!\n");
        redisFree(c);
        return -1;
    }
    freeReplyObject(r);

    if (g_is_pipeline) {
        handle_pipeline_cmds(c);
    } else {
        handle_normal_cmds(c);
    }

    redisFree(c);
    return 0;
}

long long usec(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000000) + tv.tv_usec;
}

void handle_normal_cmds(redisContext* c) {
    int idx = 0;
    redisReply* r;
    long long begin = usec();

    for (int i = 0; i < g_cmd_cnt; i++, idx++) {
        r = (redisReply*)redisCommand(c, g_read_cmd);
        if (r != nullptr) {
#ifdef _DEBUG
            printf("id: %d, data type: %d, data: %s.\n", idx, r->type, r->str);
#endif
            freeReplyObject(r);
        }
    }

    printf("normal test, cmd cnt: %d, spend time: %llu us.\n", g_cmd_cnt, usec() - begin);
}

void handle_pipeline_cmds(redisContext* c) {
    long long begin, spend;
    int ret, cnt, cmd_cnt, idx;
    redisReply* replys[PIPELINE_CMD_CNT];

    cnt = idx = 0;
    begin = usec();
    cmd_cnt = g_cmd_cnt;

    do {
        for (int i = 0; i < PIPELINE_CMD_CNT && cmd_cnt > 0; i++) {
            redisAppendCommand(c, g_read_cmd);
            cnt++;
            cmd_cnt--;
        }

        for (int i = 0; i < cnt; i++, idx++) {
            ret = redisGetReply(c, (void**)&replys[i]);
            if (ret != REDIS_OK) {
                printf("redis exec cmd failed! err: %d, errstr: %s.\n", c->err, c->errstr);
            } else {
                if (replys[i] != nullptr) {
#ifdef _DEBUG
                    printf("index: %d, data type: %d, data: %s.\n",
                           idx, replys[i]->type, replys[i]->str);
#endif
                }
            }
            freeReplyObject(replys[i]);
        }

        cnt = 0;
    } while (cmd_cnt > 0);

    spend = usec() - begin;
    printf("pipeline test, cmd cnt: %d, spend time: %llu us.\n", g_cmd_cnt, spend);
}
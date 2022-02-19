#include <mysql/mysql.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include <iostream>

#include "co_routine.h"

#define QUERY_CNT 10000

int g_test_cnt = 0;
double g_spend_time = 0.0;

typedef struct db_s {
    std::string host;
    int port;
    std::string user;
    std::string psw;
    std::string charset;
} db_t;

typedef struct task_s {
    int id;
    db_t* db;
    MYSQL* mysql;
    stCoRoutine_t* co;
} task_t;

double time_now() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return ((tv).tv_sec + (tv).tv_usec * 1e-6);
}

void show_error(MYSQL* mysql) {
    printf("error: %d, errstr: %s\n", mysql_errno(mysql), mysql_error(mysql));
}

int show_mysql_query_data(MYSQL* mysql, MYSQL_RES* res) {
    if (mysql == nullptr || res == nullptr) {
        return -1;
    }

    MYSQL_ROW row;
    MYSQL_FIELD* fields;
    int i, field_cnt, row_cnt;

    row_cnt = mysql_num_rows(res);
    fields = mysql_fetch_fields(res);
    field_cnt = mysql_num_fields(res);

    printf("----------\n");
    while ((row = mysql_fetch_row(res)) != NULL) {
        for (i = 0; i < field_cnt; i++) {
            printf("%s: %s, ", fields[i].name, (row[i] != nullptr) ? row[i] : "");
        }
        printf("\n");
    }

    return row_cnt;
}

void* co_handler_mysql_query(void* arg) {
    co_enable_hook_sys();

    int i;
    db_t* db;
    task_t* task;
    MYSQL_RES* res;
    const char* query;
    double begin, spend;

    task = (task_t*)arg;
    db = task->db;

    printf("task id: %d \n", task->id);

    /* connect mysql */
    if (task->mysql == nullptr) {
        task->mysql = mysql_init(NULL);
        if (!mysql_real_connect(
                task->mysql,
                db->host.c_str(),
                db->user.c_str(),
                db->psw.c_str(),
                "mysql",
                db->port,
                NULL, 0)) {
            show_error(task->mysql);
            return nullptr;
        }
    }

    begin = time_now();

    for (i = 0; i < QUERY_CNT; i++) {
        g_test_cnt++;
        /* select mysql. */
        query = "select * from mytest.test_async_mysql where id = 1;";
        if (mysql_real_query(task->mysql, query, strlen(query))) {
            show_error(task->mysql);
            return nullptr;
        }

        res = mysql_store_result(task->mysql);
        // show_mysql_query_data(task->mysql, res);
        mysql_free_result(res);
        // co_resume(task->co);
        // co_yield_ct();
    }

    spend = time_now() - begin;
    g_spend_time += spend;
    printf("id: %d,test cnt: %d, avg: %lf, cur spend time: %lf, total: %lf\n",
           task->id, g_test_cnt, (g_test_cnt / g_spend_time),
           spend, g_spend_time);

    /* close mysql. */
    mysql_close(task->mysql);
    delete task;
    return nullptr;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("invalid param!\n");
        return -1;
    }

    db_t* db;
    task_t* task;
    int i, co_cnt;

    co_cnt = atoi(argv[1]);
    db = new db_t{"127.0.0.1", 3306, "root", "root123!@#", "utf8mb4"};

    for (i = 0; i < co_cnt; i++) {
        task = new task_t{i, db, nullptr, nullptr};
        co_create(&(task->co), NULL, co_handler_mysql_query, task);
        co_resume(task->co);
    }

    co_eventloop(co_get_epoll_ct(), 0, 0);
    delete db;
    return 0;
}
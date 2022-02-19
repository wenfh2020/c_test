
/* 
 * wenfh2020 / 2020-07-25
 * test reflect.
 * protoc -I. --cpp_out=. msg.proto http.proto 
 * c && g++ -std='c++11' http.pb.cc msg.pb.cc test_reflect.cpp  -lprotobuf -o trf && ./trf 
 */

#include <iostream>
#include <map>
#include <set>
#include <string>

#include "http.pb.h"
#include "msg.pb.h"

#define SAFE_DELETE(x)              \
    {                               \
        if (x != nullptr) delete x; \
        x = nullptr;                \
    }

class Request {
   public:
    Request() {}
    virtual ~Request() {
        SAFE_DELETE(m_msg_head);
        SAFE_DELETE(m_msg_body);
        SAFE_DELETE(m_http_msg);
    }
    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;

    MsgHead* get_msg_head() {
        if (m_msg_head == nullptr) {
            m_msg_head = new MsgHead;
        }
        return m_msg_head;
    }
    MsgBody* get_msg_body() {
        if (m_msg_body == nullptr) {
            m_msg_body = new MsgBody;
        }
        return m_msg_body;
    }
    HttpMsg* get_http_msg() {
        if (m_http_msg == nullptr) {
            m_http_msg = new HttpMsg;
        }
        return m_http_msg;
    }
    void set_errno(int n) { m_errno = n; }
    int get_errno() { return m_errno; }

    void set_privdate(void* data) { m_privdata = data; }
    void* get_privdata() { return m_privdata; }

   private:
    MsgHead* m_msg_head = nullptr;  // protobuf msg head.
    MsgBody* m_msg_body = nullptr;  // protobuf msg body.
    HttpMsg* m_http_msg = nullptr;  // http msg.

    int m_errno = -1;            // error number.
    void* m_privdata = nullptr;  // private data.
};

class Cmd {
   public:
    enum class STATUS {
        UNKOWN = 0,
        OK = 1,
        RUNNING = 2,
        COMPLETED = 3,
        ERROR = 4,
    };

    Cmd() {}
    virtual ~Cmd() {}
    virtual Cmd::STATUS execute(std::shared_ptr<Request> req) = 0;
    virtual Cmd::STATUS call_back(HttpMsg* msg) = 0;
};

#define BEGIN_HTTP_MAP()                                                \
   public:                                                              \
    virtual Cmd::STATUS process_message(std::shared_ptr<Request> req) { \
        const HttpMsg* msg = req->get_http_msg();                       \
        if (msg == nullptr) {                                           \
            return Cmd::STATUS::ERROR;                                  \
        }                                                               \
        std::string path = msg->path();

#define HTTP_HANDLER(_path, _cmd)             \
    if (path == (_path)) {                    \
        _cmd* p = new _cmd;                   \
        Cmd::STATUS status = p->execute(req); \
        if (status == Cmd::STATUS::RUNNING) { \
            auto it = m_cmds.insert(p);       \
            if (!it.second) {                 \
                delete p;                     \
                return Cmd::STATUS::ERROR;    \
            }                                 \
            return status;                    \
        }                                     \
        delete p;                             \
        return status;                        \
    }

#define END_HTTP_MAP()          \
    return Cmd::STATUS::UNKOWN; \
    }

#define END_HTTP_MAP_EX(base)          \
    return base::process_message(msg); \
    }

class CmdHello : public Cmd {
   public:
    CmdHello() {}
    virtual ~CmdHello() {
        std::cout << "delete cmd hello" << std::endl;
    }

    virtual Cmd::STATUS execute(std::shared_ptr<Request> req) override {
        std::cout << "hello world!!!" << std::endl;
        return Cmd::STATUS::OK;
    }
    virtual Cmd::STATUS call_back(HttpMsg* msg) override {
        std::cout << msg->path() << std::endl;
        return Cmd::STATUS::OK;
    }
};

#define REGISTER_FUNC(path, func) \
    m_cmd_funcs[path] = &func;

#define CMD_FUNC(func, cmd_name)                     \
    Cmd::STATUS func(std::shared_ptr<Request> req) { \
        cmd_name* p = new cmd_name;                  \
        Cmd::STATUS status = p->execute(req);        \
        if (status == Cmd::STATUS::RUNNING) {        \
        }                                            \
        delete p;                                    \
        return status;                               \
    }

class Module {
   public:
    Module() {}
    virtual ~Module() {
        for (const auto& it : m_cmds) {
            delete it;
        }
    }

    virtual Cmd::STATUS process_message(std::shared_ptr<Request> req) {
        return Cmd::STATUS::UNKOWN;
    }

   protected:
    int m_version = 1;
    std::string m_name;
    std::string m_file_path;
    std::set<Cmd*> m_cmds;
};

// CMD_FUNC(func_cmd_hello, CmdHello)

#define REGISTER_HANDLER(class_name)                                       \
   public:                                                                 \
    typedef Cmd::STATUS (class_name::*cmd_func)(std::shared_ptr<Request>); \
    virtual Cmd::STATUS process_message(std::shared_ptr<Request> req) {    \
        const HttpMsg* msg = req->get_http_msg();                          \
        if (msg == nullptr) {                                              \
            return Cmd::STATUS::ERROR;                                     \
        }                                                                  \
        auto it = m_cmd_funcs.find(msg->path());                           \
        if (it == m_cmd_funcs.end()) {                                     \
            return Cmd::STATUS::UNKOWN;                                    \
        }                                                                  \
        return (this->*(it->second))(req);                                 \
    }                                                                      \
                                                                           \
   protected:                                                              \
    std::map<std::string, cmd_func> m_cmd_funcs;

#define REGISTER_HANDLE_FUNC(path, func) \
    m_cmd_funcs[path] = &func;

class MoudleUser : public Module {
    REGISTER_HANDLER(MoudleUser)
   public:
    void init() {
        REGISTER_HANDLE_FUNC("/kim/im/user/", MoudleUser::func_cmd_hello);
    }

   private:
    Cmd::STATUS func_cmd_hello(std::shared_ptr<Request> req) {
        std::cout << "hello world" << std::endl;
        return Cmd::STATUS::OK;
    }
};

int main(int argc, char** argv) {
    std::shared_ptr<Request> req = std::make_shared<Request>();
    HttpMsg* msg = req->get_http_msg();
    msg->set_path("/kim/im/user/");
    MoudleUser* m = new MoudleUser();
    m->init();
    m->process_message(req);
    std::map<std::string, Module*> modules;
    modules["xxxxso"] = m;
    for (const auto& it : modules) {
        delete m;
    }
    return 0;
}
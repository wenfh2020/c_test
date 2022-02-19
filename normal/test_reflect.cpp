#include <iostream>
#include <set>

class Log {
   public:
    int test = 0;
};

class INet {
   public:
    int test = 0;
};

class Request {
   public:
    Request() {}

    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;
    virtual ~Request();

    void set_errno(int n) { m_errno = n; }
    int get_errno() { return m_errno; }

    void set_privdate(void* data) { m_privdata = data; }
    void* get_privdata() { return m_privdata; }

   private:
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
    Cmd(const Cmd&) = delete;
    Cmd& operator=(const Cmd&) = delete;
    virtual ~Cmd();

    void init(Log* logger, INet* net);

    virtual Cmd::STATUS call_back(std::shared_ptr<Request> req) { return Cmd::STATUS::OK; }

    void set_errno(int err) { m_errno = err; }
    void set_error(const std::string& error) { m_error = error; }
    void set_error_info(int err, const std::string& error) {
        m_errno = err;
        m_error = error;
    }

   protected:
    uint64_t m_id = 0;
    std::shared_ptr<Request> m_req = nullptr;
    std::string m_cmd_name;
    Log* m_logger = nullptr;
    INet* m_net = nullptr;

    int m_errno = 0;
    std::string m_error;
};

class Module {
   public:
    Module() {}
    Module(Log* logger) : m_logger(logger) {}
    virtual ~Module();

    virtual Cmd::STATUS process_message(std::shared_ptr<Request> req) { return Cmd::STATUS::UNKOWN; }

    bool init(Log* logger, INet* net);
    void set_version(int ver) { m_version = ver; }
    void set_name(const std::string& name) { m_name = name; }
    std::string get_name() { return m_name; }
    void set_file_path(const std::string& path) { m_file_path = path; }

   protected:
    int m_version = 1;
    std::string m_name;
    std::string m_file_path;
    std::set<Cmd*> m_cmds;
    Log* m_logger = nullptr;
    INet* m_net = nullptr;
};

#define BEGIN_HTTP_MAP()                                                \
   public:                                                              \
    virtual Cmd::STATUS process_message(std::shared_ptr<Request> req) { \
        std::string path = "123";

#define HTTP_HANDLER(_path, _cmd, _name)        \
    if (path == (_path)) {                      \
        _cmd* p = new _cmd;                     \
        p->init(m_logger, m_net);               \
        Cmd::STATUS status = p->call_back(req); \
        if (status == Cmd::STATUS::RUNNING) {   \
            auto it = m_cmds.insert(p);         \
            if (!it.second) {                   \
                delete p;                       \
                return Cmd::STATUS::ERROR;      \
            }                                   \
            return status;                      \
        }                                       \
        delete p;                               \
        return status;                          \
    }

#define END_HTTP_MAP()          \
    return Cmd::STATUS::UNKOWN; \
    }

#define END_HTTP_MAP_EX(base)          \
    return base::process_message(req); \
    }

class CmdHello : public Cmd {
   public:
    CmdHello() {}
    ~CmdHello() {}
};

class ModuleCore : public Module {
    BEGIN_HTTP_MAP()
    HTTP_HANDLER("/kim/im/user/", CmdHello, "hello");
    END_HTTP_MAP()
};

int main(int argc, char** argv) {
    INet net;
    Log logger;
    ModuleCore m;
    return 0;
}
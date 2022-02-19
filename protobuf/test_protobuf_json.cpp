#include <google/protobuf/util/json_util.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

using google::protobuf::util::JsonStringToMessage;

#define PROTO_MSG_HEAD_LEN 15

class addr_info_t {
   public:
    std::string bind;       // bind host for inner server.
    int port = 0;           // port for inner server.
    std::string gate_bind;  // bind host for user client.
    int gate_port = 0;      // port for user client.
};

////////////////////////////////////

class node_info_t {
   public:
    addr_info_t addr_info;     // network addr info.
    std::string node_type;     // node type in cluster.
    std::string conf_path;     // config path.
    std::string work_path;     // process work path.
    int worker_processes = 0;  // number of worker's processes.
};

void test_proto_json() {
    kim::node_info node;
    node.set_name("111111");
    node.mutable_addr_info()->set_bind("wruryeuwryeuwrw");
    node.mutable_addr_info()->set_port(342);
    node.mutable_addr_info()->set_gate_bind("fsduyruwerw");
    node.mutable_addr_info()->set_gate_port(4853);

    node.set_node_type("34rw343");
    node.set_conf_path("reuwyruiwe");
    node.set_work_path("ewiruwe");
    node.set_worker_cnt(3);

    std::string json_string;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;
    MessageToJsonString(node, &json_string, options);

    // std::cout << node.SerializeAsString() << std::endl
    std::cout << json_string << std::endl;

    node.Clear();
    if (JsonStringToMessage(json_string, &node).ok()) {
        std::cout << "json to protobuf: "
                  << node.name()
                  << ", "
                  << node.mutable_addr_info()->bind()
                  << std::endl;
    }
}

int main(int argc, char** argv) {
    // check_protobuf();
    // test_server(argc, argv);
    // compare_struct();
    test_proto_json();
    return 0;
}
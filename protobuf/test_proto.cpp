/* wenfh2020 / 2020-07-08
 *
 * protoc -I. --cpp_out=. msg.proto http.proto 
 * g++ -std='c++11' http.pb.cc msg.pb.cc test_proto.cpp  -lprotobuf -o tproto && ./tproto 
 * 
 */

#include <iostream>

#include "http.pb.h"
#include "msg.pb.h"

int main(int argc, char** argv) {
    MsgHead head;
    head.set_cmd(1);
    head.set_len(11);
    head.set_seq(12);
    std::cout << head.ByteSizeLong() << std::endl;

    MsgBody body;
    body.set_data("hello");
    MsgBody_Request* req = body.mutable_req_target();
    MsgBody_Response* rsp = body.mutable_rsp_result();
    req->set_route_id(1);
    req->set_route("123");
    rsp->set_code(111);

    std::cout << body.SerializePartialAsString() << std::endl;
    return 0;
}
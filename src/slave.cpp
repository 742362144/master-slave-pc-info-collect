// Copyright (c) 2014 Baidu, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// A server to receive EchoRequest and send back EchoResponse.

#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/server.h>
#include "tools/PCInfoTools.h"
#include "pcinfo.pb.h"

//变量的初始化
DEFINE_bool(echo_attachment, true, "Echo attachment as well");
DEFINE_int32(port, 8000, "TCP Port of this server");
DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
DEFINE_int32(logoff_ms, 2000, "Maximum duration of server's LOGOFF state "
             "(waiting for client to close connection before server stops)");

// Your implementation of example::EchoService
// Notice that implementing brpc::Describable grants the ability to put
// additional information in /status.
namespace example {
class PCInfoServiceImpl : public PCInfoService {
public:
    PCInfoServiceImpl() {};
    virtual ~PCInfoServiceImpl() {};

    virtual void PCInfo(google::protobuf::RpcController* cntl_base,
                      const PCInfoRequest* request,
                      PCInfoResponse* response,
                      google::protobuf::Closure* done) {

        // 不管成功失败，done->Run()必须在请求处理完成后被用户调用一次
        // 强烈建议使用ClosureGuard确保done->Run()被调用
        // 这个对象确保在return时自动调用done->Run()
        brpc::ClosureGuard done_guard(done);

        // 获得controller
        // 将protobuf中定义的RpcController强转为brpc::Controller
        // 包含了所有request和response之外的参数集合，具体接口查阅controller.h
        brpc::Controller* cntl =
            static_cast<brpc::Controller*>(cntl_base);

        // 打印日志
        LOG(INFO) << "Received request[log_id=" << cntl->log_id()
                  << "] from " << cntl->remote_side()
                  << " to " << cntl->local_side() << noflush;
        LOG(INFO) << ": " << request->message() << noflush;
        if (!cntl->request_attachment().empty()) {
            LOG(INFO) << " (attached=" << cntl->request_attachment() << ")" << noflush;
        }
        LOG(INFO);

        // Fill response.
        example::PCInfo *info = new example::PCInfo();
        example::Cpu *cpu = new example::Cpu();

        //get cpu info
        tools::PCInfoTools *tool = new tools::PCInfoTools();
        tools::Cpu pc_cpu = tool->parseCpu();
        cpu->set_cpu(pc_cpu.getCore());
        cpu->set_frequency(pc_cpu.getFrequency());
        info->set_allocated_cpu(cpu);
        response->set_allocated_info(info);

        // 你可以通过设置控制器来压缩响应，但要注意这种压缩可能是昂贵的，在打开之前评估。
        // cntl->set_response_compress_type(brpc::COMPRESS_TYPE_GZIP);

        if (FLAGS_echo_attachment) {
            // Set attachment which is wired to network directly instead of
            // being serialized into protobuf messages.
            // 设置直接连接到网络的附件而不是被序列化为原始buf消息
            cntl->response_attachment().append(cntl->request_attachment());
        }
    }
};
}  // namespace example

int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    google::ParseCommandLineFlags(&argc, &argv, true);

    // 实例化server，只需要一个server
    brpc::Server server;

    // 实例化服务
    example::PCInfoServiceImpl pc_info_service_impl;

    // 默认构造后的Server不包含任何服务，也不会对外提供服务，仅仅是一个对象
    // 通过如下方法插入你的Service实例。
    // int AddService(google::protobuf::Service* service, ServiceOwnership ownership);
    // ownership参数为SERVER_OWNS_SERVICE，Server在析构时会一并删除Service，否则应设为SERVER_DOESNT_OWN_SERVICE
    if (server.AddService(&pc_info_service_impl,
                          brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service";
        return -1;
    }

    // 开启服务器
    brpc::ServerOptions options;
    //idle_timeout_sec对应的时间内没有读取或写出数据，则被视为”闲置”而被server主动关闭。默认值为-1，代表不开启
    options.idle_timeout_sec = FLAGS_idle_timeout_s;
    if (server.Start(FLAGS_port, &options) != 0) {  //指定端口和选项，开启服务器
        LOG(ERROR) << "Fail to start slave";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    // 原生方法
    server.RunUntilAskedToQuit();
    return 0;
}

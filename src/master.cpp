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

// A client sending requests to server every 1 second.

#include<iostream>
#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/channel.h>
#include "pcinfo.pb.h"
#include "tools/PCInfoTools.h"

DEFINE_string(attachment, "foo", "Carry this along with requests");
DEFINE_string(protocol, "baidu_std", "Protocol type. Defined in src/brpc/options.proto");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
DEFINE_string(server, "0.0.0.0:8000", "IP Address of server");
DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 100, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)");
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");
DEFINE_string(http_content_type, "application/json", "Content type of http request");

int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    google::ParseCommandLineFlags(&argc, &argv, true);

    // A Channel represents a communication line to a Server. Notice that
    // Channel is thread-safe and can be shared by all threads in your program.
    // 线程安全
    brpc::Channel channel;

    // Initialize the channel, NULL means using default options.
    // 初始化channel 为空使用默认设置
    // 使用的设置定义在gflags.h
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.connection_type = FLAGS_connection_type;
    options.timeout_ms = FLAGS_timeout_ms/*milliseconds*/;
    options.max_retry = FLAGS_max_retry;

    // 这类Init连接的服务器往往有固定的ip地址，不需要名字服务和负载均衡，创建起来相对轻量。
    // 但是请勿频繁创建使用域名的Channel。
    // 这需要查询dns，可能最多耗时10秒(查询DNS的默认超时)。重用它们。
    if (channel.Init(FLAGS_server.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.

    // example为echo.protoc内定义的包
    // 客户端都通过protoc中定义的service的stub类来发送请求
    example::PCInfoService_Stub stub(&channel);

    // Send a request and wait for the response every 1 second.
    int log_id = 0;
    while (!brpc::IsAskedToQuit()) {  // 循环直到ctrl+c退出
        // We will receive response synchronously, safe to put variables
        // on stack.
        example::PCInfoRequest request;   // EchoRequest为echo.protoc内定义的message
        example::PCInfoResponse response;     // EchoRequest为echo.protoc内定义的message
        brpc::Controller cntl;

        std::string str = "get pc info!!!!";
        request.set_message(str);

        cntl.set_log_id(log_id ++);  // set by user
        if (FLAGS_protocol != "http" && FLAGS_protocol != "h2c")  {
            // Set attachment which is wired to network directly instead of
            // being serialized into protobuf messages.
            cntl.request_attachment().append(FLAGS_attachment);
        } else {
            cntl.http_request().set_content_type(FLAGS_http_content_type);
        }

        // Because `done'(last parameter) is NULL, this function waits until
        // the response comes back or error occurs(including timedout).
        // 因为done是NULL 所以下面的函数会阻塞，知道收到response或者出错超时
        stub.PCInfo(&cntl, &request, &response, NULL);
        // 发送成功 打印response内容和日志
        if (!cntl.Failed()) {
            LOG(INFO) << "Received response from " << cntl.remote_side()
                      << ": " << "cpu num:" << response.info().cpu().cpu()
                      << " frequency :" << response.info().cpu().frequency()
                      << " latency=" << cntl.latency_us() << "us";

        } else {
            // 否则打印错误信息
            LOG(WARNING) << cntl.ErrorText();
        }
        // 睡眠1s 等待服务器回应
        usleep(FLAGS_interval_ms * 10000L);
    }
    // quit
    LOG(INFO) << "master is going to quit";
    return 0;
}

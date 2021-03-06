// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: calculator.proto

#include "calculator.pb.h"
#include "calculator.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace calculator {

static const char* Calculator_method_names[] = {
  "/calculator.Calculator/Calculate",
  "/calculator.Calculator/ReadCalculatorLoad",
};

std::unique_ptr< Calculator::Stub> Calculator::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Calculator::Stub> stub(new Calculator::Stub(channel));
  return stub;
}

Calculator::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Calculate_(Calculator_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ReadCalculatorLoad_(Calculator_method_names[1], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  {}

::grpc::Status Calculator::Stub::Calculate(::grpc::ClientContext* context, const ::calculator::OperationRequest& request, ::calculator::OperationResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Calculate_, context, request, response);
}

void Calculator::Stub::experimental_async::Calculate(::grpc::ClientContext* context, const ::calculator::OperationRequest* request, ::calculator::OperationResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Calculate_, context, request, response, std::move(f));
}

void Calculator::Stub::experimental_async::Calculate(::grpc::ClientContext* context, const ::calculator::OperationRequest* request, ::calculator::OperationResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Calculate_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::calculator::OperationResponse>* Calculator::Stub::PrepareAsyncCalculateRaw(::grpc::ClientContext* context, const ::calculator::OperationRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::calculator::OperationResponse>::Create(channel_.get(), cq, rpcmethod_Calculate_, context, request, false);
}

::grpc::ClientAsyncResponseReader< ::calculator::OperationResponse>* Calculator::Stub::AsyncCalculateRaw(::grpc::ClientContext* context, const ::calculator::OperationRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncCalculateRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::ClientReader< ::calculator::LoadResponse>* Calculator::Stub::ReadCalculatorLoadRaw(::grpc::ClientContext* context, const ::calculator::LoadIntervalRequest& request) {
  return ::grpc::internal::ClientReaderFactory< ::calculator::LoadResponse>::Create(channel_.get(), rpcmethod_ReadCalculatorLoad_, context, request);
}

void Calculator::Stub::experimental_async::ReadCalculatorLoad(::grpc::ClientContext* context, ::calculator::LoadIntervalRequest* request, ::grpc::experimental::ClientReadReactor< ::calculator::LoadResponse>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::calculator::LoadResponse>::Create(stub_->channel_.get(), stub_->rpcmethod_ReadCalculatorLoad_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::calculator::LoadResponse>* Calculator::Stub::AsyncReadCalculatorLoadRaw(::grpc::ClientContext* context, const ::calculator::LoadIntervalRequest& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::calculator::LoadResponse>::Create(channel_.get(), cq, rpcmethod_ReadCalculatorLoad_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::calculator::LoadResponse>* Calculator::Stub::PrepareAsyncReadCalculatorLoadRaw(::grpc::ClientContext* context, const ::calculator::LoadIntervalRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::calculator::LoadResponse>::Create(channel_.get(), cq, rpcmethod_ReadCalculatorLoad_, context, request, false, nullptr);
}

Calculator::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Calculator_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Calculator::Service, ::calculator::OperationRequest, ::calculator::OperationResponse>(
          [](Calculator::Service* service,
             ::grpc::ServerContext* ctx,
             const ::calculator::OperationRequest* req,
             ::calculator::OperationResponse* resp) {
               return service->Calculate(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Calculator_method_names[1],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< Calculator::Service, ::calculator::LoadIntervalRequest, ::calculator::LoadResponse>(
          [](Calculator::Service* service,
             ::grpc::ServerContext* ctx,
             const ::calculator::LoadIntervalRequest* req,
             ::grpc::ServerWriter<::calculator::LoadResponse>* writer) {
               return service->ReadCalculatorLoad(ctx, req, writer);
             }, this)));
}

Calculator::Service::~Service() {
}

::grpc::Status Calculator::Service::Calculate(::grpc::ServerContext* context, const ::calculator::OperationRequest* request, ::calculator::OperationResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Calculator::Service::ReadCalculatorLoad(::grpc::ServerContext* context, const ::calculator::LoadIntervalRequest* request, ::grpc::ServerWriter< ::calculator::LoadResponse>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace calculator


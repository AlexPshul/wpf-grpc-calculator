#pragma once
#include "calculator.grpc.pb.h"

class CalculatorService : public calculator::Calculator::Service
{
private:
    grpc_impl::ServerWriter<calculator::LoadResponse>* _pWriter;

    ::grpc::Status Calculate(::grpc::ServerContext* context, const ::calculator::OperationRequest* request, ::calculator::OperationResponse* response) override;
    ::grpc::Status ReadCalculatorLoad(::grpc::ServerContext* context, const ::calculator::LoadIntervalRequest* request, ::grpc::ServerWriter< ::calculator::LoadResponse>* writer) override;

public:
    static void RunServer();
};


#pragma once
#include "calculator.grpc.pb.h"

class CalculatorService : public calculator::Calculator::Service
{
private:
    static CalculatorService* _this;
    mutable std::mutex _notificationMutex;
    mutable std::condition_variable _notification;
    double _rate {};

    void ReportLoad(double rate);
    static void CalcLoadCallback(double rate);
    ::grpc::Status Calculate(::grpc::ServerContext* context, const ::calculator::OperationRequest* request, ::calculator::OperationResponse* response) override;
    ::grpc::Status ReadCalculatorLoad(::grpc::ServerContext* context, const ::calculator::LoadIntervalRequest* request, ::grpc::ServerWriter< ::calculator::LoadResponse>* writer) override;

public:
    static void RunServer();
};


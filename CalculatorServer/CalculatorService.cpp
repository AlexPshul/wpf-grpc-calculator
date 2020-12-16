#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "CalculatorService.h"
#include "CalculatorEngine.h"
#include <map>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "CRYPT32.LIB")

using namespace calculator;
using namespace grpc;

static std::map<OperationRequest_Operators, CalcOperations> operation_map 
{
    {OperationRequest_Operators_ADD, OP_ADD},
    {OperationRequest_Operators_SUBTRACT, OP_SUB},
    {OperationRequest_Operators_MULTIPLY, OP_MUL},
    {OperationRequest_Operators_DIVIDE, OP_DIV}
};

CalculatorService* CalculatorService::_this{ nullptr };

void CalculatorService::ReportLoad(double rate) 
{
    std::unique_lock<std::mutex> lck(_notificationMutex);
    _rate = rate;
    _notification.notify_one();
}

void CalculatorService::CalcLoadCallback(double rate)
{
    if (_this == nullptr)
        return;
    _this->ReportLoad(rate);
}

Status CalculatorService::Calculate(ServerContext* context,
                                    const OperationRequest* request, OperationResponse* response)
{
    response->set_result(0.0); //default, in case of an error

    const auto op = operation_map.find(request->operator_());
    if (op == std::end(operation_map))
    {
        return Status(INVALID_ARGUMENT, "Unknown operation");
    }

    const OperationInfo operationInfo =
    {
        request->a(),
        request->b(),
        op->second
    };

    const auto result = ::Calculate(operationInfo);

    if (result.resultStatus == OP_ERROR_DIVIDE_BY_ZERO)
    {
        return Status(INVALID_ARGUMENT, "Divide by zero");
    }

    if (result.resultStatus == OP_ERROR_UNKNOWN_OPERATION) //probably will never happen
    {
        return Status(INVALID_ARGUMENT, "Unknown operation");
    }

    response->set_result(result.calculationResult);
    return Status::OK;
}

Status CalculatorService::ReadCalculatorLoad(ServerContext* context,
    const LoadIntervalRequest* request, ServerWriter<LoadResponse>* writer)
{
    _this = this;
    RegisterForCallback(CalcLoadCallback, request->intervalinseconds());
    while(true)
    {
        std::unique_lock<std::mutex> lck(_notificationMutex);
        _notification.wait(lck);
        LoadResponse response;
        response.set_invocationsperminutes(_rate);
        writer->Write(response);
    }
    return Status::OK;
}

/*static*/ void CalculatorService::RunServer()
{
    const std::string serverAddress("0.0.0.0:50051");
    CalculatorService service;

    ServerBuilder builder;
    builder.AddListeningPort(serverAddress, InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << serverAddress << std::endl;
    server->Wait();
}

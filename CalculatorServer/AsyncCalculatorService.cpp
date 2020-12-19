#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/security/server_credentials.h>
#include "AsyncCalculatorService.h"
#include "CalculatorEngine.h"
#include <map>
#include <thread>

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


void AsyncCalculatorService::CalculateRequestHandler::OnCreate()
{
	GetService()->RequestCalculate(GetContext(), GetRequest(), GetResponder(), GetService()->GetCompletionQueue().get(), GetService()->GetCompletionQueue().get(),this);
}

void AsyncCalculatorService::CalculateRequestHandler::OnProcess()
{
	const auto op = operation_map.find(GetRequest()->operator_());
	if (op == std::end(operation_map))
	{
		GetResponder()->FinishWithError(Status(INVALID_ARGUMENT, "Unknown operation"), this);
		return;
	}

	const OperationInfo operationInfo =
	{
		GetRequest()->a(),
		GetRequest()->b(),
		op->second
	};

	const auto result = ::Calculate(operationInfo);

	if (result.resultStatus == OP_ERROR_DIVIDE_BY_ZERO)
	{
		GetResponder()->FinishWithError(Status(INVALID_ARGUMENT, "Divide by zero"), this);
		return;
	}

	if (result.resultStatus == OP_ERROR_UNKNOWN_OPERATION) //probably will never happen
	{
		GetResponder()->FinishWithError(Status(INVALID_ARGUMENT, "Unknown operation"), this);
		return;
	}

	OperationResponse operationResponse;
	operationResponse.set_result(result.calculationResult);
	GetResponder()->Finish(operationResponse, Status::OK, this);
}


void AsyncCalculatorService::HandleRequest(const std::shared_ptr<IRequestHandlerBase> &handler)
{
	{
		std::lock_guard<std::mutex> guard { _handlersMutex };
		_handlers.insert(std::make_pair(handler.get(), handler));
	}
	handler->ExecuteNext(); //create

	
	while (true)
	{
		IRequestHandlerBase *pNextHandler;
		bool bOk;

		{
			std::lock_guard<std::mutex> guard{ _completionQueueMutex };
			_completionQueue->Next(reinterpret_cast<void**>(&pNextHandler), &bOk);
		}

		if (!bOk || pNextHandler == nullptr)
			continue;

		auto[shouldFinish, shouldCreateNewInstance] = pNextHandler->ExecuteNext();
		if (shouldFinish)
		{
			std::lock_guard<std::mutex> guard{ _handlersMutex };
			_handlers.erase(pNextHandler); //remove from the map, it also clear the memory of the shared_ptr
			continue;
		}
		
		if (shouldCreateNewInstance)
		{
			auto newHandler = pNextHandler->CreateNew();
			newHandler->ExecuteNext();
			std::lock_guard<std::mutex> guard{ _handlersMutex };
			_handlers.insert(std::make_pair(newHandler.get(), newHandler));
		}
	}
}


void AsyncCalculatorService::CalculatorLoadHandler::ReportLoad(double rate)
{
    std::unique_lock<std::mutex> lck(_notificationMutex);
    _rate = rate;
    _notification.notify_one();
}

void AsyncCalculatorService::CalculatorLoadHandler::CalcLoadCallback(double rate)
{
    if (_this == nullptr)
        return;
    _this->ReportLoad(rate);
}


void AsyncCalculatorService::CalculatorLoadHandler::OnCreate()
{
	GetService()->RequestReadCalculatorLoad(GetContext(), GetRequest(), GetResponder(), GetService()->GetCompletionQueue().get(), GetService()->GetCompletionQueue().get(), this);
}

//in this implementation we do not receive multiple callback clients
void AsyncCalculatorService::CalculatorLoadHandler::OnProcess()
{
	RegisterForCallback(CalcLoadCallback, GetRequest()->intervalinseconds());
	while (true)
	{
		std::unique_lock<std::mutex> lck(_notificationMutex);
		_notification.wait(lck);
		LoadResponse response;
		response.set_invocationsperminutes(_rate);
		GetResponder()->Write(response, this);
	}
}


/*static*/ void AsyncCalculatorService::RunServer()
{
	AsyncCalculatorService service;
	ServerBuilder builder;
    const std::string serverAddress("0.0.0.0:50051");
    builder.AddListeningPort(serverAddress, InsecureServerCredentials());
	builder.RegisterService(&service);
	auto completionQueue = builder.AddCompletionQueue();
	service.SetCompletionQueue(std::move(completionQueue));
	auto server = builder.BuildAndStart();
    std::cout << "Server listening on " << serverAddress << std::endl;

	std::thread calculateRequestHandler([&](){	service.HandleRequest(std::make_shared<CalculateRequestHandler>(&service));	}); //create one reader, we can create more for concurrent request handling
	std::thread calculatorLoadRequestHandler([&]() {service.HandleRequest(std::make_shared<CalculatorLoadHandler>(&service));});
	calculateRequestHandler.join();
	calculatorLoadRequestHandler.join();
}

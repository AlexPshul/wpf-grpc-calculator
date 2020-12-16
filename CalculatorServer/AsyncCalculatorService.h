#pragma once

#include <mutex>
#include "calculator.grpc.pb.h"

class AsyncCalculatorService : public calculator::Calculator::AsyncService
{
private:

	enum class messageProcessingState : unsigned char
	{
		CREATE,
		PROCESS,
		FINISH
	};

	class IRequestHandlerBase
	{
	public:
		IRequestHandlerBase(IRequestHandlerBase &) = delete;
		IRequestHandlerBase& operator=(IRequestHandlerBase &) = delete;
		IRequestHandlerBase(IRequestHandlerBase &&) = delete;
		IRequestHandlerBase& operator=(IRequestHandlerBase &&) = delete;
		
		virtual ~IRequestHandlerBase() = default;
		IRequestHandlerBase() = default;
		virtual std::tuple<bool, bool> ExecuteNext() abstract;
	};

	
	template <typename TRequest, typename TResponder>
	class RequestHandlerBase : public IRequestHandlerBase
	{
	private:
		messageProcessingState _state{ messageProcessingState::CREATE };
		AsyncCalculatorService *_pService;
		grpc::ServerContext _context {};
		TRequest _request {};
		TResponder _responder;

	
	protected:
		AsyncCalculatorService *GetService() const { return _pService; }
		grpc::ServerContext *GetContext() { return &_context; }
		TRequest *GetRequest() { return &_request; }
		TResponder *GetResponder() { return &_responder; }
	
	public:
		RequestHandlerBase(RequestHandlerBase &) = delete;
		RequestHandlerBase& operator=(RequestHandlerBase &) = delete;
		RequestHandlerBase(RequestHandlerBase &&) = delete;
		RequestHandlerBase& operator=(RequestHandlerBase &&) = delete;
		
		RequestHandlerBase(AsyncCalculatorService *pService): _pService(pService), _responder(&_context){}

		//return [shouldFinish, shouldCreateNewInstance]
		std::tuple<bool, bool> ExecuteNext() override
		{
			switch (_state)
			{
			case messageProcessingState::CREATE:
				OnCreate();
				_state = messageProcessingState::PROCESS;
				return std::make_tuple(false, false);

			case messageProcessingState::PROCESS:
				OnProcess();
				_state = messageProcessingState::FINISH;
				return std::make_tuple(false, true);

			case messageProcessingState::FINISH:
				OnFinish();
				return std::make_tuple(true, false);
			}
			return std::make_tuple(false, false);
		}

		
		virtual void OnCreate() abstract;
		virtual void OnProcess() abstract;
		virtual void OnFinish() abstract;
		~RequestHandlerBase() override = default;

	};
	
	class CalculateRequestHandler final : public RequestHandlerBase<calculator::OperationRequest, grpc::ServerAsyncResponseWriter<calculator::OperationResponse>>
	{
	public:
		using RequestHandlerBase<calculator::OperationRequest, grpc::ServerAsyncResponseWriter<calculator::OperationResponse>>::RequestHandlerBase;
	
	private:
		void OnCreate() override;
		void OnProcess() override;
		void OnFinish() override {}
	};

	class CalculatorLoadHandler : public RequestHandlerBase<calculator::LoadIntervalRequest, grpc::ServerAsyncWriter<calculator::LoadResponse>>
	{
		using RequestHandlerBase<calculator::LoadIntervalRequest, grpc::ServerAsyncWriter<calculator::LoadResponse>>::RequestHandlerBase;

	private:
		inline static CalculatorLoadHandler* _this {};
		mutable std::mutex _notificationMutex;
		mutable std::condition_variable _notification;
		double _rate{};
		void ReportLoad(double rate);
		static void CalcLoadCallback(double rate);
		
		void OnCreate() override;
		void OnProcess() override;
		void OnFinish() override {}
	};
	
    
	std::unique_ptr<grpc::ServerCompletionQueue> _completionQueue;
	void SetCompletionQueue(std::unique_ptr<grpc::ServerCompletionQueue> unique) { _completionQueue = std::move(unique); }
	const std::unique_ptr<grpc::ServerCompletionQueue> &GetCompletionQueue() const { return _completionQueue; }

public:
	void HandleRequest(std::function<std::shared_ptr<IRequestHandlerBase>()> CreateHandler);
    static void RunServer();
};




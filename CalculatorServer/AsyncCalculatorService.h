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

	//[shouldFinish, shouldCreateNewInstance, nextState]
	typedef std::tuple<bool, bool, messageProcessingState> state_result_t;
	
	class IRequestHandlerBase
	{
	public:
		IRequestHandlerBase(IRequestHandlerBase &) = delete;
		IRequestHandlerBase& operator=(IRequestHandlerBase &) = delete;
		IRequestHandlerBase(IRequestHandlerBase &&) = delete;
		IRequestHandlerBase& operator=(IRequestHandlerBase &&) = delete;
		
		virtual ~IRequestHandlerBase() = default;
		IRequestHandlerBase() = default;
		[[nodiscard]] virtual std::shared_ptr<IRequestHandlerBase> CreateNew() const abstract;
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
			state_result_t result;
			
			switch (_state)
			{
				
			case messageProcessingState::CREATE:
				
				result = OnCreate();
				break;

			case messageProcessingState::PROCESS:
				result = OnProcess();
				break;
				
			case messageProcessingState::FINISH:
				result = OnFinish();
				break;
			}

			auto[shouldFinish, shouldCreateNewInstance, newState] = result;
			_state = newState;
			return std::make_tuple(shouldFinish, shouldCreateNewInstance);
		}

		virtual state_result_t OnCreate() abstract;
		virtual state_result_t OnProcess() abstract;
		virtual state_result_t OnFinish() { return std::make_tuple(false, false, messageProcessingState::FINISH); }
		~RequestHandlerBase() override = default;

	};
	
	class CalculateRequestHandler final : public RequestHandlerBase<calculator::OperationRequest, grpc::ServerAsyncResponseWriter<calculator::OperationResponse>>
	{
	public:
		using RequestHandlerBase<calculator::OperationRequest, grpc::ServerAsyncResponseWriter<calculator::OperationResponse>>::RequestHandlerBase;
	
	private:
		state_result_t OnCreate() override;
		state_result_t OnProcess() override;
		
		std::shared_ptr<IRequestHandlerBase> CreateNew() const override { return std::make_shared<CalculateRequestHandler>(GetService()); }
		
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
		std::once_flag initFlag;
		
		state_result_t OnCreate() override;
		state_result_t OnProcess() override;
		
		std::shared_ptr<IRequestHandlerBase> CreateNew() const override { return std::make_shared<CalculatorLoadHandler>(GetService()); }
	};
	
	std::map<void *, std::shared_ptr<IRequestHandlerBase>> _handlers{};
	std::mutex _handlersMutex{};
	std::unique_ptr<grpc::ServerCompletionQueue> _completionQueue;
	std::mutex _completionQueueMutex{};
	void SetCompletionQueue(std::unique_ptr<grpc::ServerCompletionQueue> unique) { _completionQueue = std::move(unique); }
	const std::unique_ptr<grpc::ServerCompletionQueue> &GetCompletionQueue() const { return _completionQueue; }

public:
	void HandleRequest(const std::shared_ptr<IRequestHandlerBase> &handler);
    static void RunServer();
};




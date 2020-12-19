using System;
using System.Collections.Generic;
using System.Reactive.Linq;
using System.Threading.Tasks;
using Grpc.Core;

namespace CalculatorLib
{
    public class CalculatorService
    {
        private static readonly Dictionary<OperationRequest.Types.Operators, OperationRequest.Types.Operators> OperatorTypeMapping = new Dictionary<OperationRequest.Types.Operators, OperationRequest.Types.Operators>
        {
            [OperationRequest.Types.Operators.Add] = OperationRequest.Types.Operators.Add,
            [OperationRequest.Types.Operators.Subtract] = OperationRequest.Types.Operators.Subtract,
            [OperationRequest.Types.Operators.Multiply] = OperationRequest.Types.Operators.Multiply,
            [OperationRequest.Types.Operators.Divide] = OperationRequest.Types.Operators.Divide
        };

        private readonly Calculator.CalculatorClient _client;

        public IObservable<double> OperationsPerMinute { get; }
        
        public CalculatorService()
        {
            Channel grpcChannel = new Channel("127.0.0.1:50051", ChannelCredentials.Insecure);
            _client = new Calculator.CalculatorClient(grpcChannel);

            OperationsPerMinute = ConnectToOperationsPerMinuteStream();
        }

        public async Task<CalculateResponse> CalculateAsync(OperationRequest operation)
        {
            try
            {
                var response = await _client.CalculateAsync(operation);
                return new CalculateResponse { Result = response.Result };
            }
            catch (RpcException exception)
            {
                return new CalculateResponse { Error = exception.Status.Detail };
            }
        }

        private IObservable<double> ConnectToOperationsPerMinuteStream()
        {
            IAsyncStreamReader<LoadResponse> asyncStreamReader = _client.ReadCalculatorLoad(new LoadIntervalRequest { IntervalInSeconds = 1 }).ResponseStream;

            return Observable.Create<double>(async observer =>
            {
                while (await asyncStreamReader.MoveNext())
                    observer.OnNext(asyncStreamReader.Current.InvocationsPerMinutes);

                observer.OnCompleted();
            });
        }
    }
}
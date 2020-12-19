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

        public CalculatorService()
        {
            Channel grpcChannel = new Channel("127.0.0.1:50051", ChannelCredentials.Insecure);
            _client = new Calculator.CalculatorClient(grpcChannel);

            OperationsPerMinute = ConnectToOperationsPerMinuteStream();
        }

        public IObservable<double> OperationsPerMinute { get; }
        public async Task<double> CalculateAsync(OperationRequest operation)
        {
            var response = await _client.CalculateAsync(operation);
            return response.Result;
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
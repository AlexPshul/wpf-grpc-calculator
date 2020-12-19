using System;
using System.Collections.Generic;
using System.Reactive.Linq;
using System.Threading.Tasks;
using CalculatorClient.Models;
using CalculatorLib;

namespace CalculatorClient.Services
{
    public class GrpcCalculatorService : ICalculatorService
    {
        private static readonly Dictionary<Operators, OperationRequest.Types.Operators> OperatorTypeMapping = new Dictionary<Operators, OperationRequest.Types.Operators>
        {
            [Operators.Add] = OperationRequest.Types.Operators.Add,
            [Operators.Subtract] = OperationRequest.Types.Operators.Subtract,
            [Operators.Multiply] = OperationRequest.Types.Operators.Multiply,
            [Operators.Divide] = OperationRequest.Types.Operators.Divide
        };

        private readonly CalculatorService _client = new CalculatorService();

        public IObservable<double> OperationsPerMinute => _client.OperationsPerMinute;

        public async Task<CalculateResponse> Calculate(MathOperation operation)
        {
            if (operation.Operator == null)
                throw new ArgumentNullException(nameof(operation.Operator));

            OperationRequest request = new OperationRequest
            {
                A = operation.LeftOperand,
                B = operation.RightOperand,
                Operator = OperatorTypeMapping[(Operators) operation.Operator]
            };

            return await _client.CalculateAsync(request);
        }
    }
}
using System;
using System.Reactive.Linq;
using System.Threading.Tasks;
using CalculatorClient.Models;
using CalculatorLib;

namespace CalculatorClient.Services
{
    public class LocalCalculatorService : ICalculatorService
    {
        private static readonly Random Random = new Random();

        public IObservable<double> OperationsPerMinute { get; } = Observable
            .Interval(TimeSpan.FromSeconds(1))
            .Select(_ => Random.NextDouble() * 10);
        
        public async Task<CalculateResponse> Calculate(MathOperation operation)
        {
            await Task.Yield();
            double result = 0d;
            switch (operation.Operator)
            {
                case Operators.Add:
                    result = operation.LeftOperand + operation.RightOperand;
                    break;
                case Operators.Subtract:
                    result = operation.LeftOperand - operation.RightOperand;
                    break;
                case Operators.Multiply:
                    result = operation.LeftOperand * operation.RightOperand;
                    break;
                case Operators.Divide:
                    if (operation.RightOperand.Equals(0d))
                        return new CalculateResponse { Error = "Divide by 0 error" };

                    result = operation.LeftOperand / operation.RightOperand;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            return new CalculateResponse { Result = result };
        }
    }
}
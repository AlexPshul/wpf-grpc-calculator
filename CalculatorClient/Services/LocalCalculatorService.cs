using System;
using System.Reactive.Linq;
using System.Threading.Tasks;
using CalculatorClient.Models;

namespace CalculatorClient.Services
{
    public class LocalCalculatorService : ICalculatorService
    {
        private static readonly Random Random = new Random();

        public IObservable<double> OperationsPerMinute { get; } = Observable
            .Interval(TimeSpan.FromSeconds(1))
            .Select(_ => Random.NextDouble() * 10);
        
        public async Task<double> Calculate(MathOperation operation)
        {
            await Task.Yield();
            switch (operation.Operator)
            {
                case Operators.Add:
                    return operation.LeftOperand + operation.RightOperand;
                case Operators.Subtract:
                    return operation.LeftOperand - operation.RightOperand;
                case Operators.Multiply:
                    return operation.LeftOperand * operation.RightOperand;
                case Operators.Divide:
                    return operation.LeftOperand / operation.RightOperand;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }
    }
}
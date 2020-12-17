using System;
using System.Threading.Tasks;
using CalculatorClient.Models;

namespace CalculatorClient.Services
{
    public interface ICalculatorService
    {
        IObservable<double> OperationsPerMinute { get; }
        Task<double> Calculate(MathOperation operation);
    }
}
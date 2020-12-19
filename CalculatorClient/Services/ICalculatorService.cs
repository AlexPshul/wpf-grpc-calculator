using System;
using System.Threading.Tasks;
using CalculatorClient.Models;
using CalculatorLib;

namespace CalculatorClient.Services
{
    public interface ICalculatorService
    {
        IObservable<double> OperationsPerMinute { get; }
        Task<CalculateResponse> Calculate(MathOperation operation);
    }
}
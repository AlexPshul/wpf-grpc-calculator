using CalculatorClient.Models;

namespace CalculatorClient.Services
{
    public interface ICalculatorService
    {
        double Calculate(MathOperation operation);
    }
}
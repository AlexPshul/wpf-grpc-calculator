using System;
using CalculatorClient.Models;

namespace CalculatorClient.Services
{
    public class LocalCalculatorService : ICalculatorService
    {
        public double Calculate(MathOperation operation)
        {
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
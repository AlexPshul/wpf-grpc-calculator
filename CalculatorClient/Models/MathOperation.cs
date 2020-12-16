namespace CalculatorClient.Models
{
    public class MathOperation
    {
        public double LeftOperand { get; set; }
        public double RightOperand { get; set; }
        public Operators? Operator { get; set; }
    }
}
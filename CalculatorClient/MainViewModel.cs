﻿using System;
using System.Collections.Generic;
using System.Globalization;
using System.Threading.Tasks;
using System.Windows.Input;
using CalculatorClient.Helpers;
using CalculatorClient.Models;
using CalculatorClient.Services;
using CalculatorLib;

namespace CalculatorClient
{
    public class MainViewModel : PropertyChangedBase
    {
        private static readonly ICalculatorService CalculatorService = new GrpcCalculatorService();
        private static readonly Dictionary<string, Operators> StringToOperatorsMap = new Dictionary<string, Operators>
        {
            ["+"] = Operators.Add,
            ["-"] = Operators.Subtract,
            ["*"] = Operators.Multiply,
            ["/"] = Operators.Divide
        };

        private MathOperation _currentMathOperation = new MathOperation();
        private bool _resetOnNextInput = false;

        private string _currentValue = "0";
        public string CurrentValue
        {
            get => _currentValue;
            set => Set(ref _currentValue, value);
        }

        private string _savedValue = "";
        public string SavedValue
        {
            get => _savedValue;
            set => Set(ref _savedValue, value);
        }

        private double _averageOperations;
        public double AverageOperations
        {
            get => _averageOperations;
            set => Set(ref _averageOperations, value);
        }


        public ICommand OperandManipulationCommand { get; }
        public ICommand OperatorCommand { get; }
        public ICommand ClearStateCommand { get; }

        public MainViewModel()
        {
            OperandManipulationCommand = new Command(param => AppendToCurrentInput(param as string));
            OperatorCommand = new Command(param => OperatorCall(param as string));
            ClearStateCommand = new Command(() => ClearState());

            CalculatorService.OperationsPerMinute.Subscribe(averageOperations => AverageOperations = averageOperations);
        }

        private void AppendToCurrentInput(string operandPart)
        {
            if (operandPart == null)
                return;

            if (_resetOnNextInput)
            {
                _resetOnNextInput = false;
                ClearState();
            }

            if (operandPart == "." && CurrentValue.Contains("."))
                return;

            if (operandPart == "0" && CurrentValue == "0")
                return;

            if (CurrentValue == "0" && operandPart != ".")
                CurrentValue = operandPart;
            else
                CurrentValue += operandPart;
        }

        private async Task OperatorCall(string pressedOperator)
        {
            switch (pressedOperator)
            {
                case "+":
                case "-":
                case "*":
                case "/":
                    _resetOnNextInput = false;
                    Operators currentOperator = StringToOperatorsMap[pressedOperator];
                    if (_currentMathOperation.Operator != null)
                    {
                        if (CurrentValue != "0")
                        {
                            _currentMathOperation.RightOperand = double.Parse(CurrentValue);
                            CalculateResponse middleResult = await CalculatorService.Calculate(_currentMathOperation);
                            if (!string.IsNullOrEmpty(middleResult.Error))
                            {
                                CurrentValue = middleResult.Error;
                                _resetOnNextInput = true;
                            }
                            else
                            {
                                _currentMathOperation.LeftOperand = middleResult.Result;
                                SavedValue = middleResult.Result.ToString(CultureInfo.InvariantCulture);
                            }
                        }
                    }
                    else
                    {
                        _currentMathOperation.LeftOperand = double.Parse(CurrentValue);
                    }

                    _currentMathOperation.Operator = currentOperator;
                    SavedValue = $"{_currentMathOperation.LeftOperand} {pressedOperator} ";
                    CurrentValue = "0";
                    break;
                case "=":
                    if (_currentMathOperation.Operator != null)
                    {
                        _currentMathOperation.RightOperand = double.Parse(CurrentValue);
                        CalculateResponse finalResult = await CalculatorService.Calculate(_currentMathOperation);
                        SavedValue += _currentMathOperation.RightOperand;
                        CurrentValue = !string.IsNullOrEmpty(finalResult.Error) ? finalResult.Error : finalResult.Result.ToString(CultureInfo.InvariantCulture);

                        _currentMathOperation = new MathOperation();
                        _resetOnNextInput = true;
                    }
                    break;
            }
        }

        private void ClearState()
        {
            CurrentValue = "0";
            SavedValue = "";
            _currentMathOperation = new MathOperation();
        }
    }
}
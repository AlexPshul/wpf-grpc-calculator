using System;
using System.Windows.Input;

namespace CalculatorClient.Helpers
{
    public class Command : ICommand
    {
        private readonly Action<object> _action;

        public Command(Action<object> action)
        {
            _action = action;
        }

        public Command(Action action)
        {
            _action = _ => action();
        }

        public bool CanExecute(object parameter) => true;

        public void Execute(object parameter) => _action(parameter);
        
        public event EventHandler CanExecuteChanged
        {
            add => CommandManager.RequerySuggested += value;
            remove => CommandManager.RequerySuggested -= value;
        }
    }
}
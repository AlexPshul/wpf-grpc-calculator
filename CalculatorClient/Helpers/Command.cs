using System;
using System.Threading.Tasks;
using System.Windows.Input;

namespace CalculatorClient.Helpers
{
    public class Command : PropertyChangedBase, ICommand
    {
        private readonly Action<object> _action;
        private readonly Func<object, Task> _asyncAction;

        private bool _isExecuting;
        public bool IsExecuting
        {
            get => _isExecuting;
            set
            {
                Set(ref _isExecuting, value);
                CommandManager.InvalidateRequerySuggested();
            }
        }

        public Command(Action<object> action) => _action = action;
        public Command(Action action) => _action = _ => action();
        public Command(Func<object, Task> asyncAction) => _asyncAction = asyncAction;
        public Command(Func<Task> asyncAction) => _asyncAction = _ => asyncAction();

        public bool CanExecute(object parameter) => !IsExecuting;

        public async void Execute(object parameter)
        {
            if (_action != null)
            {
                _action(parameter);
                return;
            }

            IsExecuting = true;
            await _asyncAction(parameter);
            IsExecuting = false;
        }

        public event EventHandler CanExecuteChanged
        {
            add => CommandManager.RequerySuggested += value;
            remove => CommandManager.RequerySuggested -= value;
        }
    }
}
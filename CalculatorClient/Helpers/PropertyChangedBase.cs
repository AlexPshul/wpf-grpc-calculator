using System.ComponentModel;
using System.Runtime.CompilerServices;
using CalculatorClient.Annotations;

namespace CalculatorClient.Helpers
{
    public class PropertyChangedBase : INotifyPropertyChanged
    {
        protected void Set<T>(ref T property, T value, [CallerMemberName] string propertyName = null)
        {
            if (property?.Equals(value) ?? value == null)
                return;

            property = value;
            OnPropertyChanged(propertyName);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
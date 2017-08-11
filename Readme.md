# Example of bindings for cppwinrt.

In this repo you can find creating binding from code and from xaml. Shown bindings with one segment in path (X, Person) and with two segments (Person.Age).

Code contains unnessesary parts. Once I have enough time I'm going to clear code and create completed `Property` class. If this would be possible I try to combine `ICustomPropertyProvider` and `ICustomProperty` in this class(which I believe is possible if I would make `Property` class as 'cppwinrt object').

After that my next step is going to be binding `ListView` `ItemsSource` to `IVector<CustomType>` with custom `DataTemplate`.

Any help would be appreciated.
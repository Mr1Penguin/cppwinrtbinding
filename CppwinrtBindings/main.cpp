#include "pch.h"
#include "property.h"

using namespace winrt;

using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Controls::Primitives;
using namespace winrt::Windows::UI::Xaml::Data;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Interop;
using namespace winrt::Windows::UI::Xaml::Markup;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;

struct Hash {
	size_t operator() (const event_token &token) const {
		return token.value;
	}
};

struct ViewModel : implements<ViewModel, ICustomPropertyProvider, INotifyPropertyChanged>
{
	ViewModel()
	{
		X.SetValue(L"Hello!");
	}

	void changeX()
	{
		X.SetValue(L"It works!");
		std::for_each(std::begin(handlers), std::end(handlers),
			[this](std::pair<event_token, PropertyChangedEventHandler> p) 
		{
			p.second(nullptr, PropertyChangedEventArgs(L"X"));
		});
	}

	ICustomProperty GetCustomProperty(hstring_view name)
	{
		if (name == L"X")
		{
			return X;
		}

		return nullptr;
	}

	ICustomProperty GetIndexedProperty(hstring_view name, TypeName type)
	{
		return nullptr;
	}

	hstring_view GetStringRepresentation()
	{
		return nullptr;
	}

	Windows::UI::Xaml::Interop::TypeName Type() {
		TypeName a;
		a.Kind = TypeKind::Custom;
		a.Name = L"ViewModel";
		return a;
	}

	void PropertyChanged(event_token token)
	{
		handlers.erase(token);
	}

	event_token PropertyChanged(PropertyChangedEventHandler handler)
	{
		event_token token;
		do {
			token.value = rand();

		} while (handlers.find(token) != handlers.end());
		handlers.insert({ token, handler });
		return token;
	}

	std::unordered_map<event_token,PropertyChangedEventHandler, Hash> handlers;

	Property<hstring> X { L"X", true, true };
};

struct App : ApplicationT<App>
{
	fire_and_forget OnLaunched(LaunchActivatedEventArgs const&)
	{
		auto const packageFolder = Package::Current().InstalledLocation();
		auto const file = co_await packageFolder.GetFileAsync(L"MainPage.Xaml");
		auto const xaml = co_await FileIO::ReadTextAsync(file);
		auto const uiElement = XamlReader::Load(xaml).as<Page>();

		auto btn = uiElement.FindName(L"Btn").as<ButtonBase>();
		btn.Tapped([this](auto && ...) { vm.changeX(); });
		auto tb = uiElement.FindName(L"TB").as<TextBlock>();
		event_token token;

		tb.Loaded([this, tb(tb)](auto && ...)
		{
			Binding a;
			a.Source(vm);
			a.Mode(BindingMode::OneWay);
			a.Path(PropertyPath(L"X"));
			tb.SetBinding(TextBlock::TextProperty(), a);
		});

		auto window = Window::Current();
		window.Content(uiElement);
		window.Activate();
	}

private:
	ViewModel vm;
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	Application::Start([](auto &&) { make<App>(); });
}

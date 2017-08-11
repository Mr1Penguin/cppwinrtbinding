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
		return std::hash<int64_t>{}(token.value);
	}
};

namespace wf = winrt::Windows::Foundation;

struct __declspec(uuid("3a12345f-a037-4ac0-a0ad-c4bb0bbf0112")) Person : IInspectable, implements<Person, ICustomPropertyProvider, ICustomProperty>
{
	Person(std::nullptr_t) noexcept {}

	Person()
	{
		NameProp.SetValue(L"Test");
		Age.SetValue(42);
		m_ptr = this;
	}

	Person(const Person &p)
	{
		NameProp.SetValue(p.NameProp.GetValue());
		Age.SetValue(p.Age.GetValue());
	}

	Person &operator =(const Person & other)
	{
		return *this;
	}

	ICustomProperty GetCustomProperty(hstring_view name)
	{
		if (name == L"Name")
			return NameProp;

		if (name == L"Age")
			return Age;

		return nullptr;
	}

	ICustomProperty GetIndexedProperty(hstring_view name, TypeName type)
	{
		return nullptr;
	}

	hstring_view GetStringRepresentation()
	{
		return L"Person";
	}

	Windows::UI::Xaml::Interop::TypeName Type() {
		TypeName a;
		a.Kind = TypeKind::Custom;
		a.Name = L"Person";
		return a;
	}

	bool CanRead()
	{
		return true;
	}

	bool CanWrite()
	{
		return true;
	}

	hstring_view Name()
	{
		return L"Person";
	}

	wf::IInspectable GetIndexedValue(wf::IInspectable a, wf::IInspectable b)
	{
		//not implemented
		return nullptr;
	}

	void SetIndexedValue(wf::IInspectable a, wf::IInspectable b, wf::IInspectable c)
	{
		//not implemented
	}

	wf::IInspectable GetValue(wf::IInspectable a) const
	{
		
		auto ret = this->as<wf::IInspectable>();
		return ret;
	}

	void SetValue(wf::IInspectable a, wf::IInspectable b)
	{
		*this = b.as<Person>();
	}

	Property<hstring> NameProp{ L"Name", true, true };
	Property<int> Age{ L"Age", true, true };
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
			return X;

		if (name == L"Person")
			return Person;

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
	Person Person;
};

struct App : ApplicationT<App>
{
	fire_and_forget OnLaunched(LaunchActivatedEventArgs const&)
	{
		auto const packageFolder = Package::Current().InstalledLocation();
		auto const file = co_await packageFolder.GetFileAsync(L"MainPage.Xaml");
		auto const xaml = co_await FileIO::ReadTextAsync(file);
		auto const uiElement = XamlReader::Load(xaml).as<Page>();
		uiElement.DataContext(vm);
		auto btn = uiElement.FindName(L"Btn").as<ButtonBase>();
		btn.Tapped([this](auto && ...) { vm.changeX(); });
		tb = uiElement.FindName(L"TB").as<TextBlock>();
		event_token token;

		tb.Loaded([this](auto && ...)
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
	TextBlock tb = nullptr;
	ViewModel vm;
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	Application::Start([](auto &&) { make<App>(); });
}

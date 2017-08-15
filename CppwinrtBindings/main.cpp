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
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage;

struct Hash {
	size_t operator() (const event_token &token) const {
		return std::hash<int64_t>{}(token.value);
	}
};

namespace wf = winrt::Windows::Foundation;

struct Person_impl : Property_impl
{
	Person_impl(hstring name) : Property_impl(name) {}

	ICustomProperty GetCustomProperty(hstring_view name) override
	{
		if (name == AgeProp.Name()) return AgeProp;
		if (name == NameProp.Name()) return NameProp;
		if (name == VisibleProp.Name()) return VisibleProp;
		return nullptr;
	}

	Property<bool> VisibleProp{ L"Vis" };
	Property<hstring> NameProp{ L"Name" };
	Property<int> AgeProp{ L"Age" };
};

struct __declspec(uuid("3a12345f-a037-4ac0-a0ad-c4bb0bbf0112")) Person : Property<Person, Person_impl>
{
	Person(std::nullptr_t) : Property(L"Person") {}

	Person &operator =(const Person & other)
	{
		wf::IUnknown::operator=(other);
		return *this;
	}

	Person() : Property(L"Person")
	{
		GPI()->NameProp.SetValue(hstring(L"Test"));
		GPI()->AgeProp.SetValue(42);
		SetValue(*this);
	}

	Person_impl* GPI() { return dynamic_cast<Person_impl*>(m_ptr); }
};

template<>
struct __declspec(uuid("1234567f-a037-4ac0-a0ad-c4bb0bbf0112")) winrt::ABI::Windows::Foundation::Collections::IVector<Person> : impl_IVector<Person> {};
template<>
struct __declspec(uuid("3234567f-a037-4ac0-a0ad-c4bb0bbf0112")) winrt::ABI::Windows::Foundation::Collections::IVectorView<Person> : impl_IVectorView<Person> {};
template<>
struct __declspec(uuid("4234567f-a037-4ac0-a0ad-c4bb0bbf0112")) winrt::ABI::Windows::Foundation::Collections::IIterable<Person> : impl_IIterable<Person> {};
template<>
struct __declspec(uuid("5234567f-a037-4ac0-a0ad-c4bb0bbf0112")) winrt::ABI::Windows::Foundation::Collections::IIterator<Person> : impl_IIterator<Person> {};

struct ViewModel : implements<ViewModel, ICustomPropertyProvider, INotifyPropertyChanged>
{
	ViewModel()
	{
		/*IVector<::Person> v = winrt::single_threaded_vector<::Person>();
		auto p = ::Person();
		::Person & pp = p;
		const ::Person & cpp = p;
		A(p);
		v.Append(::Person());
		v.Append(::Person());
		v.GetAt(1).GPI()->NameProp.SetValue(hstring(L"I DID IT!"));
		Persons.SetValue(v);*/
		X.SetValue(hstring(L"Hello!")); 
	}

	void A(const ::Person & p)
	{

	}

	void changeX()
	{
		X.SetValue(hstring(L"It works!"));
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
		return L"lol";
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

	Property<wf::Collections::IVector<Person>> Persons{ L"Persons" };
	Property<hstring> X { L"X" };
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

		A(tb);

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

	void A(const TextBlock & a)
	{

	}

private:
	TextBlock tb = nullptr;
	ViewModel vm;
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	Application::Start([](auto &&) { make<App>(); });
}

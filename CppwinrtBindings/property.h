#pragma once

#include "pch.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Data;

namespace winrt::ABI::Windows::Foundation
{
	template <> struct __declspec(uuid("3a14233f-a037-4ac0-a0ad-c4bb0bbf0112")) __declspec(novtable) IReference<HSTRING__ *> : impl_IReference<HSTRING__ *> {};
}

template<typename T>
struct Property : implements<Property<T>, ICustomProperty>
{
	Property(hstring_view name, bool canRead = false, bool canWrite = false) :  _name(name), _canRead(canRead), _canWrite(canWrite)
	{
		_typename = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(typeid(T).name());
	}

	bool CanRead()
	{
		return _canRead;
	}

	bool CanWrite()
	{
		return _canWrite;
	}

	hstring_view Name()
	{
		return _name;
	}

	TypeName Type() {
		TypeName a;
		a.Kind = TypeKind::Custom;
		a.Name = _typename;
		return a;
	}

	IInspectable GetIndexedValue(IInspectable a, IInspectable b)
	{
		//not implemented
		return nullptr;
	}

	void SetIndexedValue(IInspectable a, IInspectable b, IInspectable c)
	{
		//not implemented
	}

	template <typename Q = T>
	IInspectable GetValue(IInspectable a)
	{
		return _value;
	}

	template <typename Q = T>
	void SetValue(IInspectable a, IInspectable b)
	{
		_value = b;
	}

	template <>
	IInspectable GetValue<hstring>(IInspectable a)
	{
		return PropertyValue::CreateString(_value);
	}

	template <>
	IInspectable GetValue<int>(IInspectable a)
	{
		return PropertyValue::CreateInt32(_value);
	}

	template <>
	void SetValue<hstring>(IInspectable a, IInspectable b)
	{
		_value = b.as<IReference<hstring>>().Value();
	}

	template <>
	void SetValue<int>(IInspectable a, IInspectable b)
	{
		_value = b.as<IReference<int>>().Value();
	}

	void SetValue(T value)
	{
		_value = value;
	}

private:
	T _value;
	bool _canRead;
	bool _canWrite;
	hstring_view _name = L"";
	hstring_view _typename = L"";
};
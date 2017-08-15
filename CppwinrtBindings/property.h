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

namespace wf = winrt::Windows::Foundation;

struct PropertyBase : IInspectable, implements<PropertyBase, ICustomPropertyProvider, ICustomProperty>
{
	PropertyBase() {}

	PropertyBase(hstring name) : _name(name)
	{
		m_ptr = this;
	}

	PropertyBase(const PropertyBase & p)
	{
		_name = p._name;
		_canRead = p._canRead;
		_canWrite = p._canWrite;
		_typename = p._typename;
		_value = p._value;
	}

	PropertyBase &operator =(const PropertyBase & other)
	{
		return *this;
	}

	hstring_view Name()
	{
		return _name;
	}

	virtual ICustomProperty GetCustomProperty(hstring_view name)
	{
		return nullptr;
	}

	bool CanRead() { return _canRead; }
	bool CanWrite() { return _canWrite; }

	TypeName Type() {
		TypeName a;
		a.Kind = TypeKind::Custom;
		a.Name = _typename;
		return a;
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

	ICustomProperty GetIndexedProperty(hstring_view name, TypeName type)
	{
		return nullptr;
	}

	hstring_view GetStringRepresentation()
	{
		return _typename;
	}

	virtual wf::IInspectable GetValue(wf::IInspectable a)
	{
		return _value;
	}

	void SetValue(wf::IInspectable a, wf::IInspectable b)
	{
		_value = b;
	}

protected:

	bool _canRead; 
	bool _canWrite;
	hstring _name;
	hstring _typename;
	wf::IInspectable _value;
};

template<typename T, bool canRead = true, bool canWrite = false>
struct Property : PropertyBase
{
	Property(hstring name) : PropertyBase(name)
	{
		auto tmp = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(typeid(T).name());
		std::replace(std::begin(tmp), std::end(tmp), ' ', '_');
		_typename = tmp;
		if (!std::is_base_of<wf::IInspectable, T>::value)
			SetValue(T());
	}

	template <typename Q = T>
	typename std::enable_if<std::is_base_of<wf::IInspectable, Q>::value, wf::IInspectable>::type
	GetValue() const
	{
		return _value;
	}

	template <typename Q = T>
	typename std::enable_if<!std::is_base_of<wf::IInspectable, Q>::value, Q>::type
	GetValue() const
	{
		return _value.as<IReference<Q>>().Value();
	}

	template <typename Q = T>
	typename std::enable_if<std::is_base_of<wf::IInspectable, Q>::value>::type
	SetValue(wf::IInspectable arg)
	{
		_value = arg;
	}

	template <typename Q = T>
	typename std::enable_if<std::is_same<int32_t, Q>::value>::type
	SetValue(Q arg)
	{
		_value = PropertyValue::CreateInt32(arg);
	}

	template <typename Q = T>
	typename std::enable_if<std::is_same<hstring, Q>::value>::type
		SetValue(Q arg)
	{
		_value = PropertyValue::CreateString(arg);
	}

	template <typename Q = T>
	typename std::enable_if<std::is_same<bool, Q>::value>::type
		SetValue(Q arg)
	{
		_value = PropertyValue::CreateBoolean(arg);
	}
};
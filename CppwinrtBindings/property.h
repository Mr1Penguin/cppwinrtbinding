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

struct Property_impl : implements<Property_impl, ICustomPropertyProvider, ICustomProperty>
{
	Property_impl(hstring name) : _name(name)
	{

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

	void Typename(const hstring & tname)
	{
		_typename = tname;
	}

private:
	bool _canRead;
	bool _canWrite;
	hstring _name;
	hstring _typename;
	wf::IInspectable _value;
};

struct PropertyBase : IInspectable, implements<PropertyBase, ICustomPropertyProvider, ICustomProperty>
{
	void * operator new(size_t) = delete;
	PropertyBase(std::nullptr_t) noexcept {}

	PropertyBase() noexcept {}

	PropertyBase(const PropertyBase & p) : wf::IInspectable(p)
	{
	}

	PropertyBase(PropertyBase && p) : wf::IInspectable(p)
	{
	}

	PropertyBase &operator =(const PropertyBase & other)
	{
		wf::IInspectable::operator=(other);
		return *this;
	}

	/*PropertyBase &operator =(PropertyBase && other)
	{
		wf::IInspectable::operator=(other);
		return *this;
	}

	PropertyBase &operator =(std::nullptr_t) noexcept
	{
		wf::IInspectable:: operator=(nullptr);
		return *this;
	}*/

	hstring_view Name()
	{
		return dynamic_cast<Property_impl*>(m_ptr)->Name();
	}

	virtual ICustomProperty GetCustomProperty(hstring_view name)
	{
		return nullptr;
	}

	bool CanRead() { return dynamic_cast<Property_impl*>(m_ptr)->CanRead(); }
	bool CanWrite() { return dynamic_cast<Property_impl*>(m_ptr)->CanWrite(); }

	TypeName Type() {
		return dynamic_cast<Property_impl*>(m_ptr)->Type();
	}

	wf::IInspectable GetIndexedValue(wf::IInspectable a, wf::IInspectable b)
	{
		return dynamic_cast<Property_impl*>(m_ptr)->GetIndexedValue(a, b);
	}

	void SetIndexedValue(wf::IInspectable a, wf::IInspectable b, wf::IInspectable c)
	{
		dynamic_cast<Property_impl*>(m_ptr)->SetIndexedValue(a, b, c);
	}

	ICustomProperty GetIndexedProperty(hstring_view name, TypeName type)
	{
		return dynamic_cast<Property_impl*>(m_ptr)->GetIndexedProperty(name, type);
	}

	hstring_view GetStringRepresentation()
	{
		return dynamic_cast<Property_impl*>(m_ptr)->GetStringRepresentation();
	}

	virtual wf::IInspectable GetValue(wf::IInspectable a)
	{
		return dynamic_cast<Property_impl*>(m_ptr)->GetValue(a);
	}

	void SetValue(wf::IInspectable a, wf::IInspectable b)
	{
		dynamic_cast<Property_impl*>(m_ptr)->SetValue(a, b);
	}
};

template<typename T, typename T_impl = Property_impl, bool canRead = true, bool canWrite = false>
struct Property : PropertyBase
{
	Property(hstring name)
	{
		m_ptr = new T_impl(name);
		auto tmp = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(typeid(T).name());
		std::replace(std::begin(tmp), std::end(tmp), ' ', '_');
		dynamic_cast<Property_impl*>(m_ptr)->Typename(tmp);
		if (!std::is_base_of<wf::IInspectable, T>::value)
			SetValue(T());
	}

	template <typename Q = T>
	typename std::enable_if<std::is_base_of<wf::IInspectable, Q>::value, wf::IInspectable>::type
	GetValue() const
	{
		return dynamic_cast<Property_impl*>(m_ptr)->GetValue(nullptr);
	}

	template <typename Q = T>
	typename std::enable_if<!std::is_base_of<wf::IInspectable, Q>::value, Q>::type
	GetValue() const
	{
		return dynamic_cast<Property_impl*>(m_ptr)->GetValue(nullptr).as<IReference<Q>>().Value();
	}

	template <typename Q = T>
	typename std::enable_if<std::is_base_of<wf::IInspectable, Q>::value>::type
	SetValue(wf::IInspectable arg)
	{
		dynamic_cast<Property_impl*>(m_ptr)->SetValue(nullptr, arg);
	}

	template <typename Q = T>
	typename std::enable_if<std::is_same<int32_t, Q>::value>::type
	SetValue(Q arg)
	{
		dynamic_cast<Property_impl*>(m_ptr)->SetValue(nullptr, PropertyValue::CreateInt32(arg));
	}

	template <typename Q = T>
	typename std::enable_if<std::is_same<hstring, Q>::value>::type
		SetValue(Q arg)
	{
		dynamic_cast<Property_impl*>(m_ptr)->SetValue(nullptr, PropertyValue::CreateString(arg));
	}

	template <typename Q = T>
	typename std::enable_if<std::is_same<bool, Q>::value>::type
		SetValue(Q arg)
	{
		dynamic_cast<Property_impl*>(m_ptr)->SetValue(nullptr, PropertyValue::CreateBoolean(arg));
	}
};
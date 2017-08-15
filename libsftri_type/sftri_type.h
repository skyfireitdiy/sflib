#pragma once
template<typename T>
class sftri_type
{
public:
	sftri_type();
	template<typename ... Args>
	explicit sftri_type(Args...a);
	sftri_type& operator=(const T &other);
	T& operator*();
	// ReSharper disable CppNonExplicitConversionOperator
	operator bool() const;
private:
	T data__;
	bool init__{ false };
};

template <typename T>
sftri_type<T>::sftri_type()
{
}

template <typename T>
template <typename ... Args>
sftri_type<T>::sftri_type(Args... a):data__(a...)
{
}

template <typename T>
sftri_type<T>& sftri_type<T>::operator=(const T& other)
{
	if (&other == &data__)
		return *this;
	init__ = true;
	data__ = other;
	return *this;
}

template <typename T>
T& sftri_type<T>::operator*()
{
	return data__;
}

template <typename T>
sftri_type<T>::operator bool() const
{
	return init__;
}

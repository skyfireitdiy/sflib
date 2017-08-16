/// @file	sf_tri_type.h
///
/// @brief	Declares the sf triangle type class.

#pragma once

namespace skyfire
{
	/// @class	sf_tri_type
	///
	/// @brief	A sf triangle type.
	///
	/// @author	SkyFire
	/// @date	2017/8/16
	///
	/// @tparam	T	Generic type parameter.

	template<typename T>
	class sf_tri_type
	{
	public:

		/// @fn	sf_tri_type::sf_tri_type();
		///
		/// @brief	Default constructor.
		///
		/// @author	SkyFire
		/// @date	2017/8/16

		sf_tri_type();

		template<typename ... Args>

		/// @fn	explicit sf_tri_type::sf_tri_type(Args...a);
		///
		/// @brief	Constructor.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param	parameter1	The parameters to construct a T.

		explicit sf_tri_type(Args...a);

		/// @fn	sf_tri_type& sf_tri_type::operator=(const T &other);
		///
		/// @brief	Assignment operator.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param	other	The other.
		///
		/// @return	A shallow copy of this object.

		sf_tri_type& operator=(const T &other);

		/// @fn	T& sf_tri_type::operator*();
		///
		/// @brief	Indirection operator.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @return	The result of the operation.

		T& operator*();

		/// @fn	operator sf_tri_type::bool() const;
		///
		/// @brief	Cast that converts the given  to a bool.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @return	The result of the operation.

		// ReSharper disable CppNonExplicitConversionOperator
		operator bool() const;
	private:
		T data__;
		bool init__{ false };
	};



	template <typename T>
	sf_tri_type<T>::sf_tri_type()
	{}

	template <typename T>
	template <typename ... Args>
	sf_tri_type<T>::sf_tri_type(Args... a) :data__(a...)
	{}

	template <typename T>
	sf_tri_type<T>& sf_tri_type<T>::operator=(const T& other)
	{
		if (&other == &data__)
			return *this;
		init__ = true;
		data__ = other;
		return *this;
	}

	template <typename T>
	T& sf_tri_type<T>::operator*()
	{
		return data__;
	}

	template <typename T>
	sf_tri_type<T>::operator bool() const
	{
		return init__;
	}

}
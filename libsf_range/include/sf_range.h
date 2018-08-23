/*
 * sf_range range产生序列的功能
 */

#pragma once

#include <stdexcept>

namespace skyfire
{
	template<typename T>
	class sf_range_iteator__
	{
	public:
		using value_type = T;
		using size_type = size_t;
	private:
		size_type cursor__;
		const value_type step__;
		value_type value__;
	public:
		sf_range_iteator__(size_type start,const value_type& value,const value_type& step);

		value_type operator*()const;

		bool operator!=(const sf_range_iteator__<T>& other) const;

		sf_range_iteator__<T> & operator ++();
	};


	template<typename T>
	class sf_range_impl__
	{
	public:
		using value_type = T;
		using reference = const T &;
		using const_reference = const T &;
		using iterator = const sf_range_iteator__<T>;
		using const_iterator = const sf_range_iteator__<T>;
		using size_type = typename sf_range_iteator__<T>::size_type;

	private:
		const value_type begin__;
		const value_type end__;
		const value_type step__;
		const size_type max_count__;

		size_type get_max_count__() const;

	public:
		sf_range_impl__(const value_type vbegin, const value_type vend, const value_type vstep);

		size_type size()const;

		const_iterator begin()const;

		const_iterator end()const;
	};

	template<typename T>
	sf_range_impl__<T> sf_range(const T &end);

	template<typename T>
	sf_range_impl__<T> sf_range(const T &begin, const T &end);

	template<typename T>
	auto sf_range(const T &begin, const T &end, const T &step);

}

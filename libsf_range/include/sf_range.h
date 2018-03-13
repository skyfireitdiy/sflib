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
		sf_range_iteator__(size_type start,const value_type& value,const value_type& step):
		cursor__(start),value__(value),step__(step)
		{
			value__ += (step__*cursor__);
		}

		value_type operator*()const
		{
			return value__;
		}

		bool operator!=(const sf_range_iteator__<T>& other) const
		{
			return cursor__ != other.cursor__;
		}

		sf_range_iteator__<T> & operator ++()
		{
			value__ += step__;
			++cursor__;
			return *this;
		}
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

		size_type get_max_count__() const
		{
			if(step__>0&&begin__>=end__)
			{
				throw std::logic_error("End value must be greater than begin value");
			}
			if(step__<0&&begin__<=end__)
			{
				throw std::logic_error("End value must be less than begin value");
			}
			if(step__==0)
			{
				throw std::logic_error("Step can't be zero");
			}
			auto x = static_cast<size_type>((end__ - begin__) / step__);
			if(begin__+(x*step__)!=end__)
			{
				++x;
			}
			return x;
		}

	public:
		sf_range_impl__(const value_type vbegin, const value_type vend, const value_type vstep):
		begin__(vbegin),end__(vend),step__(vstep),max_count__(get_max_count__())
		{
			
		}

		size_type size()const
		{
			return max_count__;
		}

		const_iterator begin()const
		{
			return { 0, begin__, step__};
		}

		const_iterator end()const
		{
			return { max_count__,begin__,step__ };
		}
	};

	template<typename T>
	sf_range_impl__<T> sf_range(const T &end)
	{
		return { {},end,1 };
	}

	template<typename T>
	sf_range_impl__<T> sf_range(const T &begin, const T &end)
	{
		return { begin,end,1 };
	}

	template<typename T>
	auto sf_range(const T &begin, const T &end, const T &step)
	{
		return sf_range_impl__<decltype(begin + step)>{begin, end, step};
	}


}

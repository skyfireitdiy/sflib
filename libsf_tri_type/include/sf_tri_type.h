#pragma once

namespace skyfire
{
	template<typename T>
	class sf_tri_type
	{
	public:
		sf_tri_type(){}

		template<typename ... Args>
		explicit sf_tri_type(Args...a):data__(a...)
		{

		}
		sf_tri_type& operator=(const T &other)
		{
			if (&other == &data__)
				return *this;
			init__ = true;
			data__ = other;
			return *this;
		}

		T& operator*()
		{
			return data__;
		}

		operator bool() const
		{
			return init__;
		}
	private:
		T data__;
		bool init__{ false };
	};

	template <>
	class sf_tri_type<void>
	{
	public:
		sf_tri_type(bool init = false): init__(init)
		{}
		operator bool() const
		{
			return true;
		}

	private:
		bool init__{false};
	};

}
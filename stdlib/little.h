#pragma once
#include <tuple>
namespace lsd
{
	template <typename First,
		typename Second>
		class pair
	{
	public:
		pair(){}
		pair(const First& v1,const Second& v2)
		:_first(v1),_second(v2){}
		pair(First&& v1,Second&& v2)
			:_first(std::forward<First>(v1)),
			_second(std::forward<Second>(v2)){}
		bool operator > (const pair& p) const
		{
			return _first > p._first;
		}

		bool operator < (const pair& p) const
		{
			return _first < p._first;
		}

		bool operator == (const pair& p) const
		{
			return p._first == _first;
		}

		bool operator != (const pair& p) const
		{
			return !(*this == p);
		}

		template <typename U,typename T>
		pair<First, Second>& operator = (const pair<U, T>& p)
		{
			_first = p._first;
			_second = p._second;
			return *this;
		}
		First _first;
		Second _second;
	};
}
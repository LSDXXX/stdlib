#pragma once
namespace lsd
{
	template <typename T>
	class less
	{
	public:
		bool operator()(const T&v1,
			const T& v2) const
		{
			return (v1 < v2);
		}
	};

	template <typename T>
	class greater
	{
	public:
		bool operator()(const T& v1,
			const T& v2) const
		{
			return (v1 > v2);
		}
	};


}
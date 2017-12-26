#pragma once
namespace lsd
{
	template <typename T>
	class container_type
	{
	public:
		typedef  T value_type;
		typedef  T& reference_type;
		typedef  T* pointer_type;
		typedef  const T* const_pointer_type;
		typedef  const T const_type;
		typedef  const T& const_reference_type;
		typedef int size_type;
		typedef		T&&		rvalue_type;
	};
}
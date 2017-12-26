#pragma once
#include "container_base.h"
#include "iterator.h"
namespace lsd 
{
	template <typename Ty>
	class deque
		:container_type<Ty>
	{
	public:
		typedef  typename container_type<T>::value_type value_type;
		typedef  typename container_type<T>::reference_type reference_type;
		typedef  typename container_type<T>::pointer_type pointer_type;
		typedef  typename container_type<T>::const_pointer_type const_pointer_type;
		typedef  typename container_type<T>::const_type const_type;
		typedef  typename container_type<T>::const_reference_type const_reference_type;
		typedef  typename container_type<T>::size_type size_type;

		typedef IteratorCategory::random_access_iterator_tag iterator_category;
		typedef iterator_vec<vector<value_type>> iterator_type;
		typedef const_iterator_vec<vector<value_type>> const_iterator_type;

	private:

	};
}
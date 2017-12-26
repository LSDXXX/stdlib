#pragma once
#include <initializer_list>
#include "allocator.h"
#include "iterator.h"
#include "_type.h"
#include "locks.h"
#include "container_base.h"
#ifdef MutiThread
#define LOCK Lock lock
#define UNLOCK lock.unlock()
#else
#define LOCK
#endif
namespace lsd
{
	template <typename T
	,typename alloc = allocator<T>>
	class vector
		:container_type<T>
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
	public:
		vector():
			vec(nullptr),
			_end(nullptr),
			_size(0),
			_alloc_size(0){}

		vector(const vector& v)
		{
			*this = v;
		}

		vector(vector&& v)
		{
			*this = (std::forward<vector>(v));
		}

		explicit vector(size_type n)
			: _size(0),
			vec(nullptr),
			_end(nullptr),
			_alloc_size(0)
		{
			realloc(n);
		}

		vector(const std::initializer_list<value_type> l)
			:vec(nullptr),
			_end(nullptr),
			_size(0),
			_alloc_size(0)
		{
			assign(l.begin(), l.end());
		}
		
		template <typename Iter>
		vector(Iter first, Iter last)
			:vec(nullptr),
			_end(nullptr),
			_size(0),
			_alloc_size(0)
		{
			assign(first, last);
		}

		void assign(size_type n, const_reference_type v)
		{
			if (_alloc_size < n)
			{
				realloc_nocopy(n);
			}
			iterator_type it = begin();
			for (size_type i = 0;i < n;++i,it++)
			{
				_alloc.construct(it._Ptr, v);
			}
			if (_size < n)
			{
				_size = n;
				_end = vec + n;
			}
		}

		template <typename Iter>
		void assign(Iter first,
			Iter last)
		{
			size_type n = last - first;
			if (n < 0)
				return;

			vector* v = nullptr;
			if (inside(first) || inside(last - 1))
			{
				v = new vector(first, last);
				realloc(n > _size? n : _size);
				_assign(begin(), v->cbegin(), v->cend());
				delete v;
			}
			else
			{
				if(n > _size)
					realloc_nocopy(n);
				_assign(begin(), first, last);
			}
			_size += n;
			_end = vec + n;
		}



		void insert(const_iterator_type _where, const_reference_type v)
		{
			if (_where == _end)
				push_back(v);
			else
			{
				const_iterator_type cb = cbegin();
				size_type pos = _where - cbegin();
				assert(inside(_where));
				size_type nalloc = _size + 1;
				vector temp(nalloc);
				temp.resize(nalloc);
				temp._assign(temp.begin(),cbegin(), _where);
				temp._assign(temp.begin() + pos + 1,_where, cend());
				_alloc.construct(temp.data() + pos, v);
				*this = std::move(temp);
			}
		}

		void reserve(size_type n)
		{
			if (_alloc_size >= n)
				return;
			realloc(n);
		}

		void resize(size_type n)
		{
			if (_alloc_size < n)
			{
				realloc(n);
			}
			_size = n;
			_end = vec + n;
		}

		vector<value_type>& operator =(const vector& v)
		{
			if (*this == v)
				return *this;
			size_type n = v.size();
			realloc_nocopy(n);
			_size = n;
			if (is_trival_type<value_type>::result)
			{
				n *= sizeof(value_type);
				memcpy(vec, v.data(), n);
			}
			else
			{
				_end = vec;
				for (size_type i = 0;i < n;++i)
					_alloc.construct(_end++, v[i]);
			}
			return *this;
		}
		
		vector<value_type>& operator=(vector&& v)
		{
			if (*this == v)
				return *this;
			clean(vec,_alloc_size);
			_size = v._size;
			_alloc_size = v._alloc_size;
			vec = v.vec;
			_end = v._end;
			v._end = nullptr;
			v.vec = nullptr;
			v._alloc_size = 0;
			v._size = 0;
			return *this;
		}

		const_iterator_type cbegin() const
		{
			return const_iterator_type(vec);
		}

		reverse_iterator_vec<vector<value_type>> rbegin()
		{
			return reverse_iterator_vec<vector<value_type>>(_end - 1);
		}

		reverse_iterator_vec<vector<value_type>> rend()
		{
			return reverse_iterator_vec<vector<value_type>>(vec - 1);
		}

		const_reverse_iterator_vec<vector<value_type>> rcend() const
		{
			return const_reverse_iterator_vec<vector<value_type>>(_end - 1);
		}

		const_reverse_iterator_vec<vector<value_type>> rcbegin() const
		{
			return const_reverse_iterator_vec<vector<value_type>>(vec - 1);
		}

		const_iterator_type cend() const
		{
			return const_iterator_type(_end);
		}

		iterator_type begin()
		{
			return iterator_type(vec);
		}

		iterator_type end()
		{
			return iterator_type(_end);
		}

		template <typename... ARGS>
		void emplace_back(const ARGS&... args)
		{
			if (_size >= _alloc_size)
			{
				_alloc_size = (_alloc_size + 1) * 2;
				realloc(_alloc_size);
			}
			_alloc.construct(_end, args...);
			_end++;
			_size++;
		}

		void push_back(const value_type& t)
 		{
			if (_size >= _alloc_size)
			{
				size_type req = (_alloc_size + 1) * 2;
				realloc(req);
			}
			*_end++ = (t);
			_size++;
		}

		void push_back(value_type&& t)
		{
			if (_size >= _alloc_size)
			{
				size_type req = (_alloc_size + 1) * 2;
				realloc(req);
			}
			*_end++ = std::forward<value_type>(t);
			_size++;
		}

		void pop_back()
		{
			if (!_size)
				return;
			_size--;
			_end--;
		}

		value_type back() const
		{
			assert(_size);
			return *(_end - 1);
		}

		void clear()
		{
			_alloc.destroy(vec, _size);
			_size = 0;
			_end = vec;
		}

		reference_type operator[](size_type k)
		{
			assert(_size > k);
			return *(vec + k);
		}

		const_reference_type operator[](size_type k) const
		{
			assert(_size > k);
			return *(vec + k);
		}

		bool operator == (const vector& v) const
		{
			return (vec == v.vec&&_end == v._end);
		}

		~vector() 
		{ 
			clean(vec,_alloc_size);
			vec = nullptr;
			_end = nullptr;
			_alloc_size = 0;
			_size = 0;
		}

		size_type size() const { return _size; };
		pointer_type data() const { return vec; }
	private:
		alloc _alloc;
		pointer_type vec;
		pointer_type _end;
		size_type _size;
		size_type _alloc_size;

		void realloc_nocopy(size_type n)
		{
			pointer_type v = alloc_n(n);
			clean(vec,_alloc_size);
			_alloc_size = n;
			vec = v;
			_end = vec + _size;
		}

		void realloc(size_type n)
		{
			pointer_type v = alloc_n(n);
			if (is_trival_type<value_type>::result)
			{
				memcpy(v, vec, _size * sizeof(value_type));
				_end = v + _size;
			}
			else
			{
				_end = v;
				for (size_type i = 0;i < _size;++i)
					_alloc.construct(_end++, vec[i]);
				for (size_type i = _size;i < n;++i)
					_alloc.construct(v + i);
			}
			clean(vec,_alloc_size);
			vec = v;
			_alloc_size = n;
		}

		pointer_type alloc_n(size_type n)
		{

			return (pointer_type)_alloc.allocate(n);
		}

		void clean(T* p,size_type n)
		{
			if (!is_trival_type<value_type>::result)
			{
				_alloc.destroy(p, _size);
			}
			_alloc.deallocate(p, n);
		}

		template <typename iter>
		bool inside(iter it) const
		{
			return 
				((&(*it) - &(*cbegin()) >= 0) &&
				(&(*it) - &(*cbegin())) < _size) &&(_size > 0);
		}

		template <typename srcIter,
			typename dstIter>
			void _assign(dstIter dst, srcIter fsrc, srcIter lsrc)
		{
			if (is_trival_type<value_type>::result)
			{
				assert(lsrc - fsrc >= 0);
				memcpy(&(*dst), &(*fsrc), (lsrc - fsrc) * sizeof(value_type));
			}
			else
			{
				auto pdst = dst._Ptr;
				for (auto it = fsrc;it != lsrc; ++it)
				{
					_alloc.construct(pdst++, *it);
				}
			}
		}

	};
}
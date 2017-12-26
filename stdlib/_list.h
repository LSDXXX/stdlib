#pragma once
#include "allocator.h"
#include "container_base.h"
#include "iterator.h"
#include "list_node.h"

namespace lsd
{

	template <typename Ty,
	typename alloc = allocator<list_node<Ty>>>
	class list
		:container_type<Ty>
	{
	public:
		typedef  typename container_type<Ty>::value_type value_type;
		typedef  typename container_type<Ty>::reference_type reference_type;
		typedef  typename container_type<Ty>::pointer_type pointer_type;
		typedef  typename container_type<Ty>::const_pointer_type const_pointer_type;
		typedef  typename container_type<Ty>::const_type const_type;
		typedef  typename container_type<Ty>::const_reference_type const_reference_type;
		typedef  typename container_type<Ty>::size_type size_type;


		typedef  list_node<Ty>  node_type;
		typedef typename IteratorCategory::bidirection_access_iterator_tag iterator_category;
		typedef iterator_list<list<Ty>> iterator_type;
		typedef const_iterator_list<list<Ty>> const_iterator_type;

		list()
			:
			_head(nullptr),
			_tail(nullptr),
			_size(0){
			_make_guard();
		}

		list(const list& l)
			:_tail(nullptr),
			_head(nullptr),
			_size(0)
		{
			_make_guard();
			assign(l.cbegin(), l.cend());
		}

		list(list&& l)
		{
			*this = std::forward<list>(l);
		}

		list<Ty>& operator = (const list& l)
		{
			if (*this == l)
				return *this;
			iterator_type beg = begin();
			iterator_type e = end();
			_size = 0;
			_alloc.destroy(_tail, 1);
			_alloc.deallocate(_tail, 1);
			_make_guard();
			for (auto it = l.cbegin();it != l.cend();++it)
				push_back(*it);
			_clean(beg, e);
			_size = l.size();
			return *this;
		}

		list& operator=(list&& right)
		{
			_size = right._size;
			_head = right._head;
			_tail = right._tail;
			right._size = 0;
			right._head = nullptr;
			right._tail = nullptr;
			return *this;
		}

		bool operator == (const list& l)
		{
			return (_head == l._head
				&& _tail == l._tail
				&& _size == l._size);
		}

		bool operator!=(const list& l)
		{
			return !(*this == l);
		}

		explicit list(size_type n)
			:
			_head(nullptr),
			_tail(nullptr),
			_size(0)
		{
			_make_guard();
			node_type* node = _new_node();
			_insert_back(node);
			_head = node;
			for (size_type i = 1;i < n;++i)
			{
				node = _new_node();
				_insert_back(node);
			}
		}

		template<typename ForwordIter>
		list(ForwordIter first, ForwordIter last):
			_head(nullptr),
			_tail(nullptr),
			_size(0)
		{
			_make_guard;
			auto it = first;
			node_type* node = _new_node(*it++);
			_insert_back(node);
			_head = node;
			for (;it != last;it++)
			{
				node = _new_node(*it);
				_insert_back(node);
			}
		}

		~list()
		{
			_clean(begin(), end());
			_alloc.destroy(_tail, 1);
			_alloc.deallocate(_tail, 1);
			_head = nullptr;
			_tail = nullptr;
		}

		template<typename ForwordIter>
		void assign(ForwordIter first, ForwordIter last)
		{
			_assign(begin(), first, last);
		}

		void assign(size_type n, const_reference_type v)
		{
			size_type k = 0;
			for (iterator_type i = begin();k < n;++k)
			{
				if (k < _size)
				{
					*i = v;
					++i;
				}
				else
				{
					push_back(v);
				}
			}
		}

		template<typename srcForwordIter>
		void _assign(iterator_type dst,
			srcForwordIter first, srcForwordIter last)
		{
			if (first == last)
				return;
			auto it = first;
			auto d = dst;
			for (;it != last;++it,d++)
			{
				if (d != iterator_type(_tail))
				{
					*d = *it;
				}
				else
				{
					break;
				}
			}
			for (;it != last;++it)
			{
				node_type* node = _new_node(*it);
				_insert_back(node);
			}
		}

		iterator_type insert(iterator_type _where, const_reference_type v)
		{
			assert(distant(_where, _head) != -1
				&& distant(_where, head) <= _size);
			node_type *node = _new_node(v);
			if (_where._node->_prev == nullptr)
				_head = node;
			node->insert(_where._node->_prev, _where._node);
			_size++;
			return iterator_type(node);
		}

		void push_back(const_reference_type v)
		{
			node_type* node = _new_node(v);
			if (_head == _tail)
				_head = node;
			_insert_back(node);
		}

		void resize(size_type n)
		{
			if (n > _size)
			{
				for ( ; _size < n;)
				{
					node_type* node = _alloc.allocate(1);
					_insert_back(node);
				}
			}
			else
			{
				for (;_size > n;)
					pop_back();
			}
		}

		void clear()
		{
			_clean(begin(),end());
			_head = _tail;
		}

		template <typename... Types>
		iterator_type emplace(iterator_type _where,Types... args)
		{
			assert(distant(_where, _head) != -1
				&& distant(_where, _head) <= _size);
			
			 node_type* node = _new_node(args...);
			 if (_where == begin())
				 _head = node;
			 _where._node->_prev = node;
			 node->_next = _where._node;
			 _size++;
			 return iterator_type(node);
		}

		template <typename... Types>
		void emplace_back(Types... args)
		{
			node_type* node = _new_node(args...);
			if (_head == _tail)
				_head = node;
			_insert_back(node);
		}

		iterator_type erase(iterator_type _where)
		{
			assert(distant(_where, _head) != -1
				&& distant(_where, head) <= _size);
			iterator_type out = _where;
			out++;
			_remove_node(_where._node);
			return out;
		}

		iterator_type erase(iterator_type first, iterator_type last)
		{
			assert(distant(first, _head) != -1
				&& distant(first, head) <= _size);
			assert(distant(last, _head) != -1
				&& distant(last, head) <= _size);
			node_type* pre = first._node->_prev;
			node_type* next = last._node;
			_clean(first, last);
			if (pre == nullptr)
				_head = next;
			else
				pre->_next = next;
			next->_prev = pre;
			return iterator_type(next);
		}

		void pop_back()
		{
			_remove_node(_tail->_prev);
		}

		iterator_type begin()
		{
			return iterator_type(_head);
		}

		iterator_type end()
		{
			return iterator_type(_tail);
		}

		const_iterator_type cend() const
		{
			return const_iterator_type(_tail);
		}

		const_iterator_type cbegin() const
		{
			return const_iterator_type(_head);
		}

		void reverse()
		{
			node_type* e = _tail->_prev;
			for (auto it = begin();it != end();)
			{
				auto temp = it;
				temp++;
				node_type* node = it._node->_prev;
				it._node->_prev = it._node->_next;
				it._node->_next = node;
				it = temp;
			}
			_head->_next = _tail;
			_tail->_prev = _head;
			_head = e;
		}

		void swap(iterator_type i1, iterator_type i2)
		{
			node_type* prev = i1._node->_prev;
			node_type* next = i1._node->_next;
			i1._node->_prev = i2._node->_prev;
			i1._node->_next = i2._node->_next;
			i2._node->_prev = prev;
			i2._node->_next = next;
		}

		size_type size() const
		{
			return _size;
		}
		
		size_type distant(const_iterator_type first, const_iterator_type last) const
		{
			size_type n = 0;
			for (;first != last; ++n, ++first)
				if (first._node == nullptr)
				{
					n = -1;
					break;
				}
			return n;
		}
	private:
		node_type* _head;
		node_type* _tail;
		size_type _size;
		alloc _alloc;

		void _clean(iterator_type first,iterator_type last)
		{
			for (; first != last ;++first)
			{
				_alloc.destroy(first._node,  1);
				_alloc.deallocate(first._node, 1);
				_size--;
			}
		}

		void _insert_back(node_type* node)
		{
			if (_tail == _head)
				_head = node;
			node->insert(_tail->_prev, _tail);
			_size++;
		}

		void _remove_node(node_type* node)
		{
			assert(_size);
			node_type* pre = node->_prev;
			node_type* next = node->_next;
			if (pre == nullptr)
				_head = next;
			else
				pre->_next = next;
			_alloc.destroy(node, 1);
			_alloc.deallocate(node, 1);
			next->_prev = pre;
			_size--;
		}

		template <typename... TYPES>
		node_type* _new_node(TYPES... args)
		{
			node_type* l = _alloc.allocate(1);
			_alloc.construct(l,args...);
			return l;
		}

		node_type* _new_node_unconstruct()
		{
			node_type* l = _alloc.allocate(1);
			return l;
		}

		void _make_guard()
		{
			node_type* guard = _new_node();
			_tail = guard;
			_head = guard;
		}

	};
}
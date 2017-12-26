#pragma once
#include "list_node.h"
#include "Bitree_node.h"
namespace lsd
{

	class IteratorCategory
	{
	public:
		class random_access_iterator_tag {};
		class bidirection_access_iterator_tag {};
	};
	

	//迭代器类型说明基类
	template <typename Container,
		typename Value_Type,
		typename Iterator_Type>
	class iterator_base
	{
	public:
		
		typedef typename Container::iterator_category iterator_category;
		typedef typename Container::const_type const_type;
		typedef typename Container::const_reference_type const_reference_type;
		typedef typename Container::size_type size_type;
		typedef typename Container::const_pointer_type const_pointer_type;

		//存在多态的部分
		typedef typename Value_Type value_type;
		typedef typename Value_Type& reference_type;
		typedef typename Iterator_Type iterator_type;
		typedef typename Value_Type* pointer_type;
		typedef Container container_type;
	};
	
	//迭代器基本功能实现类
	template <typename Container,
	typename Value_Type,
	typename Iterator_Type,
	int K = 1>
	class _iterator_vec
		:public iterator_base<Container,
		Value_Type,
		Iterator_Type>
	{
	public:
		_iterator_vec(pointer_type i) :_Ptr(i) {}

		operator iterator_type()
		{
			return iterator_type(_Ptr);
		}

		iterator_type operator+(size_type  n)
		{
			pointer_type p1 = _Ptr + n*K;
			return iterator_type(p1);
		}
		iterator_type operator-(size_type  n)
		{
			pointer_type p1 = _Ptr - n*K;
			return iterator_type(p1);
		}

		size_type operator-(_iterator_vec it)
		{
			return (size_type)((_Ptr - it._Ptr)*K);
		}

		iterator_type operator+=(size_type  n)
		{
			_Ptr += (n*K);
			return *this;
		}
		iterator_type operator-=(size_type  n)
		{
			_Ptr -= (n*K);
			return *this;
		}

		iterator_type operator++()
		{
			_Ptr += (1 * K);
			return *this;
		}
		iterator_type operator++(int )
		{
			_Ptr += (1 * K);
			return *this;
		}


		iterator_type operator--()
		{
			_Ptr += (-1 * K);
			return *this;
		}
		iterator_type operator--(int)
		{
			_Ptr += (-1 * K);
			return *this;
		}

		reference_type operator*()
		{
			return *_Ptr;
		}
		reference_type operator[](int n)
		{
			return *(_Ptr + K*n);
		}

		pointer_type operator->()
		{
			return _Ptr;
		}
		bool operator ==(_iterator_vec it)
		{
			return (_Ptr == it._Ptr);
		}
		bool operator != (_iterator_vec it)
		{
			return !(_Ptr == it._Ptr);
		}
		bool is_empty()
		{
			return (_Ptr == nullptr);
		}
		pointer_type _Ptr;
	};

	template<typename Container>
	class const_iterator_vec;

	//普通迭代器
	template<typename Container>
	class iterator_vec
		:public _iterator_vec<Container,
		typename Container::value_type,
		typename Container::iterator_type>
	{
	public:
		 operator const_iterator_vec
		<Container>()
		{
			 return const_iterator_vec<Container>(_Ptr);
		}
		iterator_vec(pointer_type i): _iterator_vec<Container,
			typename Container::value_type,
			typename Container::iterator_type>(i){}
	};

	//const 迭代器
	template<typename Container>
	class const_iterator_vec
		:public _iterator_vec<Container,
		typename Container::const_type,
		typename Container::const_iterator_type>
	{
	public:
		const_iterator_vec(pointer_type i) :
			_iterator_vec<Container,
			typename Container::const_type,
			typename Container::const_iterator_type>(i){}
	};

	//反向迭代器
	template<typename Container>
	class reverse_iterator_vec
		:public _iterator_vec<Container,
		typename Container::value_type,
		typename Container::iterator_type,
		-1>
	{
	public:
		reverse_iterator_vec(pointer_type i) :
			_iterator_vec<Container,
			typename Container::value_type,
			typename Container::iterator_type,
			-1>(i) {}
	};

	//const 反向迭代器
	template<typename Container>
	class const_reverse_iterator_vec
		:public _iterator_vec<Container,
		typename Container::const_type,
		typename Container::const_iterator_type,
		-1>
	{
	public:
		const_reverse_iterator_vec(pointer_type i) :
			_iterator_vec<Container,
			typename Container::const_type,
			typename Container::const_iterator_type,
			-1>(i) {}
	};

//****************************************************************************
//											list_iterator
//****************************************************************************
	template <typename Container,
		typename Value_Type,
		typename Iterator_Type,
		int K = 1>
		class _iterator_list
		:public iterator_base<Container,
		Value_Type,
		Iterator_Type>
	{
	public:
		typedef typename Container::node_type node_type;
		explicit _iterator_list(node_type* node)
			:_node(node){}

		operator iterator_type()
		{
			return iterator_type(_node);
		}

		iterator_type operator++()
		{
			if (K == 1)
				_node = node_type::next(_node);
			else
				_node = node_type::prev(_node);
			return *this;
		}

		iterator_type operator--()
		{
			if (K == 1)
				_node = node_type::prev(_node);
			else
				_node = node_type::next(_node);
			return *this;
		}

		iterator_type operator--(int )
		{
			if (K == 1)
				_node = node_type::prev(_node);
			else
				_node = node_type::next(_node);
			return *this;
		}

		iterator_type operator++(int)
		{
			if (K == 1)
				_node = node_type::next(_node);
			else
				_node = node_type::prev(_node);
			return *this;
		}

		reference_type operator*()
		{
			assert(_node->_next);
			return _node->_data;
		}

		pointer_type operator->()
		{
			return &_node->_data;
		}

		bool operator ==(_iterator_list it)
		{
			return (*it._node == *_node);
		}

		bool operator != (_iterator_list it)
		{
			return !(*it._node == *_node);
		}
		node_type* _node;
	};
	template <typename Container>
	class const_iterator_list;
	template <typename Container>
	class iterator_list
		:public _iterator_list<Container,
		typename Container::value_type,
		typename Container::iterator_type>
	{
	public:
		iterator_list(node_type* node)
			:_iterator_list<Container,
			Container::value_type,
			Container::iterator_type>(node){}
		operator const_iterator_list<Container>()
		{
			return const_iterator_list<Container>(_node);
		}
	};

	template <typename Container>
	class const_iterator_list
		:public _iterator_list<Container,
		typename Container::const_type,
		typename Container::const_iterator_type>
	{
	public:
		const_iterator_list(node_type* node)
			:_iterator_list<Container,
			Container::const_type,
			Container::const_iterator_type>(node) {}
	};

	//****************************************************************************
	//											map_iterator
	//****************************************************************************
	template <typename Container,
	typename Value_Type,
	typename Iterator_Type>
	class _iterator_set
		:public iterator_base<Container,
		Value_Type,
		Iterator_Type>
	{
	public:
		typedef typename Container::node_type node_type;
		_iterator_set(node_type* i) :_node(i) {}
		_iterator_set():_node(nullptr){}
		operator iterator_type()
		{
			return iterator_type(_node);
		}


		iterator_type operator++()
		{
			_node = _node->find_next(_node);
			return iterator_type(_node);
		}
		iterator_type operator++(int)
		{
			_node = _node->find_next(_node);
			return iterator_type(_node);
		}


		iterator_type operator--()
		{
			_node = _node->find_before(_node);
			return iterator_type(_node->find_before(_node));
		}
		iterator_type operator--(int)
		{
			_node = _node->find_before(_node);
			return iterator_type(_node);
		}

		reference_type operator*()
		{
			return _node->getVal();
		}

		pointer_type operator->()
		{
			return &(_node->getVal());
		}
		bool operator ==(_iterator_set it)
		{
			return (_node == it._node);
		}
		bool operator != (_iterator_set it)
		{
			return !(*this == it);
		}
		bool is_empty()
		{
			return (_node == nullptr);
		}
		node_type* _node;
	};

	template <typename Container>
	class iterator_set
		: public _iterator_set<Container,
		typename Container::value_type,
		typename Container::iterator_type>
	{
	public:
		iterator_set(node_type* node)
			:_iterator_set<Container,
			Container::value_type,
			Container::iterator_type>(node)
		{		}
	};
}
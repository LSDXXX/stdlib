#pragma once
#include "RBTree.h"
#include "allocator.h"
#include "container_base.h"
#include "little.h"
#include "iterator.h"
namespace lsd
{

	template <typename T,
		typename CampareFunc = less<T>,
		typename alloc = allocator<T>>
	class set
		: public container_type<T>
	{
	public:
		//ºìºÚÊ÷
		typedef		RBTree<T, CampareFunc, allocator<RBTree_node<T>>>		Tree;
		typedef		iterator_set<set>		iterator_type;
		typedef		typename IteratorCategory::bidirection_access_iterator_tag iterator_category;
		typedef		typename RBTree<T>::node_type	node_type;

		set() : tree(){}

		template <typename U>
		set(const std::initializer_list<U>& l)
		{
			for (auto it = l.begin();it != l.end();++it)
				tree.insert(*it);
		}

		template <typename ForwardIter>
		set(ForwardIter beg, ForwardIter end)
		{
			for (;beg != end;++beg)
				tree.insert(*beg);
		}

		set(const set& s)
		{
			tree = s.tree;
		}

		template <typename U>
		set(const set<U>& s)
		{
			tree = s.tree;
		}

		template <typename U>
		set<T>& operator= (const set<U>& s)
		{
			if (&s == *this)
				return *this;
			tree = s.tree;
			return *this;
		}

		iterator_type insert(const_reference_type v)
		{
			return tree.insert(v);
		}

		Tree* get_tree()
		{
			return &tree;
		}

		void erase(const_reference_type v)
		{
			tree.erase(v);
		}

		iterator_type begin()
		{
			return tree.begin();
		}

		iterator_type end()
		{
			return tree.end();
		}

		size_type size()
		{
			return tree.size();
		}
	protected:
		Tree tree;

	};

	template <typename Key,
	typename Value,
	typename CampareFunc = less<lsd::pair<Key,Value>>,
	typename alloc = allocator<lsd::pair<Key, Value>>>
	class map
		:public set<lsd::pair<Key, Value>,
		CampareFunc,
		allocator<lsd::pair<Key, Value>>>
	{
	public:
		typedef				Key			_key_type;
		typedef				Value		_value_type;
		typedef				lsd::pair<Key, Value> my_pair;
		
		_value_type operator[](const Key& key)
		{
			auto _find = tree.find({ key,_value_type() });
			return _find->getVal()._second;
		}
	};
}
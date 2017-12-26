#pragma once
#include "allocator.h"
#include "Bitree_node.h"
#include "container_base.h"
#include "_functions.h"
#include "_vector.h"
namespace lsd
{
	template <typename T,
	typename CampareFunc = less<T>,
	typename alloc = allocator<AVLTree_node<T>>>
	class AVLTree
		:public BitreeBase<T,
		AVLTree_node<T>,
		CampareFunc,
		alloc>
	{
	public:

		AVLTree()
			: BitreeBase(){
		}

		node_type* insert(const_reference_type v)
		{
			if (_root == nullptr)
			{
				return find(v);
			}
			node_type* _find = find(v);
			if (_find->left() != nullptr
				||(_find->right() != nullptr
					&& _find->right() != _end))
				return _find;
			node_type* grandpa =  _find->father()->father();
			node_type* grandpa_friend = nullptr;
			bool is_rotate = false;
			while (!is_rotate
				&& grandpa != nullptr)
			{
				int n1 = _get_depth(grandpa->left());
				int n2 = _get_depth(grandpa->right());
				if (abs(n2 - n1) > 1)
				{
					is_rotate = true;
					_rotate(grandpa,v);
				}
				grandpa = grandpa->father();
			}
			return _find;
		}

		void erase(const_reference_type v)/*ÀÁ¶èÉ¾³ý*/
		{
			node_type* _find = find(v);
			_find->label = AvailableNode::no;
		}

		~AVLTree()
		{

		}
	private:


		void _rotate(node_type* node, const_reference_type v)
		{
			if (f(node->getVal(), v))
			{
				if (f(node->right()->getVal(), v))
				{
					node->right_right(&_root);
				}
				else
					node->right_left(&_root);
			}
			else
			{
				if (f(node->left()->getVal(), v))
					node->left_right(&_root);
				else
					node->left_left(&_root);
			}
		}
	};
}
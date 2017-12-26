#pragma once
#include "Bitree_node.h"
#include "allocator.h"
#include "_functions.h"
#define    BLACK (TreeColor::black)
#define    RED		  (TreeColor::red)
#define		LEFT	  (1)
#define		RIGHT  (2)
namespace lsd
{
	template <typename T,
		typename CampareFunc = less<T>,
		typename alloc = allocator<RBTree_node<T>>>
		class RBTree
		:public BitreeBase<T,
		RBTree_node<T>,
		CampareFunc,
		alloc>
	{
	public:
		typedef BitreeBase<T,
			RBTree_node<T>,
			CampareFunc,
			alloc>   base_type;
		typedef RBTree_node<T> node_type;
		RBTree()
			:BitreeBase<T,
			RBTree_node<T>,
			CampareFunc,
			alloc>(){}
		~RBTree() { }

		RBTree(const RBTree_node<T>& t)
			: : BitreeBase<T,
			RBTree_node<T>,
			CampareFunc,
			alloc>()
		{
			*this = t;
		}

		template <typename U>
		RBTree& operator=(const RBTree<U>& t)
		{
			if (&t == this)
				return *this;
			auto b = t._begin;
			RBTree& _this = *this;
			auto temp = t._end;
			auto f = [&_this,temp](node_type* node)
			{
				if(node != temp)
					_this.insert(node->getVal());
			};
			traverse(t._root, f);
			return *this;
		}

		RBTree& operator=(const RBTree& t)
		{
			if (&t == this)
				return *this;
			auto b = t._begin;
			RBTree& _this = *this;
			auto temp = t._end;
			auto f = [&_this, temp](node_type* node)
			{
				if (node != temp)
					_this.insert(node->getVal());
			};
			traverse(t._root, f);
			return *this;
		}

		node_type* insert(const_reference_type v)
		{
			node_type* node = find(v);
			_fix_insert(node);
			return node;
		}

		node_type* insert(rvalue_type v)
		{
			node_type* node = find(std::forward<value_type>(v));
			_fix_insert(node);
			return node;
		}

		void erase(const_reference_type v);

		bool operator == (const RBTree<T>& tr)
		{
			return (this == &tr);
		}

		template <typename OtherType>
		bool operator ==(OtherType) { return false; }
	private:
		//修复删除操作
		void _fix_erase(node_type* node, int direction);
		
		//把node 节点换成change节点
		void exchange(node_type* node, node_type* change)
		{
			node_type* father = node->father();
			if (!father)
			{
				_root = change;
				change->setfather(nullptr);
				return;
			}
			else if (node->isright())
				father->setright(node->right());
			else
				father->setleft(node->right());
			change->setfather(father);
		}

		//修复插入破坏红黑数性质
		void _fix_insert(node_type* node);

	};

	template <typename T,
		typename CampareFunc = less<T>,
		typename alloc = allocator<RBTree_node<T>>>
	void lsd::RBTree<T,CampareFunc,alloc>::erase(const_reference_type v)
	{
		node_type* node = find(v);
		node_type* fix = nullptr;
		node_type* father = node->father();
		int node_direction = 0;//1代表左边,2代表右边
		TreeColor color = node->color;
		_size -= 1;
		//删除节点 没有儿子
		if (node->left() == nullptr
			&& (node->right() == nullptr
				|| node->right() == _end))
		{
			if (node == _root)
			{
				_begin = nullptr;
				_root = nullptr;
				clean_node(node);
				clean_node(_end);
				return;
			}
			else
			{
				if (node->isleft())
				{
					node->father()->setleft(nullptr);
					if (node == _begin)
						_begin = node->father();
					node_direction = 1;
				}
				else
				{
					node_type* temp = nullptr;
					if (node->right() == _end)
						temp = _end;
					node->father()->setright(temp);
					node_direction = 2;
				}
			}
			fix = father;
			clean_node(node);
		}
		//删除节点无右儿子
		//
		else if (node->right() == nullptr)
		{
			exchange(node, node->left());
			node->left()->color = BLACK;
			clean_node(node);
			return;
		}
		//没有左儿子
		//和上面是对称操作
		else if (node->left() == nullptr)
		{
			if (node == _begin)
				_begin = node->find_next(node);
			exchange(node, node->right());
			node->right()->color = BLACK;
			clean_node(node);
			return;
		}
		//既有左儿子又有右儿子
		else
		{
			node_type* rc = node->right();
			node_type* _erase = rc->far_left(rc);
			if (_erase->right()
				&& _erase->right() != _end)
			{
				exchange(_erase, _erase->right());
				_erase->right()->color = BLACK;
				clean_node(_erase);
				return;
			}
			color = _erase->color;
			if (_erase == rc)
				node_direction = 2;
			else
				node_direction = 1;
			node->setVal(std::move(_erase->getVal()));
			_erase->father()->setleft(nullptr);
			fix = _erase->father();
			clean_node(_erase);
		}
		//如果删除的节点为黑色
		//需要修复操作
		if (color == BLACK)
		{
			_fix_erase(fix, node_direction);
		}
	}

	template <typename T,
		typename CampareFunc = less<T>,
		typename alloc = allocator<RBTree_node<T>>>
		void lsd::RBTree<T, CampareFunc, alloc>::
		_fix_erase(node_type* node, int direction)
	{
		if (!node)
			return;
		if (direction == LEFT)
		{
			node_type* right = node->right();
			if (right->color == RED)
			{
				//删除节点父亲节点是黑色，
				//并且删除节点的兄弟节点是红色
				node->color = RED;
				node->right_right(&_root);
				right->color = BLACK;
			}
			else
			{
				if (right->right() &&
					right->right()->color == RED)
				{
					right->color = node->color;
					right->right()->color = BLACK;
					node->color = BLACK;
					node->right_right(&_root);
				}
				else if (right->left() &&
					right->left()->color == RED)
				{
					right->left()->color = node->color;
					node->color = BLACK;
					node->right_left(&_root);
				}
				else
				{
					if (node->color == RED)
					{
						node->color = BLACK;
						right->color = RED;
					}
					else
					{
						right->color = RED;
						int direction = 0;
						if (node->father())
							direction = node->isleft() ? LEFT : RIGHT;
						_fix_erase(node->father(), direction);
					}
				}
			}
		}
		else
		{
			node_type* left = node->left();
			if (left->color == RED)
			{
				//删除节点父亲节点是黑色，
				//并且删除节点的兄弟节点是红色
				node->color = RED;
				node->left_left(&_root);
				left->color = BLACK;
			}
			else
			{
				if (left->left()
					&& (left->left() != _end))
				{
					left->left()->color = BLACK;
					left->color = node->color;
					node->color = BLACK;
					node->left_left(&_root);
				}
				else if (left->right() &&
					left->right()->color == RED)
				{
					left->right()->color = node->color;
					node->color = BLACK;
					node->left_right(&_root);
				}
				else
				{
					if (node->color == RED)
					{
						node->color = BLACK;
						left->color = RED;
					}
					else
					{
						left->color = RED;
						int direction = 0;
						if (node->father())
							direction = node->isleft() ? LEFT : RIGHT;
						_fix_erase(node->father(), direction);
					}
				}
			}
		}
	}

	template <typename T,
		typename CampareFunc = less<T>,
		typename alloc = allocator<RBTree_node<T>>>
		void lsd::RBTree<T, CampareFunc, alloc>::
		_fix_insert(node_type* node)
	{
		if (node->color == BLACK
			|| node->father() == nullptr
			|| node->father()->color == BLACK)
		{
			if (node->father() == nullptr)
				node->color = BLACK;
			return;
		}
		node_type* l_father = node->father();
		node_type* father_brother = l_father->brother();
		if (
			father_brother == nullptr ||
			father_brother->color == BLACK)
		{
			if (node->isleft())
			{
				if (l_father->isleft())
				{
					l_father->color = BLACK;
					l_father->father()->color = RED;
					l_father->father()->left_left(&_root);
				}
				else
				{
					l_father->father()->color = RED;
					node->color = BLACK;
					l_father->father()->right_left(&_root);
				}
			}
			else
			{
				if (l_father->isleft())
				{
					l_father->father()->color = RED;
					node->color = BLACK;
					l_father->father()->left_right(&_root);
				}
				else
				{
					l_father->color = BLACK;
					l_father->father()->color = RED;
					l_father->father()->right_right(&_root);
				}
			}
		}
		else
		{
			l_father->color = BLACK;
			father_brother->color = BLACK;
			l_father->father()->color = RED;
			_fix_insert(l_father->father());
		}
	}
}
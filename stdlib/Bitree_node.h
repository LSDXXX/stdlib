#pragma once
#include "container_base.h"
#define SINGLE_ROTATE(d1,d2) \
		void d1##_##d1(node_type** root)\
		{\
		node_type* f = _father;\
		node_type* child = this->d1();\
		node_type* child_##d2 = child->d2();\
		if (f)\
		{\
				if(this->is##d2())\
					f->set##d2(child);\
				else\
					f->set##d1(child);\
			child->setfather(f);\
		}\
		else\
		{\
			child->setfather(nullptr);\
			*root = child;\
		}\
		_father = child;\
		child->set##d2(this);\
		if(child_##d2)\
			child_##d2->setfather(this);\
		_##d1##_child = child_##d2;\
		}

#define DOUBLE_ROTATES(d1,d2)\
		void d1##_##d2(node_type** root)\
		{\
		node_type* child = _##d1##_child;\
		node_type* child_##d2 = child->d2();\
		node_type* grand_child = child_##d2->d1();\
		_##d1##_child = child_##d2;\
		child_##d2->setfather(this);\
		child_##d2->set##d1(child);\
		child->setfather(child_##d2);\
		if(grand_child)\
			grand_child->setfather(child);\
		child->set##d2(grand_child);\
		d1##_##d1(root);\
		}



namespace lsd
{
	enum TreeColor { red = 0, black };
	template <typename T>
	class RBTree_node;
	template <typename T>
	class AVLTree_node;
	enum AvailableNode {yes = 1,no};

	//遍历整棵二叉树
	template<typename Node,
	typename Tree,
	typename Func>
	void traverse(Node* root, Tree& t, Func f)
	{
		lsd::vector<Node*> vec;
		if (root)
			vec.push_back(root);
		while (vec.size())
		{
			Node* temp = vec.back();
			vec.pop_back();
			if (temp->left())
				vec.push_back(temp->left());
			if (temp->right())
				vec.push_back(temp->right());
			(t.*f)(temp);
		}
	}

	template<typename Node,
		typename Func>
		void traverse(Node* root, Func f)
	{
		lsd::vector<Node*> vec;
		if (root)
			vec.push_back(root);
		while (vec.size())
		{
			Node* temp = vec.back();
			vec.pop_back();
			if (temp->left())
				vec.push_back(temp->left());
			if (temp->right())
				vec.push_back(temp->right());
			f(temp);
		}
	}

	template <typename T,
		typename ChildNode>
	struct Bitree_node
	{
		typedef typename T value_type;
		typedef ChildNode node_type;
		Bitree_node()
			:_left_child(nullptr),
			_right_child(nullptr),
			_father(nullptr), 
			label(AvailableNode::yes) {}
		
		Bitree_node(const value_type& v)
			:value(v),
			_left_child(nullptr),
			_right_child(nullptr),
			_father(nullptr),
			label(AvailableNode::yes) {}

		Bitree_node(const Bitree_node& n)
			:value(n.value),
			_left_child(n._left_child),
			_right_child(n._right_child),
			_father(n._father),
			label(n.label)
		{

		}

		Bitree_node(Bitree_node&& n)
			:value(std::forward<value_type>(n.value))
			_left_child(n._left_child),
			_right_child(n._right_child),
			_father(n._father),
			label(n.label)
		{

		}

		void setleft(node_type* l)
		{
			_left_child = l;
		}

		void setright(node_type* l)
		{
			_right_child = l;
		}

		void setfather(node_type* l)
		{
			_father = l;
		}

		bool isright()
		{
			return (father()->right() == this);
		}

		bool isleft()
		{
			return !isright();
		}
		
		node_type* brother()
		{
			if (!_father)
				return nullptr;
			if (this->isleft())
				return _father->right();
			else
				return _father->left();
		}

		node_type *left() const{ return _left_child; }
		node_type *right() const { return _right_child; }
		node_type *father() const { return _father; }
		value_type& getVal() { return value; }
		const value_type& getVal() const{ return value; }
		void setVal(const value_type& v) { value = v; }
		void setVal(value_type&& v) { value = std::forward<value_type>(v); }
		//找到指定节点的后继
		node_type* find_next(node_type* node) const;

		//找到指定节点的前驱
		node_type* find_before(node_type* node) const;

		//指定节点的最左端
		node_type* far_left(node_type* b) const
		{
			node_type* before = b;
			while (b)
			{
				before = b;
				b = b->left();
			}
			return before;
		}

		//指定节点的最右端
		node_type* far_right(node_type* b) const
		{
			node_type* before = b;
			while (b
				&&b != _end)
			{
				before = b;
				b = b->right();
			}
			return b;
		}

		virtual ~Bitree_node() 
		{
			_left_child = nullptr;
			_right_child = nullptr;
			_father = nullptr;
		}
		AvailableNode label; 
	protected:
		node_type *_left_child;
		node_type *_right_child;
		node_type *_father;
		value_type value;
	};

	template <typename T,
		typename NodeType,
		typename CampareFunc,
		typename alloc = allocator<NodeType>>
		class BitreeBase
		:public container_type<T>
	{
	public:
		typedef typename NodeType node_type;

		BitreeBase()
			: _root(nullptr),
			_begin(nullptr),
			_end(nullptr),
			_size(0){
		}

		//在树中查找,查找不到就创建一个新节点
		node_type* find(const_reference_type v);

		//得到指定节点高度
		int get_depth(node_type* node) const
		{
			return _get_depth(node);
		}

		node_type* root() const
		{
			return _root;
		}

		node_type* begin() const
		{
			return _begin;
		}

		node_type* end() const
		{
			return _end;
		}

		//清理节点
		void clean_node(node_type*& node)
		{
			_alloc.destroy(node, 1);
			_alloc.deallocate(node, 1);
			node = nullptr;
		}

		virtual ~BitreeBase()
		{
			traverse(_root, *this, &BitreeBase::clean_node);
			_root = nullptr;
			_begin = nullptr;
			_end = nullptr;
		}

		virtual node_type* insert(const_reference_type v) = 0;

		size_type size() const
		{
			return _size;
		}

	protected:
		node_type* _root;
		node_type* _begin;
		node_type* _end;
		size_type _size;
		alloc _alloc;
		CampareFunc f;

		template <typename... Types>
		node_type* _new_node(Types... args)
		{
			node_type* out = _alloc.allocate(1);
			_alloc.construct(out, args...);
			return out;
		}

		node_type* _new_node_unconstruct()
		{
			node_type* out = _alloc.allocate(1);
			return out;
		}

		int _get_depth(node_type* node) const
		{
			if (node == nullptr || node == _end)
				return -1;
			int n1 = _get_depth(node->left()) + 1;
			int n2 = _get_depth(node->right()) + 1;
			return max(n1, n2);
		}
	};


	template <typename T>
	class RBTree_node
		:public Bitree_node<T,RBTree_node<T>>
	{
	public:
		RBTree_node()
			:Bitree_node<T, RBTree_node<T>>(),
			color(TreeColor::red){}
		RBTree_node(const T& v)
			:Bitree_node<T, RBTree_node<T>>(v),
			color(TreeColor::red){}
		RBTree_node(TreeColor color)
			:Bitree_node<T, RBTree_node<T>>(),
			color(color) {}
		RBTree_node(const T& v,TreeColor color)
			:Bitree_node<T, RBTree_node<T>>(v),
			color(color) {}
		RBTree_node(const RBTree_node& n)
			:Bitree_node<T, RBTree_node<T>>(n),
			color(n.color){}
		RBTree_node(RBTree_node&& n)
			:Bitree_node<T, RBTree_node<T>>(std::forward<RBTree_node>(n)),
			color(n.color) {}
		TreeColor color;
		SINGLE_ROTATE(right, left)
		SINGLE_ROTATE(left, right)
		DOUBLE_ROTATES(right, left)
		DOUBLE_ROTATES(left, right)
	};

	template <typename T>
	class AVLTree_node
		:public Bitree_node<T,AVLTree_node<T>>
	{
	public:
		AVLTree_node()
			:Bitree_node<T, AVLTree_node<T>>() {}
		AVLTree_node(const T& v)
			:Bitree_node<T, AVLTree_node<T>>(v){}
		SINGLE_ROTATE(right, left)
		SINGLE_ROTATE(left, right)
		DOUBLE_ROTATES(right, left)
		DOUBLE_ROTATES(left, right)
	};

	template <typename T,
		typename NodeType,
		typename CampareFunc,
		typename alloc = allocator<NodeType>>
		typename BitreeBase<T, NodeType, CampareFunc, alloc>::node_type* 
		BitreeBase<T,NodeType,CampareFunc,alloc>::
		find(const_reference_type v)
	{
		if (_root == nullptr)
		{
			node_type* temp = _new_node(TreeColor::black);
			_end = temp;
			_root = _new_node(v);
			_begin = _root;
			_root->setright(_end);
			_size += 1;
			return _root;
		}

		node_type* _find = _root;
		do
		{
			node_type* temp = nullptr;
			if (_find->label == AvailableNode::no)
			{
				bool y = true;
				if (_find->left() != nullptr)
					if (!f(_find->left()->getVal(), v))
						y = false;
				if (y)
				{
					if (_find->right() != nullptr)
						if (f(_find->right()->getVal(), v))
							y = false;
				}
				if (y)
				{
					_find->getVal() = v;
					_find->label = AvailableNode::yes;
					return _find;
				}
			}
			if (f(_find->getVal(), v))
			{
				temp = _find->right();
				if (temp == nullptr
					|| temp == _end)
				{
					node_type* r = _new_node(v);
					_find->setright(r);
					r->setfather(_find);
					if (temp != nullptr)
					{
						r->setright(temp);
						temp->setfather(r);
					}
					_size += 1;
					return r;
				}
				_find = temp;
			}
			else if (_find->getVal() == v)
				return _find;
			else
			{
				temp = _find->left();
				if (temp == nullptr)
				{
					temp = _new_node(v);
					_find->setleft(temp);
					temp->setfather(_find);
					if (_find == _begin)
						_begin = temp;
					_size += 1;
					return temp;
				}
				_find = temp;
			}
		} while (_find != nullptr);
		return _find;
	}

	template <typename T,
		typename ChildNode>
		typename Bitree_node<T,ChildNode>::
		node_type*
		Bitree_node<T, ChildNode>::
		find_next(node_type* node) const
	{
		if (node->right())
			return far_left(node->right());
		auto father = node->father();
		while (father
			&&node->isright())
		{
			node = father;
			father = node->father();
		}
		return father;
	}

	template <typename T,
		typename ChildNode>
		typename Bitree_node<T, ChildNode>::
		node_type*
		Bitree_node<T, ChildNode>::
		find_before(node_type* node) const
	{
		if (node->left())
			return far_right(node->right());
		auto father = node->father();
		while (father
			&&node->isleft())
		{
			node = father;
			father = node->father();
		}
		return father;
	}
}
#undef SINGLE_ROTATE
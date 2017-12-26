#pragma once
namespace lsd
{
	template <typename Ty>
	struct list_node
	{

		list_node(const Ty& data)
			:_data(data),
			_prev(nullptr),
			_next(nullptr)
		{

		}
		template <typename... Types>
		list_node(Types... args)
			:_data(args...),
			_prev(nullptr),
			_next(nullptr){}

		template <typename... Types>
		list_node(list_node* p,list_node* n,Types... args)
			: _data(args...),
			_prev(p),
			_next(n) {}

		list_node(const Ty& data, list_node* prev, list_node* next)
			:_data(data),
			_next(next),
			_prev(prev) {}

		list_node()
			:_data(),
			_next(nullptr),
			_prev(nullptr) {}

		list_node* _prev;
		list_node* _next;
		Ty _data;

		bool operator==(const list_node& node)
		{
			return (
				this == &node);
		}

		void insert(list_node* p, list_node* n)
		{
			_prev = p;
			if (p)
				p->_next = this;
			_next = n;
			if (n)
				n->_prev = this;
		}

		void insert(list_node* p, list_node* n, const Ty& v)
		{
			_data = v;
			insert(p, n);
		}

		static list_node* next(list_node* node)
		{
			return node->_next;
		}

		static list_node* prev(list_node* node)
		{
			return node->_prev;
		}

	};
}
#pragma once
#include <malloc.h>
#include <new>
#include <fstream>
#include "locks.h"
#define MAX_MM 128
#define GRANULARITY 8
#define ROUNDUP(n) (n/8*8 + ((n%8? 1:0)*8)) 
#define N 10
#define BEG(p,type,member) \
((void*)((char*)p + (size_t)(((type*)0)->member)))
#ifdef MutiThread
#define LOCK Lock lock
#define UNLOCK lock.unlock()
#else
#define LOCK
#endif

//#define DEBUG

#ifdef DEBUG
#define LOG_MEMORY(BUY,BACK) log.log_memory(BUY,BACK)
#else
#define LOG_MEMORY(BUY,BACK)
#endif // DEBUG

namespace lsd 
{
	class MemoryLog
	{
	public:
		MemoryLog(){}
		MemoryLog(const std::string& file_name)
			:_name(file_name),
			_stream(file_name,std::ios::app){
			_stream << "\n****************************************************\n";
		}
		void log_memory(int buy,int back)
		{
			_stream << "发出的内存:\t" << buy
				<< "\t回收的内存:\t" << back <<
				"\t 盈余:\t" << buy - back << "\n";
		}
		~MemoryLog()
		{
			_stream << "\n****************************************************\n";
		}
	private:
		std::string _name;
		std::ofstream _stream;
	};

	static MemoryLog log("memory.txt");
	static enum FIND {FindAll = 0,FindBig};
	struct block
	{
		block* next;
		char beg[];
	};

	struct list_head
	{
		list_head():n(0),head(nullptr){}
		typedef int size_type;
		block* head;
		size_type n;
	};

	template <int K>
	class _alloc
		: public ClassLevelLock<_alloc<K>>
	{
	public:
		typedef int size_type;
	public:
		static void* allocate(size_type n)
		{
			if (n == 0)
				return nullptr;
			size_type find = ROUNDUP(n);
			_buy += find;
			LOG_MEMORY(_buy, _back);
			//log.log_memory(_buy, _back);
			if (n >= MAX_MM)
				return malloc(n);
			LOCK;
			size_type index = find / GRANULARITY - 1;
			list_head* list = &mmArray[index];
			if (list->head ==nullptr)
			{
				void* out = find_avilible(index, FIND::FindBig);
				if (out)
					return out;
				size_type r = end - pos;
				size_type req = find * N;
				char* oldpos = pos;
				if (r > req)
				{
					_list(&pos, N, find);
					_insert(list, (block*)oldpos,N);
				}
				else if (r >= find)
				{
					size_type n = r / find;
					_list(&pos, n, find);
					_insert(list, (block*)oldpos,n);
				}
				else
				{
					if (r != 0)
					{
						size_type t = find - GRANULARITY;
						while (r > 0)
						{
							size_type k = r / t;
							if (k > 0)
							{
								_list(&pos, k, t);
								_insert(&mmArray[t / GRANULARITY - 1], (block*)oldpos,k);
								r -= k*t;
							}
							t -= GRANULARITY;
						}
					}
					if (!new_pool())
					{
						void* out = find_avilible(index, FIND::FindAll);
						if (out)
							return out;
						throw std::bad_alloc();
					}
					else
					{
						_buy -= find;
						return allocate(n);
					}
				}
			}
			return pop_front(index);
		}

		static void deallocate(void *addr, size_type n)
		{
			if (n == 0)
				return;
			int size = ROUNDUP(n);
			_back += size;
			LOG_MEMORY(_buy, _back);
			if (size >= MAX_MM)
				delete addr;
			else
			{
				LOCK;
				block* before = mmArray[size / GRANULARITY - 1].head;
				((block*)addr)->next = before;
				mmArray[size / GRANULARITY - 1].head = (block*)addr;
			}
			addr = nullptr;
		}

	private:
		static void* new_pool()
		{
			memory_pool = (char*)malloc(K);
			beg = memory_pool;
			pos = beg;
			end = beg + K;
			return beg;
		}

		static void _list(char** beg, size_type n, size_type size)
		{
			for (size_type i = 0;i < n - 1;++i)
			{
				((block*)*beg)->next = (block*)(pos + size);
				*beg += size;
			}
			((block*)*beg)->next = nullptr;
			*beg += size;
		}

		static void _insert(list_head* place, block* node,size_type n)
		{
			(place->n) += n;
			if (place->head == nullptr)
			{
				place->head = node;
			}
			else
			{
				auto temp = place->head;
				node->next = temp;
				place->head = node;
			}
		}

		static void* pop_front(size_type i)
		{
			list_head* out = &mmArray[i];
			(out->n) -= 1;
			void* temp = (void*)out->head;
			out->head = out->head->next;
			return temp;
		}

		static void* find_avilible(size_type index,FIND finds)
		{
			size_type req = (index + 1) * GRANULARITY;
			//在大内存组内寻找，如果找到就返回，把多余部分划到对应的
			//分组
			for (size_type i = index + 1;i < MAX_MM / GRANULARITY;++i)
			{
				size_type& n = mmArray[i].n;
				if (n)
				{
					size_type last = (i - index)*GRANULARITY;
					size_type last_index = i - index - 1;
					char* temp = (char*)pop_front(i);
					char* plast = temp + req;
					((block*)plast)->next = nullptr;
					_insert(&mmArray[last_index], (block*)plast,1);
					return temp;
				}
			}

			if (finds == FIND::FindAll)
				//在小内存部分寻找，内存必须是连续的
			{
				size_type i = index - 1;
				while (i >= 0)
				{
					list_head& head = mmArray[i];
					size_type nsize = (i + 1)*GRANULARITY;
					size_type n = head.n;
					block* first = (block*)head.head;
					char* end = (char*)first;
					block* before = nullptr;
					size_type find = 0;
					for (size_type k = 0;k < n;++k)
					{
						char* temp = (char*)(((block*)end)->next);
						if (end + nsize == temp)
						{
							find += nsize;
							end = temp;
						}
						else
						{
							before = (block*)end;
							first = (block*)temp;
							end = (char*)first;
							find = 0;
						}
						if (find >= req)
						{
							size_type nout = find / nsize;
							size_type last = find - req;
							size_type last_index = last / GRANULARITY - 1;
							block* after = ((block*)end)->next;
							mmArray[i].n -= nout;
							void* out = (void*)first;
							block* plast = (block*)((char*)out + find);
							_insert(&mmArray[last_index], plast, 1);
							if (before != nullptr)
								((block*)before)->next = after;
							else
								mmArray[i].head = after;
						}
					}
					--i;
				}
			}
			//山穷水尽 
			return nullptr;
		}
		static list_head mmArray[MAX_MM / GRANULARITY] ;
		static char* memory_pool ;
		static char* beg ;
		static char* end ;
		static char* pos ;
		static int _buy;
		static int _back;
	};
	template <int K>
	char* _alloc<K>::memory_pool = nullptr;
	template <int K>
	list_head _alloc<K>::mmArray[MAX_MM / GRANULARITY] = { list_head() };
	template <int K>
	char* _alloc<K>::beg = nullptr;
	template <int K>
	char* _alloc<K>::end= nullptr;
	template <int K>
	char* _alloc<K>::pos = nullptr;
	template <int K>
	int _alloc<K>::_buy = 0;
	template <int K>
	int _alloc<K>::_back = 0;
	template <typename T,
	int K = 256>
	class allocator
	{
	public:
		typedef typename int size_type;
		typedef typename T value_type;
		typedef typename T& reference_type;
		typedef typename T* pointer_type;
		typedef typename const T const_type;
		typedef typename const T& const_reference_type;
	public:
		static value_type* allocate(size_type n)
		{
			size_type size = n*sizeof(value_type);
			return (value_type*)_alloc<K>::allocate(size);
		}
		static void deallocate(value_type* addr, int n)
		{
			size_type size = n*sizeof(value_type);
			_alloc<K>::deallocate((void*)addr, size);
		}
		template <typename... Tys>
		static void construct(value_type* s,Tys... args)
		{
			new (s)value_type(args...);
		}
		static void destroy(value_type* s,int count)
		{
			auto temp = s;
			for (size_type i = 0;i < count;++i,temp++)
				temp->~T();
		}
	};
}

#pragma once
#include <Windows.h>
#include <assert.h>
#include "nocopy.h"
//ģ��Loki�߳̿�


#define ATOMIC_FUNCTIONS \

namespace lsd {
	class SpinLock
	{
	public:

		SpinLock() :m_resouce(FALSE) {}
		void acquire()
		{
			while (InterlockedExchange(&m_resouce, TRUE)
				== TRUE)
				Sleep(0);
		}
		void release()
		{
			InterlockedExchange(&m_resouce, FALSE);
		}
	private:
		SpinLock(const SpinLock&);
		SpinLock& operator =(const SpinLock&);
		DWORD m_resouce;
	};


	class SpinAndCriticalSectionLock
		:NoCopy
	{
	public:
		SpinAndCriticalSectionLock(int spincount = 4000)
			:m_spincount(spincount)
		{
			InitializeCriticalSectionAndSpinCount(&section, spincount);
		}
		void lock()
		{
			EnterCriticalSection(&section);
		}
		bool try_lock()
		{
			return (bool)TryEnterCriticalSection(&section);
		}
		void unlock()
		{
			LeaveCriticalSection(&section);
		}
		~SpinAndCriticalSectionLock()
		{
			lock();
			DeleteCriticalSection(&section);
		}
	private:
		int m_spincount;
		CRITICAL_SECTION section;
	};


	//���������
	template <typename Ty,
		typename Mutex = SpinAndCriticalSectionLock>
	class ClassLevelLock
	{
	public:
		class Lock;
		friend class Lock;
		class Lock
			:private NoCopy
		{
		public:
			explicit Lock()
			{
				assert(_Initializer.alive);
				_Initializer._mutex.lock();
			}
			void unlock()
			{
				assert(_Initializer.alive);
				_Initializer._mutex.unlock();
			}
			~Lock()
			{
				assert(_Initializer.alive);
				_Initializer._mutex.unlock();
			}
		};
	private:
		struct Initializer
		{
			Mutex _mutex;
			Initializer() :alive(false), _mutex() { alive = true; }
			~Initializer() { assert(alive); }
			bool alive;
		};
		static Initializer _Initializer;
	};
	template <typename Ty,
		typename Mutex = SpinAndCriticalSectionLock>
		typename ClassLevelLock<Ty, Mutex>::Initializer  
		ClassLevelLock<Ty, Mutex>::_Initializer;
	//������
	template <typename Ty,
		typename Mutex = SpinAndCriticalSectionLock>
		class ObjectLevelLock
	{
	public:
		class Lock;
		friend Lock;
		class Lock
			:private NoCopy
		{
		public:
			explicit Lock(const ObjectLevelLock& lock)
				:_host(lock)
			{
				_host._mutex.lock();
			}
			void unlock()
			{
				_host._mutex.unlock();
			}
			~Lock()
			{
				_host._mutex.unlock();
			}
		private:
			const ObjectLevelLock& _host;
		};
	private:
		mutable Mutex _mutex;
	};
}
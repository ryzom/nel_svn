/** \file mutex.h
 * OS independant class for the mutex management with Windows and Posix implementation
 * Classes CMutex, CSynchronized
 */

/* Copyright, 2000 Nevrax Ltd.
 *
 * This file is part of NEVRAX NEL.
 * NEVRAX NEL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * NEVRAX NEL is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with NEVRAX NEL; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef NL_MUTEX_H
#define NL_MUTEX_H

#include "types_nl.h"
#include "time_nl.h"
#include <map>

#ifdef NL_OS_UNIX
//#include <iostream>
#include <pthread.h> // PThread
#include <semaphore.h> // PThread POSIX semaphores
#include <unistd.h>
#define __forceinline
#elif defined(NL_OS_WINDOWS)
#	define NOMINMAX
#	include <windows.h>
#	ifdef NL_NO_ASM
#		include <intrin.h>
#	endif
#endif

#undef MUTEX_DEBUG


namespace NLMISC {


/*
 * This define must be disabled when sharing a mutex between several processes that can
 * have a different debug mode (because when __STL_DEBUG is on, sizeof(string) is twice
 * the common string size).
 */
#define STORE_MUTEX_NAME


// By default, all mutex use the CFairMutex class to avoid freeze problem.
#define CMutex CFairMutex

/**
 * Classic mutex implementation (not necessarly fair)
 * Don't assume the mutex are recursive (ie don't call enter() several times
 * on the same mutex from the same thread without having called leave()) ;
 * and don't assume either the threads are woken-up in the same order as they
 * were put to sleep !
 *
 * Windows: uses Mutex, cannot be shared among processes.
 * Linux: uses PThread POSIX Mutex, cannot be shared among processes.
 *
 *\code
 CUnfairMutex m;
 m.enter ();
 // do critical stuffs
 m.leave ();
 *\endcode
 * \author Vianney Lecroart, Olivier Cado
 * \author Nevrax France
 * \date 2000
 */
class CUnfairMutex
{
public:

	/// Constructor
	CUnfairMutex();
	CUnfairMutex( const std::string &name );

	/// Destructor
	~CUnfairMutex();

	/// Enter the critical section
	void	enter ();

	/// Leave the critical section
	void	leave ();

private:

#ifdef NL_OS_WINDOWS
	void *_Mutex;
#elif defined NL_OS_UNIX
	pthread_mutex_t mutex;
#else
#	error "No unfair mutex implementation for this OS"
#endif

};


// Inline assembler for gcc tutorial:
// AT&T syntax:
// - operands reversed,
// - l after opcode replaces dword ptr,
// - () instead of [],
// - immediate values prefixed by $

// Tested: works on multi-processor
#ifdef HAVE_X86_64
#	define ASM_ASWAP_FOR_GCC_XCHG __asm__ volatile( \
		    "mov %1, %%rcx;" \
		    "mov $1, %%eax;" \
		    "xchg %%eax, (%%rcx);" \
		    "mov %%eax, %0" \
	        : "=m" (result) \
		    : "m" (lockPtr) \
		    : "eax", "rcx", "memory" ); // force to use registers and memory
#else
#	define ASM_ASWAP_FOR_GCC_XCHG __asm__ volatile( \
		    "mov %1, %%ecx;" \
		    "mov $1, %%eax;" \
		    "xchg %%eax, (%%ecx);" \
		    "mov %%eax, %0" \
	        : "=m" (result) \
		    : "m" (lockPtr) \
		    : "eax", "ecx", "memory" ); // force to use registers and memory
#endif

// Tested: does not work (at least on multi-processor)! (with or without 'lock' prefix)
#define ASM_ASWAP_FOR_GCC_CMPXCHG __asm__ volatile( \
		    "mov $1, %%edx;" \
		    "mov %1, %%ecx;" \
		    "mov (%%ecx), %%eax;" \
		    "1:nop;" \
		    "lock cmpxchgl %%edx, (%%ecx);" \
		    "jne 1b;" \
		    "mov %%eax, %0" \
	        : "=m" (result) \
		    : "m" (lockPtr) \
		    : "eax", "ecx", "edx", "memory" ); // force to use registers and memory


// Tested: does not work on hyper-threading processors!
/*ASM_ASWAP_FOR_MSVC_CMPXCHG
{
	__asm
		{
			mov edx,1
			mov ecx,l
			mov eax,[ecx]
test_again:
			nop
			cmpxchg     dword ptr [ecx],edx
			jne         test_again
			mov [result],eax
		}
}*/


/**
 * Fast mutex implementation (not fairly)
 * The mutex ARE NOT recursive (ie don't call enter() several times
 * on the same mutex from the same thread without having called leave()) ;
 * The threads ARE NOT woken-up in the same order as they were put to sleep.
 * The threads ARE NOT woken-up using signals but using Sleep().
 * This mutex works but is not optimal for multiprocessors because if the mutex is locked,
 * next enter will be sleeped without waiting a little.
 *
 * Implementation notes:
 *  - Implementated under WIN32
 *  - Other OS use CMutex
 *
 * Tested: OK on Windows and Linux single & multi-processor
 *
 *\code
 CFastMutex m;
 m.enter ();
 // do critical stuffs
 m.leave ();
 *\endcode
 * \author Cyril 'Hulud' Corvazier
 * \author Olivier Cado
 * \author Nevrax France
 * \date 2002, 2003
 */
#ifdef __ppc__
// on ppc, use fait mutex because we don't have ppc implementation of fast mutex
#   define CFastMutex CFairMutex
#else

#ifdef NL_OS_WINDOWS
#pragma managed(push, off)
#endif
class CFastMutex
{
public:

	/// Constructor
	CFastMutex() : _Lock(0) {}

	/// Same as constructor, useful for init in a shared memory block
	void	init() volatile { _Lock = 0; }

	/// Atomic swap
	__forceinline static bool atomic_swap (volatile uint32 *lockPtr)
	{
		uint32 result;
#ifdef NL_OS_WINDOWS
#	ifdef NL_NO_ASM
		result = _InterlockedExchange(reinterpret_cast<volatile long *>(lockPtr), 1);
#	else
#	ifdef NL_DEBUG
		// Workaround for dumb inlining bug (returning of function goes into the choux): push/pop registers
		__asm
		{
			push eax
			push ecx
			mov ecx,lockPtr
			mov eax,1
			xchg [ecx],eax
			mov [result],eax
			pop ecx
			pop eax
		}
#	else
		__asm
		{
			mov ecx,lockPtr
			mov eax,1
			xchg [ecx],eax
			mov [result],eax
		}
#	endif // NL_DEBUG
#	endif // NL_NO_ASM
#else
		ASM_ASWAP_FOR_GCC_XCHG
#endif // NL_OS_WINDOWS
		return result != 0;
	}

	// Enter critical section
	__forceinline void enter () volatile
	{
	  //std::cout << "Entering, Lock=" << _Lock << std::endl;
		if (atomic_swap (&_Lock))
		{
			// First test
			uint i;
			for (i = 0 ;; ++i)
			{
				uint wait_time = i + 6;

				// Increment wait time with a log function
				if (wait_time > 27)
					wait_time = 27;

				// Sleep
				if (wait_time <= 20)
					wait_time = 0;
				else
					wait_time = 1 << (wait_time - 20);

				if (!atomic_swap (&_Lock))
					break;

#ifdef NL_OS_WINDOWS
				Sleep (wait_time);
#else
				//std::cout <<  "Sleeping i=" << i << std::endl;
				usleep( wait_time*1000 );
#endif
			}
		}
	}

	// Leave critical section
	__forceinline void leave () volatile
	{
		_Lock = 0;
		//std::cout << "Leave" << std::endl;
	}

private:
	volatile uint32	_Lock;
};


#endif

/**
 * Fast mutex for multiprocessor implementation (not fairly).
 * Used for multiprocessor critical section synchronisation.
 * The mutex ARE NOT recursive (ie don't call enter() several times
 * on the same mutex from the same thread without having called leave()) ;
 * The threads use a spin system to wait a little time before be put to sleep.
 * It waits using CPU time.
 *
 * Implementation notes:
 *  - Implementated under WIN32
 *  - Other OS use CMutex
 *
 *\code
 CFastMutexMP m;
 m.enter ();
 // do critical stuffs
 m.leave ();
 *\endcode
 * \author Cyril 'Hulud' Corvazier
 * \author Olivier Cado
 * \author Nevrax France
 * \date 2002, 2003
 */
#ifndef __ppc__
class CFastMutexMP
{
public:

	/// Constructor
	CFastMutexMP() : _Lock(0) {}

	/// Same as constructor, useful for init in a shared memory block
	void	init() volatile { _Lock = 0; }

	// Enter critical section
	__forceinline void enter () volatile
	{
	  //std::cout << "Entering, Lock=" << _Lock << std::endl;
		while (CFastMutex::atomic_swap (&_Lock))
		{
			static uint last = 0;
			static uint _max = 30;
			uint spinMax = _max;
			uint lastSpins = last;
			volatile uint temp = 17;
			uint i;
			for (i = 0; i < spinMax; ++i)
			{
				if (i < lastSpins/2 || _Lock)
				{
					temp *= temp;
					temp *= temp;
					temp *= temp;
					temp *= temp;
				}
				else
				{
					if (!CFastMutex::atomic_swap(&_Lock))
					{
						last = i;
						_max = 1000;
						return;
					}
				}
			}

			_max = 30;

			// First test
			for (i = 0 ;; ++i)
			{
				uint wait_time = i + 6;

				// Increment wait time with a log function
				if (wait_time > 27)
					wait_time = 27;

				// Sleep
				if (wait_time <= 20)
					wait_time = 0;
				else
					wait_time = 1 << (wait_time - 20);

				if (!CFastMutex::atomic_swap (&_Lock))
					break;

#ifdef NL_OS_WINDOWS
				Sleep (wait_time);
#else
				//std::cout <<  "Sleeping i=" << i << std::endl;
				usleep( wait_time*1000 );
#endif
			}
		}
	}

	// Leave critical section
	__forceinline void leave () volatile
	{
		_Lock = 0;
		//std::cout << "Leave" << std::endl;
	}

private:
	volatile uint32	_Lock;
};
#endif


/**
 * Windows: uses Mutex, the handle can't be shared among processes, but
 * the mutex still can be be shared by passing a common object name to
 * createByName() / createByKey(). Note: the mutex must be explicitely
 * destroyed by calling destroy().
 *
 * \author Olivier Cado
 * \author Nevrax France
 * \date 2002
 */
class CSharedMutex
{
public:

	/// Constructor (does not create the mutex, see createByName()/createByKey())
	CSharedMutex();

#ifdef NL_OS_WINDOWS
	/// Create or access an existing mutex (created by another process) with a specific object name. Returns false if it failed.
	bool	createByName( const char *objectName );
#else
	/// Create (with createNew to true) or access an existing mutex (created by another process) with a specific key. Returns false if it failed.
	bool	createByKey( int key, bool createNew );
#endif

	/// Destroy the mutex
	void	destroy();

	/// Enter the critical section
	void	enter ();

	/// Leave the critical section
	void	leave ();

private:

#ifdef NL_OS_WINDOWS
	/// The mutex handle
	void	*_Mutex;
#else
	/// The semaphore id
	int		_SemId;
#endif
};



#ifdef NL_OS_WINDOWS
/**
 * Trick to avoid including <windows.h> !
 * winbase.h: typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
 * The original RTL_CRITICAL_SECTION is in winnt.h.
 */
struct TNelRtlCriticalSection {
	void	*DebugInfo;
	long	 LockCount;
	long	 RecursionCount;
	void	*OwningThread;        // from the thread's ClientId->UniqueThread
	void	*LockSemaphore;
	uint32	 SpinCount;
};
#endif // NL_OS_WINDOWS


/**
 * Kind of "fair" mutex
 *
 * Windows: uses Critical Section, cannot be shared among processes
 * Linux: uses PThread (POSIX) semaphore, cannot be shared among processes
 *
 *\code
 CUnfairMutex m;
 m.enter ();
 // do critical stuffs
 m.leave ();
 *\endcode
 * \author Olivier Cado
 * \author Nevrax France
 * \date 2000
 *
 *\code
 CFairMutex m;
 m.enter ();
 // do critical stuffs
 m.leave ();
 *\endcode
 * \author Olivier Cado
 * \author Nevrax France
 * \date 2001
 */
class CFairMutex
{
public:

	/// Constructor
	CFairMutex();
	CFairMutex(const std::string &name);

	/// Destructor
	~CFairMutex();

	void enter ();
	void leave ();

#ifdef STORE_MUTEX_NAME
	std::string Name;
#endif

private:

#ifdef NL_OS_WINDOWS
	TNelRtlCriticalSection	_Cs;
#elif defined NL_OS_UNIX
	sem_t			_Sem;
#else
#	error "No fair mutex implementation for this OS"
#endif


#ifdef MUTEX_DEBUG
	// debug stuffs
	void debugCreateMutex();
	void debugBeginEnter();
	void debugEndEnter();
	void debugLeave();
	void debugDeleteMutex();
#endif // MUTEX_DEBUG

};


/*
 * Debug info
 */
#ifdef MUTEX_DEBUG

struct TMutexLocks
{
	TMutexLocks(uint32 m=0) : TimeToEnter(0), TimeInMutex(0), Nb(0), WaitingMutex(0), MutexNum(m), ThreadHavingTheMutex(0xFFFFFFFF), Dead(false) {}

	uint32		TimeToEnter;			// cumulated time blocking on enter
	uint32		TimeInMutex;			// cumulated time between enter and leave
	uint32		Nb;						// number of calls of enter
	uint32		WaitingMutex;			// number of thread that waiting this mutex
    sint32		MutexNum;				// identifying a mutex
	uint		ThreadHavingTheMutex;	// thread id of the thread that is in this mutex (0xFFFFFFFF if no thread)
	bool		Dead;					// True if the mutex is dead (deleted)
	std::string	MutexName;				// Name of the mutex

	NLMISC::TTicks BeginEnter;
	NLMISC::TTicks EndEnter;
};

/// Inits the "mutex debugging info system"
void initAcquireTimeMap();

/// Gets the debugging info for all mutexes (call it evenly)
std::map<CMutex*,TMutexLocks>	getNewAcquireTimes();

/// The number of created mutexes (does not take in account the destroyed mutexes)
extern uint32 NbMutexes;

#endif // MUTEX_DEBUG





// By default, all synchronization use the CFairSynchronized class to avoid freeze problem.
#define CSynchronized CFairSynchronized

/**
 * This class ensure that the Value is accessed by only one thread. First you have to create a CSynchronized class with you type.
 * Then, if a thread want to modify or do anything on it, you create a CAccessor in a \b sub \b scope. You can modify the value
 * of the CUnfairSynchronized using the value() function \b until the end of the scope. So you have to put the smaller scope as you can.
 *
 * Internally, this class uses a CUnfairMutex object (see CUnfairMutex for programming considerations).
 *
 *\code
 // the value that you want to be thread safe.
 CUnfairSynchronized<int> val;
 { // create a new scope for the access
   // get an access to the value
   CUnfairSynchronized<int>::CAccessor acces(&val);
   // now, you have a thread safe access until the end of the scope, so you can do whatever you want. for example, change the value
   acces.value () = 10;
 } // end of the access
 *\endcode
 * \author Vianney Lecroart
 * \author Nevrax France
 * \date 2000
 */
template <class T>
class CUnfairSynchronized
{
public:

	CUnfairSynchronized (const std::string &name) : _Mutex(name) { }

	/**
	 * This class give you a thread safe access to the CSynchronized Value. Look at the example in the CSynchronized.
	 */
	class CAccessor
	{
		CUnfairSynchronized<T> *Synchronized;
	public:

		/// get the mutex or wait
		CAccessor(CUnfairSynchronized<T> *cs)
		{
			Synchronized = cs;
			const_cast<CMutex&>(Synchronized->_Mutex).enter();
		}

		/// release the mutex
		~CAccessor()
		{
			const_cast<CMutex&>(Synchronized->_Mutex).leave();
		}

		/// access to the Value
		T &value()
		{
			return const_cast<T&>(Synchronized->_Value);
		}
	};

private:

	friend class CUnfairSynchronized::CAccessor;

	/// The mutex of the synchronized value.
	volatile CUnfairMutex	_Mutex;

	/// The synchronized value.
	volatile T				_Value;
};


/**
 * This class is similar to CUnfairSynchronized, but it ensures that the threads
 * are woken-up in the same order as they were put to sleep.
 * Internally, it uses a CFairMutex object instead of a CUnfairMutex object.
 * \author Olivier Cado
 * \author Nevrax France
 * \date 2001
 */
template <class T>
class CFairSynchronized
{
public:

	CFairSynchronized (const std::string &name) : _Cs(name) { }

	/**
	 * This class give you a thread safe access to the CFairSynchronized Value. Look at the example in CSynchronized.
	 */
	class CAccessor
	{
		CFairSynchronized<T> *Synchronized;
	public:

		/// get the mutex or wait
		CAccessor(CFairSynchronized<T> *cs)
		{
			Synchronized = cs;
			const_cast<CFairMutex&>(Synchronized->_Cs).enter();
		}

		/// release the mutex
		~CAccessor()
		{
			const_cast<CFairMutex&>(Synchronized->_Cs).leave();
		}

		/// access to the Value
		T &value()
		{
			return const_cast<T&>(Synchronized->_Value);
		}
	};

private:

	friend class CFairSynchronized::CAccessor;

	/// The mutex of the synchronized value.
	volatile CFairMutex	_Cs;

	/// The synchronized value.
	volatile T			_Value;
};


/** Helper class that allow easy usage of mutex to protect
 *	a local block of code with an existing mutex.
 */
template <class TMutex=CMutex>
class CAutoMutex
{
	TMutex	&_Mutex;

	// forbeden copy or assignent
	CAutoMutex(const CAutoMutex &other)
	{
	}

	CAutoMutex &operator = (const CAutoMutex &other)
	{
		return *this;
	}

public:
	CAutoMutex(TMutex &mutex)
		:	_Mutex(mutex)
	{
		_Mutex.enter();
	}

	~CAutoMutex()
	{
		_Mutex.leave();
	}

};

} // NLMISC


#endif // NL_MUTEX_H

/* End of mutex.h */

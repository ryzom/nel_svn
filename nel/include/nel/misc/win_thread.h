/** \file win_thread.h
 * Windows implementation of CThread class (look at thread.h)
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

#ifndef NL_WIN_THREAD_H
#define NL_WIN_THREAD_H

#include "types_nl.h"
#include "thread.h"

#ifdef NL_OS_WINDOWS

namespace NLMISC {


/**
 * Windows implementation of CThread class (look thread.h)
 * \author Vianney Lecroart
 * \author Nevrax France
 * \date 2000
 */
class CWinThread : public IThread
{
public:

	/// Constructor
	CWinThread(IRunnable *runnable, uint32 stackSize);

	/// Don't use this constructor, only used to initialise the main thread class
	CWinThread (void* threadHandle, uint32 threadId);

	virtual ~CWinThread();

	virtual void start();
	virtual bool isRunning();
	virtual void terminate();
	virtual void wait();
	virtual bool setCPUMask(uint64 cpuMask);
	virtual uint64 getCPUMask();
	virtual std::string getUserName();

	virtual IRunnable *getRunnable()
	{
		return Runnable;
	}

	// Win32 specific
	// Get the suspend count. Will be -1 is the thread hasn't been started yet
	int getSuspendCount() const { return _SuspendCount; }
	// Increment the suspend count, a suspend count >= 1 means the thread is suspended
	void incSuspendCount();
	/** Descrement the suspend count. Reaching 0 will resume the thread
	  * An assertion is raised is the suspend count is already 0
	  */
	void decSuspendCount();
	// Suspend the thread. No-op if already suspended
	void suspend();
	// Resume the thread. No-op if already resumed
	void resume();
	// set priority as defined by "SetThreadpriority"
	void setPriority(int priority);
	void enablePriorityBoost(bool enabled);

	/// private use
	IRunnable	*Runnable;

private:
	int			_SuspendCount;
	uint32		_StackSize;
	void		*ThreadHandle;	// HANDLE	don't put it to avoid including windows.h
	uint32		ThreadId;		// DWORD	don't put it to avoid including windows.h
	bool		_MainThread;	// true if ths thread is the main thread, else false
};

/**
 * Windows Process
 * \author Cyril 'Hulud' Corvazier
 * \author Nicolas Vizerie
 * \author Nevrax France
 * \date 2001, 2007
 */
class CWinProcess : public IProcess
{
public:

	CWinProcess (void *handle);
	virtual ~CWinProcess() {} // TODO do something with _ProcessHandle?

	virtual uint64 getCPUMask();
	virtual bool setCPUMask(uint64 mask);

	// processes helpers
	static bool   enumProcessesId(std::vector<uint32> &processesId);
	// get fully qualified path for all modules used by a given process
	static bool	  enumProcessModules(uint32 processId, std::vector<std::string> &moduleNames);
	static uint32 getProcessIdFromModuleFilename(const std::string &moduleFileName);
	static bool	  terminateProcess(uint32 processId, uint exitCode = 0);
	static bool	  terminateProcessFromModuleName(const std::string &moduleName, uint exitCode = 0);

private:
	void	*_ProcessHandle;
};



/*
//  I didn't use and test that code, enventually, but maybe useful in the future
//
// Utility class to launch a process and check if it is still running.
// Implemented under windows only for now
//
class CProcessWatch
{
public:
	CProcessWatch();
	~CProcessWatch();
	// launch a process with the given name and arguments, return true on success
	bool launch(const std::string &programName, const std::string &arguments);
	// return true if the process is still runing
	bool isRunning() const;
private:
	class CProcessWatchImpl *_PImpl;
};

*/

} // NLMISC

#endif // NL_OS_WINDOWS

#endif // NL_WIN_THREAD_H

/* End of win_thread.h */

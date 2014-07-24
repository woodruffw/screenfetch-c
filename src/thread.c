/*	thread.c
	Author: William Woodruff
	-------------

	This file provides a very basic abstraction of the pthread and winapi thread APIs.
	It is designed to provide screenfetch-c with threading capabilities on multiple systems
	with a minimal amount of fuss and cross-platform finagling.
	Like the rest of screenfetch-c, this file is licensed under the MIT License.
	You should have received a copy of it with this code.
*/

#include "thread.h"

/*  create_thread
	spawns a new thread, given a pointer to a THREAD, a function, and an argument
	on POSIX systems, calls pthread_create
	on Windows systems, calls CreateThread
	argument THREAD* thread, a pointer to the thread being created
	argument void* func_call, the function call to be made within the new thread
	argument void* arg, an argument for the function to use
*/
void create_thread(THREAD* thread, void *(*func_call)(void*), void* arg)
{
	#if defined(__CYGWIN__)
		*thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) func_call, arg, 0, NULL);
	#else
		pthread_create(thread, NULL, func_call, arg);
	#endif
}

/*  join_thread
	suspends execution of the calling thread until the argument thread has terminated its execution
	on POSIX systems, calls pthread_join
	on Windows systems, calls WaitForSingleObject
	argument THREAD thread, the thread being joined (waited on). 
*/
void join_thread(THREAD thread)
{
	#if defined(__CYGWIN__)
		WaitForSingleObject(thread, INFINITE);
	#else
		pthread_join(thread, NULL);
	#endif
}

/*  exit_thread
	exits the thread being called under
	on POSIX systems, calls pthread_exit
	on Windows systems, calls ExitThread
*/
void exit_thread()
{
	#if defined(__CYGWIN__)
		ExitThread(0);
	#else
		pthread_exit(NULL);
	#endif
}

/* EOF */

/*	threadtest.c
	Author: William Woodruff

	Tests the proper operation of the threading functions on multiple systems.

	Systems tested:
	Windows - PASS
	OS X - PASS
	Linux (#!) - PASS (required -lpthread compiler flag)
*/

#include <stdio.h>
#include <unistd.h>
#include "thread.h"

void* printtest(void* ptr) { printf("%s\n", "inside a thread"); exit_thread(); }

int main(int argc, char const *argv[])
{
	printf("%s\n", "test");
	THREAD thread;
	create_thread(&thread, printtest, NULL);
	join_thread(thread);
	printf("%s\n", "test done");
	return 0;
}
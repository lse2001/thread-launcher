thread_launcher: thread_launcher.c
	gcc -Wall -Wextra -pedantic -std=c11 -pthread thread_launcher.c -o thread_launcher

thread_launcher_gcc: thread_launcher.c
	gcc-16 -Wall -Wextra -pedantic -std=c11 -pthread thread_launcher.c -o thread_launcher_gcc

clean:
	rm -f thread_launcher thread_launcher_gcc
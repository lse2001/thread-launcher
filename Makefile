thread_launcher: thread_launcher.c
	gcc -Wall -Wextra -pedantic -std=c11 -pthread thread_launcher.c -o thread_launcher

clean:
	rm -f thread_launcher thread_launcher_gcc
1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation calls waitpid() on all forked child processes inside a loop. This ensures the shell waits for each process to finish before prompting the user for a new command. If waitpid() were missing, zombie processes would accumulate, leading to resource leaks and unpredictable behavior.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

Unused pipe ends must be closed after dup2() to prevent file descriptor leaks. If left open, processes might hang because the pipe remains open indefinitely, preventing EOF detection. Closing them ensures proper resource management and avoids deadlocks.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is built-in because it modifies the shell’s working directory, which persists across commands. If implemented as an external command, the directory change would only apply to the child process running cd, leaving the parent shell unchanged. This would make cd ineffective.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To handle unlimited piped commands, I would use dynamic memory allocation (realloc()) instead of a fixed-size array for storing commands. This allows resizing as needed. The trade-offs include increased complexity, potential fragmentation, and the need to carefully manage memory to prevent leaks.

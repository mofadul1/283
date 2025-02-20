# Assignment Questions

### 1. Why do we use `fork/execvp` instead of just calling `execvp` directly? What value does `fork` provide?

> **Answer**:  
`fork()` creates a **copy of the shell process** so that `execvp()` only replaces the child process, not the shell itself. If we called `execvp()` directly, it would **replace the shell**, and we wouldn’t be able to run any more commands.

---

### 2. What happens if `fork()` fails? How does your code handle it?

> **Answer**:  
If `fork()` fails, it means the system **couldn’t create a new process**, usually due to low memory or too many processes running. My code **prints an error message (`perror("Fork failed")`)** and returns an error code so the shell doesn’t crash.

---

### 3. How does `execvp()` find the command to run? What system variable helps with this?

> **Answer**:  
`execvp()` looks for the command **in the directories listed in the `PATH` environment variable**. If you type `ls`, it checks folders like `/bin/` and `/usr/bin/` to find the `ls` program.

---

### 4. Why do we call `wait()` in the parent process after forking? What happens if we don’t?

> **Answer**:  
`wait()` makes the shell **pause until the command finishes running**, so it doesn’t immediately print a new prompt. Without `wait()`, we’d get **zombie processes** (unfinished processes still taking up memory).

---

### 5. What does `WEXITSTATUS()` do, and why is it useful?

> **Answer**:  
`WEXITSTATUS()` **gets the exit code** of the command that just ran. If a command succeeds, it returns `0`; if it fails, it returns something else. This helps the shell **detect errors and show useful messages**.

---

### 6. How does your `build_cmd_buff()` handle quoted arguments? Why is this important?

> **Answer**:  
It **keeps spaces inside quotes** so that `echo "hello  world"` is treated as **one argument**, not two. Without this, spaces inside quotes would be lost, changing the meaning of commands.

---

### 7. What changes did you make to your parsing logic from the last assignment? Any challenges?

> **Answer**:  
I **removed the pipe (`|`) handling** since this version only runs **one command at a time**. The hardest part was making sure **quoted strings were handled correctly** while still splitting normal arguments properly.

---

### 8. Research on Linux signals.

#### **8a. What are signals in Linux, and how do they compare to other ways programs talk to each other?**

> **Answer**:  
Signals are **quick messages** the system sends to processes to make them **stop, pause, or restart**. Unlike other communication methods (like files or sockets), signals **don’t require a setup**—they happen instantly.

---

#### **8b. Describe three common signals and what they do.**

> **Answer**:  
1. **SIGKILL (`kill -9 PID`)** – **Forces** a process to stop immediately. Can’t be ignored.  
2. **SIGTERM (`kill PID`)** – **Asks** a process to stop, giving it a chance to save work.  
3. **SIGINT (`Ctrl+C`)** – Stops a running process **from the terminal** (like stopping a script).

---

#### **8c. What happens when a process gets `SIGSTOP`? Can it be ignored?**

> **Answer**:  
`SIGSTOP` **pauses** a process completely, but **can’t be ignored or caught** like `SIGINT`. This ensures that a process can always be stopped, even if it tries to block other signals.

---


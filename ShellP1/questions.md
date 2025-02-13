# Assignment Questions

### 1. Why is `fgets()` a good choice for this application?

> **Answer**:  
`fgets()` reads a full line safely, prevents buffer overflows, and stops at newline or EOF, making it ideal for a shell that processes input line by line.

---

### 2. Why use `malloc()` for `cmd_buff` instead of a fixed-size array?

> **Answer**:  
Dynamic allocation with `malloc()` allows flexibility, avoiding wasted memory with a large buffer or truncation with a small one.

---

### 3. Why must `build_cmd_list()` trim leading and trailing spaces?

> **Answer**:  
Untrimmed spaces can cause misinterpretation of commands, leading to execution errors or unexpected empty arguments.

---

### 4. Research on STDIN, STDOUT, and STDERR in Linux.

#### **4a. Three redirection examples and challenges:**
1. **Output to file (`>` and `>>`)** – `ls > output.txt`, handles file writing but needs proper error handling.  
2. **Input from file (`<`)** – `sort < names.txt`, must check if the file exists.  
3. **Redirecting errors (`2>`)** – `gcc program.c 2> errors.txt`, requires distinguishing STDERR from STDOUT.

---

#### **4b. Difference between redirection and piping?**
> **Answer**:  
Redirection interacts with files (`ls > output.txt`), while piping (`|`) connects commands (`ls | grep .txt`) for real-time processing.

---

#### **4c. Why separate STDERR from STDOUT?**
> **Answer**:  
Mixing errors with regular output can cause confusion, especially in automation. Keeping them separate ensures clarity.

---

#### **4d. How should our shell handle errors?**
> **Answer**:  
Use exit codes to detect failures and show meaningful messages. Allow merging STDERR with STDOUT using `2>&1` (`ls > output.txt 2>&1`).

---


# Assignment Questions

### 1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

> **Answer**:  
`fgets()` is ideal because it safely reads a full line of input, prevents buffer overflows, and stops at a newline or EOF. This makes it perfect for a shell that processes commands line by line, ensuring a smooth interaction without unexpected truncations or errors.

---

### 2. You needed to use `malloc()` to allocate memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

> **Answer**:  
Using `malloc()` allows dynamic memory allocation, making the shell more flexible. A fixed-size array could waste memory if too large or truncate input if too small. Dynamic allocation ensures we allocate exactly what’s needed, making our shell more efficient.

---

### 3. In `dshlib.c`, the function `build_cmd_list()` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

> **Answer**:  
Trimming spaces ensures commands are correctly interpreted. Without it, extra spaces could cause incorrect parsing, unexpected empty arguments, or execution failures. For example, `"   ls  -l   "` might be misinterpreted, leading to errors or improper argument handling.

---

### 4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

#### **4a. One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.**

> **Answer**:  
1. **Redirecting output to a file (`>` and `>>`)**  
   - Example: `ls > output.txt` (creates/overwrites a file), `echo "Hello" >> log.txt` (appends to a file).  
   - **Challenge:** Ensuring correct file permissions and handling accidental overwrites while distinguishing between append (`>>`) and overwrite (`>`).

2. **Redirecting input from a file (`<`)**  
   - Example: `sort < names.txt` (reads input from `names.txt`).  
   - **Challenge:** The shell must check if the file exists before passing it as input. If the file is missing, an error should be displayed.

3. **Redirecting errors (`2>`)**  
   - Example: `gcc program.c 2> errors.txt` (captures error messages separately).  
   - **Challenge:** Keeping error messages separate from normal output, ensuring they aren’t lost, and allowing optional merging with `2>&1`.

---

#### **4b. You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.**

> **Answer**:  
Redirection (`>`, `<`, `>>`) controls **where input or output goes** (files). Piping (`|`) connects **two commands**, sending the output of one as input to another.  
- **Example of redirection:** `ls > files.txt` (stores `ls` output in a file).  
- **Example of piping:** `ls | grep ".txt"` (filters `ls` output for `.txt` files).  
Piping is real-time and used for chaining commands, while redirection primarily interacts with files.

---

#### **4c. STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?**

> **Answer**:  
Separating STDERR from STDOUT prevents errors from mixing with normal output. This is crucial for automation, where scripts may depend on clean output. If errors were mixed in, parsing output would become difficult, leading to unintended behavior.

---

#### **4d. How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?**

> **Answer**:  
Our shell should detect failures using exit codes and display useful error messages. If users want to merge STDOUT and STDERR, we should support `2>&1`, which redirects STDERR into STDOUT, allowing both to be captured together in a file (`ls > output.txt 2>&1`). Handling these cases ensures flexibility and usability.

---


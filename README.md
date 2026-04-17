*This project has been created as part of the 42 curriculum by maaugust, rjorge-p.*

<div align="center">
  <img src="https://raw.githubusercontent.com/rfs-hybrid/42-Common-Core/main/assets/covers/cover-minishell-bonus.png" alt="Minishell Cover" width="100%" />
</div>

<div align="center">
  <h1>🐚 Minishell: As beautiful as a shell</h1>
  <img src="https://img.shields.io/badge/Language-C-blue" />
  <img src="https://img.shields.io/badge/Grade-125%2F100-success" />
  <img src="https://img.shields.io/badge/Norminette-Passing-success" />
</div>

---

## 💡 Description
**Minishell** is a major milestone in the 42 curriculum. The objective is to write a simple yet fully functional UNIX shell, taking Bash as the primary reference. 

This project requires a deep dive into process creation (`fork`), file descriptor manipulation (`dup2`, `pipe`), signal handling (`sigaction`), and rigorous memory management. Beyond execution, the core challenge of Minishell lies in parsing. To handle complex command sequences, operator precedence, and variable expansions, this implementation utilizes a custom **Lexer** and a **Recursive Descent Parser** to build an **Abstract Syntax Tree (AST)** prior to execution.

---

## 📑 Features

### 🔹 Mandatory Features
* **Abstract Syntax Tree (AST):** Input is tokenized and parsed into a binary tree, naturally enforcing operator precedence and ensuring robust execution logic.
* **Concurrent Pipelines (`|`):** Commands in a pipeline are executed simultaneously in parallel processes, strictly managing open file descriptors to prevent deadlocks and pipe buffer overflows.
* **Redirections (`<`, `>`, `>>`):** Full support for input/output redirection, including handling standard file descriptors dynamically.
* **Here-Documents (`<<`):** Robust heredoc implementation that correctly expands environment variables unless the delimiter is quoted. Uses a non-blocking `get_next_line` approach to prevent terminal freeze-ups when receiving piped input from automated testers.
* **Signal Handling:** Bash-accurate signal handling for `Ctrl+C` (SIGINT), `Ctrl+\` (SIGQUIT), and `Ctrl+D` (EOF), effectively managing parent and child process states.

### 🚀 Bonus Features
* **Logical Operators (`&&` and `||`):** Full short-circuit evaluation for logical AND/OR operations based on command exit statuses.
* **Subshells / Parentheses (`()`):** Support for priority grouping of commands, allowing complex sequences like `(cmd1 || cmd2) | cmd3`.
* **Wildcard Expansion (`*`):** Accurate wildcard pattern matching for files in the current working directory, including ambiguous redirect detection (e.g., `> *`).
* **Environment Variables (`$`):** Expansion of environment variables, including the exit status variable (`$?`), and proper word-splitting for unquoted variables.
* **Variable Assignment (`VAR=value`):** Support for local shell variables, as well as the advanced append operator (`+=`).
* **Tilde Expansion (`~`):** Natively expands the tilde to the user's `$HOME` directory, smoothly integrating with the `cd` builtin and command arguments.

### 🛠️ Built-in Commands
The shell natively implements the following built-ins with full POSIX accuracy (running in the main process to preserve state):
* `echo` (with `-n` flag support)
* `cd` (with relative/absolute pathing and `HOME`/`OLDPWD` fallback)
* `pwd`
* `export` (with `+=` append support and alphabetical sorting)
* `unset`
* `env`
* `exit` (Safely parses numeric arguments using rigorous 64-bit integer bounds checking (`LONG_MAX` / `LONG_MIN`) to detect overflows. Correctly applies 8-bit modulo wrapping for the final exit code and suppresses the "exit" printout when executed inside a pipeline.)

---

## 🧠 Algorithm & Data Structure

This shell operates on a strict lifecycle for every line of input. To mimic Bash's true behavior, the pipeline is divided into distinct phases, each handling specific POSIX edge cases.

### 1. Initialization & State Management
When the shell starts, it duplicates the `envp` array into a dynamic linked list.
* **Missing Environment Fallbacks:** If run with `env -i ./minishell`, the shell dynamically reconstructs essential variables (`PWD` via `getcwd()`, sets `SHLVL=1`, and defaults `PATH`). 
* **The `_` Variable:** Automatically updates the `$_` environment variable with the last argument of the previous command.
* **Three-State Signal Architecture:** The shell dynamically toggles `sigaction` states to prevent core dumps and UI glitches:
  1. **Interactive State:** `Ctrl+C` clears the prompt via `readline`, `Ctrl+\` is safely ignored (`SIG_IGN`).
  2. **Parent Execution State:** Signals are ignored (`SIG_IGN`) so the main shell doesn't die while waiting for blocking child processes.
  3. **Child Execution State:** Signals are restored to default (`SIG_DFL`) before `execve` so external binaries (like `cat` or `grep`) can be properly interrupted or dumped.

### 2. Lexical Analysis (Tokenizer)
The raw input string is sliced into a linked list of tokens (`TOKEN_WORD`, `TOKEN_PIPE`, `TOKEN_REDIR_IN`, etc.).
* **Quote Handling:** Single quotes (`'`) preserve the literal value of all characters. Double quotes (`"`) preserve literal values except for `$`, `\`, and `"`. Unclosed quotes trigger an immediate syntax error, halting execution.
* **Operator Identification:** Operators like `|`, `||`, `&&`, `<<`, and `>>` are isolated from surrounding words, even without spacing (e.g., `ls>file`).

### 3. Expansion Engine
Before parsing, words undergo a strict sequence of expansions mandated by POSIX:
1. **Tilde Expansion:** Replaces `~/` with the `$HOME` path.
2. **Variable Expansion:** Replaces `$VAR` with its environment value. Also handles the special `$?` (exit code) and `$$` (shell PID) variables.
3. **Word Splitting:** If an unquoted variable expands to a string containing spaces (e.g., `A="ls -l"`), it is dynamically split into multiple tokens.
4. **Wildcard Globbing (`*`):** Expands `*` to match files in the current working directory. Hidden files are ignored unless the pattern explicitly starts with `.`. *Note: A sentinel character (`\002`) is used internally to distinguish a literal quoted `*` from a globbing wildcard.*
5. **Ambiguous Redirect Detection:** If a redirection target (like `> $EMPTY` or `> *.c`) expands to zero or multiple words, the shell throws an `ambiguous redirect` error instead of a generic file error.
6. **Quote Removal:** Finally, all unescaped quotes are stripped from the tokens.

### 4. Parser (Abstract Syntax Tree)
Instead of a flat linked list, the token list is fed into a **Recursive Descent Parser** to build a binary Abstract Syntax Tree (AST). This inherently solves operator precedence.
* **Precedence Levels:** `parse_logic` (`&&`, `||`) calls `parse_pipeline` (`|`), which calls `parse_single_command`. This ensures that a command like `cmd1 || cmd2 | cmd3` groups correctly as `cmd1 || (cmd2 | cmd3)`.
* **Subshells (`()`):** Parentheses reset the parsing precedence, wrapping the inner logic in an `AST_SUBSHELL` node. Redirections can be attached directly to the subshell (e.g., `(cmd1 && cmd2) > file`).

### 5. Pre-Execution: Here-Documents
Before the AST executes, all `<<` heredocs are processed. 
* **Temp Files:** Temporary files (e.g., `/tmp/.here_doc_X`) are safely generated.
* **Non-Blocking Reads:** To prevent terminal freeze-ups when receiving piped input from automated testers, heredocs read from `STDIN` using `get_next_line` in the parent process if `!isatty(STDIN_FILENO)`.
* **Input Stream Overriding:** Correctly processes consecutive heredocs (`<< eof1 << eof2`), initializing all buffers sequentially but ensuring only the final file descriptor is bound to the command's standard input.
* **Quoted Delimiters:** If the delimiter is quoted (e.g., `<< "EOF"`), variable expansion inside the heredoc is disabled.
* **EOF Warnings:** Pressing `Ctrl+D` prematurely prints a Bash-accurate warning: `minishell: warning: here-document delimited by end-of-file`.

### 6. Execution Engine
The executor traverses the AST recursively.
* **Redirection Overriding:** Perfectly mimics Bash behavior for infinite chained redirections (e.g., `< file1 < file2 > out1 > out2`). The executor evaluates, opens, and creates all files in sequence to enforce strict permission checking, but only the **last** valid input (including heredocs) and the **last** valid output are actually bound to the command's standard file descriptors.
* **Concurrent Pipelines & `SIGPIPE` Mastery:** Unlike basic implementations that wait for the left child before forking the right, this shell forks *both* children concurrently to prevent deadlocks when exceeding the 65KB OS pipe buffer limit. It meticulously manages signal inheritance—restoring `SIGPIPE` to `SIG_DFL` in child processes so that broken pipes (e.g., `ls | invalid_command`) trigger a silent OS-level termination (Signal 13) rather than crashing with a `write()` error, achieving strict Bash parity.
* **Logical Short-Circuiting:** `&&` and `||` nodes evaluate the exit status of their left child before deciding whether to execute their right child.
* **Exit Codes:** Uses `WIFEXITED`, `WTERMSIG`, and `stat` to generate accurate exit codes:
  * `0`: Success.
  * `1`: General error / Command failure.
  * `2`: Syntax Error.
  * `126`: Permission Denied / Is a directory.
  * `127`: Command not found / No such file.
  * `130 / 131`: Killed by `SIGINT` / `SIGQUIT`.

### 7. Built-ins & Local Variables
Built-in commands execute directly in the main process to preserve state changes. To handle temporary command redirections (e.g., `export VAR=1 > file`), `STDIN` and `STDOUT` are safely duplicated (`dup`/`dup2`) and restored after the builtin executes.
* **`echo`:** Prints arguments to standard output. Fully supports the `-n` flag to suppress trailing newlines, accurately parsing complex consecutive combinations (e.g., `echo -n -nnn -n`). Includes strict write-error checking to fail safely if the output destination is broken or full.
* **`cd`:** Navigates the filesystem. Supports relative/absolute paths, and handles argument-less calls (goes to `$HOME`) and `cd -` (goes to `$OLDPWD`). Dynamically updates the environment, featuring an advanced logical fallback to reconstruct the path if `getcwd` fails (e.g., if the current directory was deleted by another terminal).
* **`pwd`:** Prints the current working directory. Prioritizes reading the `$PWD` environment variable to respect symlinks, falling back to a physical `getcwd()` call only if the environment variable is missing.
* **`export`:** Adds or updates environment variables. Strictly validates POSIX identifier syntax. It fully supports the advanced `+=` append operator. When called with no arguments, it prints a strictly alphabetically sorted list of the environment in the format `declare -x KEY="VALUE"`.
* **`unset`:** Safely searches and removes variables from the environment linked list, handling memory deallocation cleanly. Rejects unsupported option flags but correctly unsets variables mixed within the argument list.
* **`env`:** Prints the current environment. To ensure strict predictable execution, it explicitly rejects any arguments or options, and filters out local shell variables or exported variables that do not have a defined value.
* **Local Assignments:** The shell natively understands local environment declarations without the `export` keyword (e.g., `VAR=123`), saving them safely into the environment list as unexported variables.

---

## 🛠️ Instructions

### 📦 Installation
Ensure you have a UNIX-based operating system (Linux/macOS), a C compiler (`gcc`/`clang`), and the `readline` library installed. Clone the repository and compile the project using the provided `Makefile`:

```bash
git clone https://github.com/your-username/minishell.git
cd minishell
make
```

> **Note on Compilation:** Because this shell natively integrates all advanced features (like the AST, short-circuit logic, and wildcards) into a single unified architecture, the compilation is an all-in-one process. Running `make bonus` will build the exact same `./minishell` executable as `make all`.

### 🧹 Cleaning & Utilities
* `make clean`: Removes the compiled object files (`.o`).
* `make fclean`: Removes object files, the executable, and cleans the embedded `libft`.
* `make re`: Performs a clean re-build.
* `make valgrind`: Compiles the project and automatically runs it through Valgrind, applying a custom `.supps/readline.supp` file to rigorously test for memory leaks and open file descriptors (`--track-fds=yes`).

### 💻 Usage

**1. Interactive Prompt:**
Run the executable to launch the shell.
```bash
./minishell
```

**2. Execution Examples:**
You can use the prompt exactly as you would use standard Bash, combining pipes, redirections, logic, and subshells!
```bash
minishell@localhost:~$ ls -la | grep ".c" | wc -l
minishell@localhost:~$ (echo "Hello" && echo "World") > output.txt
minishell@localhost:~$ < in1 < in2 cat > out1 > out2
minishell@localhost:~$ export VAR="42"
minishell@localhost:~$ echo $VAR
```

---

## 🎬 Live Testing Showcases

*A live demonstration of the shell handling standard execution, concurrent pipelines, here-documents, and environment expansions (proven by the custom `$0` expansion watermark!).*

Click on any of the dropdowns below to watch unedited video demonstrations of the shell handling complex grouped scenarios. Videos include playback controls so you can pause and verify the exact terminal outputs!

<details>
<summary><b><big>1. The Pipeline & Heredoc Stress Test</big></b></summary>
<br>
<i>Demonstrating concurrent pipe execution, infinite chained redirections, and non-blocking heredocs with variable expansion.</i><br>
<div align="center">
  <video src="https://github.com/user-attachments/assets/4c9158ea-c80b-40f7-b010-8fd17a391b56" width="800" controls="controls"></video>
</div>
<br>
</details>

<details>
<summary><b><big>2. Subshells & Logic Short-Circuiting</big></b></summary>
<br>
<i>Testing priority grouping `()` and strict short-circuit logic `&&` / `||` based on command exit statuses.</i><br>
<div align="center">
  <video src="https://github.com/user-attachments/assets/302ab2ba-25fd-4ace-aa84-ff242b60ce9d" width="800" controls="controls"></video>
</div>
<br>
</details>

<details>
<summary><b><big>3. Expansion, Wildcards & Ambiguous Redirects</big></b></summary>
<br>
<i>Showing accurate globbing `*`, variable splitting, exit code `$?` retrieval, and the ambiguous redirect protection.</i><br>
<div align="center">
  <video src="https://github.com/user-attachments/assets/c753e84c-0f20-41fa-9f28-f9c84eac6e88" width="800" controls="controls"></video>
</div>
<br>
</details>

<details>
<summary><b><big>4A. Built-ins: Echo & Quote Parsing</big></b></summary>
<br>
<i>Proving accurate string parsing, empty quote handling, and consecutive `-n` flag logic.</i><br>
<div align="center">
  <video src="https://github.com/user-attachments/assets/00fbe7d3-858c-4226-8fe4-e5dad06bfbc7" width="800" controls="controls"></video>
</div>
<br>
</details>

<details>
<summary><b><big>4B. Built-ins: Navigation & Directory Traps</big></b></summary>
<br>
<i>Testing absolute/relative `cd` paths, `$OLDPWD` fallbacks, and the deleted working directory trap.</i><br>
<div align="center">
  <video src="https://github.com/user-attachments/assets/52a491e2-2cbe-431d-b2ba-843495e5f535" width="800" controls="controls"></video>
</div>
<br>
</details>

<details>
<summary><b><big>4C. Built-ins: Export, Unset & The PATH Wipe</big></b></summary>
<br>
<i>Validating environment list management, the `+=` append operator, and graceful fallbacks when `$PATH` is removed.</i><br>
<div align="center">
  <video src="https://github.com/user-attachments/assets/a1865026-0347-481e-b041-28b00fb7e477" width="800" controls="controls"></video>
</div>
<br>
</details>

<details>
<summary><b><big>4D. Built-ins: Exit Bounds & SHLVL Depth</big></b></summary>
<br>
<i>Testing `long long` integer overflows, non-numeric syntax errors, and shell depth tracking.</i><br>
<div align="center">
  <video src="https://github.com/user-attachments/assets/a6a6d0f1-623c-4fdf-9958-0c893c079fb6" width="800" controls="controls"></video>
</div>
<br>
</details>

<details>
<summary><b><big>5. Signal Handling & Exit Codes</big></b></summary>
<br>
<i>Verifying interactive terminal states (Ctrl+C, Ctrl+\, Ctrl+D) and strict bash-compliant exit codes (126, 127, 130, 131).</i><br>
<div align="center">
  <video src="https://github.com/user-attachments/assets/e125908c-e950-4d46-9003-2fe485ea8a58" width="800" controls="controls"></video>
</div>
<br>
</details>

---

## 🧪 Testing & Edge Cases

Minishell has an infamous reputation at 42 as the ultimate edge-case simulator. Because it combines string parsing, process state management, and strict memory manipulation, an operation that works perfectly in isolation might crash the shell when combined with something else. **Extensive, outside-the-box testing is highly advised to test the limits of your architecture.**

### 1. The "Side-by-Side" Bash Baseline
The most effective way to guarantee your shell's accuracy is the side-by-side comparison. Open two terminal windows side-by-side: run standard `bash` in one and `./minishell` in the other. Execute the exact same commands, pipeline edge cases, and intentional syntax errors in both. Compare the standard output, standard error messages, and exact exit codes (`echo $?`) to ensure a 1:1 behavioral match. This was the primary testing methodology used to achieve the rigorous accuracy of this repository.

### 2. The State Regression Trap (Signals & Terminal)
A common pitfall is assuming that because a feature works once, it will work forever. For example, `Ctrl+C` might properly cancel your prompt on launch. However, after executing a piped command (`cat | ls`), the child processes alter the terminal attributes and default signal handlers. If the parent process does not explicitly restore the interactive signal handlers after the pipeline finishes, pressing `Ctrl+C` again might instantly kill your shell or cause unintentional bugs. 
* **Always retest basic inputs and signals *after* executing complex commands or failing syntax errors.**

### 3. Memory Leak & File Descriptor Verification (Valgrind)
Just like Pipex, tracking open File Descriptors is essential. Leaving a pipe or a `here_doc` file descriptor open will cause deadlocks or `EMFILE` crashes during massive pipelines. Ensure no unexpected segmentation faults occur when parsing garbage input.
Use the provided `make valgrind` rule, which automatically applies the `readline.supp` suppression file and tracks FDs:
```bash
make valgrind
```
* **Test unclosed quotes:** Do they leak the allocated tokens before the syntax error exit?
* **Test heredoc interruptions:** If you press `Ctrl+C` *inside* a heredoc, does the shell cleanly free the allocated AST and close the temporary pipe?

### 4. "Outside the Box" Edge Cases
Push the parser and executor to their absolute limits to ensure undefined behavior is properly handled:
* **Ambiguous Redirects:** `export A="a b c"`, then try to redirect: `echo hello > $A`.
* **Empty Environment:** Launch the shell via `env -i ./minishell`, then test `cd`, `pwd`, and external commands without `PATH`.
* **Wildcard Edge Cases:** Run `*` in an empty directory, or test complex quoting like `echo "*"`.
* **Complex Quoting:** Check the variable expansion differences between `echo " '$USER' "` and `echo ' "$USER" '`.

### 5. Recommended Test Suites & Third-Party Testers
Do not rely solely on manual testing. I strongly recommend checking out these community resources to aggressively stress-test your implementation:

* **[Minishell Mandatory Test Cases (Notion)](https://www.notion.so/meeru/1d02544e44e2807d9013fd3eefbfebf4):** An incredible manual checklist of mandatory edge cases provided by another 42 student.
* **[LucasKuhn / minishell_tester](https://github.com/LucasKuhn/minishell_tester):** A fantastic automated suite for testing parsing, execution, and builtins.
* **[zstenger93 / 42_minishell_tester](https://github.com/zstenger93/42_minishell_tester):** Excellent for aggressive stress testing and evaluating shell robustness against massive edge cases.

---

### 🚨 The Norm
Moulinette relies on a program called `norminette` to check if your files comply with the 42 Norm. Every single `.c` and `.h` file must pass this check. If there is a norm error, you will receive a 0.

**The 42 Header:**
Before writing any code, every file must start with the standard 42 header. `norminette` will automatically fail any file missing this specific signature.
```c
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:23:54 by maaugust          #+#    #+#             */
/*   Updated: 2026/01/25 00:06:42 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
```

Run the following command in the root of your repository to check all your files at once:
```bash
norminette -R CheckForbiddenSourceHeader srcs/ includes/
```

---

## 📚 Resources & References

**System Manuals:**
* `info readline` / `man 3 history` - Essential manuals for the GNU Readline library. *Note: While history functions (`add_history`) can be found in man pages, the specific readline state functions (`rl_clear_history`, `rl_on_new_line`, `rl_replace_line`, `rl_redisplay`) are exhaustively documented in the GNU `info` system.*
* `man 2 fork` / `man 2 waitpid` / `man 2 execve` - Core system calls for process creation, execution, and synchronization.
* `man 2 pipe` / `man 2 dup2` - Manuals detailing anonymous pipes and file descriptor duplication for routing I/O.
* `man 2 sigaction` / `man 2 signal` - Manuals for handling and overriding terminal signals (`SIGINT`, `SIGQUIT`).
* `man 3 getcwd` / `man 2 chdir` - System calls used for the `cd` and `pwd` built-ins.
* `man 3 opendir` / `man 3 readdir` / `man 2 stat` - Directory traversal and file metadata (vital for wildcard `*` expansion).
* `man 2 unlink` / `man 3 isatty` - Manuals for deleting temporary files (heredocs) and verifying terminal file descriptors.

**Documentation & Manuals:**
* **[GNU Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html):** The ultimate source of truth for shell behavior, expansions, and exit codes.
* **[Bash Man Page](https://man7.org/linux/man-pages/man1/bash.1.html):** Standard Linux manual page for Bash.
* **[Linuxize Bash Tutorials](https://linuxize.com/tags/bash/):** Excellent practical examples of bash scripting and behavior.

**Articles & Guides:**
* **[Binary Tree Data Structure (GeeksforGeeks)](https://www.geeksforgeeks.org/dsa/binary-tree-data-structure/):** The foundational data structure used to build the Abstract Syntax Tree.
* **[Recursive Descent Parser (GeeksforGeeks)](https://www.geeksforgeeks.org/compiler-design/recursive-descent-parser/):** Technical explanation of top-down parsing.
* **[Recursive Descent Parser (Wikipedia)](https://en.wikipedia.org/wiki/Recursive_descent_parser):** Core concepts and theory behind the parsing strategy used in this shell.
* **[Zouhairlrs - Minishell: Building a mini-bash (Medium)](https://medium.com/@zouhairlrs/minishell-building-a-mini-bash-a-42-project-5dc20d671fbb):** Comprehensive guide and architectural breakdown of the 42 Minishell project.
* **[MannBell - Minishell: Building a mini-bash (Medium)](https://m4nnb3ll.medium.com/minishell-building-a-mini-bash-a-42-project-b55a10598218):** Another excellent perspective on task management, testing, and system calls for this project.

**Video Tutorials:**
* **[rj45 Creates - Data Oriented C Compiler](https://www.youtube.com/playlist?list=PLilenfQGj6CFnrwWd0OLJ1wqRun8DtOAS):** Video series covering advanced C programming and compiler design.
* **[CodeVault - Unix Processes in C](https://www.youtube.com/playlist?list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY):** The absolute best video playlist for mastering `fork`, `pipe`, `dup2`, and UNIX process management.
* **[CodeVault - Advanced C Programming](https://www.youtube.com/playlist?list=PLfqABt5AS4FlupIbjomBwloifIqlw1ONI):** Additional high-quality tutorials on C system programming.
* **[Oceano - Let's build a super simple shell in C](https://www.youtube.com/watch?v=yTR00r8vBH8):** A great starting point for understanding the basic `fork`, `execve`, and `wait` loop.
* **[Neso Academy - Top Down Parsers](https://www.youtube.com/watch?v=iddRD8tJi44):** Video explaining the theory and rules behind top-down parsing.
* **[hpp3 - Recursive Descent Parsing](https://www.youtube.com/watch?v=SToUyjAsaFk):** Excellent video tutorial on writing a recursive descent parser.

**42 Standards:**
* **[42 Norm V4](https://cdn.intra.42.fr/pdf/pdf/96987/en.norm.pdf):** The strict coding standard for 42 C projects.
* **[Official 42 Norminette Repository](https://github.com/42School/norminette):** The open-source linter enforcing the strict 42 coding standard.

---

### 🤖 AI Usage & Transparency
In the spirit of transparency and the learning objectives of the 42 curriculum, here is how AI tools were utilized during this project:

* **Repository Review & Refactoring:** AI was used as a sounding board to review the final repository structure, catch edge-case discrepancies (like SIGPIPE error suppression), and verify memory safety inside complex pipeline loops.
* **Documentation & Build Automation:** Assisted in generating comprehensive Doxygen documentation for the header files and structuring this `README.md` layout to provide a clear, detailed, and professional overview of the entire project's architecture.
* **Zero Core Logic Generation:** **No core execution or parsing logic was generated by AI**. The Lexer, Recursive Descent Parser, Abstract Syntax Tree construction, environment expansion engine, pipe concurrency, and built-in implementations were 100% manually researched and written. This strict boundary was maintained to ensure a fundamental, hands-on mastery of UNIX process management and C programming.

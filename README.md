
# Custom Shell Implementation in C



## Project Overview

This project is a custom shell implementation in C, which mimics some of the functionalities found in standard Unix/Linux shells. The shell supports various built-in commands, input handling, and manages processes both in the foreground and background. It is built by integrating several modules, each handling different aspects of the shell's functionality.


## File Structure

1. **main.c**:
   - **Description**: This is the entry point of the shell. It handles user input and orchestrates the execution of commands using the various modules defined in the project.
   - **Functionality**: 
     - Reads user input.
     - Executes commands using the corresponding modules.
     - Also handles aliasing of commands (reads from the file `.myshrc`)

2. **display.c**:
   - **Description**: Manages the shell prompt display and any related output formatting.
   - **Functionality**: 
     - Generates the shell prompt with the format `<Username@SystemName:~>`.
     - Handles special formatting needs.

3. **input.c**:
   - **Description**: Handles input processing, including tokenizing and cleaning up user inputs.
   - **Functionality**: 
     - Removes extra spaces and tabs.
     - Tokenizes the command strings.
     - Validates and sanitizes the input.

4. **hop.c**:
   - **Description**: Implements the `hop` command, which is similar to the `cd` command in Unix/Linux.
   - **Functionality**: 
     - Changes the working directory.
     - Supports flags like `.`, `..`, `~`, and `-`.
    - **Assumptions**: 
      - For commands such as "hop dir -", the previous directory is the path to dir.

5. **reveal.c**:
   - **Description**: Implements the `reveal` command, which lists files and directories in the specified paths.
   - **Functionality**:
     - Displays files in lexicographical order.
     - Supports flags `-a` (for hidden files) and `-l` (for detailed information).
     - If reveal - is called when the previous directory hasn't been set, "oldpwd not set" is printed (even for commands such as reveal -l ; reveal -).
     - Color coding reflects on the entire file/dirname.
    - **Assumptions**: 
      - There is no maximum cap on the number of files in a directory, since this information is calculated and used to allocate memory appropriately.


6. **log.c**:
   - **Description**: Manages a history log of commands executed in the shell.
   - **Functionality**: 
     - Stores commands in a linked list.
     - Provides functionalities like viewing and purging, and executing commands from the log.
     - All commands are stored as they are entered, even erroneous commands.
     - New commands are only saved to the file log.txt if the command "end" is run to close the shell. If the shell is killed with ctrl c or in any other way, it won't be updated.

7. **sys_commands.c**:
   - **Description**: Executes system commands and manages background and foreground processes.
   - **Functionality**:
     - Handles `fork`, `execvp`, and process management.
     - Manages background processes and outputs timing information for long-running commands.
     - '&' does not work for log, reveal, hop etc.; if this is attempted, "invalid input" is entered

8. **proclore.c**:
   - **Description**: Implements the `proclore` command to retrieve process information.
   - **Functionality**:
     - Fetches and displays information like PID, process status, process group, virtual memory, and executable path.
     - If the path to an executable is locked behind permissions, "access to this path is prohibited" is printed for the path and all other info is printed normally.

9. **seek.c**:
   - **Description**: Implements the `seek` command, which finds files or directories based on the specified pattern.
   - **Functionality**:
     - Searches through directories.
     - Supports flags `-d` for directories, `-f` for files, and `-e` for executing or displaying the found items.
     - seek <flags><target> - searches in the directory that was hopped to last.'

10. **io_redirection.c**:
   - **Description**: Allows redirection of input and output using operators like <, >, and >>. This includes handling input from files, and output to files, both for custom and system commands.
   - **Functionality**:
     - Supports `<` (read), `>` (write) and `>>` (append) symbols.
     - Works for both system commands (such as `echo`) and custom commands (such as `hop`).

11. **pipes.c**:
   - **Description**: Implements functionality for piping output from one command into another using the pipe (|) operator.
   - **Functionality**:
     - Redirects the output of each command into the input of the next.
     - Works for both system commands (such as `echo`) and custom commands (such as `hop`)

12. **activities.c**:
   - **Description**: Dislays a list of all processes currently running that were created by the shell, in ascending order of their process IDs.
   - **Functionality**:
     - Displays the pid, command name and state description (whether running or stopped) for all such processes.
     - Displays an error if the `/proc` file of the given pid does not exist (i.e., if the process does not exist or has been terminated).
13. **signals.c**:
   - **Description**: Handles signal events like Ctrl+C (SIGINT), Ctrl+Z (SIGTSTP), and SIGCONT to manage running or stopped processes. It ensures that signals can be sent to either individual processes or process groups.
   - **Functionality**:
     - `ping` command, which allows a signal of the user's choice to be sent to a particular process.
     - Supports Ctrl+C (which interrupts and kills all foreground processes) CTRL+D (which kills all processes and logs out of the shell) and CTRL+Z(which stops all foreground processes, but does not kill them)

14. **fg_bg.c**:
   - **Description**: Implements features to handle foreground and background processes, including the ability to send signals to stop (Ctrl+Z), continue (SIGCONT), or terminate (SIGINT) processes. Background processes can be brought to the foreground, and vice versa.
   - **Functionality**:
     - The command `fg` brings a bacground or stopped process to the foreground and hands it control of the terminal.
     - The command `bg` brings a foreground process to the background.
15. **iMan.c**:
   - **Description**: The iMan command fetches man pages from the internet via sockets. This allows users to view manual pages for commands without needing them locally installed.
   - **Functionality**:
     - Displays the man page of the given system command, fetched from man.he.net
16. **all_functions.h**:  Header file that contains function definitions of all functions used in the project.






## Makefile

The project includes a `Makefile` that automates the compilation process. Running the `make` command in the terminal will compile all the `.c` files into an executable named `a.out`.

- **Usage**:
  - **Compile the project**: `make`
  - **Run the shell**: `./a.out`
  - **Clean the build**: `make clean`



## Assumptions


1. **Input Handling**:
   - The shell assumes that input commands will not contain whitespace characters within filenames or paths unless encapsulated by quotes.
   - The `hop` and `reveal` commands assume the paths provided are valid and exist.

2. **Path Handling**:
   - The shell assumes that the home directory is fixed at `/mnt/c/Users/Nikhil/Desktop/OSN/MP1`, and `~` refers to this directory.
   - Relative paths are resolved based on the current working directory or the previous directory for the `hop` command.

3. **File and Directory Permissions**:
   - The shell assumes that file and directory permissions can be checked using standard Unix permission bits and that these permissions are consistent with typical Unix file system behavior.

4. **Command Execution**:
   - The shell assumes that all system commands are valid and available in the system's `PATH`. Invalid commands are handled by printing an error message.
   - Background processes are managed by the shell, but it assumes that the child processes behave as typical Unix processes, including signaling their termination.

5. **Process Management**:
   - The `proclore`and `activities` commands assume that the `/proc` filesystem is accessible and that process information is formatted as expected in Linux systems.

6. **File System Interaction**:
   - The shell assumes that directories and files can be traversed without any special filesystem constraints, and the `seek` command is optimized for typical file systems.
7. **Input size**:
   - The shell assumes that the size of the input will not be more than 4096 characters



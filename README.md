# [cshell](https://github.com/martinord/cshell)
Based on a coursework on low-level programming of Operating Systems. This is ment to be executed in a Linux environment

## Quick start

### Compile
```bash
make
```

### Execute
```bash
./shell
```

<br>

## Commands
#### Basics

* `autores [-l] [-n]`
 Prints the authors' details

  * `-l` prints the logins
  * `-n` prints the names



  * `pid [-p]`
  Prints the PID of the process executing the shell

    * `-p` prints the PID of the parent process


  * `fin` or  `end` or `exit`
  Ends the shell

#### File systems

  * `info [file1] [file2] ... `
  Givesinformation about the files or directories supplied as arguments.


  *  `recursive [ON | OFF]`
  Enables or disables the recursivity in the behaviour of the command `list`.


  *  `list [-l] [file1] [file2]...`
  Prints the PID of the process executing the shell.
    * `-l` lists the files and directories with more information  

  *  `eliminate [-f] <file*name>`
  Removes a file or a directory.
    * `-f` if it is a directory, it removes all contents


#### Memory

  *  `malloc [-deallocate] [size]`
  Shows the allocated memory using this command. If *size* is specified, the shell allocates *size* bytes of memory, showing the memory address of the allocated memory.
    * `-deallocate` It deallocates a block previously allocated of *size* bytes

  * `mmap [file*name] [permissions]`</summary>
  This command maps a file in memory with the *permissions* in `rwx` format without spaces. If no *file*name* is specified, the allocated files are listed.

  *Example: `--> mmap textfile.txt rw`*

  *  `sharednew [k] [size]`
  Creates a shared memory of key *k* and allocates it in the process address space. If no arguments are specified, `sharednew` will list the addresses allocated.


  *  `shared [-deallocate] [k]`
  Gets the shared memory of key *k* and allocates it in the process address space. If not arguments are specified, `shared` will display the addrsses allocated.
    *  `-deallocate` It deallocates a block previously allocated with key *k*

  *  `rmkey <k>`
  Removes the shared memory region with key *k*. It is not produced any deallocation of memory.


  *  `allocation`
  Shows all allocated memory.


  * `deallocate [addr]`
  Deallocates the memory allocated in the address *add*.

  * `mem`
  Prints the memory addrsses of three program functions.


  * `memdump <address> [count]`
  Prints the contents of a memory address. If *count* is specified, it prints *count* bytes.

  * `recursivefunction <n>`
  Function that shows how the recursivity works. This function is calling itself *n* times.

  * `read <file> <addr> [count]`
  Reads *count* bytes of file *file* into the memory address *addr*. If *count* is not specified, the whole archive is read.

  * `write <file> <addr> <count> [-o]`
  Writes to the file *file* the contents of address *addr*. The amount of bytes written are specified in *count*.
    * `-o` If the file exists, overwrite it.

#### Processes

  * `setpriority <pid> [value]`
  Sets the priority of the process with PID *pid* to *value*. If value is not specified, `setpriority` will show the priority of the process with PID *pid*.

  * `fork`
  The shell creates a child process executing the same code: it creates a child shell. The parent shell waits for the child to terminate.

  * `searchlist [+<dir> | -path | <exfile>]`
  Lists the directories where the shell searches for executable files.
    * `+<dir>` It adds a directory to the search list.
    * `-path` The directories in the environment variable PATH are added to the list.
    * `<exfile>` Searhes for *exfile* in the search list and shows the complete path where the executable file is.

  * `exec <program> [arg1 arg2 ...] [@<priority>]`
  Replaces the shell code and executes the program *program* with priority (if specified) *priority*.

  * `<program> [arg1 arg2 ...] [@<priority>]`
  Executes the program *program* in the foreground with an optionally specified priority.

  * `background [arg1 arg2 ...] [@<priority>]`
  Executes the program *program* in the background with an optionally specified priority.

  * `jobs`
  Lists the processes running in background of the shell.

  * `proc [pid]`
  Shows the information of the process with PID *pid*.

  * `clearjobs`
  Empties the list of background processes.


## Licence

Source code can be found on [github](https://github.com/martinord/cshell), licenced under [GPL-3.0](https://opensource.org/licenses/GPL-3.0).

Developed by [Martiño Rivera](https://github.com/martinord) and [Carmen Corrales]()

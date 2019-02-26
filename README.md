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
<details>
  <summary> `autores [-l] [-n]`</summary>
  It prints the authors' details
  <list>
    <li> `-l` prints the logins </li>
    <li> `-n` prints the names </li>
  </list>
</details>

<details>
  <summary> `pid [-p]`</summary>
  Prints the PID of the process executing the shell
  <list>
    <li> `-p` prints the PID of the parent process </li>
  </list>
</details>

<details>
  <summary> `fin` or  `end` or `exit`</summary>
  Ends the shell
</details>

#### File systems
<details>
  <summary> `info [file1] [file2] ... `</summary>
  <p>Givesinformation about the files or directories supplied as arguments.</p>
</details>

<details>
  <summary> `recursive [ON | OFF]`</summary>
  Enables or disables the recursivity in the behaviour of the command `list`.
</details>

<details>
  <summary> `list [-l] [file1] [file2]...`</summary>
  Prints the PID of the process executing the shell
  <list>
    <li> `-l` lists the files and directories with more information  </li>
  </list>
</details>

<details>
  <summary> `eliminate [-f] <file_name>`</summary>
  Remove a file or a directory.
  <list>
    <li> `-f` if it is a directory, it removes all contents </li>
  </list>
</details>

#### Memory
<details>
  <summary> `malloc [-deallocate] [size]`</summary>
  It shows the allocated memory using this command. If _size_ is specified, the shell allocates _size_ bytes of memory, showing the memory address of the allocated memory.
  <list>
    <li> `-deallocate` It deallocates a block previously allocated of _size_ bytes </li>
  </list>
</details>

<details>
  <summary> `mmap [file_name] [permissions]`</summary>
  <p> This command maps a file in memory with the _permissions_ in `rwx` format without spaces. If no _file_name_ is specified, the allocated files are listed. </p>
  Example: `--> mmap textfile.txt rw`
</details>

<details>
  <summary> `sharednew [k] [size]`</summary>
  It creates a shared memory of key _k_ and allocates it in the process address space. If no arguments are specified, `sharednew` will list the addresses allocated.
</details>

<details>
  <summary> `shared [-deallocate] [k]`</summary>
  It gets the shared memory of key _k_ and allocates it in the process address space. If not arguments are specified, `shared` will display the addrsses allocated.
  <list>
    <li> `-deallocate` It deallocates a block previously allocated with key _k_ </li>
  </list>
</details>

<details>
  <summary> `rmkey <k>`</summary>
  Removes the shared memory region with key _k_. It is not produced any deallocation of memory.
</details>

<details>
  <summary> `allocation`</summary>
  It shows all allocated memory.
</details>

<details>
  <summary> `deallocate [addr]`</summary>
  Deallocates the memory allocated in the address _add_.
</details>

<details>
  <summary> `mem`</summary>
  Prints the memory addrsses of three program functions.
</details>

<details>
  <summary> `memdump <address> [count]`</summary>
  Prints the contents of a memory address. If _count_ is specified, it prints _count_ bytes.
</details>


## Licence

Source code can be found on [github](https://github.com/martinord/cshell), licenced under [GPL-3.0](https://opensource.org/licenses/GPL-3.0).

Developed by [Martiño Rivera](https://github.com/martinord) and [Carmen Corrales]()

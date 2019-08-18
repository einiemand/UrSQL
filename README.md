# UrSQL
UrSQL is a Relational DataBase Management System(RDBMS) written in C++ and supports a subset functionalities of MySQL. 
## Getting Started
### Prerequisites
* Windows 10
    * Option 1 (not recommended, MinGW has issues with C++17)
        * cmake
        * MinGW 64-bit
    * Option 2
        * Visual Studio 2017 15.7 (MSVC 19.14) or higher
* Ubuntu Linux 16.04 LTS or higher
    * cmake
    * gcc 7 or higher

Note: not tested on other platforms
### Build
Clone this repo
```
$ git clone https://github.com/einiemand/UrSQL.git
```
* Windows

    * If you have `Visual Studio` installed, simply opening the `.sln` file should be enough. Or you can create project files with cmake.
    * If you are building with `MinGW`, make sure to specify the compiler with
```
$ cmake -G "MinGW Makefiles" path/to/root
```
* Linux
```
$ mkdir build && cd build
$ cmake ..
$ make
$ cd ../bin && ./ursql
```

Note: The build type is Release and `-O3` is used by default. To switch to Debug and help me find bugs, you might want to do
```
$ cmake -DCMAKE_BUILD_TYPE=Debug ..
```
It will show error messages in terminal before crashing.
## Use
UrSQL can be run in 2 ways
* If no additional command-line argument is given, you will enter interactive mode and commands must be typed in command prompt.
* If you already have a file that contains several SQL commands, you can run them all by setting the second argument to be the path to that file.
```
$ ./ursql path/to/file
```
## Syntax
Each command must end with a semicolon.
* Basic commands

Show help information
```
$ ursql> help;
```
Show UrSQL version
```
$ ursql> version;
```
Shut down UrSQL
```
$ ursql> quit;
```
Note: This is the only safe way to exit UrSQL, otherwise not all the changes will be saved (sorry).
* Database-related commands

Show all databases
```
$ ursql> show databases;
```
Create a database
```
$ ursql> create database <dbname>;
```
Drop a database
```
$ ursql> drop database <dbname>;
```
Select a database to use
```
$ ursql> use <dbname>;
```
Describe a database
```
$ ursql> desc database <dbname>;
$ ursql> describe database <dbname>;
```
* Table-related commands

Show all tables in the current database
```
$ ursql> show tables;
```
Describe a table
```
$ ursql> desc table <tbname>;
$ ursql> describe table <tbname>;
```
Create a table
```
$ ursql> create table <tbname>(<colname> <type> [<option>], ...);
``` 
Drop a table
```
$ ursql> drop table <tbname>;
```
* SQL commands

Select rows
```
$ ursql> select <colname>, ... from <tbname> [where <condition>, ... order by <colname>];
```
Insert rows
```
$ ursql> insert into <tbname>(<colname>, ...) values(<value>, ...), ...;
```
Update rows
```
$ ursql> update <tbname> set <colname> = <value> [where <condition>...];
```
Delete rows
```
$ ursql> delete from <tbname> where <condition>...
```
## Example
An example file is located in `example` folder. Run it by
```
$ cd bin
$ ./ursql ../example/ursql_example.sql
```
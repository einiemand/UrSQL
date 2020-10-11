# UrSQL
UrSQL is a Relational DataBase Management System(RDBMS) written in C++ and supports a subset functionalities of MySQL. 
## Getting Started
### Prerequisites
* Windows 10
    * Option 1 (not recommended, MinGW has issues with C++17)
        * cmake
        * MinGW 64-bit
    * Option 2
        * Visual Studio 2017 15.7 (MSVC 19.14) or later
* Ubuntu Linux 16.04 LTS or later
    * cmake
    * gcc 7.0 or later

Note: not tested on other platforms
### Build
Clone this repo
```
$ git clone https://github.com/einiemand/UrSQL.git
```
#### Windows
* If you have `Visual Studio` installed, simply opening the `.sln` file should be enough. Or you can create project files with cmake.
* If you are building with `MinGW`, make sure to specify the compiler with
```
$ cmake -G "MinGW Makefiles" path/to/root
```
#### Linux
* Simply run the shell script
```
$ ./unix_build.sh
```
* It will run `cmake` and `make`. The executable will be put in `<UrSQL>/bin`.  
* Note: The build type is Release and `-O3` is used by default in `CMakeLists.txt`. If you use the script to build, the build type will be Debug. It will show error messages in terminal if something wrong happens before UrSQL crashes.


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
On my machine (gcc 7.4.0, Ubuntu 18.04 LTS amd64), the results are
```
einiemand@einiemand-hs:~/Repos/UrSQL/bin$ ./ursql ../example/ursql_example.sql

ursql> create database ursqldb;
Query OK, database 'ursqldb' created (0.00 sec)

ursql> use ursqldb;
Database changed (0.00 sec)

ursql> create table userinfo (id integer primary key auto_increment, name varchar not null, age int not null, address varchar);
Query ok, table 'userinfo' created (0.00 sec)

ursql> show tables;
+-------------------+
| Tables_in_ursqldb |
+-------------------+
| userinfo          |
+-------------------+
1 row(s) in set (0.00 sec)

ursql> desc table userinfo;
+---------+---------+------+-----+---------+-------+
| Field   | Type    | Null | Key | Default | Extra |
+---------+---------+------+-----+---------+-------+
| id      | int     | NO   | PRI | NULL    |       |
| name    | varchar | NO   |     | NULL    |       |
| age     | int     | NO   |     | NULL    |       |
| address | varchar | YES  |     | NULL    |       |
+---------+---------+------+-----+---------+-------+
4 row(s) in set (0.00 sec)

ursql> insert into userinfo (name, age, address) values ('user1', 24, "mirage"), ("another_user", 18, 'inferno');
Query ok, 2 row(s) inserted (0.00 sec)

ursql> select * from userinfo order by id;
+----+--------------+-----+---------+
| id | name         | age | address |
+----+--------------+-----+---------+
| 0  | user1        | 24  | mirage  |
| 1  | another_user | 18  | inferno |
+----+--------------+-----+---------+
Query ok, 2 row(s) affected (0.00 sec)

ursql> select name, address, age from userinfo where age = 18 and address == 'inferno';
+--------------+---------+-----+
| name         | address | age |
+--------------+---------+-----+
| another_user | inferno | 18  |
+--------------+---------+-----+
Query ok, 1 row(s) affected (0.00 sec)

ursql> delete from userinfo where id = 1;
Query ok, 1 row(s) affected (0.00 sec)

ursql> select * from userinfo;
+----+-------+-----+---------+
| id | name  | age | address |
+----+-------+-----+---------+
| 0  | user1 | 24  | mirage  |
+----+-------+-----+---------+
Query ok, 1 row(s) affected (0.00 sec)

ursql> insert into userinfo (name, age) values ('Lanaya', 55), ("Mirana", 66), ("Kunkka", 22), ("Kalashnikov", 100);
Query ok, 4 row(s) inserted (0.00 sec)

ursql> select * from userinfo;
+----+-------------+-----+---------+
| id | name        | age | address |
+----+-------------+-----+---------+
| 0  | user1       | 24  | mirage  |
| 2  | Lanaya      | 55  | NULL    |
| 3  | Mirana      | 66  | NULL    |
| 4  | Kunkka      | 22  | NULL    |
| 5  | Kalashnikov | 100 | NULL    |
+----+-------------+-----+---------+
Query ok, 5 row(s) affected (0.00 sec)

ursql> select name from userinfo where 30 < age order by name;
+-------------+
| name        |
+-------------+
| Kalashnikov |
| Lanaya      |
| Mirana      |
+-------------+
Query ok, 3 row(s) affected (0.00 sec)

ursql> update userinfo set age = 80 where age > 80;
Query ok, 1 row(s) affected (0.00 sec)

ursql> select * from userinfo;
+----+-------------+-----+---------+
| id | name        | age | address |
+----+-------------+-----+---------+
| 0  | user1       | 24  | mirage  |
| 2  | Lanaya      | 55  | NULL    |
| 3  | Mirana      | 66  | NULL    |
| 4  | Kunkka      | 22  | NULL    |
| 5  | Kalashnikov | 80  | NULL    |
+----+-------------+-----+---------+
Query ok, 5 row(s) affected (0.00 sec)

ursql> drop table userinfo;
Query ok, 5 row(s) affected (0.00 sec)

ursql> drop database ursqldb;
Query OK, database 'ursqldb' dropped (0.00 sec)

ursql> quit;
Bye (0.00 sec)
```

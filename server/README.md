# Building CoopAndras Server

## 1 Tools

CMake (at least 3.24)  
GCC/LLVM Compiler (or VS++ Compiler)  
Git  
GNU Make (or Ninja)  

## 2 Libraries

**enet** library  

## 3 Building

### For Microsoft Windows

#### * Step 1
clone CoopAndreas Repository by doing this command :  
``git clone https://gihub.com/Tornamic/CoopAndreas.git``

#### * Step 2
go to server folder in repository and do this commands :  

```bash
mkdir build
cmake -S . -B build
cd build
```
#### * Step 3
after **Step 2** do this command 
```bash
make
```
then open  ``CoopAndreasServer.exe`` and enjoy

### For GNU/Linux


#### * Step 1
clone CoopAndreas Repository by doing this command :
``git clone https://gihub.com/Tornamic/CoopAndreas.git``

#### * Step 2
go to server folder in repository and do this commands :

```bash
mkdir build
cmake -S . -B build
cd build
```
#### * Step 3
after **Step 2** do this command 
```bash
make
```
then open  ``./CoopAndreasServer`` in terminal and enjoy
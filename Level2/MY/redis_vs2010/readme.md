Redis + LibUV prototype README
===
Special thanks to Dušan Majkic (dmajkic, https://github.com/dmajkic/redis/) for his project on GitHub that gave us the opportunity to quickly learn some on the intricacies of Redis code. His project also helped us to build our prototype quickly.

### Get source repository
First clone the Redis sources from https://github.com/antirez/redis.

- On your computer create a working folder and cd into it.
- Clone antirez/redis repository:

        git clone https://github.com/antirez/redis.git

    Note: on Windows this may take 2 minutes as the jemalloc folder contains an invalid file name (prn.h). This folder is not used for Windows, but the file may cause git to timeout.

- Switch to commit 3fac86ff1d (the patch is based on this commit):

        cd redis
        git checkout 3fac86ff1d

    This command will cause HEAD to reference 3fac86ff1d commit directly, creating detached HEAD state.  The next step will create a new branch, which will attach HEAD to that branch.  Warnings about detached HEAD could be ignored.

- Create a new branch:

        git checkout -b 2.4_win_uv

### Apply the patch to support Windows using Libuv

- Download and apply patch on this branch. This enables Redis to build for Windows. The patch includes a full copy of Libuv sources (the version included in the patch is different than the one available on github, some changes have been added to the code).

    The patch is included in this gist, and can be downloaded [here](https://raw.github.com/gist/1439660/d729b823a7ef50ef8ba54393675fb678e740ca4b/redis24_win_uv.patch).

    Assuming you have a copy of the patch ([redis24_win_uv.patch](https://raw.github.com/gist/1439660/d729b823a7ef50ef8ba54393675fb678e740ca4b/redis24_win_uv.patch)) do:

        git am redis24_win_uv.patch

    The following warnings can be ignored:

        warning: squelched 210 whitespace errors
        warning: 215 lines add whitespace errors.

    Alternatively use curl to download the patch and apply it directly with 'git am':

`curl https://raw.github.com/gist/1439660/d729b823a7ef50ef8ba54393675fb678e740ca4b/redis24_win_uv.patch | git am`


### How to build Redis

Once the patch is applied, Redis can be built in 3 different ways:

- Windows:
  - With libuv backend using using Visual Studio

- *nix:
  - With libuv backend
  - With existing backend (default)

## Redis on Windows

### Build pthreads library using Visual Studio

You can use the free Express Edition available at http://www.microsoft.com/visualstudio/en-us/products/2010-editions/visual-cpp-express.

Redis uses pthreads. The build assumes it is linked as a static library. 
You need to get Windows pthreads sources and build as a static library.

1. You can download the code from ftp://sourceware.org/pub/pthreads-win32/pthreads-w32-2-8-0-release.exe.

2. Extract the files to a folder on your local drive.

3. From your local folder, copy all the files from Pre-built.2/include to your redis folder under 
deps/pthreads-win32/include (the directory needs to be created).

4. From Visual Studio, do Open Project, and navigate to the installed folder, and then pthreads.2.

5. Open the pthread project file (pthread.dsp).

6. VS will ask to convert to the current version format. Select Yes.

7. Open project properties and change the following:

    - Change configuration type from Dynamic Library (.dll) to Static Library (.lib)
    - Change Target Extension for .dll to .lib
    - Under C-C++/Code Generation, change Runtime Library choice to Multi-threaded (/MT)

8. Build the project. This will create a pthread.lib under pthreads.2 directory.

9. Copy pthread.lib to your redis folder under deps/pthreads-win32/lib/release|debug (you have to create the directories).

###Build Redis using Visual Studio

- Open the solution file msvs\redisserver.sln in Visual Studio 10, and build.

    This should create the following executables in the msvs\$(Configuration) folder:

    - redis-server.exe
    - redis-benchmark.exe
    - redis-cli.exe
    - redis-check-dump.exe
    - redis-check-aof.exe

    If there is an error in finding the pthreads.lib file, check that you have selected the same configuration as used for building the pthreads.lib file, and that you copied it to the expected location.

## Build Redis for Linux
- With libuv backend:

        make USE_LIBUV=yes

- With default backend:

        make

### Release Notes

This is a pre-release version of the software and is not fully tested.  
This is intended to be a 32bit release only. No work has been done in order to produce a 64bit version of Redis on Windows.
To run the test suite requires some manual work:

- The tests assume that the binaries are in the src folder, so you need to copy the binaries from the msvs folder to src. 
- The tests make use of TCL. This must be installed separately.
- To run the tests in a command window: ‘tclsh8.5.exe tests/test_helper.tcl’.

Running the tests sometimes results in failures, and sometimes passes. We haven’t spent too much time investigating the failures but they could be related with issues with TCL on Windows.
Persisting the Redis data to disk should be done as a background activity. Currently in Windows this is done as a foreground activity and may cause delays in responding to other commands when it is running.
If you want to use a client on the same machine as redis-server on Windows, you can use a named pipe instead of a socket for client-server communication.

- To do this a configuration file must be provided to the server.
- To specify a pipe named 'redis.sock' in the configuration file set the following:

        port 0
        unixsocket \\.\pipe\redis.sock

### TODO

- Snapshotting (Fork and Write) is not perfect, right now we simply block requests while memory is dumped on disk. We are working on a solution that will give us better performance. An update will be released soon.

# README

Please have a look at the
[docs repository](https://git.chaotikum.org/ReadMailReallyFast/docs) if you wish
to know some of the internals or like to consult further documentation.

## compiling
 In order to compile rmrf you need to invoke
 <code>make all</code>
 This requires the following:
  * an POSIX compatible OS
  * an C++17 compatible compiler
  * libncurses-dev (version >= 6)
  * libopenssl
  * libev-dev
  * boost (i.e. libboost-test-dev if you plan to run the tests)
  * lua or python if you plan to use scripting

 There is no <code>./configure</code> nor <code>make install</code> yet.

## List of go dependancies
  * https://github.com/rthornton128/goncurses

### List of dependant FreeBSD packages
  * ncurses
  * devel/boost-all
  * lang/lua53
  * For testing: devel/lua-lunit


### List of Linux specific dependancies
  * libnl3

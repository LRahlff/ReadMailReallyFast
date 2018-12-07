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
  * libncurses-dev
  * libopenssl
  * boost (as long as you plan to run the tests)
  * lua or python if you plan to use scripting

 There is no <code>./configure</code> nor <code>make install</code> yet.
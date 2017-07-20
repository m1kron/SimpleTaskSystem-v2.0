/////////////////////////////////////////////////////
//                                                 //
//         SIMPLE TASK SYSTEM 2.0                  //
//                                                 //
/////////////////////////////////////////////////////


Simple Task System - a C++ library for parallel programming on multi-core processors.

Main features of the library includes:
- auto-balancing based of amount of work and number of available cores
- dynamic parallelism - ability to create and schedule new tasks from already-being-executed ones
- base support for building dependency graphs
- ability to suspend and later resume task execution at arbitrary points
- implementation of base parallel algorithms - parallel for and parallel sort


For more information, please visit: https://pkowalczyk.wordpress.com/2017/07/16/simple-task-system-2-0.


This repositorty is organised as follows:

*libs\simpleTaskSystem contains source code of Simple Task System.
*libs\commonLib contanins project with common stuff( like asserts or logs ) needed by other projects.
*libs\basicThreadingLib contains implementation of threading primitives used by Simple Task System, like atomic, mutex, thread etc. This library was mostly done for educational purposes.

*unitTests contains unit tests of the libraries above. All tests are based on Google Test framework. 
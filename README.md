# kirin
Kirin is a lightweight framework for multithreading and asynchronous programming. All codes are implemented using C/C++.

## Introduction ##
* **common**     
The codes under folder common are some fundamental functions for the framework. It covers string utils, singleton functions, etc.

* **timer**      
The implementation of timer using epoll can be found in this folder. Test cases are provided for a deep understanding of the timer.
**Note:** The timer service employs the cpu tick instead of using system calls, such as time() or gettimeofday(), thereby removing the requirement of explicitly switching processes from user space to kernel space.

* **message**   
Message is used to distinguish between different purposes and actions. A new action may implement the class message_base.

* **async**     
Asynchronous actions and context are defined in this folder.

* **job**       
The working thread and thread pool are implemented in this folder. Please refer to the test cases for more details.

* **manager**   
Everyone can easily implement their multithreading and asynchronous actions using the working manager in this folder. A simple example is provided herein.

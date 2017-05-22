# kirin
Kirin is a lightweight framework for multithreading and asynchronous programming. All codes are implemented using C/C++.

## Introduction ##
* **common**     
All files in this folder are some fundamental functions or utils for the framework. It covers string utils, singleton functions, etc.

* **timer**      
The implementation of timer using epoll can be found in this folder. Test cases are provided for a deep understanding of the timer.   
**Note:** The timer service employs the cpu tick instead of using system calls, such as time() or gettimeofday(), thereby removing the requirement of explicitly switching processes from user space to kernel space. However, every coin has two sides. The drawback is that some extra global argumets are introduced.

* **message**   
Message is used to distinguish between different purposes and actions. A new action may implement the class message_base.

* **async**     
Asynchronous actions and context are defined in this folder.

* **job**       
The working thread and thread pool are implemented in this folder. Please refer to the test cases for more details.

* **manager**   
Everyone can easily implement their multithreading and asynchronous actions using the working manager in this folder. A simple example is provided herein.

* **memory**   
A memory pool is implemented in the folder 'memory'. The framework uses reference technology to avoid frequently new/delete or malloc/free memories.


problem: 
1. Implement a system timer that can be use by user program (timer_create, timer_settime, timer_delete, hw_timer_isr)
2. Is there limitations user have to know?

give: 
//it reset hardware timer, provdie by hardware
API hw_set_timer(int relevate_timeout_ms);
the hardare timer only record a time, each hw_set_timer will override the record time.



Answer:
Summary of Key Considerations for Users:
1. Timer precision depends on hardware limitations and software implementation, which may not support high-resolution timers in all cases.
2. Thread creation overhead becomes significant if too many timers expire frequently. Consider using thread pools or other mechanisms to reduce this overhead.
3. Concurrency issues such as race conditions can occur when multiple threads access the timer list. Proper synchronization mechanisms are essential.
4. Data structure limitations: The time complexity of operations on std::multimap is acceptable for smaller timer sets but may become a bottleneck for large-scale systems.
5. Handling repetitive timers could lead to time drift unless you carefully manage how expiration times are recalculated.
6. System resource limitations such as memory, file descriptors, and CPU usage could be overwhelmed by too many timers or frequent thread creation.
7. Interrupt handling: If many timers expire at once, interrupt storms may occur, leading to system instability or performance issues.

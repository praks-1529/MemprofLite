MemprofLite
===========

This is a memory profiler that intercepts every malloc/free calls and records them. 
This can be used to profile both C/C++ applications. You just need to link to the 
shared library and you are ready to profile your application. If you do not want to 
profile your application always, then you can use appropriate API's to start/stop 
the profiler.

Overview:
This is a memory profiler to track the memory allocations in your product. 
Whenever you link  this library in your program and turn the profiler ON, all 
the malloc() calls are intercepted by this profiler and are being tracked. If you 
are using any memory manager already in your product and still want to use this 
profiler, you can do it. However that may need modifying a part of the code and 
re-compiling the code. Also the library is thread safe and helps you track the 
memory consumed in each thread. 

Linking:
To link the profiler in to your product add -L<path_to_library> -lmemproflit in 
your link line. You also have to use -lpthread -ldl -lbfd -liberty to you link 
line if your product doesn't have it already as this library needs it

Usage model 1:
Use API's as below 

/* Product code */
.
.
memproflite_start();
.
.
/* Product code to be tracked */
.
.
memproflite_stop()

Usage model 2:
If the memproflite library was compiled using START_STATIC, then there is no need to add above 
memproflite_(start()|stop()) API's to start/stop profiling. This usage model is targeted to 
those who do not want to make any code changes to their application. 

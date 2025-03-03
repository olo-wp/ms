# Finished process server (fps) for Minix operating system

-----

This server allows tracking of finished processes in Minix, which does not support that by default. It implements two interfaces:

1. proc_watch.h (not blocking)
   - int watch_exit(endpoint_t ep); \
   express interest of process with endpoint ep.
   - int cancel_watch_exit(endpoint_t ep); \
   cancel interest
   - int query_exit(endpoint_t *epp); \
   puts one of finished processes endpoint into *epp and returns, how many more processes we were interested in finished, but where not returned by this function.
2. proc_lock.h (blocking)
   - int wait_exit(pid_t p); \
   wait for process p to end.

### How to use ?

You should have ssh configured on your Minix machine.

1. copy src to /usr on your Minix machine
2. copy proc_lock.h, proc_watch.h, tst.h to /usr/include
3. make | make install in both /usr/src/include and /usr/src/lib (might take a while the first time)
4. make install in /usr/src/releasetools
5. service up /usr/sbin/fps

proc_watch.h is available for system processes only, while proc_lock.h is available for all.
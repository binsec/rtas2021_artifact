wait(e1)                          wait on event e1
wait(e1 | e2 | e3)                wait on events e1, e2 or e3
wait(e1 & e2 & e3)                wait on events e1, e2 and e3
wait(10.0, SC_NS, e1)             wait on event e1,                               timeout after 10.0 ns
wait(10.0, SC_NS, e1 | e2 | e3)   wait on events e1, e2 or e3,                               timeout after 10.0 ns
wait(10.0, SC_NS, e1 & e2 & e3)   wait on events e1, e2 and e3,                               timeout after 10.0 ns
wait(SC_ZERO_TIME)                wait one delta cycle

e1.notify()                       notify instantly
e1.notify(SC_ZERO_TIME)           notify next delta cycle
e1.notify(10.0, SC_NS)            notify in 10.0 ns
e1.notify(t)                      notify in t (of type sc_time)

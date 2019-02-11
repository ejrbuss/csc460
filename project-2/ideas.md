# RTO Ideas

## Day One

```c
struct Task {
    void * state;    // Freed at end if not NULL
    int instance;    // Int id, who I am, different for every Task
    int period;      // Dynamic period in ms
    int offset;      // Dynamic offset
    int compute_avg; // Dynamically updated average compute time
    int (*fn)(Task * self);
};

int tast_run(Task * self) {
    return TRUE; // schedule me again!
    return FALSE; // I am done
}

struct Listener {
    int (*immediate)(Listener * self);
    int (*when_ready)(Listener * self);
    // ...
};

// Provide profiling

// Mask all interrupts
// On idle > min_idle_for_interrupts
// Check masked interrupt flags 
// Dispatch activatd interrupt

// Alternative
// Disable interrupts during tasks
// Renable after

schedule_once(task);
schedule(task);
listen(message, listener);
```

## Day Two
```c

// Urgency, how to deal with input that needs attention immediately

Task * task        = get_task();
task->fn           = my_fun;
task->state        = malloc(/*...*/);
task->compute_time = 1;
// Periodic (offset, period)
schedule(task, 30, 100);
// After a delay
schedule_once(task, 30);
// Schedule as soon as possible
schedule_now(task);

// Use pool allocator for tasks

// Calculate schedule once? How feasible is this?

// Recursive scbeduling of one shot tasks?
int handler(Task * self) {
    return TRUE; // One shot task returning TRUE would mean re-activate
}

```

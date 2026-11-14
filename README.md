# CampusCompute

CampusCompute is a C-based operating-systems mini-project demonstrating job
management, CPU scheduling, synchronization, inter-process communication,
process monitoring, resource limits, and deadlock avoidance on Linux/Ubuntu
or WSL.

## Features

The CLI provides:

1. Add Job
2. Display Jobs
3. FCFS Scheduling
4. Round Robin Scheduling
5. Non-preemptive Priority Scheduling
6. Banker's Algorithm safety check
7. Concurrent producer-consumer logger
8. Shared-memory IPC with named POSIX semaphores
9. Normal worker completion and controlled simulated worker failure
10. `RLIMIT_NOFILE` resource-limit demonstration
11. Exit

Jobs have a nonnegative process ID, arrival time, and priority, and a positive
burst time. Duplicate process IDs are rejected. Numeric inputs are read as
complete lines; malformed input does not leave trailing characters to corrupt
the next prompt. Job times are also bounded so the worst-case accumulated
scheduling timeline fits in the schedulers' integer time fields.

## Scheduling behavior

- **FCFS:** Runs jobs in arrival order. Equal arrival times retain the order in
  which jobs were added.
- **Round Robin:** Runs each ready job for at most the entered positive time
  quantum. Jobs are visited in arrival order (insertion order for ties). Jobs
  arriving during a time slice are added to the ready queue before an
  unfinished job at the end of that slice is re-queued. CPU idle periods are
  shown.
- **Priority:** Non-preemptive; among arrived jobs, the smallest numeric
  priority value runs first. Equal priorities retain insertion order. The
  timeline includes CPU idle periods.

Each scheduler reports completion time (CT), turnaround time (TAT), waiting
time (WT), and averages. Scheduling operates on a copy of the job list, so
running a scheduler does not overwrite the jobs' original status fields.

## Other demonstrations

- **Banker's Algorithm:** Accepts process and resource dimensions, Allocation,
  Maximum, and Available matrices/vectors; derives Need and reports either a
  safe sequence or the processes that cannot finish.
- **Concurrent logger:** A producer thread records its successful enqueue
  activity as sequence-numbered events in a fixed-size bounded queue. A
  consumer thread logs them. Mutex-protected condition variables coordinate
  full and empty queue states. A stop item terminates the consumer before
  synchronization objects are destroyed.
- **Shared-memory IPC:** Parent and child exchange a message through a
  POSIX shared-memory object. Two named semaphores coordinate request and
  response. Names are unique to the process, and the demonstration unmaps,
  closes, and unlinks only the IPC resources it created.
- **Worker process:** The parent uses `fork()` and `waitpid()` to demonstrate
  normal completion and a deliberate, controlled nonzero child exit. The
  latter is explicitly labeled a simulated worker failure; it is not an
  actual system failure.
- **Resource limit:** Temporarily configures `RLIMIT_NOFILE`, opens
  `/dev/null` descriptors until the operating system reports `EMFILE`, closes
  those descriptors, and restores the original limit.

## Modules

| Files | Responsibility |
| --- | --- |
| `src/job.c`, `src/job.h` | Job data, validation, initialization, and display |
| `src/fcfs.c`, `src/fcfs.h` | First-Come, First-Served scheduler |
| `src/rr.c`, `src/rr.h` | Round Robin scheduler |
| `src/priority.c`, `src/priority.h` | Non-preemptive priority scheduler |
| `src/banker.c`, `src/banker.h` | Banker's safety algorithm |
| `src/logger.c`, `src/logger.h` | Bounded producer-consumer event logger |
| `src/ipc.c`, `src/ipc.h` | POSIX shared-memory and semaphore demonstration |
| `src/worker.c`, `src/worker.h` | Child-process completion/failure demonstration |
| `src/resource_limit.c`, `src/resource_limit.h` | `RLIMIT_NOFILE` demonstration |
| `src/input.c`, `src/input.h` | Strict line-based integer input |
| `src/main.c` | Main menu and integration |
| `Makefile` | Build and clean targets |

## Build and run

Requirements: GCC, GNU Make, Linux/Ubuntu/WSL, POSIX threads, POSIX shared
memory/semaphores, `fork()`/`waitpid()`, `/proc/self/fd`, and resource-limit
support. On current Linux systems `-pthread` and `-lrt` link the required
interfaces; newer glibc versions may provide the real-time interfaces directly
from libc.

From the project root:

```sh
make
./build/campuscompute
```

The build uses `-std=c11 -Wall -Wextra -Wpedantic -O2`. `make clean` removes
the generated `build/` directory.
The equivalent explicit GCC build (including every module and required
libraries) is:

```sh
mkdir -p build
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc \
  src/main.c src/input.c src/job.c src/fcfs.c src/rr.c src/priority.c \
  src/banker.c src/logger.c src/ipc.c src/worker.c src/resource_limit.c \
  -pthread -lrt -o build/campuscompute
```

## Testing scenarios

Start the application and choose each menu option independently. Example
checks:

- **Jobs and schedulers:** Add PID `1`, arrival `0`, burst `5`, priority `2`;
  add PID `2`, arrival `2`, burst `3`, priority `1`. Try adding PID `1` again
  and confirm rejection. Run FCFS, Round Robin (quantum `2`), and Priority;
  results should include the idle-free timelines and per-job CT/TAT/WT.
  Repeat with a job arriving after time `0` to check idle timeline entries.
- **Banker's safe example:** 5 processes, 3 resources. Allocation rows:
  `0 1 0`, `2 0 0`, `3 0 2`, `2 1 1`, `0 0 2`. Maximum rows:
  `7 5 3`, `3 2 2`, `9 0 2`, `2 2 2`, `4 3 3`. Available: `3 3 2`.
  This should report a safe state.
- **Banker's unsafe example:** 2 processes, 1 resource. Allocation: `1`, `1`;
  Maximum: `2`, `2`; Available: `0`. This should report an unsafe state.
- **Logger:** Select option 7 and confirm all events are printed and the
  termination message appears.
- **IPC:** Select option 8 and confirm the child-reply message is printed;
  repeating the option should still work after IPC cleanup.
- **Worker:** Select option 9 and confirm normal exit and the separately
  labeled controlled simulated failure are both detected by the parent.
- **Resource limit:** Select option 10 and confirm `EMFILE` is observed and
  the original limit is restored.
- **Input handling:** Enter nonnumeric menu input, an out-of-range menu choice,
  a nonnumeric job field, a duplicate ID, a nonpositive burst/quantum, and
  invalid Banker's dimensions. The menu should remain usable without crashing
  or looping on the bad token.
- **Integration:** Enter `11` at the menu to exit. EOF also exits cleanly.

## Team work ownership

- **Team member 1:** FR1 job management, FR2 CPU schedulers, and FR5 Banker's
  Algorithm.
- **Team member 2:** FR3 concurrent logger, FR4 IPC, FR6 worker failure
  handling, and FR7 resource limits and cleanup.
- **Both team members:** FR8 CLI/error handling, integration, testing, and
  documentation/build instructions.

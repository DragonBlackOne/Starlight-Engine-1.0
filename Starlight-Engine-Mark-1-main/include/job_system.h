#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H

#include <stdbool.h>
#include <SDL2/SDL_atomic.h>

// Function signature for a Job
typedef void (*JobFunction)(void* data);

// Initialize the thread pool. 
// If num_threads <= 0, it creates threads based on the hardware CPU core count - 1.
bool job_system_init(int num_threads);

// Pushes a single job to the queue.
// Pass an SDL_atomic_t counter to track completion, or NULL if it's fire-and-forget.
// The counter MUST be initialized to 0 and incremented BEFORE pushing the job.
void job_system_push(JobFunction func, void* data, SDL_atomic_t* counter);

// Busy-waits (or yields) until the provided atomic counter reaches zero.
void job_system_wait(SDL_atomic_t* counter);

// Shuts down the thread pool and cleans up resources.
void job_system_shutdown(void);

#endif

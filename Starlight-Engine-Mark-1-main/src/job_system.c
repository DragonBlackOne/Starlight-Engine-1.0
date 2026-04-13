#include "job_system.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_JOBS 4096

typedef struct {
    JobFunction func;
    void* data;
    SDL_atomic_t* counter;
} Job;

static struct {
    Job queue[MAX_JOBS];
    int head;
    int tail;
    int job_count;
    
    SDL_Thread** threads;
    int num_threads;
    bool running;
    
    SDL_mutex* mutex;
    SDL_cond* wake_cond;
} g_job_system = {0};

static bool job_system_try_pop(Job* out_job) {
    bool has_job = false;
    SDL_LockMutex(g_job_system.mutex);
    if (g_job_system.job_count > 0) {
        *out_job = g_job_system.queue[g_job_system.tail];
        g_job_system.tail = (g_job_system.tail + 1) % MAX_JOBS;
        g_job_system.job_count--;
        has_job = true;
    }
    SDL_UnlockMutex(g_job_system.mutex);
    return has_job;
}

static int worker_thread_func(void* args) {
    while (g_job_system.running) {
        Job job;
        bool has_job = false;
        
        SDL_LockMutex(g_job_system.mutex);
        while (g_job_system.running && g_job_system.job_count == 0) {
            SDL_CondWait(g_job_system.wake_cond, g_job_system.mutex);
        }
        
        if (g_job_system.job_count > 0) {
            job = g_job_system.queue[g_job_system.tail];
            g_job_system.tail = (g_job_system.tail + 1) % MAX_JOBS;
            g_job_system.job_count--;
            has_job = true;
        }
        SDL_UnlockMutex(g_job_system.mutex);
        
        if (has_job && job.func) {
            job.func(job.data);
            if (job.counter) {
                SDL_AtomicDecRef(job.counter);
            }
        }
    }
    return 0;
}

bool job_system_init(int num_threads) {
    if (g_job_system.running) return true;
    
    if (num_threads <= 0) {
        num_threads = SDL_GetCPUCount() - 1;
        if (num_threads < 1) num_threads = 1;
    }
    
    g_job_system.head = 0;
    g_job_system.tail = 0;
    g_job_system.job_count = 0;
    g_job_system.running = true;
    g_job_system.num_threads = num_threads;
    
    g_job_system.mutex = SDL_CreateMutex();
    g_job_system.wake_cond = SDL_CreateCond();
    
    g_job_system.threads = (SDL_Thread**)malloc(sizeof(SDL_Thread*) * num_threads);
    for (int i = 0; i < num_threads; ++i) {
        char name[32];
        snprintf(name, sizeof(name), "Worker_%d", i);
        g_job_system.threads[i] = SDL_CreateThread(worker_thread_func, name, (void*)(intptr_t)i);
    }
    
    printf("[ENGINE] Job System Initialized with %d worker threads.\n", num_threads);
    return true;
}

void job_system_push(JobFunction func, void* data, SDL_atomic_t* counter) {
    if (!g_job_system.running) return;
    
    if (counter) {
        SDL_AtomicIncRef(counter);
    }
    
    SDL_LockMutex(g_job_system.mutex);
    if (g_job_system.job_count >= MAX_JOBS) {
        // Queue full fallback: Execute immediately on main thread.
        SDL_UnlockMutex(g_job_system.mutex);
        func(data);
        if (counter) SDL_AtomicDecRef(counter);
        return;
    }
    
    g_job_system.queue[g_job_system.head].func = func;
    g_job_system.queue[g_job_system.head].data = data;
    g_job_system.queue[g_job_system.head].counter = counter;
    
    g_job_system.head = (g_job_system.head + 1) % MAX_JOBS;
    g_job_system.job_count++;
    
    SDL_CondSignal(g_job_system.wake_cond);
    SDL_UnlockMutex(g_job_system.mutex);
}

void job_system_wait(SDL_atomic_t* counter) {
    if (!counter) return;
    
    int spin_count = 0;
    while (SDL_AtomicGet(counter) > 0) {
        Job job;
        if (job_system_try_pop(&job)) {
            spin_count = 0;
            if (job.func) {
                job.func(job.data);
                if (job.counter) SDL_AtomicDecRef(job.counter);
            }
        } else {
            if (spin_count < 100) {
                SDL_Delay(0); // Yield initialmente
                spin_count++;
            } else {
                SDL_Delay(1); // Backoff completo para nao fritar CPU
            }
        }
    }
}

void job_system_shutdown(void) {
    if (!g_job_system.running) return;
    
    g_job_system.running = false;
    SDL_CondBroadcast(g_job_system.wake_cond);
    
    for (int i = 0; i < g_job_system.num_threads; ++i) {
        SDL_WaitThread(g_job_system.threads[i], NULL);
    }
    
    free(g_job_system.threads);
    SDL_DestroyCond(g_job_system.wake_cond);
    SDL_DestroyMutex(g_job_system.mutex);
    
    printf("[ENGINE] Job System Shutdown.\n");
}

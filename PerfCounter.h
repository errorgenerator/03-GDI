#ifndef PERF_COUNTER_H
#define PERF_COUNTER_H

/*******************************************************
        Header file for elapsed time measurement
        usefull to benchmark code or such
        copied from http://www.songho.ca/misc/timer/timer.html

        !!!!    DONT FORGET TO #define FOR YOUR PLATFORM  !!!!
        #define WINDOWS_P if using windows        

********************************************************/
#ifdef WINDOWS_P
    
    #include <time.h>
    #include <Windows.h>
    #include <stdio.h>
    typedef struct PerfCounter
    {
        LARGE_INTEGER frequency;
        LARGE_INTEGER t1,t2;
        double elapsedTime;
       
    }PerfCounter;

    static inline void startTimer(PerfCounter* pc)
    {
        QueryPerformanceFrequency(&(pc->frequency));
        QueryPerformanceCounter(&(pc->t1));
    }
       
    static inline void stopTimer(PerfCounter* pc)
    {
        QueryPerformanceCounter(&(pc->t2));
        pc->elapsedTime = (pc->t2.QuadPart - pc->t1.QuadPart) * 1000.0 / (double)pc->frequency.QuadPart;
    }

    static inline void stopTimerPrint(PerfCounter* pc)
    {
        QueryPerformanceCounter(&(pc->t2));
        pc->elapsedTime = (pc->t2.QuadPart - pc->t1.QuadPart) * 1000.0 / (double)pc->frequency.QuadPart;
        printf("elapsed time: %.10f ms\n", pc->elapsedTime);
    }
   
#else
    #include<sys/time.h>
    #include<stdio.h>
    typedef struct PerfCounter
    {
        struct timeval t1,t2;
        double elapsedTime;

    }PerfCounter;
        
    static inline void startTimer(PerfCounter* pc)
    {
        gettimeofday(&(pc->t1), 0);
    }
        
    static inline void stopTimer(PerfCounter* pc)
    {
        gettimeofday(&(pc->t2), 0);
        pc->elapsedTime = (pc->t2.tv_sec - pc->t1.tv_sec) * 1000.0; //sec to ms
        pc->elapsedTime += (pc->t2.tv_usec - pc->t1.tv_usec) / 1000.0; // us to ms
    }
        
    static inline void stopTimerPrint(PerfCounter* pc)
    {
        gettimeofday(&(pc->t2), 0);
        pc->elapsedTime = (pc->t2.tv_sec - pc->t1.tv_sec) * 1000.0; //sec to ms
        pc->elapsedTime += (pc->t2.tv_usec - pc->t1.tv_usec) / 1000.0; // us to ms
        printf("elapsed time: %.10f ms\n", pc->elapsedTime);
    }
        

#endif


#endif

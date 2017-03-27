/*
 * @file thread-test.cpp
 *
 * Uses printf, so don't use Java console.
 *
 * @author Jeff Lutgen
 */

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "strlib.h"
#include "thread.h"
#include "simpio.h"
#include "console.h"

using namespace std;

static Lock countLock;
static const int COUNT_LIMIT = 150;
static const int TCOUNT = 20;
static const int THREADS = 9;
static int sharedCount = 0;

void threadyWithData(int &depth) {
    const int MAX_DEPTH = 3;
    const int NUM_THREADS = 5;

    Thread child[NUM_THREADS];
    int newDepth = depth + 1;
    if (depth < MAX_DEPTH) {
        for (int i = 0; i < NUM_THREADS; i++) {
            child[i] = fork(threadyWithData, newDepth);
        }
    }

    stringstream ss;
    ss << "======= hello from " << getCurrentThread().toString() << ", level " << depth;
    printf("%s\n", ss.str().c_str());

    if (depth < MAX_DEPTH) {
        for (int i = 0; i < NUM_THREADS; i++) {
            join(child[i]);
        }
    }
    //usleep(1000000);
}

void depthTest() {
    const int NUM_THREADS = 5;
    Thread child[NUM_THREADS];
    Vector<string> data(NUM_THREADS);
    int depth = 0;
    for (int i=0; i<NUM_THREADS; i++) {
        child[i] = fork(threadyWithData, depth);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        stringstream ss;
        ss << "calling join, " << i << " child id" << child[i].getId();
        printf("%s\n", ss.str().c_str());
        join(child[i]);
    }
    cout << "All joined " << endl;
}

void inc_count()
{
    long my_id = getCurrentThread().getId();
    for (int i = 0; i < TCOUNT; i++) {
        synchronized(countLock) {
            sharedCount++;

            /*
            Check the value of count and signal waiting thread when condition is
            reached.  Note that this occurs while lock is locked
            */
            if (sharedCount == COUNT_LIMIT) {
                countLock.signal();
                printf("inc_count(): thread %ld, sharedCount = %d  Threshold reached.\n",
                       my_id, sharedCount);
            }
            printf("inc_count(): thread %ld, sharedCount = %d, iteration %d, releasing lock\n",
                   my_id, sharedCount, i);
        }
        /* Do some "work" so threads can alternate on lock */
        //sleep(1);
    }
}

void watch_count()
{
    long my_id = getCurrentThread().getId();
    printf("Starting watch_count(): thread %ld\n", my_id);

    /*
      Lock countLock and wait for signal.  Note that the wait
      routine will automatically and atomically unlock the lock while it waits.
      Also, note that if COUNT_LIMIT is reached before this routine is run by
      the waiting thread, the loop will be skipped to prevent wait()
      from never returning.
      */
    synchronized(countLock) {
        while (sharedCount < COUNT_LIMIT) {
            countLock.wait();
            printf("watch_count(): thread %ld Condition signal received.\n", my_id);
            sharedCount += 125;
            printf("watch_count(): thread %ld sharedCount now = %d.\n", my_id, sharedCount);
        }
    }
}

/*
 * Adapted from https://computing.llnl.gov/tutorials/pthreads/#ConVarSignal
 */
void waitSignalTest() {
    Thread threads[THREADS];

    threads[0] = fork(watch_count);
    for (int i=1; i<THREADS; i++) {
        threads[i] = fork(inc_count);
    }

    /* Wait for all threads to complete */
    for (int i=0; i<THREADS; i++) {
        join(threads[i]);
    }
    cout << "waitSignalTest(): Waited on " << THREADS << " threads. Done." << endl;
}

int main() {
    cout << "Thread tests!" << endl;
    string yn = getLine("Run depth test? (y/n): ");
    if (yn == "y")
        depthTest();
    yn = getLine("Run waitSignal test? (y/n): ");
    if (yn == "y")
        waitSignalTest();
    return 0;
}


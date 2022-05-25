//
// Created by xjf on 5/24/22.
//

#ifndef PROJECT2_THREADPOOL_H
#define PROJECT2_THREADPOOL_H
#include "TaskQueue.h"
#include "TaskQueue.cpp"
template <typename T>
class ThreadPool {

public:

    ThreadPool (int min, int max);

    ~ThreadPool();

    void addTask(Task<T> task);

    int getBusyNum();

    int getAliveNum();

private:

    static void* worker(void *arg);

    static void* manager(void *arg);

    void threadExit();


private:
    TaskQueue<T> *taskQ;

    pthread_t managerID;
    pthread_t *threadIDs;
    int minNum;
    int maxNum;
    int busyNum;
    int liveNum;
    int exitNum;
    pthread_mutex_t mutexPool;
    pthread_cond_t notEmpty;
    static const int NUMBER = 2;
    bool shutdown;

};


#endif //PROJECT2_THREADPOOL_H

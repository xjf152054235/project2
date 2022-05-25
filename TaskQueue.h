//
// Created by xjf on 5/24/22.
//

#ifndef PROJECT2_TASKQUEUE_H
#define PROJECT2_TASKQUEUE_H

#include <queue>
#include <pthread.h>
using callback = void (*)(void* arg);

template <typename T>
struct Task{
    Task<T>(){
        function = nullptr;
        arg = nullptr;
    }
    Task<T>(callback f, void* arg):function(f),arg((T*)arg){

    }
    callback function;
    void *arg;
};
template <typename T>
class TaskQueue {
public:
    TaskQueue();
    ~TaskQueue();

    void addTask(Task<T> task);

    void addTask(callback f, void* arg);

    Task<T> getTask();

    size_t taskNumber(){
        return  m_taskQ.size();
    }
private:
    std::queue<Task<T>> m_taskQ;
    pthread_mutex_t m_mutex;
};


#endif //PROJECT2_TASKQUEUE_H

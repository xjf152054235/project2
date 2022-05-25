//
// Created by xjf on 5/24/22.
//

#include "ThreadPool.h"
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>

using namespace std;

template <typename T>
ThreadPool<T>::ThreadPool(int min, int max) {

    do{
        taskQ = new TaskQueue<T>;
        if(taskQ == nullptr){
            if(taskQ == nullptr){
                cout<<"new taskQ fail..."<<endl;
                break;
            }
        }
        threadIDs = new pthread_t[max];
        if(threadIDs == nullptr){
            cout<<"new threadIDs fail..."<<endl;
            break;
        }
        memset(threadIDs, 0, sizeof(pthread_t)*max);
        minNum = min;
        maxNum = max;
        busyNum = 0;
        liveNum = min;
        exitNum = 0;

        if(pthread_mutex_init(&mutexPool, NULL) != 0 ||
                pthread_cond_init(&notEmpty, NULL) != 0){
             cout<<"mutex or condition init fail..."<<endl;
            break;
        }

        shutdown = false;

        pthread_create(&managerID, NULL, manager, this);
        for(int i = 0; i < min; ++i){
            pthread_create(&threadIDs[i], NULL, worker, this);
        }
        return;
    } while (0);

    if(threadIDs) delete[] threadIDs;
    if(taskQ) delete taskQ;
}

template <typename T>
void *ThreadPool<T>::worker(void *arg) {

    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    while(true){
        pthread_mutex_lock(&pool->mutexPool);

        while(pool->taskQ->taskNumber() == 0 && !pool->shutdown){
            pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);

            if(pool->exitNum > 0){
                pool->exitNum--;
                if(pool->liveNum > pool->minNum){
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mutexPool);
                    pool->threadExit();
                }
            }
        }

        if(pool->shutdown){
            pthread_mutex_unlock(&pool->mutexPool);
            pool->threadExit();
        }

        Task<T> task = pool->taskQ->getTask();

        pthread_mutex_unlock(&pool->mutexPool);
        cout<<"thread"<<to_string(pthread_self())<<"start working..."<<endl;
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexPool);

        task.function(task.arg);
        delete task.arg;
        task.arg = nullptr;

        cout<<"thread"<<to_string(pthread_self())<<"end working..."<<endl;

        pthread_mutex_lock(&pool->mutexPool);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexPool);
    }
    return nullptr;
}

template <typename T>
void ThreadPool<T>::threadExit() {
    pthread_t tid = pthread_self();
    for(int i = 0; i < maxNum; ++i){
        if(threadIDs[i] == tid){
            threadIDs[i] = 0;
            cout<<"threadExit() called, "<<to_string(tid)<<"exiting..."<<endl;
            break;
        }
    }
    pthread_exit(NULL);
}

template <typename T>
void *ThreadPool<T>::manager(void *arg) {

    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    sleep(3);

    pthread_mutex_lock(&pool->mutexPool);
    int queueSize = pool->taskQ->taskNumber();
    int liveNum = pool->liveNum;
    int busyNUm = pool->busyNum;
    pthread_mutex_unlock(&pool->mutexPool);

    if(queueSize > liveNum && liveNum < pool->maxNum){
        pthread_mutex_lock(&pool->mutexPool);

        int counter = 0;
        for(int i = 0; i < pool->maxNum && counter < NUMBER
            && pool->liveNum < pool->maxNum; ++i){
            if(pool->threadIDs[i] == 0){
                pthread_create(&pool->threadIDs[i], NULL, worker,pool);
                counter++;
                pool->liveNum++;
            }
        }
        pthread_mutex_unlock(&pool->mutexPool);
    }

    if(busyNUm*2 < liveNum && liveNum > pool->minNum){
        pthread_mutex_lock(&pool->mutexPool);
        pool->exitNum = NUMBER;
        pthread_mutex_unlock(&pool->mutexPool);
        for(int i = 0; i < NUMBER; ++i){
            pthread_cond_signal(&pool->notEmpty);
        }
    }
    return nullptr;
}

template <typename T>
void ThreadPool<T>::addTask(Task<T> task) {

    if(shutdown){
        return;
    }

    taskQ->addTask(task);
    pthread_cond_signal(&notEmpty);

}

template <typename T>
int ThreadPool<T>::getBusyNum() {
    pthread_mutex_lock(&mutexPool);
    int busyNum = this->busyNum;
    pthread_mutex_unlock(&mutexPool);
    return busyNum;
}

template <typename T>
int ThreadPool<T>::getAliveNum() {
    pthread_mutex_lock(&mutexPool);
    int aliveNum = this->liveNum;
    pthread_mutex_unlock(&mutexPool);
    return aliveNum;
}

template <typename T>
ThreadPool<T>::~ThreadPool() {
    shutdown = true;
    pthread_join(managerID,NULL);

    for(int i = 0; i < liveNum;++i){
        pthread_cond_signal(&notEmpty);
    }

    if(taskQ){
        delete taskQ;
    }
    if(threadIDs){
        delete []threadIDs;
    }

    pthread_mutex_destroy(&mutexPool);
    pthread_cond_destroy(&notEmpty);

}



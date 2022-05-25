#include "ThreadPool.h"
#include "ThreadPool.cpp"
#include <unistd.h>
#include <stdio.h>
void taskFunc(void* arg){
    int num = *(int*)arg;
    printf("thread %ld is working, number = %d\n", pthread_self(),num);
    sleep(1);
}
int main() {
    ThreadPool<int> pool(3,10);
    for(int i = 0; i < 50; ++i){
        int *num = new int(i+100);
        pool.addTask(Task<int>(taskFunc,num));
    }
    sleep(20);
    return 0;
}

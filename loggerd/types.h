#ifndef __TYPES_H_INCLUDE__
#define __TYPES_H_INCLUDE__

#include "utils.h"

//接收数据块Block:
typedef struct RecvBuffer
{
   uchar *data_start_ptr;//数据开始指针
   uchar *data_end_ptr;//数据结束指针
   //uchar *to_write_ptr;//要写数据位置
   bool free;
}RecvBuffer;

typedef struct Block
{
    uchar *pHeadPtr;
    uchar *pTailPtr;
    RecvBuffer *recvBufs;//数组指针 buffer_number个
    bool free;// block status
    unsigned int bufIndexToWrite;
}Block;

typedef struct ThreadData
{
    bool free;
    bool have_data;
//    uchar *start_data_pt;
//    uchar *end_data_pt;
    RecvBuffer *recv_buffer;
    Block* block;
}ThreadData;

typedef struct ConfData
{
    int local_port;
    int thread_number;
    size_t size_of_buffer;
    int buffer_number_in_block;
    int block_number;
}ConfData;

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


int init_mem();
void release_mem();
void init_objects();
size_t get_size_of_block();

RecvBuffer *get_recv_buffer(Block *pBlock, int index);
Block *get_block(int index);
ThreadData* get_thread_data(int index);
void init_recv_buffer(RecvBuffer *pRecvBuffer, int index_in_all);
void init_thread_data(ThreadData *pThreadData);

ThreadData* get_one_free_thread_data();
Block *get_on_free_block();

void reset_block(Block *pBlock);

RecvBuffer *get_next_free_recv_buffer(Block *pBlock);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__TYPES_H_INCLUDE__

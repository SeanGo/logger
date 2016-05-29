#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "utils.h"

extern ConfData configData;

//static uchar* base_recv_data_memory = NULL;
//static uchar* base_objects_memory = NULL;

uchar* base_recv_data_memory = NULL;
uchar* base_objects_memory = NULL;

static size_t get_recv_buffer_mem_size();
static int create_recv_buffer_mem();
static void release_recv_buffer_mem();

static int create_objs_mem();
static void release_objs_mem();

int init_mem()
{
    if(0!=create_recv_buffer_mem())
    {
        return 1;
    }

    if(0!=create_objs_mem())
    {
        release_recv_buffer_mem();
        return 1;
    }

    init_objects();
    return 0;
}

void release_mem()
{
    release_objs_mem();
    release_recv_buffer_mem();
}

size_t get_size_of_block()
{
    return sizeof(Block)+configData.buffer_number_in_block*sizeof(RecvBuffer);
}

static int create_objs_mem()
{
    base_objects_memory = (uchar*)calloc(1, get_size_of_block()*configData.block_number
                             +configData.thread_number*sizeof(ThreadData));

//    printf("base_objects_memory:%d-%d\n", base_objects_memory,
//           base_objects_memory+get_size_of_block()*configData.block_number
//           +configData.thread_number*sizeof(ThreadData));

    return base_objects_memory==0?1:0;
}

static void release_objs_mem()
{
    if (base_objects_memory != NULL)
    {
        free(base_objects_memory);
        base_objects_memory = NULL;
        printf("release_objs_mem()\n");
    }
}

static int create_recv_buffer_mem()
{
    base_recv_data_memory = (uchar*)calloc(1,get_recv_buffer_mem_size());
//    printf("base_recv_data_memory:%d-%d\n", base_recv_data_memory,
//           base_recv_data_memory+get_recv_buffer_mem_size());
    return base_recv_data_memory==0?1:0;
}

static void release_recv_buffer_mem()
{
    if(base_recv_data_memory!=NULL)
    {
        free(base_recv_data_memory);
        base_recv_data_memory = 0;
        printf("release_recv_buffer_mem()\n");
    }
}

static size_t get_recv_buffer_mem_size()
{
    return configData.block_number*configData.buffer_number_in_block
            *configData.size_of_buffer;
}

RecvBuffer *get_recv_buffer(Block *pBlock, int index)//|block|recvbuffer|
{
    return (RecvBuffer*)((uchar*)pBlock+sizeof(Block)+index*sizeof(RecvBuffer));
}

Block *get_block(int index)
{
    return (Block*)(base_objects_memory+index*get_size_of_block());
}

ThreadData *get_thread_data(int index)
{
    return (ThreadData*)(base_objects_memory+configData.block_number*get_size_of_block())+index;
}

void init_recv_buffer(RecvBuffer* pRecvBuffer, int index_in_all)
{
    pRecvBuffer->data_start_ptr = base_recv_data_memory+index_in_all*configData.size_of_buffer;
    pRecvBuffer->data_end_ptr = pRecvBuffer->data_start_ptr;
    //pRecvBuffer->to_write_ptr = pRecvBuffer->data_start_ptr;
    pRecvBuffer->free = true;
}

void init_thread_data(ThreadData *pThreadData)
{
    pThreadData->block = NULL;
//    pThreadData->end_data_pt = NULL;
//    pThreadData->start_data_pt = NULL;
    pThreadData->recv_buffer = NULL;
    pThreadData->free = true;
    pThreadData->have_data = false;
}

void init_objects()
{
    int i=0;
    int j=0;
    for (i=0; i<configData.block_number;i++)
    {
        Block *pBlock = get_block(i);
        pBlock->free = true;
        pBlock->bufIndexToWrite = 0;
        pBlock->pHeadPtr = NULL;
        pBlock->pTailPtr = NULL;
//        printf("block:%d-%d\n", (int)pBlock, (int)(void*)pBlock+sizeof(Block));
        for(j=0; j<configData.buffer_number_in_block;j++)//init buffers
        {
            RecvBuffer *pRecvBuffer = get_recv_buffer(pBlock, j);
            init_recv_buffer(pRecvBuffer, i*configData.buffer_number_in_block+j);
//            printf("data_start_ptr:%d-%d\n",(int)pRecvBuffer->data_start_ptr,
//                   (int)pRecvBuffer->data_start_ptr+configData.size_of_buffer);
//            printf("recv:%d-%d\n", (int)pRecvBuffer, (int)(void*)pRecvBuffer+sizeof(RecvBuffer));
        }
    }

    int n=0;
    for(n=0; n<configData.thread_number; n++)//init threaddatas
    {
        ThreadData *pThreadData = get_thread_data(n);
        init_thread_data(pThreadData);
//        printf("pThreadData:%d-%d\n", (int)pThreadData, (int)(void*)pThreadData+sizeof(ThreadData));
    }
}

ThreadData *get_one_free_thread_data()
{
    int i=0;
    for(i=0; i<configData.thread_number; i++)
    {
        ThreadData* pThreadData = get_thread_data(i);
        if (pThreadData->free == true)
        {
            printf("get_one_free_thread_data index =%d\n", i);
            return pThreadData;
        }
    }
    return NULL;
}

void reset_block(Block *pBlock)
{
    pBlock->free = true;
    pBlock->bufIndexToWrite = 0;
    int i=0;
    for(i=0; i<configData.buffer_number_in_block; i++)
    {
        RecvBuffer *pRecvBuffer = get_recv_buffer(pBlock, i);
        pRecvBuffer->data_end_ptr = pRecvBuffer->data_start_ptr;
        //pRecvBuffer->to_write_ptr = pRecvBuffer->data_start_ptr;
        pRecvBuffer->free = true;
    }
}


Block *get_on_free_block()
{
    int i=0;
    for(i=0; i<configData.block_number;i++)
    {
        Block *pBlock = get_block(i);
        if(pBlock->free == true)
        {
            reset_block(pBlock);
            return pBlock;
        }
    }
    return NULL;
}

RecvBuffer *get_next_free_recv_buffer(Block *pBlock)
{
    int index = pBlock->bufIndexToWrite;
    RecvBuffer *pRecvBuffer = get_recv_buffer(pBlock, index);
    if(pRecvBuffer->free == false)
    {
        return NULL;
    }
    return pRecvBuffer;
}

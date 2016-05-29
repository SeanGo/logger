#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "parserthread.h"
#include "types.h"
#include "mylog.h"
#include "utils.h"


static void *pasreProc(void *);

void *pasreProc(void *p) {
  static int threadIndex = 0;
  threadIndex++;
  int nThreadID = threadIndex;
  ThreadData *pThreadData = (ThreadData *)p;
  char fileName[120];
  sprintf(fileName, "./%dtxt.txt", threadIndex);
  FILE *file = fopen(fileName, "a+");
  while (!nStop) {
    if (pThreadData->have_data == true) {
      pThreadData->free = false;
      uchar *start = pThreadData->recv_buffer->data_start_ptr;
      int src_left_len = pThreadData->recv_buffer->data_end_ptr -
                         pThreadData->recv_buffer->data_start_ptr + 1;

      uchar *line_start = NULL;
      int is_full_line = false;
      while (true) {
        int nLength = get_next_line(start, src_left_len, &line_start,
                                    &src_left_len, &is_full_line);
        if (nLength > 0) {
          char buf[1024];
          memcpy(buf, line_start, nLength);
          buf[nLength] = '\0';
          strcat(buf, "\n");
          static int lineId = 1;
          // printf("[%d]line:%s\n", lineId, buf);
          fwrite(buf, strlen(buf), 1, file);
          fflush(file);
          // fwrite(buf,"\n",1,file);
          // mylog(LOG_FILE, L_INFO, "[%d]line:%s\n", lineId, buf);
          lineId++;
        }

        if (src_left_len == 0) {
          pThreadData->recv_buffer->data_end_ptr =
              pThreadData->recv_buffer->data_start_ptr;
          pThreadData->recv_buffer->free = true;
          pThreadData->free = true;
          pThreadData->have_data = false;
          printf("thread %d pasre over\n", nThreadID);
          mylog(LOG_FILE, L_INFO, "thread %d pasre over", nThreadID);
          break;
        }
        start = line_start + nLength;
      }
    } else {
      // printf("free!!!\n");
      usleep(1);
    }
  }
  fclose(file);
  mylog(LOG_FILE, L_INFO, "parse thread exit...");
  return 0;
}

int create_parse_threads() {
  int i = 0;
  for (i = 0; i < (int)configData.thread_number; i++) {
    ThreadData *pThreadData = get_thread_data(i);
    pthread_t id;
    if (0 != pthread_create(&id, NULL, pasreProc, pThreadData)) {
      return 1;
    }
  }
  return 0;
}

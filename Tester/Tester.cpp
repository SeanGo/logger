#include "gtest/gtest.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "types.h"
#include "mylog.h"

extern uchar *base_recv_data_memory;
extern uchar *base_objects_memory;

char LOG_FILE[] = "./log";
ConfData configData;
int nStop = 0;

TEST(TestTypedefs, TestTypedefs) {
  configData.block_number = 10;
  configData.buffer_number_in_block = 3;
  configData.size_of_buffer = 1024;
  configData.thread_number = 3;

  int nRet = init_mem();
  EXPECT_EQ(nRet, 0);

  for (int i = 0; i < configData.block_number; i++) {
    Block *pBlock = get_block(i);
    pBlock->free = true;
    pBlock->bufIndexToWrite = 0;
    pBlock->pHeadPtr = NULL;
    pBlock->pTailPtr = NULL;
    if (i == 0) {
      EXPECT_EQ(reinterpret_cast<void *>(base_objects_memory),
                reinterpret_cast<void *>(pBlock));
    }

    for (int j = 0; j < configData.buffer_number_in_block; j++) // init buffers
    {
      RecvBuffer *pRecvBuffer = get_recv_buffer(pBlock, j);
      init_recv_buffer(pRecvBuffer, i * configData.buffer_number_in_block + j);
      if (j == 0 && i == 0) {
        EXPECT_EQ(reinterpret_cast<void *>(pRecvBuffer->data_start_ptr),
                  reinterpret_cast<void *>(base_recv_data_memory));
      } else if (j == configData.buffer_number_in_block - 1 &&
                 i == configData.block_number - 1) {
        EXPECT_EQ(
            reinterpret_cast<void *>(pRecvBuffer->data_start_ptr +
                                     configData.size_of_buffer),
            reinterpret_cast<void *>(base_recv_data_memory +
                                     configData.block_number *
                                         configData.buffer_number_in_block *
                                         configData.size_of_buffer));
      }
    }
  }

  for (int n = 0; n < configData.thread_number; n++) // init threaddatas
  {
    ThreadData *pThreadData = get_thread_data(n);
    init_thread_data(pThreadData);
    if (n == configData.buffer_number_in_block) {
      EXPECT_EQ(reinterpret_cast<void *>(base_objects_memory +
                                         get_size_of_block() *
                                             configData.block_number),
                reinterpret_cast<void *>(reinterpret_cast<char *>(pThreadData) +
                                         sizeof(ThreadData)));
    }
  }
  release_mem();
}

TEST(TestTypedefs, TestTypedefsInit) {
  configData.block_number = 10;
  configData.buffer_number_in_block = 3;
  configData.size_of_buffer = 1024;
  configData.thread_number = 3;

  int nRet = init_mem();
  EXPECT_EQ(nRet, 0);
  init_objects();
  release_mem();
}

TEST(TestMyLog, TestMyLog) {
  set_debug_flag(L_DBG);

  int nRet = mylog("./log", L_DBG, "debug log");
  EXPECT_EQ(nRet, 0);

  nRet = mylog("./log", L_DBG, "debug log %s", "msg11111111111");
  EXPECT_EQ(nRet, 0);

  nRet = mylog("./log", L_INFO, "info log");
  EXPECT_EQ(nRet, 0);

  nRet = mylog("./log", L_ERR, "error log");
  EXPECT_EQ(nRet, 0);
}

TEST(TestgetEndHalfLine, getEndHalfLine0) {
  char szData[512] = "log0";
  char *pLineStart = NULL;
  char *pLineEnd = NULL;
  bool haveMultiLine = false;
  bool bRet = get_end_half_line(szData, szData + strlen(szData), &pLineStart,
                                &pLineEnd, &haveMultiLine);
  EXPECT_EQ(bRet, true);
  EXPECT_EQ(haveMultiLine, false);
  EXPECT_EQ(pLineStart, szData);
  printf("%s\n", pLineStart);
}

TEST(TestgetEndHalfLine, getEndHalfLine1) {
  bool haveMultiLine = false;
  char szData[512] = "log0\r\nlog1";
  char *pLineStart = NULL;
  char *pLineEnd = NULL;
  bool bRet = get_end_half_line(szData, szData + strlen(szData), &pLineStart,
                                &pLineEnd, &haveMultiLine);
  EXPECT_EQ(bRet, true);
  EXPECT_EQ(haveMultiLine, true);
  EXPECT_EQ(pLineStart, szData + 6);
  printf("%s\n", pLineStart);
}

TEST(TestgetEndHalfLine, getEndHalfLine2) {
  char szData[512] = "log0\r\n";
  char *pLineStart = NULL;
  char *pLineEnd = NULL;
  bool haveMultiLine = false;
  bool bRet = get_end_half_line(szData, szData + strlen(szData), &pLineStart,
                                &pLineEnd, &haveMultiLine);
  EXPECT_EQ(bRet, false);
  EXPECT_EQ(haveMultiLine, true);
  EXPECT_EQ(pLineStart, szData + 6);
  printf("%s\n", pLineStart);
}

TEST(readNextLine, readNextLine0) {
  int nFullLine = 0;
  char szData[1024] = "May\n 06 19:39:58 hitrade1\r\n Receive Nak";

  char *pLineStart = NULL;
  char *pLineEnd = NULL;
  int nLen = read_next_line(szData, &pLineStart, &pLineEnd, &nFullLine);
  EXPECT_STREQ(pLineStart, "May");
  EXPECT_EQ(pLineEnd, pLineStart + 3);
  EXPECT_EQ(nLen, 3);
  EXPECT_EQ(strlen(pLineStart), 3);
  EXPECT_EQ(nFullLine, 1);

  nLen = read_next_line(pLineEnd + 1, &pLineStart, &pLineEnd, &nFullLine);
  EXPECT_STREQ(pLineStart, " 06 19:39:58 hitrade1");
  EXPECT_EQ(pLineEnd, pLineStart + 21);
  EXPECT_EQ(nLen, 21);
  EXPECT_EQ(strlen(pLineStart), 21);
  EXPECT_EQ(nFullLine, 1);

  nLen = read_next_line(pLineEnd + 1, &pLineStart, &pLineEnd, &nFullLine);
  EXPECT_STREQ(pLineStart, " Receive Nak");
  EXPECT_EQ(pLineEnd, pLineStart + 12);
  EXPECT_EQ(nLen, 12);
  EXPECT_EQ(strlen(pLineStart), 12);
  EXPECT_EQ(nFullLine, 0);

  nLen = read_next_line(pLineEnd + 1, &pLineStart, &pLineEnd, &nFullLine);
  EXPECT_STREQ(pLineStart, "");
  EXPECT_EQ(pLineEnd, pLineStart);
  EXPECT_EQ(nLen, 0);
  EXPECT_EQ(strlen(pLineStart), 0);
  EXPECT_EQ(nFullLine, 0);
}

TEST(get_next_line, get_next_line) {
  int nFullLine = 0;
  char szData[1024] = "May\n 06 19:39:58 hitrade1\r\n Receive Nak";

  char *pLineStart = NULL;
  int srcLength = strlen((char *)szData);
  int nLen =
      get_next_line(szData, srcLength, &pLineStart, &srcLength, &nFullLine);
  EXPECT_EQ(pLineStart, szData);
  EXPECT_EQ(nLen, 3);
  EXPECT_EQ(srcLength, 36);
  EXPECT_EQ(nFullLine, 1);

  pLineStart = pLineStart + nLen;
  nLen =
      get_next_line(pLineStart, srcLength, &pLineStart, &srcLength, &nFullLine);
  EXPECT_STREQ(pLineStart, szData + 4);
  EXPECT_EQ(nLen, 21);
  EXPECT_EQ(srcLength, 14);
  EXPECT_EQ(nFullLine, 1);

  pLineStart = pLineStart + nLen;
  nLen =
      get_next_line(pLineStart, srcLength, &pLineStart, &srcLength, &nFullLine);
  EXPECT_STREQ(pLineStart, " Receive Nak");
  EXPECT_EQ(nLen, 12);
  EXPECT_EQ(srcLength, 0);
  EXPECT_EQ(nFullLine, 0);
}

TEST(readNextLine, readNextLine1) {
  int nFullLine = 0;
  char szData[1024] = "May\n 06 19:39:58 hitrade1\r\n";

  char *pLineStart = NULL;
  char *pLineEnd = NULL;
  int nLen = read_next_line(szData, &pLineStart, &pLineEnd, &nFullLine);
  EXPECT_STREQ(pLineStart, "May");
  EXPECT_EQ(pLineEnd, pLineStart + 3);
  EXPECT_EQ(nLen, 3);
  EXPECT_EQ(strlen(pLineStart), 3);
  EXPECT_EQ(nFullLine, 1);

  nLen = read_next_line(pLineEnd + 1, &pLineStart, &pLineEnd, &nFullLine);
  EXPECT_STREQ(pLineStart, " 06 19:39:58 hitrade1");
  EXPECT_EQ(pLineEnd, pLineStart + 21);
  EXPECT_EQ(nLen, 21);
  EXPECT_EQ(strlen(pLineStart), 21);
  EXPECT_EQ(nFullLine, 1);

  nLen = read_next_line(pLineEnd + 1, &pLineStart, &pLineEnd, &nFullLine);
  EXPECT_STREQ(pLineStart, "");
  EXPECT_EQ(pLineEnd, pLineStart);
  EXPECT_EQ(nLen, 0);
  EXPECT_EQ(strlen(pLineStart), 0);
  EXPECT_EQ(nFullLine, 0);

  nLen = read_next_line(pLineEnd + 1, &pLineStart, &pLineEnd, &nFullLine);
  EXPECT_STREQ(pLineStart, "");
  EXPECT_EQ(pLineEnd, pLineStart);
  EXPECT_EQ(nLen, 0);
  EXPECT_EQ(strlen(pLineStart), 0);
  EXPECT_EQ(nFullLine, 0);
}

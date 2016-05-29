#ifndef __UTILS_H_INCLUDE__
#define __UTILS_H_INCLUDE__

//char* getNextLine(char *szInSrc, int *nInSrcLength, char *outLine, int *outLineLength);
#include <time.h>

typedef char BYTE;
typedef unsigned char uchar;

#if !defined(__cplusplus)
    typedef unsigned char bool;
    #define true 1
    #define false 0
#endif



#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

char *get_current_time(char t[]);

int get_next_line(uchar *szInSrc,
    int nInSrcLength,
    uchar **pOutLineStartPos,
    int *nLeftStringLength,
    int *nIsFullLine);

int read_next_line(const char *src, char ** pLineStart, char ** pLineEnd, int *pFullLine);

bool get_end_half_line(const uchar *str_src_start, const uchar *str_src_end,
                    uchar **out_last_line_start, uchar **out_last_lineEnd, int *have_multi_lines);

//2016/02/01 08:18:40
int format_time1(char * szTime, time_t *out_time);

//20151214 20:32:08
int format_time2(char * time, time_t *out_time);

char *get_data_at(const char* szInSrc, int nInSrcLength, int nInIndex, char *outData);


#ifdef __cplusplus
}
#endif //__cplusplus



#endif //__UTILS_H_INCLUDE__

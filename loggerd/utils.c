#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
/*
params:
    szInSrc:src data
    nInSrcLength:src data length
    pOutLineStartPos:first line start pos
    nLeftStringLength:after this,left string length

return:
    fist line length
*/
int get_next_line(uchar *szInSrc,
                  int nInSrcLength,
                  uchar **pOutLineStartPos,
                  int *nLeftStringLength,
                  int *nIsFullLine)
{
    uchar *src = szInSrc;
    uchar *pLineEndPos = src;
    *nLeftStringLength = nInSrcLength;
    *nIsFullLine = 1;

    //获取起始位置
    while (*src == '\n' || *src == '\r')
    {
        if (src-szInSrc>=nInSrcLength)
        {
            *pOutLineStartPos = src;//获取起始不为\n或\r的值
            pLineEndPos = src;
            *nIsFullLine = 0;
            return 0;
        }
        src++;
        (*nLeftStringLength)--;
    }

    if(*src == '\0')
    {
        *pOutLineStartPos = src;//获取起始不为\n或\r的值
        pLineEndPos = src;
        *nLeftStringLength = 0;
        *nIsFullLine = 0;
        return 0;
    }

    *pOutLineStartPos = src;//获取起始不为\n或\r的值

    //获取结束位置
    while (*src != '\n' && *src != '\r' && *src != '\0')
    {
        if (src-szInSrc>=nInSrcLength)
        {
            break;
        }
        src++;
        (*nLeftStringLength)--;
    }

    if(*src == '\0')
    {
        *nIsFullLine = 0;
    }
    pLineEndPos = src;
    return pLineEndPos - (*pOutLineStartPos);
}

int read_next_line(const char *src, char ** pLineStart, char ** pLineEnd, int *pFullLine)
{
    *pFullLine = 0;
    char *pStart = (char*)src;
    char *pEnd = pStart;

    while (*pStart == '\r' || *pStart == '\n')
    {
        pStart++;
    }

    pEnd = pStart;
    while (*pEnd!='\r' && *pEnd !='\n' && *pEnd !='\0')
    {
        pEnd++;
    }

    if (*pEnd == '\0')
    {
        *pFullLine = 0;
    }
    else
    {
        *pFullLine = 1;
    }

    *pEnd = '\0';

    *pLineStart = pStart;
    *pLineEnd = pEnd;
    return (pEnd-pStart);//do not have \0 in length
}

/**
  return:true-half line,false-full line
*/
bool get_end_half_line(const uchar *str_src_start, const uchar *str_src_end,
                       uchar ** out_last_line_start, uchar **out_last_line_end,
                       int *have_multi_mines)
{
    uchar *p = (uchar*)str_src_end;
    *out_last_line_end = (uchar*)str_src_end;
    *have_multi_mines = false;

    while(*p != '\n' && p>str_src_start)
    {
        p--;
    }

    if(*p == '\n')
    {
        p++;
        *have_multi_mines = true;
    }

    *out_last_line_start = p;
    if(*out_last_line_end==*out_last_line_start
            &&**out_last_line_end!='\0'
            &&**out_last_line_end!='\n')//just one char
    {
        return true;
    }
    return (*out_last_line_end-*out_last_line_start)>0?true:false;
}

//2016/02/01 08:18:40
int format_time1(char * szTime, time_t *out_time)
{
    int nRet = -1;
    struct tm tm1;
    if(sscanf(szTime, "%4d/%2d/%2d %2d:%2d:%2d",
              &tm1.tm_year,
              &tm1.tm_mon,
              &tm1.tm_mday,
              &tm1.tm_hour,
              &tm1.tm_min,
              &tm1.tm_sec)==6)
    {
        nRet = 0;
    }

    tm1.tm_year -= 1900;
    tm1.tm_mon --;
    tm1.tm_isdst=-1;

    *out_time = mktime(&tm1);
    return nRet;
}

//20151214 20:32:08
int format_time2(char * time, time_t *out_time)
{
    int bRet = -1;

    struct tm tm1;
    if (sscanf(time, "%4d%2d%2d %2d:%2d:%2d",
               &tm1.tm_year,
               &tm1.tm_mon,
               &tm1.tm_mday,
               &tm1.tm_hour,
               &tm1.tm_min,
               &tm1.tm_sec)==6)
    {
        bRet = 0;
    }

    tm1.tm_year -= 1900;
    tm1.tm_mon --;
    tm1.tm_isdst=-1;

    *out_time = mktime(&tm1);
    return bRet;
}

char *get_data_at(const char* szInSrc, int nInSrcLength, int nInIndex, char *outData)
{
    char *pDes = outData;
    char *p = (char*)szInSrc;
    int nCount = 0;
    int i = 0;
    for (; i<nInSrcLength&&nCount<nInIndex; i++)//查找第nInIndex-1次出现的位置
    {
        while (*p != '|')
        {
            p++;
            if ((p-szInSrc)>=nInSrcLength)
            {
                outData[0] = '\0';
                return outData;
            }
        }
        p++;
        nCount++;
    }

    while(p-szInSrc<nInSrcLength && *p!= '|')
    {
        *pDes++ = *p++;
    }
    *pDes = '\0';

    return outData;
}

char* get_current_time(char t[])
{
    time_t nowtime = time(NULL);
    struct tm *now = localtime(&nowtime);
    sprintf(t, "%04d-%02d-%02d %02d:%02d:%02d\nunix time: %ld\n",
            now->tm_year+1900, now->tm_mon+1, now->tm_mday,
            now->tm_hour, now->tm_min, now->tm_sec, (long)nowtime);
    return t;
}

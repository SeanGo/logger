#ifndef __MYLOG_H_INCLUDE__
#define __MYLOG_H_INCLUDE__

#define L_DBG			1
#define L_AUTH			2
#define L_INFO			3
#define L_ERR			4
#define L_PROXY			5
#define L_WRN           6
#define L_CONS			128
/* for windows
	The following ifdef block is the standard way of creating macros which make exporting 
	from a DLL simpler. All files within this DLL are compiled with the SQL_DRIVER_EXPORTS
	symbol defined on the command line. this symbol should not be defined on any project
	that uses this DLL. This way any other project whose source files include this file see 
	functions as being imported from a DLL, wheras this DLL sees symbols
	defined with this macro as being exported.
*/
typedef struct LRAD_NAME_NUMBER {
	char	*name;
	int		number;
} LRAD_NAME_NUMBER;

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


extern void set_debug_flag(int flag);
extern int dbg(const char * filename, const char *msg, ...);
extern int mylog(const char * filename, int lvl, const char *msg, ...);
extern int mylog3(FILE * fp, int lvl, const char *msg, ...);
extern int mylog(const char * filename, int lvl, const char *msg, ...);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__MYLOG_H_INCLUDE__

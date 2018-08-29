#ifndef __LIBRARY_DBAPI_H__
#define __LIBRARY_DBAPI_H__

#include <windows.h>

typedef struct {
    DWORD  id;
    char   name   [33];
    char   author [33];
    char   press  [33];
    double price;
    char   isbn   [16];
    char   shelf  [16];
    int    status;
    char   comment[256];
} BOOKITEM;

int libdb_init(void);
int libdb_add_book  (char *name, char *author, char *press, float price, char *isbn, char *shelf, char *comment, DWORD *bookid);
int libdb_query_book(char *name, char *author, char *press, char *isbn, DWORD bookid, int page, BOOKITEM *list, int *num);

#endif

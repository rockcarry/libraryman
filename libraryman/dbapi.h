#ifndef __LIBRARY_DBAPI_H__
#define __LIBRARY_DBAPI_H__

#include <windows.h>

#define PAGE_SIZE  10

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

typedef struct {
    DWORD  id;
    char   name    [33];
    int    sex;
    char   idcard  [20];
    int    maxbrwnum;
    int    maxbrwdays;
    char   password[16];
} USERITEM;

int libdb_init(void);

int libdb_add_book   (char *name, char *author, char *press, double price, char *isbn, char *shelf, char *comment, DWORD *bookid);
int libdb_query_book (char *name, char *author, char *press, char *isbn, DWORD bookid, int *total, int page, BOOKITEM *list, int *num);
int libdb_modify_book(BOOKITEM *item);

int libdb_add_user   (char *name, int sex, char *idcard, char *password, DWORD *userid);
int libdb_query_user (char *name, int sex, char *idcard, DWORD userid, int *total, int page, USERITEM *list, int *num);
int libdb_modify_user(USERITEM *item);

#endif


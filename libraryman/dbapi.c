#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include "stdefine.h"
#include "dbapi.h"

static int callback(void *data, int argc, char **argv, char **colname)
{
    int      *rows = NULL;
    BOOKITEM *list = NULL;
    int       i;

    if (!data) return 0;
    rows = (int     *)((void**)data)[0];
    list = (BOOKITEM*)((void**)data)[1];
    if (!rows || !list) return 0;

    i = *rows;
    list[i].id     = atoi(argv[0]);
    list[i].price  = atof(argv[4]);
    list[i].status = atoi(argv[8]);
    strcpy(list[i].name   , argv[1]);
    strcpy(list[i].author , argv[2]);
    strcpy(list[i].press  , argv[3]);
    strcpy(list[i].isbn   , argv[5]);
    strcpy(list[i].shelf  , argv[6]);
    strcpy(list[i].comment, argv[9]);
    (*rows)++;
    return 0;
}

int libdb_init(void)
{
    char *err  = NULL;
    int   rc   = 0;
    char *sql1 = "create table BookTable ("
                 "Id      integer  primary key autoincrement,"
                 "Name    char(33) not null,"
                 "Author  char(33),"
                 "Press   char(33),"
                 "Price   real     default 0.0,"
                 "ISBN    char(16),"
                 "Shelf   char(16),"
                 "Status  integer  default -1,"
                 "Comment text"
                 ");";
    char *sql2 = "create table UserTable ("
                 "Id          integer  primary key autoincrement,"
                 "Name        char(33) not null,"
                 "Sex         char(7 ),"
                 "IdCardNum   char(20) not null,"
                 "Password    char(16) not null"
                 ");";

    char *sql3 = "create table BorrowTable ("
                 "Id          integer  primary key autoincrement,"
                 "BookId      integer  not null,"
                 "UserId      integer  not null,"
                 "BorrowDate  date     not null,"
                 "ReturnDate  date     not null"
                 ");";

    sqlite3 *db = NULL;
    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    rc = sqlite3_exec(db, sql1, callback, 0, &err);
    if (rc) {
        printf("failed to create table BookTable !\n");
        printf("%s.\n", err);
//      goto done;
    }

    rc = sqlite3_exec(db, sql2, callback, 0, &err);
    if (rc) {
        printf("failed to create table UserTable !\n");
        printf("%s.\n", err);
//      goto done;
    }

    rc = sqlite3_exec(db, sql3, callback, 0, &err);
    if (rc) {
        printf("failed to create table BorrowTable !\n");
        printf("%s.\n", err);
//      goto done;
    }

done:
    if (db) sqlite3_close(db);
    return rc;
}

int libdb_add_book(char *name, char *author, char *press, float price, char *isbn, char *shelf, char *comment, DWORD *bookid)
{
    char     sql[256];
    char    *err = NULL;
    int      rc  = 0;
    sqlite3 *db  = NULL;

    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    sprintf(sql, "insert into BookTable (Name, Author, Press, Price, ISBN, Shelf, Status, Comment) values ('%s', '%s', '%s', %f, '%s', '%s', %d, '%s');",
        name, author, press, price, isbn, shelf, 0, comment);
    rc = sqlite3_exec(db, sql, callback, 0, &err);
    if (rc) {
        printf("failed to add book !\n");
        printf("%s.\n", err);
//      goto done;
    } else {
        *bookid = (DWORD)sqlite3_last_insert_rowid(db);
    }

done:
    if (db) sqlite3_close(db);
    return rc;
}

int libdb_query_book(char *name, char *author, char *press, char *isbn, DWORD bookid, int page, BOOKITEM *list, int *num)
{
    char     sql[256];
    char    *err = NULL;
    int      rc  = 0;
    int      rows= 0;
    void    *data[2];
    sqlite3 *db  = NULL;

    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    sprintf(sql, "select * from BookTable where TRUE");
    if (strcmp(name, "*") != 0) {
        strcat(sql, " and Name like '");
        strcat(sql, name);
        strcat(sql, "'");
    }
    if (strcmp(author, "*") != 0) {
        strcat(sql, " and Author like '");
        strcat(sql, author);
        strcat(sql, "'");
    }
    if (strcmp(press, "*") != 0) {
        strcat(sql, " and Press like '");
        strcat(sql, press);
        strcat(sql, "'");
    }
    if (strcmp(isbn, "*") != 0) {
        strcat(sql, " and ISBN = '");
        strcat(sql, isbn);
        strcat(sql, "'");
    }
    if (bookid != 0) {
        char strid[12] = {0};
        itoa(bookid, strid, 10);
        strcat(sql, " and Id = ");
        strcat(sql, strid);
    }
    if (page > 0) {
        char offset[12] = {0};
        itoa(page * 10, offset, 10);
        strcat(sql, " limit 10 offset ");
        strcat(sql, offset);
    }
    strcat(sql, ";");

    data[0] = &rows;
    data[1] =  list;
    rc = sqlite3_exec(db, sql, callback, data, &err);
    if (rc) {
        printf("failed to query book !\n");
        printf("%s.\n", err);
//      goto done;
    }

done:
    if (db) sqlite3_close(db);
    *num = rows;
    return rc;
}

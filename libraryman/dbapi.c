#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include "stdefine.h"
#include "dbapi.h"

static int callback_get_count(void *data, int argc, char **argv, char **colname)
{
    int *count = (int*)data;
    *count = atoi(argv[0]);
    return 0;
}

static int callback_book_query(void *data, int argc, char **argv, char **colname)
{
    int      *rows = NULL;
    BOOKITEM *list = NULL;
    int       i;

    if (!data) return 0;
    rows = (int     *)((void**)data)[0];
    list = (BOOKITEM*)((void**)data)[1];
    if (!rows || !list) return 0;

    i = *rows;
    if (i < PAGE_SIZE) {
        list[i].id     = atoi(argv[0]);
        list[i].price  = atof(argv[4]);
        list[i].status = atoi(argv[8]);
        strcpy(list[i].name   , argv[1]);
        strcpy(list[i].author , argv[2]);
        strcpy(list[i].press  , argv[3]);
        strcpy(list[i].isbn   , argv[5]);
        strcpy(list[i].shelf  , argv[6]);
        strcpy(list[i].comment, argv[9]);
    }
    (*rows)++;
    return 0;
}

static int callback_user_query(void *data, int argc, char **argv, char **colname)
{
    int      *rows = NULL;
    USERITEM *list = NULL;
    int       i;

    if (!data) return 0;
    rows = (int     *)((void**)data)[0];
    list = (USERITEM*)((void**)data)[1];
    if (!rows || !list) return 0;

    i = *rows;
    if (i < PAGE_SIZE) {
        list[i].id        = atoi(argv[0]);
        list[i].sex       = atoi(argv[2]);
        list[i].maxbrwnum = atoi(argv[4]);
        list[i].maxbrwdays= atoi(argv[5]);
        strcpy(list[i].name    , argv[1]);
        strcpy(list[i].idcard  , argv[3]);
        strcpy(list[i].password, argv[6]);
    }
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
                 "Status  integer  default 1,"
                 "Comment text"
                 ");";
    char *sql2 = "create table UserTable ("
                 "Id         integer  primary key autoincrement,"
                 "Name       char(33) not null,"
                 "Sex        integer,"
                 "IdCardNum  char(20) not null,"
                 "MaxBrwNum  integer  default 3,"
                 "MaxBrwDays integer  default 3,"
                 "Password   char(16) not null"
                 ");";

    char *sql3 = "create table BorrowTable ("
                 "Id            integer  primary key autoincrement,"
                 "BookId        integer  not null,"
                 "UserId        integer  not null,"
                 "BorrowDate    datetime not null,"
                 "LimitRetDate  datetime not null,"
                 "ActualRetDate datetime default (date('9999-12-31'))"
                 ");";

    sqlite3 *db = NULL;
    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    rc = sqlite3_exec(db, sql1, NULL, NULL, &err);
    if (rc) {
        printf("failed to create table BookTable !\n");
        printf("%s.\n", err);
//      goto done;
    }

    rc = sqlite3_exec(db, sql2, NULL, NULL, &err);
    if (rc) {
        printf("failed to create table UserTable !\n");
        printf("%s.\n", err);
//      goto done;
    }

    rc = sqlite3_exec(db, sql3, NULL, NULL, &err);
    if (rc) {
        printf("failed to create table BorrowTable !\n");
        printf("%s.\n", err);
//      goto done;
    }

done:
    if (db) sqlite3_close(db);
    return rc;
}

int libdb_add_book(char *name, char *author, char *press, double price, char *isbn, char *shelf, char *comment, DWORD *bookid)
{
    char     sql[256];
    sqlite3 *db  = NULL;
    char    *err = NULL;
    int      rc  = 0;

    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    sprintf(sql, "insert into BookTable (Name, Author, Press, Price, ISBN, Shelf, Status, Comment) values ('%s', '%s', '%s', %f, '%s', '%s', %d, '%s');",
        name, author, press, price, isbn, shelf, 0, comment);
    rc = sqlite3_exec(db, sql, NULL, NULL, &err);
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

int libdb_query_book(char *name, char *author, char *press, char *isbn, DWORD bookid, int *total, int page, BOOKITEM *list, int *num)
{
    char     sql [256];
    char     cond[256];
    sqlite3 *db  = NULL;
    char    *err = NULL;
    int      rc  = 0;
    int      rows= 0;
    void    *data[2];

    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    sprintf(cond, " where TRUE");
    if (strcmp(name, "*") != 0) {
        strcat(cond, " and Name like '");
        strcat(cond, name);
        strcat(cond, "'");
    }
    if (strcmp(author, "*") != 0) {
        strcat(cond, " and Author like '");
        strcat(cond, author);
        strcat(cond, "'");
    }
    if (strcmp(press, "*") != 0) {
        strcat(cond, " and Press like '");
        strcat(cond, press);
        strcat(cond, "'");
    }
    if (strcmp(isbn, "*") != 0) {
        strcat(cond, " and ISBN = '");
        strcat(cond, isbn);
        strcat(cond, "'");
    }
    if (bookid != 0) {
        char strid[12] = {0};
        itoa(bookid, strid, 10);
        strcat(cond, " and Id = ");
        strcat(cond, strid);
    }
    if (page >= 0) {
        char offset  [12] = {0};
        char pagesize[12] = {0};
        itoa(page * PAGE_SIZE, offset  , 10);
        itoa(1    * PAGE_SIZE, pagesize, 10);
        strcat(cond, " limit " );
        strcat(cond, pagesize  );
        strcat(cond, " offset ");
        strcat(cond, offset    );
    }

    if (total) {
        sprintf(sql, "select count(*) from BookTable ");
        strcat (sql, cond);
        strcat (sql, ";");
        rc = sqlite3_exec(db, sql, callback_get_count, total, &err);
        if (rc) {
            printf("failed to get count !\n");
            printf("%s.\n", err);
    //      goto done;
        }
    }

    sprintf(sql, "select * from BookTable ");
    strcat (sql, cond);
    strcat (sql, ";");

    data[0] = &rows;
    data[1] =  list;
    rc = sqlite3_exec(db, sql, callback_book_query, data, &err);
    if (rc) {
        printf("failed to query book !\n");
        printf("%s.\n", err);
//      goto done;
    }

done:
    if (db) sqlite3_close(db);
    if (num) *num = rows;
    return rc;
}

int libdb_modify_book(BOOKITEM *item)
{
    char     sql[256];
    sqlite3 *db  = NULL;
    char    *err = NULL;
    int      rc  = 0;

    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    sprintf(sql, "update BookTable set Name = '%s', Author = '%s', Press = '%s', Price = %f, ISBN = '%s', Shelf = '%s', Status = %d, Comment = '%s' where Id = %d;",
        item->name, item->author, item->press, item->price, item->isbn, item->shelf, item->status, item->comment, item->id);
    rc = sqlite3_exec(db, sql, NULL, NULL, &err);
    if (rc) {
        printf("failed to modify book !\n");
        printf("%s.\n", err);
//      goto done;
    }

done:
    if (db) sqlite3_close(db);
    return rc;
}

int libdb_add_user(char *name, int sex, char *idcard, char *password, DWORD *userid)
{
    char     sql[256];
    sqlite3 *db  = NULL;
    char    *err = NULL;
    int      rc  = 0;

    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    sex = sex ? 1 : 0;
    sprintf(sql, "insert into UserTable (Name, Sex, IdCardNum, Password) values ('%s', %d, '%s', '%s');",
        name, sex, idcard, password);
    rc = sqlite3_exec(db, sql, NULL, NULL, &err);
    if (rc) {
        printf("failed to add user !\n");
        printf("%s.\n", err);
//      goto done;
    } else {
        *userid = (DWORD)sqlite3_last_insert_rowid(db);
    }

done:
    if (db) sqlite3_close(db);
    return rc;
}

int libdb_query_user(char *name, int sex, char *idcard, DWORD userid, int *total, int page, USERITEM *list, int *num)
{
    char     sql [256];
    char     cond[256];
    sqlite3 *db  = NULL;
    char    *err = NULL;
    int      rc  = 0;
    int      rows= 0;
    void    *data[2];

    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    sprintf(cond, " where TRUE");
    if (strcmp(name, "*") != 0) {
        strcat(cond, " and Name like '");
        strcat(cond, name);
        strcat(cond, "'");
    }
    if (sex != -1) {
        char str[12] = {0};
        itoa(sex, str, 10);
        strcat(cond, " and Sex = ");
        strcat(cond, str);
    }
    if (strcmp(idcard, "*") != 0) {
        strcat(cond, " and IdCardNum like '");
        strcat(cond, idcard);
        strcat(cond, "'");
    }
    if (userid != 0) {
        char str[12] = {0};
        itoa(userid, str, 10);
        strcat(cond, " and Id = ");
        strcat(cond, str);
    }
    if (page >= 0) {
        char offset  [12] = {0};
        char pagesize[12] = {0};
        itoa(page * PAGE_SIZE, offset  , 10);
        itoa(1    * PAGE_SIZE, pagesize, 10);
        strcat(cond, " limit " );
        strcat(cond, pagesize  );
        strcat(cond, " offset ");
        strcat(cond, offset    );
    }

    if (total) {
        sprintf(sql, "select count(*) from UserTable ");
        strcat (sql, cond);
        strcat (sql, ";");
        rc = sqlite3_exec(db, sql, callback_get_count, total, &err);
        if (rc) {
            printf("failed to get count !\n");
            printf("%s.\n", err);
    //      goto done;
        }
    }

    sprintf(sql, "select * from UserTable ");
    strcat (sql, cond);
    strcat (sql, ";");

    data[0] = &rows;
    data[1] =  list;
    rc = sqlite3_exec(db, sql, callback_user_query, data, &err);
    if (rc) {
        printf("failed to query user !\n");
        printf("%s.\n", err);
//      goto done;
    }

done:
    if (db) sqlite3_close(db);
    if (num) *num = rows;
    return rc;
}

int libdb_modify_user(USERITEM *item)
{
    char     sql[256];
    sqlite3 *db  = NULL;
    char    *err = NULL;
    int      rc  = 0;

    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    sprintf(sql, "update UserTable set Name = '%s', Sex = %d, IdCardNum = '%s', MaxBrwNum = %d, MaxBrwDays = %d, Password = '%s' where Id = %d;",
        item->name, item->sex, item->idcard, item->maxbrwnum, item->maxbrwdays, item->password, item->id);
    rc = sqlite3_exec(db, sql, NULL, NULL, &err);
    if (rc) {
        printf("failed to modify user !\n");
        printf("%s.\n", err);
//      goto done;
    }

done:
    if (db) sqlite3_close(db);
    return rc;
}

int libdb_query_borrow(DWORD userid, char *datetime, int before, int *total, int page, BOOKITEM *list, int *num)
{
    char     sql[256];
    sqlite3 *db  = NULL;
    char    *err = NULL;
    int      rc  = 0;
    int      rows= 0;
    void    *data[2];

    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    if (total) {
        sprintf(sql, "select count(BookTable.Id) from BookTable, BorrowTable where BookTable.Id = BorrowTable.BookId and BorrowTable.UserId = %d and BorrowTable.ActualRetDate = date('9999-12-31')", userid);
        if (datetime) {
            strcat(sql, " and BorrowTable.BorrowDate ");
            strcat(sql, before ? "<= datetime('" : ">= datetime('");
            strcat(sql, datetime);
            strcat(sql, "')");
        }
        strcat(sql, ";");
        rc = sqlite3_exec(db, sql, callback_get_count, total, &err);
        if (rc) {
            printf("failed to get borrow count !\n");
            printf("%s.\n", err);
    //      goto done;
        }
    }

    sprintf(sql, "select BookTable.* from BookTable, BorrowTable where BookTable.Id = BorrowTable.BookId and BorrowTable.UserId = %d and BorrowTable.ActualRetDate = date('9999-12-31')", userid);
    if (datetime) {
        strcat(sql, " and BorrowTable.BorrowDate ");
        strcat(sql, before ? "<= datetime('" : ">= datetime('");
        strcat(sql, datetime);
        strcat(sql, "')");
    }
    if (page >= 0) {
        char offset  [12] = {0};
        char pagesize[12] = {0};
        itoa(page * PAGE_SIZE, offset  , 10);
        itoa(1    * PAGE_SIZE, pagesize, 10);
        strcat(sql, " limit " );
        strcat(sql, pagesize  );
        strcat(sql, " offset ");
        strcat(sql, offset    );
    }
    strcat(sql, ";");

    data[0] = &rows;
    data[1] =  list;
    rc = sqlite3_exec(db, sql, callback_book_query, data, &err);
    if (rc) {
        printf("failed to query borrow book !\n");
        printf("%s.\n", err);
//      goto done;
    }

done:
    if (db) sqlite3_close(db);
    if (num) *num = rows;
    return rc;
}

int libdb_borrow_book(DWORD userid, DWORD bookid, char *datetime, int maxbrwdays)
{
    char     sql[256];
    sqlite3 *db  = NULL;
    char    *err = NULL;
    int      rc  = 0;

    rc = sqlite3_open("library.db", &db);
    if (rc) {
        printf("failed to open database !\n");
        goto done;
    }

    sprintf(sql, "update BookTable set Status = 2 where Id = %d;"
                 "insert into BorrowTable (BookId, UserId, BorrowDate, LimitRetDate) values (%d, %d, datetime('%s'), datetime('%s', '+%d days'));",
            bookid, bookid, userid, datetime, datetime, maxbrwdays);
    rc = sqlite3_exec(db, sql, NULL, NULL, &err);
    if (rc) {
        printf("failed to query borrow book !\n");
        printf("%s.\n", err);
//      goto done;
    }

done:
    if (db) sqlite3_close(db);
    return rc;
}

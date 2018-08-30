#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include "stdefine.h"
#include "dbapi.h"
#include "adminui.h"

#define ADMIN_PASSWORD  "admin888"
#define MAX_RETRY_TIMES 3

static char *BOOK_STATUS_STR[] = {
    "未知",
    "在库",
    "借出",
    "损坏",
    "遗失",
};

static char *USER_SEX_STR[] = {
    "男",
    "女",
};

static void handle_init_db(void)
{
    char str [16];
    char temp[256];
    int  rc = 0;
    printf("\n请输入 confirm_initdb 以确认执行初始化操作：");
    scanf("%14s", str); fgets(temp, 256, stdin);
    if (strcmp(str, "confirm_initdb") == 0) {
        printf("正在执行初始化数据库操作...\n");
        rc = libdb_init();
        printf("初始化数据库%s!\n", rc ? "失败" : "成功");
    } else {
        printf("无效的输入，放弃执行初始化数据库操作！\n");
    }
}

static void handle_add_book(void)
{
    char     temp[256];
    BOOKITEM item   = {0};
    DWORD    bookid = 0;
    int      rc     = 0;

    printf("\n");
    printf("新书录入\r\n");
    printf("--------\r\n");
    printf("书  名："); scanf("%63s" , item.name   ); fgets(temp, 256, stdin);
    printf("作  者："); scanf("%63s" , item.author ); fgets(temp, 256, stdin);
    printf("出版社："); scanf("%63s" , item.press  ); fgets(temp, 256, stdin);
    printf("价  格："); scanf("%lf"  ,&item.price  ); fgets(temp, 256, stdin);
    printf("ISBN  ："); scanf("%15s" , item.isbn   ); fgets(temp, 256, stdin);
    printf("书  架："); scanf("%15s" , item.shelf  ); fgets(temp, 256, stdin);
    printf("备  注："); scanf("%255s", item.comment); fgets(temp, 256, stdin);
    rc = libdb_add_book(item.name, item.author, item.press, item.price, item.isbn, item.shelf, item.comment, &bookid);
    if (rc) {
        printf("\n录入失败！\n");
    } else {
        printf("\n录入成功！\n");
        printf("该书的编码为：B%010d\n", bookid);
        printf("正在打印条码...   \n"); Sleep(1000);
        printf("打印条码打印完毕！\n"); Sleep(500 );
        printf("请确保将条码正确的贴在书籍的指定位置！\n\n");
    }

    printf("(任意键继续...)\n");
    getch();
}

static void handle_mod_book(void)
{
    BOOKITEM item[PAGE_SIZE];
    char  barcode[12 ] = {0};
    char  temp   [256];
    char  yesno  [2];
    DWORD bookid = 0;
    int   rc     = 0;
    int   n      = 0;

    printf("\n");
    printf("书籍信息修改\r\n");
    printf("------------\r\n");
    printf("请扫描条码（或输入图书编码）："); scanf("%11s", barcode); fgets(temp, 256, stdin);
    if (barcode[0] != 'B') {
        printf("\n非法的图书编码！\n");
        printf("(任意键继续...)\n");
        getch();
        return;
    }

    bookid = atoi(&barcode[1]);
    libdb_query_book("*", "*", "*", "*", bookid, NULL, -1, item, &n);
    if (n == 0) {
        printf("\n没有找到该书籍的相关信息！\n");
        printf("(任意键继续...)\n");
        getch();
        return;
    }

    if (item[0].status < 0) item[0].status = 0;
    if (item[0].status > 4) item[0].status = 4;

    printf("\n");
    printf("书籍信息：\n");
    printf("----------\n");

    printf("书  名：%s\n"    , item[0].name);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的书名："); scanf("%63s", item[0].name); fgets(temp, 256, stdin);
    }

    printf("作  者：%s\n"    , item[0].author);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的作者："); scanf("%63s", item[0].author); fgets(temp, 256, stdin);
    }

    printf("出版社：%s\n"    , item[0].press);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的出版社："); scanf("%63s", item[0].press); fgets(temp, 256, stdin);
    }

    printf("价  格：%.2f\n"  , item[0].price);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的价格："); scanf("%lf", &item[0].price); fgets(temp, 256, stdin);
    }

    printf("ISBN  ：%s\n"    , item[0].isbn);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的 ISBN："); scanf("%15s", item[0].isbn); fgets(temp, 256, stdin);
    }

    printf("书  架：%s\n"    , item[0].shelf);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的书架："); scanf("%15s", item[0].shelf); fgets(temp, 256, stdin);
    }

    printf("状  态：%s\n"    , BOOK_STATUS_STR[item[0].status]);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请选择新的状态：\n");
        printf("0 - 未知\n");
        printf("1 - 在库\n");
        printf("2 - 借出\n");
        printf("3 - 损坏\n");
        printf("3 - 遗失\n");
        scanf("%d", &(item[0].status));
    }

    printf("备  注：%s\n"    , item[0].comment);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的备注："); scanf("%255s", item[0].comment); fgets(temp, 256, stdin);
    }

    rc = libdb_modify_book(&(item[0]));
    printf("修改书籍信息%s!\n", rc ? "失败" : "成功");
    printf("(任意键继续...)\n");
    getch();
}

static void handle_query_book_by_barcode(void)
{
    BOOKITEM item[PAGE_SIZE];
    char  barcode[12 ] = {0};
    char  temp   [256];
    DWORD bookid = 0;
    int   rc     = 0;
    int   n      = 0;

    printf("\n");
    printf("扫码查书\r\n");
    printf("--------\r\n");
    printf("请扫描条码："); scanf("%11s", barcode); fgets(temp, 256, stdin);
    if (barcode[0] != 'B') {
        printf("\n非法的图书编码！\n");
        printf("(任意键继续...)\n");
        getch();
        return;
    }

    bookid = atoi(&barcode[1]);
    libdb_query_book("*", "*", "*", "*", bookid, NULL, -1, item, &n);
    if (n == 0) {
        printf("\n没有找到该书籍的相关信息！\n");
        printf("(任意键继续...)\n");
        getch();
        return;
    }

    if (item[0].status < 0) item[0].status = 0;
    if (item[0].status > 4) item[0].status = 4;

    printf("\n");
    printf("书籍详情：\n");
    printf("----------\n");
    printf("编  码：B%010d\n", item[0].id);
    printf("书  名：%s\n"    , item[0].name);
    printf("作  者：%s\n"    , item[0].author);
    printf("出版社：%s\n"    , item[0].press);
    printf("价  格：%.2f\n"  , item[0].price);
    printf("ISBN  ：%s\n"    , item[0].isbn);
    printf("书  架：%s\n"    , item[0].shelf);
    printf("状  态：%s\n"    , BOOK_STATUS_STR[item[0].status]);
    printf("备  注：%s\n"    , item[0].comment);
    printf("\n");
    printf("(任意键继续...)\n");
    getch();
}

static void handle_query_book_by_condition(void)
{
    BOOKITEM item[PAGE_SIZE];
    char  name   [64];
    char  author [64];
    char  press  [64];
    char  isbn   [16 ];
    char  temp   [256];
    int   total   = 0;
    int   pagenum = 0;
    int   pagecur = 0;
    int   rc      = 0;
    int   n       = 0;
    int   i;

    printf("\n");
    printf("条件查书\r\n");
    printf("--------\r\n");
    printf("请输入查询条件：\n");
    printf("书  名："); scanf("%63s" , name   ); fgets(temp, 256, stdin);
    printf("作  者："); scanf("%63s" , author ); fgets(temp, 256, stdin);
    printf("出版社："); scanf("%63s" , press  ); fgets(temp, 256, stdin);
    printf("ISBN  ："); scanf("%15s" , isbn   ); fgets(temp, 256, stdin);

    libdb_query_book(name, author, press, isbn, 0, &total, -1, NULL, NULL);
    pagenum = (total + PAGE_SIZE - 1) / PAGE_SIZE;
    if (total == 0) {
        printf("未找到符合查询条件的书籍！\n");
        goto done;
    }

    while (1) {
        libdb_query_book(name, author, press, isbn, 0, NULL, pagecur, item, &n);
        printf("\n");
        printf("编码         书名          作者      出版社        价格   状态  书架\n");
        printf("----------------------------------------------------------------------\n");
        for (i=0; i<n&&i<PAGE_SIZE; i++) {
            if (item[i].status < 0) item[i].status = 0;
            if (item[i].status > 4) item[i].status = 4;
            printf("B%010d  %-12s  %-8s  %-12s  %5.2f  %s  %s\n", item[i].id, item[i].name, item[i].author, item[i].press, item[i].price, BOOK_STATUS_STR[item[0].status], item[i].shelf);
        }
        printf("\n");
        printf("记录数：%d      分页：%d/%d\n\n", total, pagecur+1, pagenum);
        switch (getch()) {
        case ',':
        case '-':
        case '<':
            if (pagecur > 0) pagecur--;
            break;
        case '.':
        case '+':
        case '>':
            if (pagecur < pagenum - 1) pagecur++;
            break;
        case 'q':
        case 'Q':
            return;
        }
    }

done:
    printf("(任意键继续...)\n");
    getch();
}

static void handle_add_user(void)
{
    char     temp[256];
    USERITEM item   = {0};
    DWORD    userid = 0;
    int      rc     = 0;

    printf("\n");
    printf("新用户注册\r\n");
    printf("----------\r\n");
    printf("姓  名："); scanf("%32s", item.name    ); fgets(temp, 256, stdin);
    printf("性  别：（请选择 0-男 1-女）"); scanf("%d", &item.sex); fgets(temp, 256, stdin);
    printf("身份证："); scanf("%18s", item.idcard  ); fgets(temp, 256, stdin);
    printf("密  码："); scanf("%15s", item.password); fgets(temp, 256, stdin);
    rc = libdb_add_user(item.name, item.sex, item.idcard, item.password, &userid);
    if (rc) {
        printf("\n注册失败！\n");
    } else {
        printf("\n注册成功！\n");
        printf("新用户的编码为：U%010d\n", userid);
        printf("请根据该编码制作用户借书卡！\n\n");
    }

    printf("(任意键继续...)\n");
    getch();
}

static void handle_mod_user(void)
{
    USERITEM item[PAGE_SIZE];
    char  barcode[12 ] = {0};
    char  temp   [256];
    char  yesno  [2];
    DWORD userid = 0;
    int   rc     = 0;
    int   n      = 0;

    printf("\n");
    printf("用户信息修改\r\n");
    printf("------------\r\n");
    printf("请扫描条码（或输入借书卡编码）："); scanf("%11s", barcode); fgets(temp, 256, stdin);
    if (barcode[0] != 'U') {
        printf("\n非法的借书卡编码！\n");
        printf("(任意键继续...)\n");
        getch();
        return;
    }

    userid = atoi(&barcode[1]);
    libdb_query_user("*", -1, "*", userid, NULL, -1, item, &n);
    if (n == 0) {
        printf("\n没有找到该用户的相关信息！\n");
        printf("(任意键继续...)\n");
        getch();
        return;
    }

    printf("\n");
    printf("用户信息：\n");
    printf("----------\n");

    printf("姓  名：%s\n"    , item[0].name);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的姓名："); scanf("%32s", item[0].name); fgets(temp, 256, stdin);
    }

    printf("性  别：%s\n"    , USER_SEX_STR[item[0].sex]);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请选择新的性别：（0-男 1-女）"); scanf("%d", &item[0].sex); fgets(temp, 256, stdin);
    }

    printf("身份证：%s\n"    , item[0].idcard);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的身份证号码："); scanf("%18s", item[0].idcard); fgets(temp, 256, stdin);
    }

    printf("借书数限：%d\n"  , item[0].maxbrwnum);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的借书数限："); scanf("%d", &item[0].maxbrwnum); fgets(temp, 256, stdin);
    }

    printf("借书时限：%d\n"  , item[0].maxbrwdays);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的借书时限："); scanf("%d", &item[0].maxbrwdays); fgets(temp, 256, stdin);
    }

    printf("密  码：%s\n"    , item[0].password);
    printf("需要修改吗？(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("请输入新的密码："); scanf("%15s", item[0].password); fgets(temp, 256, stdin);
    }

    rc = libdb_modify_user(&(item[0]));
    printf("修改用户信息%s!\n", rc ? "失败" : "成功");
    printf("(任意键继续...)\n");
    getch();
}

static void handle_query_user_by_barcode(void)
{
    USERITEM item[PAGE_SIZE];
    char  barcode[12 ] = {0};
    char  temp   [256];
    DWORD userid = 0;
    int   rc     = 0;
    int   n      = 0;

    printf("\n");
    printf("扫码查用户\r\n");
    printf("----------\r\n");
    printf("请扫描条码："); scanf("%11s", barcode); fgets(temp, 256, stdin);
    if (barcode[0] != 'U') {
        printf("\n非法的用户编码！\n");
        printf("(任意键继续...)\n");
        getch();
        return;
    }

    userid = atoi(&barcode[1]);
    libdb_query_user("*", -1, "*", userid, NULL, -1, item, &n);
    if (n == 0) {
        printf("\n没有找到该用户的相关信息！\n");
        printf("(任意键继续...)\n");
        getch();
        return;
    }

    printf("\n");
    printf("用户详情：\n");
    printf("----------\n");
    printf("编  码：U%010d\n", item[0].id);
    printf("姓  名：%s\n"    , item[0].name);
    printf("性  别：%s\n"    , USER_SEX_STR[item[0].sex]);
    printf("身份证：%s\n"    , item[0].idcard);
    printf("数  限：%d\n"    , item[0].maxbrwnum);
    printf("时  限：%d\n"    , item[0].maxbrwdays);
    printf("密  码：%s\n"    , item[0].password);
    printf("\n");
    printf("(任意键继续...)\n");
    getch();
}

static void handle_query_user_by_condition(void)
{
    USERITEM item[PAGE_SIZE];
    char  name   [33];
    int   sex     = -1;
    char  idcard [20];
    char  temp   [256];
    int   total   = 0;
    int   pagenum = 0;
    int   pagecur = 0;
    int   rc      = 0;
    int   n       = 0;
    int   i;

    printf("\n");
    printf("条件查用户\r\n");
    printf("----------\r\n");
    printf("请输入查询条件：\n");
    printf("姓  名："); scanf("%32s", name    ); fgets(temp, 256, stdin);
    printf("性  别：（请选择 0-男 1-女）"); scanf("%d", &sex); fgets(temp, 256, stdin);
    printf("身份证："); scanf("%18s", idcard  ); fgets(temp, 256, stdin);

    libdb_query_user(name, sex, idcard, 0, &total, -1, NULL, NULL);
    pagenum = (total + PAGE_SIZE - 1) / PAGE_SIZE;
    if (total == 0) {
        printf("\n未找到符合查询条件的用户！\n");
        goto done;
    }

    while (1) {
        libdb_query_user(name, sex, idcard, 0, NULL, pagecur, item, &n);
        printf("\n");
        printf("编码         姓名      性别  身份证             数限  时限   密码\n");
        printf("-----------------------------------------------------------------------\n");
        for (i=0; i<n&&i<PAGE_SIZE; i++) {
            printf("U%010d  %-8s  %-4s  %s  %-4d  %-4d  %s\n", item[i].id, item[i].name, USER_SEX_STR[item[i].sex], item[i].idcard, item[i].maxbrwnum, item[i].maxbrwdays, item[i].password);
        }
        printf("\n");
        printf("记录数：%d      分页：%d/%d\n\n", total, pagecur+1, pagenum);
        switch (getch()) {
        case ',':
        case '-':
        case '<':
            if (pagecur > 0) pagecur--;
            break;
        case '.':
        case '+':
        case '>':
            if (pagecur < pagenum - 1) pagecur++;
            break;
        case 'q':
        case 'Q':
            return;
        }
    }

done:
    printf("(任意键继续...)\n");
    getch();
}

static void handle_borrow_book(void)
{
    USERITEM userlist[PAGE_SIZE];
    BOOKITEM booklist[PAGE_SIZE];
    char   barcode[12] = {0};
    char   temp[256];
    DWORD  userid = 0;
    DWORD  bookid = 0;
    int    total  = 0;
    int    n      = 0;
    int    i;
    time_t t      = time(NULL);
    char   stime[64];
    strftime(stime, sizeof(stime), "%Y-%m-%d %H:%M:%S", localtime(&t));

    printf("\n");
    printf("借书\r\n");
    printf("----\r\n");
    printf("请扫描借书卡条码："); scanf("%11s", barcode); fgets(temp, 256, stdin);
    if (barcode[0] != 'U') {
        printf("\n非法的用户编码！\n");
        printf("(任意键继续...)\n");
        getch();
        return;
    }

    userid = atoi(&barcode[1]);
    libdb_query_user("*", -1, "*", userid, NULL, -1, userlist, &n);
    if (n == 0) {
        printf("\n没有找到该用户的相关信息！\n");
        printf("(任意键继续...)\n");
        getch();
        return;
    }

    while (1) {
        libdb_query_borrow(userid, NULL, 0, &total, -1, NULL, NULL);
        printf("\n");
        printf("姓名：%s    借书数限：%d 本    借书时限：%d 天    已借书：%d 本\n", userlist[0].name, userlist[0].maxbrwnum, userlist[0].maxbrwdays, total);
        printf("---------------------------------------------------------------\n");
        if (total >= userlist[0].maxbrwnum) {
            printf("已经借满 %d 本书了！\n", userlist[0].maxbrwnum);
            printf("(任意键继续...)\n");
            getch();
            return;
        }

        n = 0; libdb_query_borrow(userid, stime, 0, NULL, 0, booklist, &n);
        for (i=0; i<n&&i<PAGE_SIZE; i++) {
            printf("B%010d  %-12s  %-8s  %-12s  %5.2f  %s\n", booklist[i].id, booklist[i].name, booklist[i].author, booklist[i].press, booklist[i].price, BOOK_STATUS_STR[booklist[0].status]);
        }

        printf("请扫描书籍条码："); scanf("%11s", barcode); fgets(temp, 256, stdin);
        if (strcmp(barcode, "quit") == 0) break;
        if (barcode[0] != 'B') {
            printf("\n非法的书籍编码！\n");
        } else {
            bookid = atoi(&barcode[1]);
            libdb_borrow_book(userid, bookid, stime, userlist[0].maxbrwdays);
        }
    }
}

static void handle_return_book(void)
{
}

int enter_adminui(char *code)
{
    char password[16];
    char temp[256];
    int  login = 0;
    int  retry = MAX_RETRY_TIMES;
    static DWORD last_login_fail_tick = 0;

    if (last_login_fail_tick != 0 && GetTickCount() - last_login_fail_tick < 30 * 60 * 1000) {
        printf("\n哥们儿！\n你尝试输入管理密码太频繁了吧！\n半小时之后再进来！\n\n！");
        return 0;
    }

    printf("\n");
    printf("请输入管理密码：");
    scanf("%15s", password); fgets(temp, 256, stdin);

    while (1) {
        if (strcmp(password, ADMIN_PASSWORD) == 0) {
            login = 1;
            break;
        }
        if (--retry) {
            printf("\n");
            printf("密码错误，请重新输入（%d）：", retry);
            scanf("%15s", password); fgets(temp, 256, stdin);
        } else {
            break;
        }
    }

    if (!login) {
        printf("\n");
        printf("登录管理失败，请稍后重试！\n\n");
        last_login_fail_tick = GetTickCount();
    }

    while (1) {
        char op[2];
        printf("\n");
        printf("请选择操作：\n");
        printf("------------\n");
        printf("1. 扫码查书\n");
        printf("2. 条件查书\n");
        printf("3. 新书录入\n");
        printf("4. 书籍信息修改\n");
        printf("5. 扫码查用户\n");
        printf("6. 条件查用户\n");
        printf("7. 新用户注册\n");
        printf("8. 用户信息修改\n");
        printf("9. 初始化数据库\n");
        printf("B. 借书\n");
        printf("R. 还书\n");
        printf("Q. 登出\n\n");
        scanf("%1s", op); fgets(temp, 256, stdin);
        if (op[0] == 'Q') {
            printf("\n已退出管理界面\n\n");
            break;
        } else {
            switch (op[0]) {
            case '1': handle_query_book_by_barcode  (); break;
            case '2': handle_query_book_by_condition(); break;
            case '3': handle_add_book(); break;
            case '4': handle_mod_book(); break;
            case '5': handle_query_user_by_barcode  (); break;
            case '6': handle_query_user_by_condition(); break;
            case '7': handle_add_user(); break;
            case '8': handle_mod_user(); break;
            case '9': handle_init_db (); break;
            case 'B': handle_borrow_book(); break;
            case 'R': handle_return_book(); break;
            }
        }
    }

    return 0;
}


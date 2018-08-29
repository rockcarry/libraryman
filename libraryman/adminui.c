#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
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
    char  name   [64];
    char  author [64];
    char  press  [64];
    float price;
    char  isbn   [16 ];
    char  shelf  [16 ];
    char  comment[256];
    char  temp   [256];
    DWORD bookid = 0;
    int   rc     = 0;

    printf("\n");
    printf("新书录入\r\n");
    printf("--------\r\n");
    printf("书  名："); scanf("%63s" , name   ); fgets(temp, 256, stdin);
    printf("作  者："); scanf("%63s" , author ); fgets(temp, 256, stdin);
    printf("出版社："); scanf("%63s" , press  ); fgets(temp, 256, stdin);
    printf("价  格："); scanf("%f"   ,&price  ); fgets(temp, 256, stdin);
    printf("ISBN  ："); scanf("%15s" , isbn   ); fgets(temp, 256, stdin);
    printf("书  架："); scanf("%15s" , shelf  ); fgets(temp, 256, stdin);
    printf("备  注："); scanf("%255s", comment); fgets(temp, 256, stdin);
    rc = libdb_add_book(name, author, press, price, isbn, shelf, comment, &bookid);
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
    BOOKITEM item[10 ] = {0};
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
    libdb_query_book("*", "*", "*", "*", bookid, -1, item, &n);
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
        printf("请输入新的价格："); scanf("%f", &(item[0].price)); fgets(temp, 256, stdin);
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
    BOOKITEM item[10 ] = {0};
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
    libdb_query_book("*", "*", "*", "*", bookid, -1, item, &n);
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
    BOOKITEM item[10];
    char  name   [64];
    char  author [64];
    char  press  [64];
    char  isbn   [16 ];
    char  temp   [256];
    int   rc = 0;
    int   n  = 0;
    int   i;

    printf("\n");
    printf("条件查书\r\n");
    printf("--------\r\n");
    printf("请输入查询条件：\n");
    printf("书  名："); scanf("%63s" , name   ); fgets(temp, 256, stdin);
    printf("作  者："); scanf("%63s" , author ); fgets(temp, 256, stdin);
    printf("出版社："); scanf("%63s" , press  ); fgets(temp, 256, stdin);
    printf("ISBN  ："); scanf("%15s" , isbn   ); fgets(temp, 256, stdin);
    libdb_query_book(name, author, press, isbn, 0, 0, item, &n);

    printf("\n");
    printf("编码\t\t书名\t\t作者\t出版社\t\t价格\t状态\t书架\n");
    printf("------------------------------------------------------------------------------\n");
    for (i=0; i<n; i++) {
        if (item[i].status < 0) item[i].status = 0;
        if (item[i].status > 4) item[i].status = 4;
        printf("B%010d\t%-10s\t%s\t%-10s\t%.2f\t%s\t%s\n", item[i].id, item[i].name, item[i].author, item[i].press, item[i].price, BOOK_STATUS_STR[item[0].status], item[i].shelf);
    }
    printf("\n");
    printf("(任意键继续...)\n");
    getch();
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
        if (op[0] == '8') {
            printf("\n已退出管理界面\n\n");
            break;
        } else {
            switch (op[0]) {
            case '1':
                handle_query_book_by_barcode();
                break;
            case '2':
                handle_query_book_by_condition();
                break;
            case '3':
                handle_add_book();
                break;
            case '4':
                handle_mod_book();
                break;
            case '9':
                handle_init_db();
                break;
            }
        }
    }

    return 0;
}


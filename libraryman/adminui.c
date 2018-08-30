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
    "δ֪",
    "�ڿ�",
    "���",
    "��",
    "��ʧ",
};

static char *USER_SEX_STR[] = {
    "��",
    "Ů",
};

static void handle_init_db(void)
{
    char str [16];
    char temp[256];
    int  rc = 0;
    printf("\n������ confirm_initdb ��ȷ��ִ�г�ʼ��������");
    scanf("%14s", str); fgets(temp, 256, stdin);
    if (strcmp(str, "confirm_initdb") == 0) {
        printf("����ִ�г�ʼ�����ݿ����...\n");
        rc = libdb_init();
        printf("��ʼ�����ݿ�%s!\n", rc ? "ʧ��" : "�ɹ�");
    } else {
        printf("��Ч�����룬����ִ�г�ʼ�����ݿ������\n");
    }
}

static void handle_add_book(void)
{
    char     temp[256];
    BOOKITEM item   = {0};
    DWORD    bookid = 0;
    int      rc     = 0;

    printf("\n");
    printf("����¼��\r\n");
    printf("--------\r\n");
    printf("��  ����"); scanf("%63s" , item.name   ); fgets(temp, 256, stdin);
    printf("��  �ߣ�"); scanf("%63s" , item.author ); fgets(temp, 256, stdin);
    printf("�����磺"); scanf("%63s" , item.press  ); fgets(temp, 256, stdin);
    printf("��  ��"); scanf("%lf"  ,&item.price  ); fgets(temp, 256, stdin);
    printf("ISBN  ��"); scanf("%15s" , item.isbn   ); fgets(temp, 256, stdin);
    printf("��  �ܣ�"); scanf("%15s" , item.shelf  ); fgets(temp, 256, stdin);
    printf("��  ע��"); scanf("%255s", item.comment); fgets(temp, 256, stdin);
    rc = libdb_add_book(item.name, item.author, item.press, item.price, item.isbn, item.shelf, item.comment, &bookid);
    if (rc) {
        printf("\n¼��ʧ�ܣ�\n");
    } else {
        printf("\n¼��ɹ���\n");
        printf("����ı���Ϊ��B%010d\n", bookid);
        printf("���ڴ�ӡ����...   \n"); Sleep(1000);
        printf("��ӡ�����ӡ��ϣ�\n"); Sleep(500 );
        printf("��ȷ����������ȷ�������鼮��ָ��λ�ã�\n\n");
    }

    printf("(���������...)\n");
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
    printf("�鼮��Ϣ�޸�\r\n");
    printf("------------\r\n");
    printf("��ɨ�����루������ͼ����룩��"); scanf("%11s", barcode); fgets(temp, 256, stdin);
    if (barcode[0] != 'B') {
        printf("\n�Ƿ���ͼ����룡\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    bookid = atoi(&barcode[1]);
    libdb_query_book("*", "*", "*", "*", bookid, NULL, -1, item, &n);
    if (n == 0) {
        printf("\nû���ҵ����鼮�������Ϣ��\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    if (item[0].status < 0) item[0].status = 0;
    if (item[0].status > 4) item[0].status = 4;

    printf("\n");
    printf("�鼮��Ϣ��\n");
    printf("----------\n");

    printf("��  ����%s\n"    , item[0].name);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µ�������"); scanf("%63s", item[0].name); fgets(temp, 256, stdin);
    }

    printf("��  �ߣ�%s\n"    , item[0].author);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µ����ߣ�"); scanf("%63s", item[0].author); fgets(temp, 256, stdin);
    }

    printf("�����磺%s\n"    , item[0].press);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µĳ����磺"); scanf("%63s", item[0].press); fgets(temp, 256, stdin);
    }

    printf("��  ��%.2f\n"  , item[0].price);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µļ۸�"); scanf("%lf", &item[0].price); fgets(temp, 256, stdin);
    }

    printf("ISBN  ��%s\n"    , item[0].isbn);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µ� ISBN��"); scanf("%15s", item[0].isbn); fgets(temp, 256, stdin);
    }

    printf("��  �ܣ�%s\n"    , item[0].shelf);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µ���ܣ�"); scanf("%15s", item[0].shelf); fgets(temp, 256, stdin);
    }

    printf("״  ̬��%s\n"    , BOOK_STATUS_STR[item[0].status]);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("��ѡ���µ�״̬��\n");
        printf("0 - δ֪\n");
        printf("1 - �ڿ�\n");
        printf("2 - ���\n");
        printf("3 - ��\n");
        printf("3 - ��ʧ\n");
        scanf("%d", &(item[0].status));
    }

    printf("��  ע��%s\n"    , item[0].comment);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µı�ע��"); scanf("%255s", item[0].comment); fgets(temp, 256, stdin);
    }

    rc = libdb_modify_book(&(item[0]));
    printf("�޸��鼮��Ϣ%s!\n", rc ? "ʧ��" : "�ɹ�");
    printf("(���������...)\n");
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
    printf("ɨ�����\r\n");
    printf("--------\r\n");
    printf("��ɨ�����룺"); scanf("%11s", barcode); fgets(temp, 256, stdin);
    if (barcode[0] != 'B') {
        printf("\n�Ƿ���ͼ����룡\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    bookid = atoi(&barcode[1]);
    libdb_query_book("*", "*", "*", "*", bookid, NULL, -1, item, &n);
    if (n == 0) {
        printf("\nû���ҵ����鼮�������Ϣ��\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    if (item[0].status < 0) item[0].status = 0;
    if (item[0].status > 4) item[0].status = 4;

    printf("\n");
    printf("�鼮���飺\n");
    printf("----------\n");
    printf("��  �룺B%010d\n", item[0].id);
    printf("��  ����%s\n"    , item[0].name);
    printf("��  �ߣ�%s\n"    , item[0].author);
    printf("�����磺%s\n"    , item[0].press);
    printf("��  ��%.2f\n"  , item[0].price);
    printf("ISBN  ��%s\n"    , item[0].isbn);
    printf("��  �ܣ�%s\n"    , item[0].shelf);
    printf("״  ̬��%s\n"    , BOOK_STATUS_STR[item[0].status]);
    printf("��  ע��%s\n"    , item[0].comment);
    printf("\n");
    printf("(���������...)\n");
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
    printf("��������\r\n");
    printf("--------\r\n");
    printf("�������ѯ������\n");
    printf("��  ����"); scanf("%63s" , name   ); fgets(temp, 256, stdin);
    printf("��  �ߣ�"); scanf("%63s" , author ); fgets(temp, 256, stdin);
    printf("�����磺"); scanf("%63s" , press  ); fgets(temp, 256, stdin);
    printf("ISBN  ��"); scanf("%15s" , isbn   ); fgets(temp, 256, stdin);

    libdb_query_book(name, author, press, isbn, 0, &total, -1, NULL, NULL);
    pagenum = (total + PAGE_SIZE - 1) / PAGE_SIZE;
    if (total == 0) {
        printf("δ�ҵ����ϲ�ѯ�������鼮��\n");
        goto done;
    }

    while (1) {
        libdb_query_book(name, author, press, isbn, 0, NULL, pagecur, item, &n);
        printf("\n");
        printf("����         ����          ����      ������        �۸�   ״̬  ���\n");
        printf("----------------------------------------------------------------------\n");
        for (i=0; i<n&&i<PAGE_SIZE; i++) {
            if (item[i].status < 0) item[i].status = 0;
            if (item[i].status > 4) item[i].status = 4;
            printf("B%010d  %-12s  %-8s  %-12s  %5.2f  %s  %s\n", item[i].id, item[i].name, item[i].author, item[i].press, item[i].price, BOOK_STATUS_STR[item[0].status], item[i].shelf);
        }
        printf("\n");
        printf("��¼����%d      ��ҳ��%d/%d\n\n", total, pagecur+1, pagenum);
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
    printf("(���������...)\n");
    getch();
}

static void handle_add_user(void)
{
    char     temp[256];
    USERITEM item   = {0};
    DWORD    userid = 0;
    int      rc     = 0;

    printf("\n");
    printf("���û�ע��\r\n");
    printf("----------\r\n");
    printf("��  ����"); scanf("%32s", item.name    ); fgets(temp, 256, stdin);
    printf("��  �𣺣���ѡ�� 0-�� 1-Ů��"); scanf("%d", &item.sex); fgets(temp, 256, stdin);
    printf("���֤��"); scanf("%18s", item.idcard  ); fgets(temp, 256, stdin);
    printf("��  �룺"); scanf("%15s", item.password); fgets(temp, 256, stdin);
    rc = libdb_add_user(item.name, item.sex, item.idcard, item.password, &userid);
    if (rc) {
        printf("\nע��ʧ�ܣ�\n");
    } else {
        printf("\nע��ɹ���\n");
        printf("���û��ı���Ϊ��U%010d\n", userid);
        printf("����ݸñ��������û����鿨��\n\n");
    }

    printf("(���������...)\n");
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
    printf("�û���Ϣ�޸�\r\n");
    printf("------------\r\n");
    printf("��ɨ�����루��������鿨���룩��"); scanf("%11s", barcode); fgets(temp, 256, stdin);
    if (barcode[0] != 'U') {
        printf("\n�Ƿ��Ľ��鿨���룡\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    userid = atoi(&barcode[1]);
    libdb_query_user("*", -1, "*", userid, NULL, -1, item, &n);
    if (n == 0) {
        printf("\nû���ҵ����û��������Ϣ��\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    printf("\n");
    printf("�û���Ϣ��\n");
    printf("----------\n");

    printf("��  ����%s\n"    , item[0].name);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µ�������"); scanf("%32s", item[0].name); fgets(temp, 256, stdin);
    }

    printf("��  ��%s\n"    , USER_SEX_STR[item[0].sex]);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("��ѡ���µ��Ա𣺣�0-�� 1-Ů��"); scanf("%d", &item[0].sex); fgets(temp, 256, stdin);
    }

    printf("���֤��%s\n"    , item[0].idcard);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µ����֤���룺"); scanf("%18s", item[0].idcard); fgets(temp, 256, stdin);
    }

    printf("�������ޣ�%d\n"  , item[0].maxbrwnum);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µĽ������ޣ�"); scanf("%d", &item[0].maxbrwnum); fgets(temp, 256, stdin);
    }

    printf("����ʱ�ޣ�%d\n"  , item[0].maxbrwdays);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µĽ���ʱ�ޣ�"); scanf("%d", &item[0].maxbrwdays); fgets(temp, 256, stdin);
    }

    printf("��  �룺%s\n"    , item[0].password);
    printf("��Ҫ�޸���(Y/N) "); scanf("%1s", yesno); fgets(temp, 256, stdin);
    if (yesno[0] == 'Y') {
        printf("�������µ����룺"); scanf("%15s", item[0].password); fgets(temp, 256, stdin);
    }

    rc = libdb_modify_user(&(item[0]));
    printf("�޸��û���Ϣ%s!\n", rc ? "ʧ��" : "�ɹ�");
    printf("(���������...)\n");
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
    printf("ɨ����û�\r\n");
    printf("----------\r\n");
    printf("��ɨ�����룺"); scanf("%11s", barcode); fgets(temp, 256, stdin);
    if (barcode[0] != 'U') {
        printf("\n�Ƿ����û����룡\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    userid = atoi(&barcode[1]);
    libdb_query_user("*", -1, "*", userid, NULL, -1, item, &n);
    if (n == 0) {
        printf("\nû���ҵ����û��������Ϣ��\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    printf("\n");
    printf("�û����飺\n");
    printf("----------\n");
    printf("��  �룺U%010d\n", item[0].id);
    printf("��  ����%s\n"    , item[0].name);
    printf("��  ��%s\n"    , USER_SEX_STR[item[0].sex]);
    printf("���֤��%s\n"    , item[0].idcard);
    printf("��  �ޣ�%d\n"    , item[0].maxbrwnum);
    printf("ʱ  �ޣ�%d\n"    , item[0].maxbrwdays);
    printf("��  �룺%s\n"    , item[0].password);
    printf("\n");
    printf("(���������...)\n");
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
    printf("�������û�\r\n");
    printf("----------\r\n");
    printf("�������ѯ������\n");
    printf("��  ����"); scanf("%32s", name    ); fgets(temp, 256, stdin);
    printf("��  �𣺣���ѡ�� 0-�� 1-Ů��"); scanf("%d", &sex); fgets(temp, 256, stdin);
    printf("���֤��"); scanf("%18s", idcard  ); fgets(temp, 256, stdin);

    libdb_query_user(name, sex, idcard, 0, &total, -1, NULL, NULL);
    pagenum = (total + PAGE_SIZE - 1) / PAGE_SIZE;
    if (total == 0) {
        printf("\nδ�ҵ����ϲ�ѯ�������û���\n");
        goto done;
    }

    while (1) {
        libdb_query_user(name, sex, idcard, 0, NULL, pagecur, item, &n);
        printf("\n");
        printf("����         ����      �Ա�  ���֤             ����  ʱ��   ����\n");
        printf("-----------------------------------------------------------------------\n");
        for (i=0; i<n&&i<PAGE_SIZE; i++) {
            printf("U%010d  %-8s  %-4s  %s  %-4d  %-4d  %s\n", item[i].id, item[i].name, USER_SEX_STR[item[i].sex], item[i].idcard, item[i].maxbrwnum, item[i].maxbrwdays, item[i].password);
        }
        printf("\n");
        printf("��¼����%d      ��ҳ��%d/%d\n\n", total, pagecur+1, pagenum);
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
    printf("(���������...)\n");
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
    printf("����\r\n");
    printf("----\r\n");
    printf("��ɨ����鿨���룺"); scanf("%11s", barcode); fgets(temp, 256, stdin);
    if (barcode[0] != 'U') {
        printf("\n�Ƿ����û����룡\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    userid = atoi(&barcode[1]);
    libdb_query_user("*", -1, "*", userid, NULL, -1, userlist, &n);
    if (n == 0) {
        printf("\nû���ҵ����û��������Ϣ��\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    while (1) {
        libdb_query_borrow(userid, NULL, 0, &total, -1, NULL, NULL);
        printf("\n");
        printf("������%s    �������ޣ�%d ��    ����ʱ�ޣ�%d ��    �ѽ��飺%d ��\n", userlist[0].name, userlist[0].maxbrwnum, userlist[0].maxbrwdays, total);
        printf("---------------------------------------------------------------\n");
        if (total >= userlist[0].maxbrwnum) {
            printf("�Ѿ����� %d �����ˣ�\n", userlist[0].maxbrwnum);
            printf("(���������...)\n");
            getch();
            return;
        }

        n = 0; libdb_query_borrow(userid, stime, 0, NULL, 0, booklist, &n);
        for (i=0; i<n&&i<PAGE_SIZE; i++) {
            printf("B%010d  %-12s  %-8s  %-12s  %5.2f  %s\n", booklist[i].id, booklist[i].name, booklist[i].author, booklist[i].press, booklist[i].price, BOOK_STATUS_STR[booklist[0].status]);
        }

        printf("��ɨ���鼮���룺"); scanf("%11s", barcode); fgets(temp, 256, stdin);
        if (strcmp(barcode, "quit") == 0) break;
        if (barcode[0] != 'B') {
            printf("\n�Ƿ����鼮���룡\n");
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
        printf("\n���Ƕ���\n�㳢�������������̫Ƶ���˰ɣ�\n��Сʱ֮���ٽ�����\n\n��");
        return 0;
    }

    printf("\n");
    printf("������������룺");
    scanf("%15s", password); fgets(temp, 256, stdin);

    while (1) {
        if (strcmp(password, ADMIN_PASSWORD) == 0) {
            login = 1;
            break;
        }
        if (--retry) {
            printf("\n");
            printf("����������������루%d����", retry);
            scanf("%15s", password); fgets(temp, 256, stdin);
        } else {
            break;
        }
    }

    if (!login) {
        printf("\n");
        printf("��¼����ʧ�ܣ����Ժ����ԣ�\n\n");
        last_login_fail_tick = GetTickCount();
    }

    while (1) {
        char op[2];
        printf("\n");
        printf("��ѡ�������\n");
        printf("------------\n");
        printf("1. ɨ�����\n");
        printf("2. ��������\n");
        printf("3. ����¼��\n");
        printf("4. �鼮��Ϣ�޸�\n");
        printf("5. ɨ����û�\n");
        printf("6. �������û�\n");
        printf("7. ���û�ע��\n");
        printf("8. �û���Ϣ�޸�\n");
        printf("9. ��ʼ�����ݿ�\n");
        printf("B. ����\n");
        printf("R. ����\n");
        printf("Q. �ǳ�\n\n");
        scanf("%1s", op); fgets(temp, 256, stdin);
        if (op[0] == 'Q') {
            printf("\n���˳��������\n\n");
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


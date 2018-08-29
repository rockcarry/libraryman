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
    "δ֪",
    "�ڿ�",
    "���",
    "��",
    "��ʧ",
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
    printf("����¼��\r\n");
    printf("--------\r\n");
    printf("��  ����"); scanf("%63s" , name   ); fgets(temp, 256, stdin);
    printf("��  �ߣ�"); scanf("%63s" , author ); fgets(temp, 256, stdin);
    printf("�����磺"); scanf("%63s" , press  ); fgets(temp, 256, stdin);
    printf("��  ��"); scanf("%f"   ,&price  ); fgets(temp, 256, stdin);
    printf("ISBN  ��"); scanf("%15s" , isbn   ); fgets(temp, 256, stdin);
    printf("��  �ܣ�"); scanf("%15s" , shelf  ); fgets(temp, 256, stdin);
    printf("��  ע��"); scanf("%255s", comment); fgets(temp, 256, stdin);
    rc = libdb_add_book(name, author, press, price, isbn, shelf, comment, &bookid);
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

static void handle_query_book_by_barcode(void)
{
    BOOKITEM item[10 ] = {0};
    char  barcode[12 ] = {0};
    char  temp   [256];
    DWORD bookid = 0;
    int   rc     = 0;
    int   n      = 0;

    printf("\n");
    printf("ɨ�����\r\n");
    printf("--------\r\n");
    printf("��ɨ�����룺"); scanf("%11s", barcode); fgets(temp, 256, stdin);
    bookid = atoi(&barcode[1]);
    libdb_query_book("*", "*", "*", "*", bookid, -1, item, &n);
    if (n == 0) {
        printf("\nû���ҵ����鼮�������Ϣ��\n");
        printf("(���������...)\n");
        getch();
        return;
    }

    item[0].status += 1;
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
    printf("��������\r\n");
    printf("--------\r\n");
    printf("�������ѯ������\n");
    printf("��  ����"); scanf("%63s" , name   ); fgets(temp, 256, stdin);
    printf("��  �ߣ�"); scanf("%63s" , author ); fgets(temp, 256, stdin);
    printf("�����磺"); scanf("%63s" , press  ); fgets(temp, 256, stdin);
    printf("ISBN  ��"); scanf("%15s" , isbn   ); fgets(temp, 256, stdin);
    libdb_query_book(name, author, press, isbn, 0, 0, item, &n);

    printf("\n");
    printf("����\t\t����\t\t����\t������\t\t�۸�\t״̬\t���\n");
    printf("------------------------------------------------------------------------------\n");
    for (i=0; i<n; i++) {
        item[i].status += 1;
        if (item[i].status < 0) item[i].status = 0;
        if (item[i].status > 4) item[i].status = 4;
        printf("B%010d\t%-10s\t%s\t%-10s\t%.2f\t%s\t%s\n", item[i].id, item[i].name, item[i].author, item[i].press, item[i].price, BOOK_STATUS_STR[item[0].status], item[i].shelf);
    }
    printf("\n");
    printf("(���������...)\n");
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
        printf("5. ���û�ע��\n");
        printf("6. �û���Ϣ�޸�\n");
        printf("7. ��ʼ�����ݿ�\n");
        printf("8. �ǳ�\n\n");
        scanf("%1s", op); fgets(temp, 256, stdin);
        if (op[0] == '8') {
            printf("\n���˳��������\n\n");
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
            case '7':
                handle_init_db();
                break;
            }
        }
    }

    return 0;
}


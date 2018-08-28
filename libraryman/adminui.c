#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "adminui.h"

#define ADMIN_PASSWORD  "admin888"
#define MAX_RETRY_TIMES 3

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
        printf("8. �ǳ�\n");
        scanf("%1s", op); fgets(temp, 256, stdin);
        if (op[0] == '8') {
            printf("\n���˳��������\n\n");
            break;
        }
    }

    return 0;
}


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
        printf("5. 新用户注册\n");
        printf("6. 用户信息修改\n");
        printf("7. 初始化数据库\n");
        printf("8. 登出\n");
        scanf("%1s", op); fgets(temp, 256, stdin);
        if (op[0] == '8') {
            printf("\n已退出管理界面\n\n");
            break;
        }
    }

    return 0;
}


#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include "stdefine.h"
#include "adminui.h"

static int check_exit(char *code)
{

    return 0;
}

int main(void)
{
    char code[17];
    char temp[256];

    while (1) {
        printf("��ɨ�����룺");
        scanf("%16s", code); fgets(temp, 256, stdin);

        if (  stricmp(code, "exit") == 0
           || stricmp(code, "quit") == 0)
        {
            char yesno[2];
            printf("�Ƿ��˳�ͼ�����ϵͳ��(Y/N) ");
            scanf("%1s", yesno); fgets(temp, 256, stdin);
            if (stricmp(yesno, "Y") == 0) {
                break;
            } else {
                printf("\n");
                continue;
            }
        }

        switch (code[0]) {
        case 'U':
            break;
        case 'A':
            enter_adminui(code);
            break;
        case 'B':
            break;
        default:
            printf("��Ч������");
            break;
        }
//      printf("%s\n", code);
    }

    return 0;
}

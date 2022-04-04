#include "mm.h"
#include<time.h>
#include<pthread.h>
#include<windows.h>
#pragma comment(lib, "pthreadVC2.lib")
PM pmhead;
//extern char* simu_path;
//extern PM pmhead;//ȫ�ֱ�������������ͷ������Ϊͷ���ڲ�������
extern UM memory[PAGE_NUM];//����飬�����ڱ�ʾÿ����ʹ���˶��٣�1��PAGE_SIZE֮�����ʹ����
int sign = 0;
pthread_mutex_t mute;
UM memory[PAGE_NUM] = { 0, };

/*int min(int a, int b) {
    return a < b ? a : b;
}*/

boolea is_page_of(PM process, int at) {//�鿴�����at�Ƿ��ڳ���process��ҳ����
    PL current;
    boolea flag = FALSE;
    for (current = process->firstpage;current;current = current->nextmem)
        if (at == current->mem_at) {
            flag = TRUE;
            break;
        }
    return flag;
}

PM put_page(ID id) {//��һ���������˴�С�Ľ��̷���ҳ��������size���͵�ǰҳ�����������PM�ṹ����

    int size = rand() % PRO_MAX;
    PM process = (PM)malloc(sizeof(pm));
    memset(process, 0, sizeof(pm));
    process->size = size;
    process->PM_id = id;

    /* �㷨˼�루ǰ����������򵽵�Ҫ����Щ�����ַ�������Ǽ��裩
     * ׼����׼��һ��int���͵ı���alloc��¼������Ĵ�С
     * ��һ������0~PAGE_NUM-1������һ�����������Ϊ���������Ҫ��һ���飬�����ͬһ�����̵�֮ǰ�Ŀ���ظ������������
     * �ڶ������鿴�ո��Ǹ����Ƿ�ʹ�ù������δ��ʹ�ã�memory[randֵ] = 0��������memory[randֵ] = min(PAGE_SIZE,alloc)���������ʹ�������alloc<memory[randֵ]���򵽵��Ĳ�
     * ��������ִ��alloc -= memory[randֵ]�����alloc = 0����ɷ���,����ص���һ��
     * ���Ĳ���������п��Ƿ��ѱ�ʹ�ã�����ǣ��򱨸��ڴ治�㣬����ѡ����һ��δʹ�õĿ�ţ������randֵ�޸�Ϊ������֣��ص���2��
     */
    PL new_pl;
    int alloc = size;
    int i;
part1:
    int ran = rand() % PAGE_NUM;
    while (is_page_of(process, ran))
        ran = rand() % PAGE_NUM;

part2:
    if (!memory[ran])
        memory[ran] = min(PAGE_SIZE, alloc);
    else if (alloc < memory[ran]) goto part4;
part3:
    alloc -= memory[ran];
    new_pl = (PL)malloc(sizeof(pl));
    memset(new_pl, 0, sizeof(pl));
    new_pl->mem_at = ran;
    new_pl->PM_id = id;
    new_pl->nextmem = process->firstpage;
    process->firstpage = new_pl;
    if (!alloc)
    {
        sign = 1;
        return process;
    }
    else goto part1;
part4:
    for (i = 0;memory[i] || i < PAGE_NUM;i++);
    if (i == PAGE_NUM) {
        printf("�ڴ治��");
        sign = 1;
        return process;
    }
    else {
        ran = i;
        goto part2;
    }
}

void put_process_into_pm(PM process) {//��process���ڽ��̱���
    if (pmhead != NULL)
    {
        process->next_pro = pmhead->next_pro;
        pmhead->next_pro = process;
    }
    else
    {
        pmhead->next_pro = process;
    }

}

PM find_pro(ID id) {//���ݽ���id�ҵ�����ָ��
    PM current;
    for (current = pmhead;current->PM_id == id;current = current->next_pro);
    return current;
}

void clr_page(PM process) {//���ҳ��
    PL current, needfree;
    current = process->firstpage;
    //ɾ��process��ҳ��
    while (current) {
        memory[current->mem_at] = 0;
        needfree = current;
        current = current->nextmem;
        free(needfree);
    }
    process->firstpage = NULL;
    process->size = 0;
    sign = 1;
}

void clr_pro(PM process) {//���process��ҳ���ռ�ã��Լ�process����
    PM last, next;

    for (last = pmhead;last->next_pro != process;last = last->next_pro);
    next = process->next_pro;
    last->next_pro = next;
    clr_page(process);
    //ɾ��process����
    free(process);
    sign = 1;
}

void copy_page(PM from, PM to) {
    if (!(to->firstpage))
        clr_page(to);
    PL temp, current;//temp:��ʱָ�룬����ռ��ã�current:��ǰ����λ��
    if (from->firstpage == NULL)
        printf("���ܴӿյĽ��̸���");
    else
    {
        for (current = from->firstpage; current->nextmem; current = current->nextmem) {
            temp = (PL)malloc(sizeof(pl));
            memcpy(temp, current, sizeof(pl));
            temp->PM_id = to->PM_id;
            temp->nextmem = to->firstpage;
            to->firstpage = temp;
        }
        sign = 1;
    }
}

void* show_mm_info(void* args) {
    pthread_mutex_lock(&mute);
    //printf("%d", sign);

    while (1)
    {
        if (sign == 1)
        {
            sign = 0;
            printf("\n�������\t����ռ���ڴ�\tҳ��������\n");
            PM current_pm;
            PL current_pl;
            for (current_pm = pmhead->next_pro; current_pm != NULL; current_pm = current_pm->next_pro) {
                printf("%d\t\t%d B\t\t", current_pm->PM_id, current_pm->size);
                for (current_pl = current_pm->firstpage; current_pl; current_pl = current_pl->nextmem)
                    printf("%d ", current_pl->mem_at);
                printf("\n");
            }
            printf("�ڴ�ʹ�������\n");
            for (int i = 0; i < PAGE_NUM / 8;i++)
            {
                for (int j = 0; j < 8;j++)
                    printf("%d B\t", memory[i * 8 + j]);
                printf("\n");
            }
        }
    }

    pthread_mutex_unlock(&mute);
}

void init_mm()
{
    srand((unsigned)time(NULL));
    pmhead = (PM)malloc(sizeof(pm));
    pmhead->next_pro = NULL;
}
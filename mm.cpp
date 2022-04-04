#include "mm.h"
#include<time.h>
#include<pthread.h>
#include<windows.h>
#pragma comment(lib, "pthreadVC2.lib")
PM pmhead;
//extern char* simu_path;
//extern PM pmhead;//全局变量，进程链表头，仅作为头，内不存数据
extern UM memory[PAGE_NUM];//物理块，数组内表示每个块使用了多少，1到PAGE_SIZE之间代表使用了
int sign = 0;
pthread_mutex_t mute;
UM memory[PAGE_NUM] = { 0, };

/*int min(int a, int b) {
    return a < b ? a : b;
}*/

boolea is_page_of(PM process, int at) {//查看物理块at是否在程序process的页表中
    PL current;
    boolea flag = FALSE;
    for (current = process->firstpage;current;current = current->nextmem)
        if (at == current->mem_at) {
            flag = TRUE;
            break;
        }
    return flag;
}

PM put_page(ID id) {//给一个仅分配了大小的进程分配页表，即根据size，和当前页表情况，补充PM结构内容

    int size = rand() % PRO_MAX;
    PM process = (PM)malloc(sizeof(pm));
    memset(process, 0, sizeof(pm));
    process->size = size;
    process->PM_id = id;

    /* 算法思想（前提是这里程序到底要用哪些虚拟地址都有我们假设）
     * 准备：准备一个int类型的变量alloc记录待分配的大小
     * 第一步：在0~PAGE_NUM-1中生成一个随机数，作为这个进程需要的一个块，如果与同一个进程的之前的块号重复，则重新随机
     * 第二步：查看刚刚那个块是否被使用过，如果未被使用（memory[rand值] = 0），则令memory[rand值] = min(PAGE_SIZE,alloc)，但如果已使用且如果alloc<memory[rand值]，则到第四步
     * 第三步：执行alloc -= memory[rand值]，如果alloc = 0，完成分配,否则回到第一步
     * 第四步：检测所有块是否都已被使用，如果是，则报告内存不足，否则选出第一个未使用的块号，将这个rand值修改为这个数字，回到第2步
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
        printf("内存不足");
        sign = 1;
        return process;
    }
    else {
        ran = i;
        goto part2;
    }
}

void put_process_into_pm(PM process) {//将process置于进程表中
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

PM find_pro(ID id) {//根据进程id找到进程指针
    PM current;
    for (current = pmhead;current->PM_id == id;current = current->next_pro);
    return current;
}

void clr_page(PM process) {//清空页表
    PL current, needfree;
    current = process->firstpage;
    //删除process的页表
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

void clr_pro(PM process) {//清空process对页表的占用，以及process本身
    PM last, next;

    for (last = pmhead;last->next_pro != process;last = last->next_pro);
    next = process->next_pro;
    last->next_pro = next;
    clr_page(process);
    //删除process本身
    free(process);
    sign = 1;
}

void copy_page(PM from, PM to) {
    if (!(to->firstpage))
        clr_page(to);
    PL temp, current;//temp:临时指针，申请空间用，current:当前复制位置
    if (from->firstpage == NULL)
        printf("不能从空的进程复制");
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
            printf("\n进程序号\t进程占用内存\t页表块号数组\n");
            PM current_pm;
            PL current_pl;
            for (current_pm = pmhead->next_pro; current_pm != NULL; current_pm = current_pm->next_pro) {
                printf("%d\t\t%d B\t\t", current_pm->PM_id, current_pm->size);
                for (current_pl = current_pm->firstpage; current_pl; current_pl = current_pl->nextmem)
                    printf("%d ", current_pl->mem_at);
                printf("\n");
            }
            printf("内存使用情况：\n");
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
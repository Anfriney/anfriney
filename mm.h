#ifndef MM_H
#define MM_H

#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define PAGE_SIZE 1024
#define PAGE_NUM 32
#define PRO_MAX 2048
//PRO_MAX表示进程占的最大内存
typedef int ID;
typedef int used_mm, UM;//表示物理块所使用的内存量
typedef int boolea;
#define TRUE 1
#define FALSE 0


typedef struct page_link {//采用链表的方式来内存的占用
    int mem_at;//物理块号
    ID PM_id;
    struct page_link* nextmem;
}pl, * PL;

typedef struct pro_mem {//代表进程(内存管理部分)的结构，以链表方式存储
    unsigned long size;
    ID PM_id;
    PL firstpage;
    struct pro_mem* next_pro;
}pm, * PM;

//PM pmhead;//全局变量，进程链表头
/*
 *
 *int pages[PAGE_NUM] = { 0, };//表示页表
 */
//int min(int a, int b);

boolea is_page_of(PM process, int at);//查看物理块at是否在程序process的页表中

PM put_page(ID id);//给一个仅分配了大小的进程分配页表，即根据size，和当前页表情况，补充PM结构内容

void put_process_into_pm(PM process);//将process置于进程表中

PM find_pro(ID id);//根据进程id找到进程指针

void clr_page(PM process);//清空页表

void clr_pro(PM process);//清空process对页表的占用，以及process本身

void copy_page(PM from, PM to);//复制页表

void* show_mm_info(void* args);//打印当前内存占用信息，以及剩余内存信息

void init_mm();//初始化内存管理

#endif
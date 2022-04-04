#ifndef MM_H
#define MM_H

#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define PAGE_SIZE 1024
#define PAGE_NUM 32
#define PRO_MAX 2048
//PRO_MAX��ʾ����ռ������ڴ�
typedef int ID;
typedef int used_mm, UM;//��ʾ�������ʹ�õ��ڴ���
typedef int boolea;
#define TRUE 1
#define FALSE 0


typedef struct page_link {//��������ķ�ʽ���ڴ��ռ��
    int mem_at;//������
    ID PM_id;
    struct page_link* nextmem;
}pl, * PL;

typedef struct pro_mem {//�������(�ڴ������)�Ľṹ��������ʽ�洢
    unsigned long size;
    ID PM_id;
    PL firstpage;
    struct pro_mem* next_pro;
}pm, * PM;

//PM pmhead;//ȫ�ֱ�������������ͷ
/*
 *
 *int pages[PAGE_NUM] = { 0, };//��ʾҳ��
 */
//int min(int a, int b);

boolea is_page_of(PM process, int at);//�鿴�����at�Ƿ��ڳ���process��ҳ����

PM put_page(ID id);//��һ���������˴�С�Ľ��̷���ҳ��������size���͵�ǰҳ�����������PM�ṹ����

void put_process_into_pm(PM process);//��process���ڽ��̱���

PM find_pro(ID id);//���ݽ���id�ҵ�����ָ��

void clr_page(PM process);//���ҳ��

void clr_pro(PM process);//���process��ҳ���ռ�ã��Լ�process����

void copy_page(PM from, PM to);//����ҳ��

void* show_mm_info(void* args);//��ӡ��ǰ�ڴ�ռ����Ϣ���Լ�ʣ���ڴ���Ϣ

void init_mm();//��ʼ���ڴ����

#endif
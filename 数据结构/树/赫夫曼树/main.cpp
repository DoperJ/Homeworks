//
//  main.cpp
//  赫夫曼树
//
//  Created by 江泽群 on 04/06/2017.
//  Copyright © 2017 江泽群. All rights reserved.
//
#define INFINITY numeric_limits<int>::max();

#include <iostream>
#include <limits>
using namespace std;

class HuffNode
{
public:
    HuffNode() {parent = lchild = rchild = NULL; weight = INFINITY;}
    //为树的结点分配权值、确定父子关系
    void assign(int w, HuffNode *p = NULL,
             HuffNode *lc = NULL, HuffNode *rc = NULL)
    {
        weight = w;
        if (lc)
        {
            lc->parent = this;
            this->lchild = lc;
        }
        if (rc)
        {
            rc->parent = this;
            this->rchild = rc;
        }
    }
    //定义一组接口
    int get_weight() {return weight;}
    bool has_parent() {return bool(parent);}
    HuffNode* get_parent() {return parent;}
    HuffNode* get_lchild() {return lchild;}
private:
    HuffNode *parent, *lchild, *rchild;
    int weight;
};

class HuffTree
{
public:
    //构造赫夫曼树，将其各个及对应的权值分别用一个数组存储
    HuffTree(int n_leaf, int *weight_list)
    {
        this->n_leaf = n_leaf;
        int size = 2 * n_leaf - 1;
        ht = new HuffNode[size];
        int *w_list = new int[size];
        for (int i = 0; i < n_leaf; i++)
            w_list[i] = weight_list[i];
        for (int i = n_leaf; i < size; i++)
            w_list[i] = INFINITY;
        for (int i = 0; i < n_leaf; i++)
            ht[i].assign(w_list[i]);
        //利用循环建立赫夫曼树
        for (int i = n_leaf; i < size; i++)
        {
            int min_1 = 0, min_2 = 0;
            int min_weight_1 = INFINITY;
            int min_weight_2 = INFINITY;
            //查找权值最小的结点
            for (int j = 0; j < i; j++)
            {
                if (ht[j].has_parent())
                    continue;
                
                if (ht[j].get_weight() < min_weight_1)
                {
                    min_weight_1 = ht[j].get_weight();
                    min_1 = j;
                }
            }
            //查找权值次小的结点
            for (int j = 0; j < i; j++)
            {
                if (ht[j].has_parent())
                    continue;
                if (ht[j].get_weight() > min_weight_1
                    && ht[j].get_weight() < min_weight_2)
                {
                    min_weight_2 = ht[j].get_weight();
                    min_2 = j;
                }
            }
            ht[i].assign(w_list[min_1] + w_list[min_2], NULL,
                         &ht[min_1], &ht[min_2]);
            w_list[i] = w_list[min_1] + w_list[min_2];
        }
        //根据生成时间，输出结点的权值序列
        for (int i = 0; i < size; i++)
            cout << ht[i].get_weight() << '\t';
        cout << endl;
    }
    //获得编号为i的结点的度
    int dimension_ht(int i)
    {
        int d = 1;
        HuffNode *child = &ht[i];
        for (; child->has_parent(); child = child->get_parent())
            d++;
        return d;
    }
    //计算赫夫曼树对应的赫夫曼编码，分别用字符串指针数组存储
    char** encoding()
    {
        char **Huff_codes = new char*[n_leaf];
        for (int i=0; i<n_leaf; i++)
        {
            HuffNode *child = &ht[i];
            HuffNode *parent = NULL;
            //利用结点的度获取其对应赫夫曼编码的长度
            int cur = dimension_ht(i);
            char *h_code = new char[cur--];
            h_code[cur] = '\0';
            //从叶子结点到根向上回溯
            while (child->has_parent())
            {
                parent = child->get_parent();
                if (parent->get_lchild() == child)
                {
                    h_code[--cur] = '0';
                }
                else
                {
                    h_code[--cur] = '1';
                }
                child = child->get_parent();
            }
            Huff_codes[i] = h_code;
        }
        return Huff_codes;
    }
    
private:
    HuffNode *ht;
    int n_leaf;
};

int main(int argc, const char * argv[]) {
    int weight_list[] = {2, 4, 7, 15};
    int length = sizeof(weight_list) / sizeof(int);
    //建立赫夫曼树
    HuffTree ht = HuffTree(length, weight_list);
    char **hCodes = ht.encoding();
    cout << "输出赫夫曼编码:" << endl;
    for (int i=0; i<length; i++)
        cout << hCodes[i] << endl;
    return 0;
}

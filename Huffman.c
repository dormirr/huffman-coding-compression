/*
构造哈夫曼树，得到各字符的哈夫曼编码。
*/

#include "Custom.h"
#include "Node.c"

//找到最小和次小的两个结点
void Select(Huffman *huffman_tree, unsigned int n, int *a, int *b)
{
	unsigned int i;
	unsigned long min = ULONG_MAX;
	for (i = 0; i < n; ++i)
		if (huffman_tree[i].parent == 0 && huffman_tree[i].frequency < min)
		{
			min = huffman_tree[i].frequency;
			*a = i;
		}
	//做标记已选中
	huffman_tree[*a].parent = 1;

	min = ULONG_MAX;
	for (i = 0; i < n; ++i)
		if (huffman_tree[i].parent == 0 && huffman_tree[i].frequency < min)
		{
			min = huffman_tree[i].frequency;
			*b = i;
		}
}

//建立哈夫曼树
void CreateTree(Huffman *huffman_tree, unsigned int char_kind, unsigned int number_node)
{
	unsigned int i;
	int a, b;
	for (i = char_kind; i < number_node; ++i)
	{
		//选择最小的两个结点
		Select(huffman_tree, i, &a, &b);
		huffman_tree[a].parent = huffman_tree[b].parent = i;
		huffman_tree[i].lchild = a;	
		huffman_tree[i].rchild = b;
		huffman_tree[i].frequency = huffman_tree[a].frequency + huffman_tree[b].frequency;
	}
}

//生成哈夫曼编码
void HuffmanCode(Huffman *huffman_tree, unsigned int char_kind)
{
	unsigned int i;
	int cur, next, index;
	//暂存编码，最多256个叶子，编码长度不超过255
	char *code_temporarily = (char *)malloc(256 * sizeof(char));
	code_temporarily[256 - 1] = '\0';

	for (i = 0; i < char_kind; ++i)
	{
		//编码临时空间索引初始化
		index = 256 - 1;

		//从叶子向根反向遍历求编码
		for (cur = i, next = huffman_tree[i].parent; next != 0; cur = next, next = huffman_tree[next].parent)
		{
			if (huffman_tree[next].lchild == cur)
			{
				//左‘0’
				code_temporarily[--index] = '0';
			}
			else
			{
				//右‘1’
				code_temporarily[--index] = '1';
			}
		}
		//为第i个字符编码动态分配存储空间 
		huffman_tree[i].code = (char *)malloc((256 - index) * sizeof(char));
		//正向保存编码到树结点相应域中
		strcpy(huffman_tree[i].code, &code_temporarily[index]);
	}
	//释放编码临时空间
	free(code_temporarily);
}
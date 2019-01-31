/*
译码：对文件B进行译码，得到文件C
*/

#include "Custom.h"
#include "Node.c"

int Uncompress(char *input_file_name, char *output_file_name)
{
	unsigned int i;
	unsigned long file_length;
	//控制文件写入长度
	unsigned long file_write_length = 0;
	FILE *input_file, *output_file;
	//存储字符种类
	unsigned int char_kind;
	unsigned int number_node;
	HuffmanTree huffman_tree;
	//暂存8bits编码
	unsigned char temporary_char;
	//保存根节点索引，供匹配编码使用
	unsigned int root;

	//以二进制方式打开压缩文件
	input_file = fopen(input_file_name, "rb");

	if (input_file == NULL)
	{
		return -1;
	}

	//读取压缩文件前端的字符及对应编码，用于重建哈夫曼树
	//读取字符种类数
	fread((char *)&char_kind, sizeof(unsigned int), 1, input_file);
	if (char_kind == 1)
	{
		//读取唯一的字符
		fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
		//读取文件长度
		fread((char *)&file_length, sizeof(unsigned long), 1, input_file);
		//打开压缩后将生成的文件
		output_file = fopen(output_file_name, "wb");
		while (file_length--)
		{
			fwrite((char *)&temporary_char, sizeof(unsigned char), 1, output_file);
		}
		fclose(input_file);
		fclose(output_file);
	}
	else
	{
		//根据字符种类数，计算建立哈夫曼树所需结点数 
		number_node = 2 * char_kind - 1;
		//动态分配哈夫曼树结点空间
		huffman_tree = (Huffman *)malloc(number_node * sizeof(Huffman));
		//读取字符及对应权重，存入哈夫曼树节点
		for (i = 0; i < char_kind; ++i)
		{
			//读入字符
			fread((char *)&huffman_tree[i].uchar, sizeof(unsigned char), 1, input_file);
			//读入字符对应权重
			fread((char *)&huffman_tree[i].frequency, sizeof(unsigned long), 1, input_file);
			huffman_tree[i].parent = 0;
		}
		//初始化后number_node-char_kins个结点的parent
		for (; i < number_node; ++i)
		{
			huffman_tree[i].parent = 0;
		}

		//重建哈夫曼树（与压缩时的一致）
		CreateTree(huffman_tree, char_kind, number_node);

		//读完字符和权重信息，紧接着读取文件长度和编码，进行解码
		//读入文件长度
		fread((char *)&file_length, sizeof(unsigned long), 1, input_file);
		//打开压缩后将生成的文件
		output_file = fopen(output_file_name, "wb");
		root = number_node - 1;
		while (1)
		{
			//读取一个字符长度的编码（8位）
			fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);

			//处理读取的一个字符长度的编码
			for (i = 0; i < 8; ++i)
			{
				//由根向下直至叶节点正向匹配编码对应字符
				if (temporary_char & 128)
				{
					root = huffman_tree[root].rchild;
				}
				else
				{
					root = huffman_tree[root].lchild;
				}

				if (root < char_kind)
				{
					fwrite((char *)&huffman_tree[root].uchar, sizeof(unsigned char), 1, output_file);
					++file_write_length;
					//控制文件长度，跳出内层循环
					if (file_write_length == file_length)
					{
						break;
					}
					//复位为根索引，匹配下一个字符
					root = number_node - 1;
				}
				//将编码缓存的下一位移到最高位，供匹配
				temporary_char <<= 1;
			}
			//控制文件长度，跳出外层循环
			if (file_write_length == file_length)
			{
				break;
			}
		}

		//关闭文件
		fclose(input_file);
		fclose(output_file);

		//释放内存
		free(huffman_tree);
	}
}
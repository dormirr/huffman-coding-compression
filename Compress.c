/*
按照哈夫曼编码将文件A翻译为Huffman编码文件B。
*/

#include "Custom.h"
#include "Node.c"

// 压缩函数
int Compress(char *input_file_name, char *output_file_name)
{
	unsigned int i, j;
	//字符种类
	unsigned int char_kind;
	//暂存8bits字符
	unsigned char temporary_char;
	unsigned long file_length = 0;
	FILE *input_file, *output_file;
	CharactersFrequency temporary_node;
	unsigned int number_node;
	HuffmanTree huffman_tree;
	//待存编码缓冲区
	char code_the_buffer[256] = "\0";
	unsigned int code_length;

	/*
	** 动态分配256个结点，暂存字符频度，
	** 统计并拷贝到树结点后立即释放
	*/
	CharactersFrequency *temporary_char_frequency = (CharactersFrequency *)malloc(256 * sizeof(CharactersFrequency));

	//初始化暂存结点
	for (i = 0; i < 256; ++i)
	{
		temporary_char_frequency[i].frequency = 0;
		//数组的256个下标与256种字符对应
		temporary_char_frequency[i].uchar = (unsigned char)i;
	}

	//遍历文件，获取字符频度
	input_file = fopen(input_file_name, "rb");
	//判断输入文件是否存在
	if (input_file == NULL)
	{
		return -1;
	}

	//读入一个字符
	fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
	while (!feof(input_file))
	{
		//统计下标对应字符的权重，利用数组的随机访问快速统计字符频度
		++temporary_char_frequency[temporary_char].frequency;
		++file_length;
		//读入一个字符
		fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
	}
	fclose(input_file);

	// 排序，将频度为零的放最后，剔除
	for (i = 0; i < 256 - 1; ++i)
	{
		for (j = i + 1; j < 256; ++j)
			if (temporary_char_frequency[i].frequency < temporary_char_frequency[j].frequency)
			{
				temporary_node = temporary_char_frequency[i];
				temporary_char_frequency[i] = temporary_char_frequency[j];
				temporary_char_frequency[j] = temporary_node;
			}
	}

	// 统计实际的字符种类（出现次数不为0）
	for (i = 0; i < 256; ++i)
	{
		if (temporary_char_frequency[i].frequency == 0)
		{
			break;
		}
	}
	char_kind = i;

	if (char_kind == 1)
	{
		//打开压缩后将生成的文件
		output_file = fopen(output_file_name, "wb");
		//写入字符种类
		fwrite((char *)&char_kind, sizeof(unsigned int), 1, output_file);
		//写入唯一的字符
		fwrite((char *)&temporary_char_frequency[0].uchar, sizeof(unsigned char), 1, output_file);
		//写入字符频度，也就是文件长度
		fwrite((char *)&temporary_char_frequency[0].frequency, sizeof(unsigned long), 1, output_file);
		free(temporary_char_frequency);
		fclose(output_file);
	}
	else
	{
		//根据字符种类数，计算建立哈夫曼树所需结点数
		number_node = 2 * char_kind - 1;
		//动态建立哈夫曼树所需结点
		huffman_tree = (Huffman *)malloc(number_node * sizeof(Huffman));

		//初始化前char_kind个结点
		for (i = 0; i < char_kind; ++i)
		{
			//将暂存结点的字符和频度拷贝到树结点
			huffman_tree[i].uchar = temporary_char_frequency[i].uchar;
			huffman_tree[i].frequency = temporary_char_frequency[i].frequency;
			huffman_tree[i].parent = 0;
		}
		//释放字符频度统计的暂存区
		free(temporary_char_frequency);

		//初始化后number_node-char_kind个结点
		for (; i < number_node; ++i)
		{
			huffman_tree[i].parent = 0;
		}

		//创建哈夫曼树
		CreateTree(huffman_tree, char_kind, number_node);

		//生成哈夫曼编码
		HuffmanCode(huffman_tree, char_kind);

		//写入字符和相应权重，供解压时重建哈夫曼树
		//打开压缩后将生成的文件
		output_file = fopen(output_file_name, "wb");
		//写入字符种类
		fwrite((char *)&char_kind, sizeof(unsigned int), 1, output_file);
		for (i = 0; i < char_kind; ++i)
		{
			//写入字符（已排序，读出后顺序不变）
			fwrite((char *)&huffman_tree[i].uchar, sizeof(unsigned char), 1, output_file);
			//写入字符对应权重
			fwrite((char *)&huffman_tree[i].frequency, sizeof(unsigned long), 1, output_file);
		}

		//紧接着字符和权重信息后面写入文件长度和字符编码
		//写入文件长度
		fwrite((char *)&file_length, sizeof(unsigned long), 1, output_file);
		//以二进制形式打开待压缩的文件
		input_file = fopen(input_file_name, "rb");
		//每次读取8bits
		fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
		while (!feof(input_file))
		{
			//匹配字符对应编码
			for (i = 0; i < char_kind; ++i)
			{
				if (temporary_char == huffman_tree[i].uchar)
				{
					strcat(code_the_buffer, huffman_tree[i].code);
				}
			}
			//以8位（一个字节长度）为处理单元
			while (strlen(code_the_buffer) >= 8)
			{
				//清空字符暂存空间，改为暂存字符对应编码
				temporary_char = '\0';
				for (i = 0; i < 8; ++i)
				{
					//左移一位，为下一个bit腾出位置
					temporary_char <<= 1;
					if (code_the_buffer[i] == '1')
					{
						//当编码为"1"，通过或操作符将其添加到字节的最低位
						temporary_char |= 1;
					}
				}
				//将字节对应编码存入文件
				fwrite((char *)&temporary_char, sizeof(unsigned char), 1, output_file);
				//编码缓存去除已处理的前八位
				strcpy(code_the_buffer, code_the_buffer + 8);
			}
			//每次读取8bits
			fread((char *)&temporary_char, sizeof(unsigned char), 1, input_file);
		}
		//处理最后不足8bits编码
		code_length = strlen(code_the_buffer);
		if (code_length > 0)
		{
			temporary_char = '\0';
			for (i = 0; i < code_length; ++i)
			{
				temporary_char <<= 1;
				if (code_the_buffer[i] == '1')
				{
					temporary_char |= 1;
				}
			}
			//将编码字段从尾部移到字节的高位
			temporary_char <<= 8 - code_length;
			//存入最后一个字节
			fwrite((char *)&temporary_char, sizeof(unsigned char), 1, output_file);      
		}

		// 关闭文件
		fclose(input_file);
		fclose(output_file);

		// 释放内存
		for (i = 0; i < char_kind; ++i)
		{
			free(huffman_tree[i].code);
		}
		free(huffman_tree);
	}
}
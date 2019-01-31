#include "Custom.h"
#include "Node.c"

int main()
{
	while (1)
	{
		int n, flag = 0;
		char name[2222], names[2222], input_file_name[2222] = "D:\\张天赐\\Documents\\数据结构与算法综合设计\\用哈夫曼编码实现压缩软件\\资源文件\\", output_file_name[2222] = "D:\\张天赐\\Documents\\数据结构与算法综合设计\\用哈夫曼编码实现压缩软件\\资源文件\\";
		printf("1、压缩\n2、解压缩\n输入其他退出,请选择功能：");
		scanf("%d", &n);
		getchar();
		switch (n)
		{
		case 1: 
			printf("请输入将要进行操作的文件名：");
			gets(name);
			strcat(input_file_name, name);
			printf("请输入进行操作后输出的文件名：");
			gets(names);
			strcat(output_file_name, names); 
			printf("正在进行压缩\n");

			flag = Compress(input_file_name, output_file_name);
			break;
		case 2: 
			printf("请输入将要进行操作的文件名：");
			gets(name);
			strcat(input_file_name, name);
			printf("请输入进行操作后输出的文件名：");
			gets(names);
			strcat(output_file_name, names); 
			printf("正在进行解压缩\n");

			flag = Uncompress(input_file_name, output_file_name);
			break;
		default:
			return 0;
		}

		if (flag == -1)
		{
			printf("文件\"%s\"不存在!\n", input_file_name);
		}
		else
		{
			printf("完成!\n\n");
		}
	}
}

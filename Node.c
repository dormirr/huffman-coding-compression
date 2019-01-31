// 统计字符频度的临时结点
typedef struct {
	unsigned char uchar;			// 以8bits为单元的无符号字符
	unsigned long frequency;		// 每类（以二进制编码区分）字符出现频度
} CharactersFrequency;

// 哈夫曼树结点
typedef struct {
	unsigned char uchar;				// 以8bits为单元的无符号字符
	unsigned long frequency;			// 每类（以二进制编码区分）字符出现频度
	char *code;						// 字符对应的哈夫曼编码（动态分配存储空间）
	int parent, lchild, rchild;		// 定义双亲和左右孩子
} Huffman, *HuffmanTree;
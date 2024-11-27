# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <math.h>
#pragma warning(disable:4996)
struct SNode {
	long int freq; // 頻率
	int depth; // 深度，編碼長度
	unsigned char gray;
	struct SNode * pPar; 
	struct SNode * pLeft;
	struct SNode * pRight; 
	char * code; // 位元組編碼

};
struct SNode nodes[256]; // 各位元組編碼
struct SNode tree[520];
struct SNode* head = 0; // 編碼臨時隊列，指向第一位的指針
struct SNode* temp = 0; // 臨時指針，用於解碼時，指向當前已走到的結點
int m_iCodeNum; // 已經編碼的個數
int m_iRead;
long int imcodenum = 0;
double efficiency;
double avglen;
int totallens;
//////////////初始化函數///////////////////
void init(struct SNode*p, int n) {
	int i;
	for (i = 0; i < n; i++) {
		p->code = "";
		p->freq = 0;
		p->pLeft = NULL;
		p->pPar = NULL;
		p->pRight = NULL;
		p->depth = 0;
		p->gray = i;
		p = p + 1;
	}
}
///////////統計每個灰度值的頻數////////////////////
void freqadd(unsigned char n)
{
	nodes[n].freq++;
}
//////////////計算平均編碼長度及編碼效率///////////
void geteffavg(){
	int sumfreq=0;
	avglen = 0;
	double xinyuanshang = 0;
	for(int i = 0; i<256;i++){
		sumfreq = sumfreq + nodes[i].freq;
	}
	for(int i = 0; i < 256;i++){
		avglen += (nodes[i].depth-1) * (double)nodes[i].freq / (double)sumfreq;
			}
	efficiency = avglen/8;
	printf("編碼的平均碼長為%lf\n",avglen);
	printf("編碼效率為%lf\n",efficiency);
	
}
/////////灰度值升序創建臨時鍊表//////////////
void addtolist(struct SNode* pNode)
{
	if (head == NULL)  				// 隊列為空，		

	{
		head = pNode;// 直接加入頭部
	}
	else
	{
		if (pNode->freq < head->freq)//當新加入=head結點的頻數時，將新結點放到隊首
		{
			pNode->pPar = head;
			head = pNode;
		}
		else
		{
			struct	SNode* p = head;
			while ((p->pPar != 0) && (p->pPar->freq < pNode->freq))
			{
				p = p->pPar;
			}
			pNode->pPar = p->pPar;
			p->pPar = pNode;
		}
	}
}
//////////////////生成編碼//////////////////////////////
void createcode(struct SNode * snode) {
	struct	SNode* p = snode;
	int post = 0;
	if (snode->depth == 1) {//臨時隊列中只有一個元素
		snode->code = (char *)malloc(sizeof(char));
		snode->code[0] = '0';
	}
	else {
		snode->code = (char *)malloc(sizeof(char)*snode->depth);//為編碼結果分配儲存空間
		post = snode->depth - 1;
		snode->code[post] = '\0';
		post--;
		while (p->pPar != NULL) {
			if (p == p->pPar->pLeft) // 該結點為其父節點的左孩子
			{
				snode->code[post] = '0';
			}
			else
			{
				snode->code[post] = '1';//該結點為其父節點的右孩子
			}
			post--;
			p = p->pPar;
		}
	}
}
/////////// 增加結點兒子節點的編碼長度//////////////////////////
void AddChildLen(struct SNode* pNode)
{
	pNode->depth++;
	if (pNode->pLeft != NULL)
	{
		AddChildLen(pNode->pLeft);
	}
	if (pNode->pRight != NULL)
	{
		AddChildLen(pNode->pRight);
	}
}
/////////////////構建編碼huffman樹/////////////////
void createcodetree(struct SNode *ptree, int len) {
	int offset, i = 0;

	while (head != NULL && head->pPar != NULL)
	{
		// 取出前兩個頻數最小結點
		struct SNode *p1 = head;
		struct SNode *p2 = head->pPar;
		head = p2->pPar;
		offset = len + i;
		ptree[offset].freq = p1->freq + p2->freq; // 頻率為兩者之和
		ptree[offset].pLeft = p1;
		ptree[offset].pRight = p2;
		p1->pPar = &ptree[offset];
		p2->pPar = &ptree[offset];
		AddChildLen(&ptree[offset]);//調整樹中每個結點的深度
		addtolist(&ptree[offset]);//將新產生的結點，放到臨時隊列中
		i++;
	}
	// 構建樹完成，生成編碼
	for (i = 0; i < 256; i++)
	{
		if (nodes[i].freq > 0)
		{
			createcode(&nodes[i]);
		}
	}


}
/////////////////////////////////返回編碼後的所有碼元的長度和////////////////////////////
int getallcodelen() {
	int alllen = 0, i = 0;
	for (; i < 256; i++) {
		alllen = alllen + nodes[i].depth - 1;
	}
	return alllen;
}
////////////////////將所有的結點的編碼放到codebuf緩衝中/////////////////////////
char * getcode(int len) {
	char *codebuf;
	int i, offset = 0;
	codebuf = (char *)malloc(sizeof(char)*(len + 1));
	codebuf[len] = '\0';
	for (i = 0; i < 256; i++) {
		
			strcpy(codebuf + offset, nodes[i].code);
			offset += (nodes[i].depth - 1);
		
	}

	return codebuf;
}
////////////////////////////編碼//////////////////////////////////
void huffmancode() {
	struct SNode *p;//中間指針
	unsigned char b[512];//做緩衝用
	char * buf, *test;
	int n = 0, len = 0, j = 0;
	int i = 0;
	int codelens = 0;//所有編碼的總長度。
	int count = 0;
	FILE * fpin, *fpout;
	init(nodes, 256);//將每一個結點初始化
	init(tree, 511);
	fpin = fopen("lena.bmp", "rb");
	fseek(fpin, 1078, 0);//跳過前54位元組
	len = fread(b, sizeof(char), 512, fpin);//循環讀出圖像的數據部分
	while (len > 0)
	{
		for (i = 0; i < len; i++)
		{
			freqadd(b[i]);//統計該灰度值的頻數
		}
		len = fread(b, sizeof(char), 512, fpin);
		count++;
	}
	printf("讀取次數為%d\n", count);
	for (i = 0, len = 0; i < 256; i++) {//將個結點加入臨時隊列，並從小到大排序
		if (nodes[i].freq > 0) {
			addtolist(&nodes[i]);
			nodes[i].depth++;
			len++;
			tree[i] = nodes[i];
		}

	}
	createcodetree(tree, len);//建立huffman樹，並進行編碼
	codelens = getallcodelen();//得到所有元素編碼後總的編碼長度
	buf = getcode(codelens);//將所有的編碼暫存到buf緩衝裡
	printf("每個灰度值所對應的編碼如下所示\n");
	for (n = 0; n < 256; n++) {
		printf("%d:%s\n", n, nodes[n].code);
	}
	printf("\n\n");

	fpout = fopen("huffman.txt", "wb+");
	count = 0;
	fseek(fpin, 1078, 0);
	len = fread(b, sizeof(char), 512, fpin);//循環讀出圖像的數據部分
	while (len > 0) {
		for (i = 0; i < len; i++) {
			n = b[i];
			for (j = 0; j < (nodes[n].depth - 1); j++) {

				fwrite(&nodes[n].code[j], sizeof(char), 1, fpout);
			}
		}
		len = fread(b, sizeof(char), 512, fpin);

	}
	fclose(fpin);//關閉文件指針
	fclose(fpout);

}
////////////////////解碼////////////////////////
void decode() {
	FILE * hufin, *hufout, *imgin;
	struct SNode *p;
	int i = 0, n = 0, num = 0;
	char b;
	char debuf[1968288];

	imgin = fopen("lena.bmp", "rb");
	hufout = fopen("decode.bmp", "wb+");
	hufin = fopen("huffman.txt", "rt+");
	fread(debuf, sizeof(char), 1078, imgin);//從源文件獲取文件頭
	fwrite(debuf, sizeof(char), 1078, hufout);//寫入檔案頭
	hufin = fopen("huffman.txt", "rt+");
	fgets(debuf, 1968288, hufin);  //讀出所有的編碼序列
	p = head;                
	for (i = 0; debuf[i] != '\0'; i++) {//開始解碼
		switch (debuf[i]) {
		case '0': {
			if (p->pLeft != NULL) {//編碼為0並且左孩子不為空，指向左孩子。
				p = p->pLeft;
				break;
			}
			else {              //左孩子為空，為葉子結點，取出其灰度值
				b = p->gray;
				fwrite(&b, sizeof(char), 1, hufout);

				p = head;
				i = i - 1;//因為有i++
				break;
			}

		}
		case '1': {
			if (p->pRight != NULL) {//編碼為1並且右孩子不為空，指向右孩子
				p = p->pRight;
				break;
			}

			else {//右孩子為空，為葉子結點，取出其灰度值
				b = p->gray;
				fwrite(&b, sizeof(char), 1, hufout);
				p = head;
				i = i - 1;
				break;
			}
		}
		default: break;
		}
	}
	b = p->gray;//獲得最後一個編碼結果
	fwrite(&b, sizeof(char), 1, hufout);
}


void main() {
	char ans = 0;
	huffmancode();
	geteffavg();
	printf("輸入d開始解碼");
	scanf("%c", &ans);
	switch (ans) {
	case 'd':
		decode();
		printf("解碼完成");
		break;
	default:break;
	}


}
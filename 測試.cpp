# include <stdio.h>
# include <string.h>
# include <stdlib.h>
struct SNode {
	long int freq; // 頻率
	int depth; // 深度，編碼長度
	char gray;
	struct SNode * pPar; // 父結點 
	struct SNode * pLeft; // 左結點
	struct SNode * pRight; // 右結點
	char * code; // 位元組編碼

};
struct SNode nodes[256]; // 各位元組編碼
struct SNode tree[520];
struct SNode* head = 0; // 編碼臨時隊列，指向第一位的指針
struct SNode* temp = 0; // 臨時指針，用於解碼時，指向當前已走到的結點
int m_iCodeNum; // 已經編碼的個數
int m_iRead;
long int imcodenum = 0;
//////////////對結點數組進行初始化///////////////////
void init(struct SNode*p, int n) {
	int i;
	for (i = 0; i < n; i++) {
		p->code = NULL;
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
	nodes[n].freq++;//nodes第n個元素儲存的為灰度值為n對應的結點
}
/////////將每個灰度值根據其頻數插入大臨時鍊表中，由小到大//////////////
void addtolist(struct SNode* pNode)
{
	if (head == NULL)  				// 隊列為空，		

	{
		head = pNode;// 直接加入頭部
	}
	else
	{
		if (pNode->freq < head->freq)//當新加入的結點的頻數小於head結點的頻數時，將新結點放到隊首
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
/////////// 增加結點兒子編碼長度//////////////////////////
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
	unsigned char b[1024];//做緩衝用
	char * buf, *test;
	int n = 0, len = 0, j = 0;
	int i = 0;
	int codelens = 0;//所有編碼的總長度。
	FILE * fpin, *fpout;
	init(nodes, 256);//將每一個結點初始化
	init(tree, 511);
	//for(i=0;i<256;i++){
		//printf("gray is %d\n",nodes[i].gray);
	//}
	if ((fpin = fopen("lena.bmp", "rb")) == NULL) {
		printf("file open error\n");
	}//打開圖像文件
	fseek(fpin, 54, 0);//前54位元組的文件為文件頭和圖像資訊需要跳過
	len = fread(b, sizeof(char), 1024, fpin);//循環讀出圖像的數據部分
	while (len > 0)
	{

		for (i = 0; i < len; i++)
		{
			freqadd(b[i]);//統計該灰度值的頻數
		}
		len = fread(b, sizeof(char), 1024, fpin);
	}
	///根據灰度值的頻數從小到大進行排序///////
	for (i = 0, len = 0; i < 256; i++) {//將個結點加入臨時隊列，並從小到大排序
		if (nodes[i].freq > 0) {
			addtolist(&nodes[i]);
			nodes[i].depth++;
			// printf("%d\n",nodes[i].freq);
			len++;
			tree[i] = nodes[i];
		}

	}
	createcodetree(tree, len);//建立huffman樹，並進行編碼

	codelens = getallcodelen();//得到所有元素編碼後總的編碼長度
	//printf("%d\n",codelens);
	buf = getcode(codelens);//將所有的編碼暫存到buf緩衝裡
	printf("-----------------------------每個灰度值所對應的編碼---------------------------\n");
	for (n = 0; n < 256; n++) {
		printf("%d:%s\n", n, nodes[n].code);
	}
	printf("\n\n");
	printf("圖像編碼正在進行中.....\n\n");

	if ((fpout = fopen("huffman.txt", "wb+")) == NULL) {
		printf("file open error");
		exit(1);
	}
	fseek(fpin, 54, 0);
	len = fread(b, sizeof(char), 1024, fpin);//循環讀出圖像的數據部分
	while (len > 0) {
		for (i = 0; i < len; i++) {
			n = b[i];
			for (j = 0; j < (nodes[n].depth - 1); j++) {

				fwrite(&nodes[n].code[j], sizeof(char), 1, fpout);
				//imcodenum++;
			}
		}
		len = fread(b, sizeof(char), 1024, fpin);
	}
	//printf("imcodenum=%ld\n",imcodenum);
	printf("圖像編碼完成\n\n");
	fclose(fpin);//關閉文件指針
	fclose(fpout);

}
////////////////////解碼////////////////////////
void decode() {
	FILE * hufin, *hufout, *imgin;
	struct SNode *p;
	int i = 0, n = 0, num = 0;
	char b;
	char debuf[507132];
	//debuf=(char *)malloc(sizeof(char)*(len+1));//分配所需的記憶體
	//debuf[len]='\0'; 
	if ((imgin = fopen("lena.bmp", "rb")) == NULL) {
		printf("file open error\n");
		exit(1);
	}

	if ((hufout = fopen("decode.bmp", "wb+")) == NULL) {//打開儲存解碼結果的文件
		printf("file open error");
		exit(1);
	}

	if ((hufin = fopen("huffman.txt", "rt+")) == NULL) {//打開儲存編碼結果的文件
		printf("huffman.txt open error\n");
		exit(1);
	}
	fread(debuf, sizeof(char), 54, imgin);//讀出原始圖像的文件頭，圖像資訊放到b中，因為文件頭沒有進行編碼所以無需解碼直接拷貝
	fwrite(debuf, sizeof(char), 54, hufout);//將文件頭，圖像資訊放到解碼後的文件中
	//n=len+1;
	fgets(debuf, 507132, hufin);  //從文件中讀出所有的編碼序列，只讀n-1個第，並自動使b【n】為‘\0’
	p = head;                //每次解碼都要從樹的根節點開始
	for (i = 0; debuf[i] != '\0'; i++) {//開始解碼
		switch (debuf[i]) {
		case '0': {
			if (p->pLeft != NULL) {//如果編碼為0並且左孩子不為空，那麼p指向p的左孩子。
				p = p->pLeft;
				break;
			}
			else {              //如果左孩子為空，則此結點為葉子結點，取出其灰度值作為解碼結果
				b = p->gray;
				fwrite(&b, sizeof(char), 1, hufout);

				p = head;
				i = i - 1;//此時i指向下一個碼元的開始位置，但是for循環還有一個i++所以此時需要將i減一
				break;
			}

		}
		case '1': {
			if (p->pRight != NULL) {//如果編碼為1並且右孩子孩子不為空，那麼p指向p的右孩子。
				p = p->pRight;
				break;
			}

			else {//如果右孩子孩子為空，則此結點為葉子結點，取出其灰度值作為解碼結果
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
	b = p->gray;//上面的switch語句未能統計左後一個解碼結果，所以在此統計一下
	fwrite(&b, sizeof(char), 1, hufout);
}


int main() {
	char ans = 0;
	huffmancode();
	printf("需要解碼嗎？y(解碼)/n(不解碼)\n");
	scanf("%c", &ans);
	switch (ans) {
	case 'y':printf("解碼正在進行中......\n\n");
		decode();//解碼
		printf("解碼已完成，請到源文件所在目錄查看結果\n\n");
		break;
	default:break;
	}
	return 0;

}
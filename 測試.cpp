# include <stdio.h>
# include <string.h>
# include <stdlib.h>
struct SNode {
	long int freq; // �W�v
	int depth; // �`�סA�s�X����
	char gray;
	struct SNode * pPar; // �����I 
	struct SNode * pLeft; // �����I
	struct SNode * pRight; // �k���I
	char * code; // �줸�սs�X

};
struct SNode nodes[256]; // �U�줸�սs�X
struct SNode tree[520];
struct SNode* head = 0; // �s�X�{�ɶ��C�A���V�Ĥ@�쪺���w
struct SNode* temp = 0; // �{�ɫ��w�A�Ω�ѽX�ɡA���V��e�w���쪺���I
int m_iCodeNum; // �w�g�s�X���Ӽ�
int m_iRead;
long int imcodenum = 0;
//////////////�ﵲ�I�Ʋնi���l��///////////////////
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
///////////�έp�C�Ӧǫ׭Ȫ��W��////////////////////
void freqadd(unsigned char n)
{
	nodes[n].freq++;//nodes��n�Ӥ����x�s�����ǫ׭Ȭ�n���������I
}
/////////�N�C�Ӧǫ׭Ȯھڨ��W�ƴ��J�j�{������A�Ѥp��j//////////////
void addtolist(struct SNode* pNode)
{
	if (head == NULL)  				// ���C���šA		

	{
		head = pNode;// �����[�J�Y��
	}
	else
	{
		if (pNode->freq < head->freq)//��s�[�J�����I���W�Ƥp��head���I���W�ƮɡA�N�s���I��춤��
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
//////////////////�ͦ��s�X//////////////////////////////
void createcode(struct SNode * snode) {
	struct	SNode* p = snode;
	int post = 0;
	if (snode->depth == 1) {//�{�ɶ��C���u���@�Ӥ���
		snode->code = (char *)malloc(sizeof(char));
		snode->code[0] = '0';
	}
	else {
		snode->code = (char *)malloc(sizeof(char)*snode->depth);//���s�X���G���t�x�s�Ŷ�
		post = snode->depth - 1;
		snode->code[post] = '\0';
		post--;
		while (p->pPar != NULL) {
			if (p == p->pPar->pLeft) // �ӵ��I������`�I�����Ĥl
			{
				snode->code[post] = '0';
			}
			else
			{
				snode->code[post] = '1';//�ӵ��I������`�I���k�Ĥl
			}
			post--;
			p = p->pPar;
		}
	}
}
/////////// �W�[���I��l�s�X����//////////////////////////
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
/////////////////�c�ؽs�Xhuffman��/////////////////
void createcodetree(struct SNode *ptree, int len) {
	int offset, i = 0;

	while (head != NULL && head->pPar != NULL)
	{
		// ���X�e����W�Ƴ̤p���I
		struct SNode *p1 = head;
		struct SNode *p2 = head->pPar;
		head = p2->pPar;


		offset = len + i;
		ptree[offset].freq = p1->freq + p2->freq; // �W�v����̤��M
		ptree[offset].pLeft = p1;
		ptree[offset].pRight = p2;
		p1->pPar = &ptree[offset];
		p2->pPar = &ptree[offset];
		AddChildLen(&ptree[offset]);//�վ�𤤨C�ӵ��I���`��
		addtolist(&ptree[offset]);//�N�s���ͪ����I�A����{�ɶ��C��
		i++;
	}
	// �c�ؾ𧹦��A�ͦ��s�X
	for (i = 0; i < 256; i++)
	{
		if (nodes[i].freq > 0)
		{
			createcode(&nodes[i]);
		}
	}


}
/////////////////////////////////��^�s�X�᪺�Ҧ��X�������שM////////////////////////////
int getallcodelen() {
	int alllen = 0, i = 0;
	for (; i < 256; i++) {
		alllen = alllen + nodes[i].depth - 1;
	}
	return alllen;
}
////////////////////�N�Ҧ������I���s�X���codebuf�w�Ĥ�/////////////////////////
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
////////////////////////////�s�X//////////////////////////////////
void huffmancode() {
	struct SNode *p;//�������w
	unsigned char b[1024];//���w�ĥ�
	char * buf, *test;
	int n = 0, len = 0, j = 0;
	int i = 0;
	int codelens = 0;//�Ҧ��s�X���`���סC
	FILE * fpin, *fpout;
	init(nodes, 256);//�N�C�@�ӵ��I��l��
	init(tree, 511);
	//for(i=0;i<256;i++){
		//printf("gray is %d\n",nodes[i].gray);
	//}
	if ((fpin = fopen("lena.bmp", "rb")) == NULL) {
		printf("file open error\n");
	}//���}�Ϲ����
	fseek(fpin, 54, 0);//�e54�줸�ժ���󬰤���Y�M�Ϲ���T�ݭn���L
	len = fread(b, sizeof(char), 1024, fpin);//�`��Ū�X�Ϲ����ƾڳ���
	while (len > 0)
	{

		for (i = 0; i < len; i++)
		{
			freqadd(b[i]);//�έp�Ӧǫ׭Ȫ��W��
		}
		len = fread(b, sizeof(char), 1024, fpin);
	}
	///�ھڦǫ׭Ȫ��W�Ʊq�p��j�i��Ƨ�///////
	for (i = 0, len = 0; i < 256; i++) {//�N�ӵ��I�[�J�{�ɶ��C�A�ñq�p��j�Ƨ�
		if (nodes[i].freq > 0) {
			addtolist(&nodes[i]);
			nodes[i].depth++;
			// printf("%d\n",nodes[i].freq);
			len++;
			tree[i] = nodes[i];
		}

	}
	createcodetree(tree, len);//�إ�huffman��A�öi��s�X

	codelens = getallcodelen();//�o��Ҧ������s�X���`���s�X����
	//printf("%d\n",codelens);
	buf = getcode(codelens);//�N�Ҧ����s�X�Ȧs��buf�w�ĸ�
	printf("-----------------------------�C�Ӧǫ׭ȩҹ������s�X---------------------------\n");
	for (n = 0; n < 256; n++) {
		printf("%d:%s\n", n, nodes[n].code);
	}
	printf("\n\n");
	printf("�Ϲ��s�X���b�i�椤.....\n\n");

	if ((fpout = fopen("huffman.txt", "wb+")) == NULL) {
		printf("file open error");
		exit(1);
	}
	fseek(fpin, 54, 0);
	len = fread(b, sizeof(char), 1024, fpin);//�`��Ū�X�Ϲ����ƾڳ���
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
	printf("�Ϲ��s�X����\n\n");
	fclose(fpin);//���������w
	fclose(fpout);

}
////////////////////�ѽX////////////////////////
void decode() {
	FILE * hufin, *hufout, *imgin;
	struct SNode *p;
	int i = 0, n = 0, num = 0;
	char b;
	char debuf[507132];
	//debuf=(char *)malloc(sizeof(char)*(len+1));//���t�һݪ��O����
	//debuf[len]='\0'; 
	if ((imgin = fopen("lena.bmp", "rb")) == NULL) {
		printf("file open error\n");
		exit(1);
	}

	if ((hufout = fopen("decode.bmp", "wb+")) == NULL) {//���}�x�s�ѽX���G�����
		printf("file open error");
		exit(1);
	}

	if ((hufin = fopen("huffman.txt", "rt+")) == NULL) {//���}�x�s�s�X���G�����
		printf("huffman.txt open error\n");
		exit(1);
	}
	fread(debuf, sizeof(char), 54, imgin);//Ū�X��l�Ϲ�������Y�A�Ϲ���T���b���A�]������Y�S���i��s�X�ҥH�L�ݸѽX��������
	fwrite(debuf, sizeof(char), 54, hufout);//�N����Y�A�Ϲ���T���ѽX�᪺���
	//n=len+1;
	fgets(debuf, 507132, hufin);  //�q���Ū�X�Ҧ����s�X�ǦC�A�uŪn-1�ӲġA�æ۰ʨ�b�in�j����\0��
	p = head;                //�C���ѽX���n�q�𪺮ڸ`�I�}�l
	for (i = 0; debuf[i] != '\0'; i++) {//�}�l�ѽX
		switch (debuf[i]) {
		case '0': {
			if (p->pLeft != NULL) {//�p�G�s�X��0�åB���Ĥl�����šA����p���Vp�����Ĥl�C
				p = p->pLeft;
				break;
			}
			else {              //�p�G���Ĥl���šA�h�����I�����l���I�A���X��ǫ׭ȧ@���ѽX���G
				b = p->gray;
				fwrite(&b, sizeof(char), 1, hufout);

				p = head;
				i = i - 1;//����i���V�U�@�ӽX�����}�l��m�A���Ofor�`���٦��@��i++�ҥH���ɻݭn�Ni��@
				break;
			}

		}
		case '1': {
			if (p->pRight != NULL) {//�p�G�s�X��1�åB�k�Ĥl�Ĥl�����šA����p���Vp���k�Ĥl�C
				p = p->pRight;
				break;
			}

			else {//�p�G�k�Ĥl�Ĥl���šA�h�����I�����l���I�A���X��ǫ׭ȧ@���ѽX���G
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
	b = p->gray;//�W����switch�y�y����έp����@�ӸѽX���G�A�ҥH�b���έp�@�U
	fwrite(&b, sizeof(char), 1, hufout);
}


int main() {
	char ans = 0;
	huffmancode();
	printf("�ݭn�ѽX�ܡHy(�ѽX)/n(���ѽX)\n");
	scanf("%c", &ans);
	switch (ans) {
	case 'y':printf("�ѽX���b�i�椤......\n\n");
		decode();//�ѽX
		printf("�ѽX�w�����A�Ш췽���Ҧb�ؿ��d�ݵ��G\n\n");
		break;
	default:break;
	}
	return 0;

}
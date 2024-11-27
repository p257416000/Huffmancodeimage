#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SNode {
    long int freq;          // 頻率
    int depth;              // 深度，編碼長度
    char gray;              // 灰度值
    struct SNode* pPar;     // 父節點 
    struct SNode* pLeft;    // 左子節點
    struct SNode* pRight;   // 右子節點
    char* code;             // Huffman 編碼
};

struct SNode nodes[256];    // 每個灰度值的節點
struct SNode tree[512];     // Huffman 樹的節點
struct SNode* head = NULL;  // 臨時隊列的頭指針
int totalNodes;             // 樹中節點的總數

// 初始化節點數組
void init(struct SNode* p, int n) {
    for (int i = 0; i < n; i++) {
        p[i].freq = 0;
        p[i].depth = 0;
        p[i].gray = i;
        p[i].pPar = NULL;
        p[i].pLeft = NULL;
        p[i].pRight = NULL;
        p[i].code = NULL;
    }
}

// 統計灰度值的頻數
void freqadd(unsigned char n) {
    nodes[n].freq++;
}

// 添加節點到臨時隊列中（按頻率升序）
void addToList(struct SNode* pNode) {
    if (head == NULL) {
        head = pNode;
    }
    else if (pNode->freq < head->freq) {
        pNode->pPar = head;
        head = pNode;
    }
    else {
        struct SNode* p = head;
        while (p->pPar && p->pPar->freq <= pNode->freq) {
            p = p->pPar;
        }
        pNode->pPar = p->pPar;
        p->pPar = pNode;
    }
}

// 構建 Huffman 編碼樹
void createCodeTree() {
    int idx = 256;
    while (head && head->pPar) {
        struct SNode* p1 = head;
        struct SNode* p2 = head->pPar;
        head = head->pPar->pPar;

        tree[idx].freq = p1->freq + p2->freq;
        tree[idx].pLeft = p1;
        tree[idx].pRight = p2;
        p1->pPar = &tree[idx];
        p2->pPar = &tree[idx];

        addToList(&tree[idx]);
        idx++;
    }
    totalNodes = idx;
}

// 生成編碼（遞迴處理）
void generateCode(struct SNode* node, const char* code, int depth) {
    if (!node) return;

    node->depth = depth;
    if (!node->pLeft && !node->pRight) {
        node->code = strdup(code);
    }
    else {
        char leftCode[depth + 2];
        char rightCode[depth + 2];
        snprintf(leftCode, sizeof(leftCode), "%s0", code);
        snprintf(rightCode, sizeof(rightCode), "%s1", code);
        generateCode(node->pLeft, leftCode, depth + 1);
        generateCode(node->pRight, rightCode, depth + 1);
    }
}
// Huffman 編碼
void huffmanEncode(const char* inputFile, const char* outputFile) {
    FILE* fpin = fopen(inputFile, "rb");
    if (!fpin) {
        perror("Input file open error");
        return;
    }
    FILE* fpout = fopen(outputFile, "wb");
    if (!fpout) {
        perror("Output file open error");
        fclose(fpin);
        return;
    }

    unsigned char buffer[1024];
    init(nodes, 256);
    init(tree, 512);

    // 統計頻率
    fread(buffer, 54, 1, fpin);  // 跳過文件頭
    size_t len;
    while ((len = fread(buffer, 1, sizeof(buffer), fpin)) > 0) {
        for (size_t i = 0; i < len; i++) {
            freqadd(buffer[i]);
        }
    }

    // 建構 Huffman 樹
    for (int i = 0; i < 256; i++) {
        if (nodes[i].freq > 0) {
            addToList(&nodes[i]);
			
        }
    }
    createCodeTree();
    generateCode(&tree[totalNodes - 1], "", 0);

    // 寫入編碼文件
    rewind(fpin);
    fread(buffer, 54, 1, fpin);  // 寫入文件頭
    fwrite(buffer, 54, 1, fpout);

    while ((len = fread(buffer, 1, sizeof(buffer), fpin)) > 0) {
        for (size_t i = 0; i < len; i++) {
			
            struct SNode* node = &nodes[buffer[i]];
            fwrite(node->code, 1, strlen(node->code), fpout);
			printf("%d\n",node->code);
        }
    }

    fclose(fpin);
    fclose(fpout);
}

// Huffman 解碼
void huffmanDecode(const char* encodedFile, const char* decodedFile) {
    FILE* fpin = fopen(encodedFile, "rb");
    if (!fpin) {
        perror("Encoded file open error");
        return;
    }
    FILE* fpout = fopen(decodedFile, "wb");
    if (!fpout) {
        perror("Decoded file open error");
        fclose(fpin);
        return;
    }

    unsigned char buffer[1024];
    fread(buffer, 54, 1, fpin);  // 讀入文件頭
    fwrite(buffer, 54, 1, fpout);

    struct SNode* node = &tree[totalNodes - 1];
    int bit;
    while ((bit = fgetc(fpin)) != EOF) {
        node = (bit == '0') ? node->pLeft : node->pRight;

        if (!node->pLeft && !node->pRight) {
            fputc(node->gray, fpout);
            node = &tree[totalNodes - 1];
        }
    }

    fclose(fpin);
    fclose(fpout);
}

int main() {
    char choice;
    printf("開始進行 Huffman 編碼...\n");
    huffmanEncode("lena.bmp", "huffman_encoded.txt");

    printf("是否進行解碼？(y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
        printf("解碼進行中...\n");
        huffmanDecode("huffman_encoded.txt", "decoded.bmp");
        printf("解碼完成，請檢查 decoded.bmp 文件。\n");
    }
    else {
        printf("操作完成。\n");
    }

    return 0;
}

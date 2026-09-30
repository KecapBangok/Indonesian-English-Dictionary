#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_KATA 600
#define MAX_LEN 255
#define HASH_SIZE 101
#define MAX_OP_STACK 500

// — STRUCTS —
typedef struct {
    char kode[5];
    char indo[50];
    char eng[50];
    char sinonim_indo[50];
    char sinonim_eng[50];
    char definisi[MAX_LEN];
} Kamus;

typedef enum { OP_ADD, OP_DEL } OpType;

typedef struct {
    OpType type;
    Kamus data;
    int idx_before;
} Operation;

typedef struct BSTNode {
    char huruf;
    int idxKata[MAX_KATA];
    int jumlah;
    struct BSTNode *left, *right;
} BSTNode;

typedef struct AVLNode {
    int idx;
    char kata[50];
    int height;
    struct AVLNode *left, *right;
} AVLNode;

typedef struct HashNode {
    int idx;
    struct HashNode *next;
} HashNode;

typedef struct CDLLNode {
    char huruf;
    BSTNode *bstNode;
    struct CDLLNode *next, *prev;
} CDLLNode;

typedef struct QueueNode {
    Kamus data;
    struct QueueNode *next;
} QueueNode;

typedef struct { 
    QueueNode *front, *rear; 
} Queue;

// -- GLOBALS --
Kamus kamusList[MAX_KATA];
int jumlahKata = 0;
HashNode *hashTable[HASH_SIZE] = {0};
BSTNode *bstRoot = NULL;
AVLNode *avlRoot = NULL;
CDLLNode *cdllHead = NULL;
Queue tambahQueue, hapusQueue;

Operation undoStack[MAX_OP_STACK];
Operation redoStack[MAX_OP_STACK];
int undoTop = 0, redoTop = 0;

// -- PROTOTYPES --
void bacaFile(const char *filename);
void tulisFile(const char *filename);
void tulisInOrder(BSTNode *node, FILE *fp);
unsigned int hashFunc(const char *str);
void hashInsert(int idx);
int hashSearch(const char *kata);
void hashDeleteIdx(int idx);

BSTNode* newBSTNode(char huruf);
BSTNode* bstInsert(BSTNode *root, char huruf);
BSTNode* bstSearch(BSTNode *root, char huruf);
void urutkanNode(BSTNode *n);
BSTNode* buatBSTDariArray();
void urutkanKataDalamNode(BSTNode *node);
void inorderCDLL(BSTNode *r, CDLLNode **head, CDLLNode **prev);
void buatCDLL(BSTNode *root);

AVLNode* avlInsert(AVLNode *node, int idx);
AVLNode* avlSearch(AVLNode *node, const char *kata);

void initQueue(Queue *q);
int isEmptyQueue(Queue *q);
void enqueue(Queue *q, Kamus data);
Kamus dequeue(Queue *q);

void processTambah();
void freeAndRebuild();
void processHapus();
void freeBST();
void freeAVL();
void freeCDLL();
void doUndo();
void doRedo();
void tampilMenu();
void displayAll();
void searchMenu();
void addMenu();
void deleteMenu();
void undoMenu();
void redoMenu();

// --- IMPLEMENTASI ---
int main() {
    bacaFile("kamus.txt");
    for (int i = 0; i < jumlahKata; i++) hashInsert(i);
    bstRoot = buatBSTDariArray();
    buatCDLL(bstRoot);
    for (int i = 0; i < jumlahKata; i++) avlRoot = avlInsert(avlRoot, i);
    initQueue(&tambahQueue); 
    initQueue(&hapusQueue);

    int pi;
    do {
        tampilMenu();
        scanf("%d", &pi); 
        getchar();
        switch (pi) {
            case 1: displayAll(); break;
            case 2: searchMenu(); break;
            case 3: addMenu(); break;
            case 4: deleteMenu(); break;
            case 5: printf("Keluar.\n"); break;
            default: printf("Pilihan tidak valid.\n");
        }
    } while (pi != 5);
    printf("Terima Kasih telah menggunakan kamus kami!!");
    printf("Selamat tinggal!!");
    return 0;
}

// --- FILE IO ---
void bacaFile(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) { 
        perror("Error membuka file");
        // Jangan exit() agar bisa buat file baru nanti
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp) && jumlahKata < MAX_KATA) {
        line[strcspn(line, "\n")] = '\0';
        
        // Handle empty lines
        if (strlen(line) == 0) continue;

        // Safe tokenization with validation
        char *tokens[6] = {0};
        char *tok = strtok(line, "#");
        for (int i = 0; i < 6 && tok != NULL; i++) {
            tokens[i] = tok;
            tok = strtok(NULL, "#");
        }

        // Skip invalid lines
        if (!tokens[0] || !tokens[1] || !tokens[2] || 
            !tokens[3] || !tokens[4] || !tokens[5]) {
            printf("Warning: Baris invalid: %s\n", line);
            continue;
        }

        // Safe copy with boundary checking
        #define SAFE_COPY(dest, src, size) do { \
            strncpy(dest, src, size-1); \
            dest[size-1] = '\0'; \
        } while(0)

        SAFE_COPY(kamusList[jumlahKata].kode, tokens[0], sizeof(kamusList[jumlahKata].kode));
        SAFE_COPY(kamusList[jumlahKata].indo, tokens[1], sizeof(kamusList[jumlahKata].indo));
        SAFE_COPY(kamusList[jumlahKata].eng, tokens[2], sizeof(kamusList[jumlahKata].eng));
        SAFE_COPY(kamusList[jumlahKata].sinonim_indo, tokens[3], sizeof(kamusList[jumlahKata].sinonim_indo));
        SAFE_COPY(kamusList[jumlahKata].sinonim_eng, tokens[4], sizeof(kamusList[jumlahKata].sinonim_eng));
        SAFE_COPY(kamusList[jumlahKata].definisi, tokens[5], sizeof(kamusList[jumlahKata].definisi));

        jumlahKata++;
    }
    fclose(fp);
}

void tulisFile(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) { 
        perror("Error tulis file"); 
        return; 
    }

    // Tulis data secara in-order traversal
    tulisInOrder(bstRoot, fp);
    
    fclose(fp);
}

// Fungsi rekursif terpisah untuk traversal in-order
void tulisInOrder(BSTNode *node, FILE *fp) {
    if (!node) return;
    
    // Traverse left subtree
    tulisInOrder(node->left, fp);
    
    // Tulis semua kata dalam node ini
    for (int i = 0; i < node->jumlah; i++) {
        Kamus *k = &kamusList[node->idxKata[i]];
        fprintf(fp, "%s#%s#%s#%s#%s#%s\n",
            k->kode, k->indo, k->eng,
            k->sinonim_indo, k->sinonim_eng,
            k->definisi);
    }
    
    // Traverse right subtree
    tulisInOrder(node->right, fp);
}

// -- HASH TABLE --
unsigned int hashFunc(const char *str) {
    unsigned int h = 0;
    while (*str) h = h * 31 + tolower(*str++);
    return h % HASH_SIZE;
}

void hashInsert(int idx) {
    unsigned int h = hashFunc(kamusList[idx].indo);
    HashNode *cur = hashTable[h];
    while (cur) {
        if (cur->idx == idx) return; // Sudah ada, tidak perlu insert lagi
        cur = cur->next;
    }
    // Tambahkan node baru
    HashNode *node = malloc(sizeof(HashNode));
    node->idx = idx;
    node->next = hashTable[h];
    hashTable[h] = node;
}

int hashSearch(const char *kata) {
    unsigned int h = hashFunc(kata);
    HashNode *cur = hashTable[h];
    while (cur) {
        if (strcasecmp(kata, kamusList[cur->idx].indo) == 0) return cur->idx;
        cur = cur->next;
    }
    return -1;
}

// -- BST & CDLL --
BSTNode* newBSTNode(char huruf) {
    BSTNode *n = malloc(sizeof(BSTNode));
    n->huruf = huruf;
    n->jumlah = 0;
    n->left = n->right = NULL;
    return n;
}

BSTNode* bstInsert(BSTNode *root, char huruf) {
    if (!root) return newBSTNode(huruf);
    if (huruf < root->huruf) root->left = bstInsert(root->left, huruf);
    else if (huruf > root->huruf) root->right = bstInsert(root->right, huruf);
    return root;
}

BSTNode* bstSearch(BSTNode *root, char huruf) {
    if (!root || root->huruf == huruf) return root;
    if (huruf < root->huruf) return bstSearch(root->left, huruf);
    return bstSearch(root->right, huruf);
}

void urutkanNode(BSTNode *n) {
    if (!n) return;
        urutkanKataDalamNode(n);
        urutkanNode(n->left);
        urutkanNode(n->right);
}

BSTNode* buatBSTDariArray() {
    BSTNode *root = NULL;
    
    // 1. Buat node BST untuk setiap huruf awal
    for (int i = 0; i < jumlahKata; i++) {
        char h = toupper(kamusList[i].indo[0]);
        // Cek apakah node sudah ada
        BSTNode *existing = bstSearch(root, h);
        if (!existing) {
            root = bstInsert(root, h);
        }
    }
    urutkanNode(root);
    return root;
}
    

// Fungsi untuk mengisi dan mengurutkan kata dalam sebuah node
void urutkanKataDalamNode(BSTNode *node) {
    if (!node) return;
    
    // Kumpulkan semua indeks untuk huruf ini
    int count = 0;
    for (int i = 0; i < jumlahKata; i++) {
        if (toupper(kamusList[i].indo[0]) == node->huruf) {
            node->idxKata[count++] = i;
        }
    }
    node->jumlah = count;
    
    // Urutkan indeks berdasarkan kata Indonesia
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            char *kata1 = kamusList[node->idxKata[i]].indo;
            char *kata2 = kamusList[node->idxKata[j]].indo;
            if (strcasecmp(kata1, kata2) > 0) {
                int temp = node->idxKata[i];
                node->idxKata[i] = node->idxKata[j];
                node->idxKata[j] = temp;
            }
        }
    }
    
    // BERIKUTNYA: BUAT KODE ANGKA BERDASARKAN URUTAN
    for (int i = 0; i < count; i++) {
        int idx = node->idxKata[i];
        // Format kode: huruf + 3 digit angka (dimulai dari 1)
        snprintf(kamusList[idx].kode, sizeof(kamusList[idx].kode), 
                 "%c%03d", node->huruf, i+1);
    }
}

void inorderCDLL(BSTNode *r, CDLLNode **head, CDLLNode **prev) {
    if (!r) return;
    
    inorderCDLL(r->left, head, prev);
    
    CDLLNode *c = malloc(sizeof(CDLLNode));
    if (!c) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    
    c->huruf = r->huruf;
    c->bstNode = r;
    
    // ... sisanya sama ...
    
    inorderCDLL(r->right, head, prev);
}

void buatCDLL(BSTNode *root) {
    if (!root) {
        cdllHead = NULL;
        return;
    }
    
    cdllHead = NULL;
    CDLLNode *prev = NULL;
    inorderCDLL(root, &cdllHead, &prev);
}

// --- AVL ---
int max(int a, int b) { return (a > b) ? a : b; }
int heightAVL(AVLNode *n) { return n ? n->height : 0; }

AVLNode* newAVLNode(int idx) {
    AVLNode *n = malloc(sizeof(AVLNode));
    n->idx = idx;
    strcpy(n->kata, kamusList[idx].indo);
    n->height = 1;
    n->left = n->right = NULL;
    return n;
}

AVLNode* rightRotate(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(heightAVL(y->left), heightAVL(y->right)) + 1;
    x->height = max(heightAVL(x->left), heightAVL(x->right)) + 1;
    return x;
}

AVLNode* leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(heightAVL(x->left), heightAVL(x->right)) + 1;
    y->height = max(heightAVL(y->left), heightAVL(y->right)) + 1;
    return y;
}

AVLNode* avlInsert(AVLNode *node, int idx) {
    if (!node) return newAVLNode(idx);
    int cmp = strcasecmp(kamusList[idx].indo, node->kata);
    if (cmp < 0) node->left = avlInsert(node->left, idx);
    else if (cmp > 0) node->right = avlInsert(node->right, idx);
    else return node;

    node->height = 1 + max(heightAVL(node->left), heightAVL(node->right));
    int balance = heightAVL(node->left) - heightAVL(node->right);

    if (balance > 1 && cmp < 0) return rightRotate(node);
    if (balance < -1 && cmp > 0) return leftRotate(node);
    if (balance > 1 && cmp > 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && cmp < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

AVLNode* avlSearch(AVLNode *node, const char *kata) {
    if (!node) return NULL;
    int cmp = strcasecmp(kata, node->kata);
    if (cmp == 0) return node;
    if (cmp < 0) return avlSearch(node->left, kata);
    return avlSearch(node->right, kata);
}

// — QUEUE —
void initQueue(Queue *q) { q->front = q->rear = NULL; }
int isEmptyQueue(Queue *q) { return q->front == NULL; }

void enqueue(Queue *q, Kamus data) {
    QueueNode *n = malloc(sizeof(QueueNode));
    n->data = data;
    n->next = NULL;
    if (q->rear) q->rear->next = n;
    else q->front = n;
    q->rear = n;
}

Kamus dequeue(Queue *q) {
    QueueNode *n = q->front;
    Kamus d = n->data;
    q->front = n->next;
    if (!q->front) q->rear = NULL;
    free(n);
    return d;
}

// — OPERASI UTAMA —
void processTambah() {
    if (isEmptyQueue(&tambahQueue)) return;
    
    Kamus baru = dequeue(&tambahQueue);
    if (jumlahKata >= MAX_KATA) {
        printf("    Kapasitas penuh!\n\n");
        return;
    }

    // Simpan di akhir array (dengan kode sementara)
    kamusList[jumlahKata] = baru;
    jumlahKata++;
    
    // Perbarui struktur data
    freeAndRebuild();
    tulisFile("kamus.txt");
    printf("    Penambahan kata '%s' telah berhasil dilakukan.\n\n", baru.indo);
}

void freeAndRebuild() {
    // Hapus struktur lama dengan urutan benar
    freeCDLL(&cdllHead);
    freeBST(bstRoot);
    freeAVL(avlRoot);
    
    // Reset global pointers
    bstRoot = NULL;
    avlRoot = NULL;
    cdllHead = NULL;
    
    // Reset hash table
    memset(hashTable, 0, sizeof(hashTable));
    
    // Bangun ulang struktur
    for (int i = 0; i < jumlahKata; i++) 
        hashInsert(i);
    
    bstRoot = buatBSTDariArray();
    buatCDLL(bstRoot);
    
    for (int i = 0; i < jumlahKata; i++) 
        avlRoot = avlInsert(avlRoot, i);
}

void processHapus() {
    if (isEmptyQueue(&hapusQueue)) return;
    Kamus d = dequeue(&hapusQueue);
    int idx = hashSearch(d.indo);
    if (idx >= 0) {
        // Hapus dari array
        for (int i = idx; i < jumlahKata - 1; i++) 
            kamusList[i] = kamusList[i + 1];
        jumlahKata--;
        
        freeAndRebuild();
        tulisFile("kamus.txt");
        printf("Hapus kata '%s' berhasil.\n\n", d.indo);
    } else {
        printf("Kata '%s' tidak ditemukan.\n\n", d.indo);
    }
}

void freeBST(BSTNode *node) {
    if (!node) return;
    freeBST(node->left);
    freeBST(node->right);
    free(node);
}

void freeAVL(AVLNode *node) {
    if (!node) return;
    freeAVL(node->left);
    freeAVL(node->right);
    free(node);
}

void freeCDLL(CDLLNode **head) {
    if (*head == NULL) return;
    
    CDLLNode *current = *head;
    CDLLNode *nextNode = NULL;
    CDLLNode *start = *head;
    
    do {
        nextNode = current->next;
        free(current);
        current = nextNode;
    } while (current != start);
    
    *head = NULL;
}

// — MENU FUNCTIONS —
void tampilMenu() {
    puts("");
    printf("  __\n");
    printf(" (`/\\\n");
    printf(" `=\\/\\ __...--~~~~~-._   _.-~~~~~--...__\n");
    printf("  `=\\/\\               \\ /               \\\\\n");
    printf("   `=\\/                V                 \\\\\n");
    printf("   //_\\___--~~~~~~-._  |  _.-~~~~~~--...__\\\\\n");
    printf("  //  ) (..----~~~~._\\ | /_.~~~~----.....__\\\\\n");
    printf(" ===( INK )==========\\\\|//====================\n");
    printf("____\\___/____________`---`____________________________\n");
    printf("\n\n");
    printf("  ================== KAMUS DIGITAL ==================\n");
    printf("              1. Tampilkan semua kata\n");
    printf("              2.     Cari kata\n");
    printf("              3.    Tambah kata\n");
    printf("              4.    Hapus kata\n");
    printf("              5.      Keluar\n");
    printf("              Pilihan: ");
}

// -- FUNGSI TAMPILAN & MENU --
void displayAll() {
    puts("");
    printf("\n  ================= DAFTAR SEMUA KATA =================\n");
    for(int i = 0; i < jumlahKata; i++) {
        Kamus *k = &kamusList[i];
        printf("    [%s] %s - %s\n", k->kode, k->indo, k->eng);
        printf("    Sinonim: %s / %s\n", k->sinonim_indo, k->sinonim_eng);
        printf("    Definisi: %s\n\n", k->definisi);
    }
    printf("    Total kata: %d\n", jumlahKata);
}

void searchMenu() {
    puts("");
    printf("                ______              \n");
    printf("             .-'      `-.           \n");
    printf("           .'            `.         \n");
    printf("          /                \\        \n");
    printf("         ;                 ;`       \n");
    printf("         |                 |;       \n");
    printf("         ;                 ;|       \n");
    printf("         '\\               / ;       \n");
    printf("          \\`.           .' /        \n");
    printf("           `.`-._____.-' .'         \n");
    printf("             / /`_____.-'           \n");
    printf("            / / /                   \n");
    printf("           / / /                    \n");
    printf("          / / /                     \n");
    printf("         / / /                      \n");
    printf("        / / /                       \n");
    printf("       / / /                        \n");
    printf("       \\/_/                         \n");
    printf("\n\n");
    printf("  =================== PENCARIAN ===================\n");
    printf("          1. Berdasarkan huruf awal\n");
    printf("          2.  Pencarian spesifik\n");
    printf("          Pilih: ");
    char opt[3];
    fgets(opt, sizeof(opt), stdin);
    getchar();

    int o = opt[0] - '0';
    if(o == 1) {
        printf("    Masukkan huruf awal: ");
        char input[10];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("    Error membaca input\n");
            return;
        }

        // Hapus newline jika ada
        if (input[strlen(input)-1] == '\n') {
            input[strlen(input)-1] = '\0';
        } else {
            // Bersihkan buffer jika input terlalu panjang
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        // Validasi: harus tepat 1 karakter
        if (strlen(input) != 1) {
            printf("    Input harus satu huruf\n\n");
            return;
        }

        char c = input[0];
        if (!isalpha(c)) {
            printf("    Input harus berupa huruf\n\n");
            return;
        }
        BSTNode* node = bstSearch(bstRoot, toupper(c));
        if(!node || node->jumlah == 0) {
            printf("    Tidak ada kata dengan huruf %c\n\n", toupper(c));
        } else {
            printf("\n  Kata dengan awalan huruf %c\n\n", toupper(c));
            for(int i = 0; i < node->jumlah; i++) {
                Kamus *k = &kamusList[node->idxKata[i]];
                printf("    - [%s] %s (%s)\n", k->kode,k->indo, k->eng);
            }
        }
    } else if(o == 2) {
        printf("    Masukkan kata lengkap: ");
        char key[50];
        if (fgets(key, sizeof(key), stdin) == NULL) {
            printf("    Error membaca input\n\n");
            return;
        }
        key[strcspn(key, "\n")] = '\0';  // Hapus newline
        // Validasi: hanya huruf dan spasi
        char *p = key;
        int valid = 1;
        for(; *p; p++) {
            if (!isalpha((unsigned char)*p) && *p != ' ') {
                valid = 0;
                break;
            }
        }
        if (!valid) {
            printf("    Input hanya boleh huruf dan spasi\n\n");
            return;
        }
        AVLNode* found = avlSearch(avlRoot, key);
        if(found) {
            Kamus *k = &kamusList[found->idx];
            puts("");
            printf("\n  ================= HASIL PENCARIAN ================= \n");
            printf("    Kode : %s\n", k->kode);
            printf("    Indonesia: %s\n", k->indo);
            printf("    Inggris : %s\n", k->eng);
            printf("    Sinonim ID: %s\n", k->sinonim_indo);
            printf("    Sinonim EN: %s\n", k->sinonim_eng);
            printf("    Definisi : %s\n", k->definisi);
        } else {
            printf("Kata '%s' tidak ditemukan.\n", key);
        }
    } else {
        printf("Opsi tidak valid\n");
    }
}

void addMenu() {
    Kamus baru;
    puts("");
    printf("(\\ \n");
    printf("\\'\\ \n");
    printf(" \\'\\     __________  \n");
    printf("/ '|    ()_________) \n");
    printf(" \\ '/    \\ ~~~~~~~~ \\ \n");
    printf("   \\       \\ ~~~~~~   \\ \n");
    printf("   ==).      \\__________\\ \n");
    printf("  (__)        ()__________) \n");
    printf("\n\n");
    printf("\n  ================ TAMBAH KATA BARU ================ \n");

    printf("    Kata (Indonesia): ");
    fgets(baru.indo, sizeof(baru.indo), stdin);
    baru.indo[strcspn(baru.indo, "\n")] = '\0';

    if (!isalpha(baru.indo[0])) {
        printf("    Error: Kata harus diawali huruf!\n");
        return;
    }

    if (hashSearch(baru.indo) >= 0) {
        printf("    Kata sudah ada dalam kamus\n");
        return;
    }

    // Input data lainnya
    printf("    Terjemahan (Inggris): ");
    fgets(baru.eng, sizeof(baru.eng), stdin);
    baru.eng[strcspn(baru.eng, "\n")] = '\0';

    printf("    Sinonim Indonesia: ");
    fgets(baru.sinonim_indo, sizeof(baru.sinonim_indo), stdin);
    baru.sinonim_indo[strcspn(baru.sinonim_indo, "\n")] = '\0';

    printf("    Sinonim Inggris: ");
    fgets(baru.sinonim_eng, sizeof(baru.sinonim_eng), stdin);
    baru.sinonim_eng[strcspn(baru.sinonim_eng, "\n")] = '\0';

    printf("    Definisi: ");
    fgets(baru.definisi, sizeof(baru.definisi), stdin);
    baru.definisi[strcspn(baru.definisi, "\n")] = '\0';

    // Simpan sementara data baru
    if (jumlahKata >= MAX_KATA) {
        printf("    Kapasitas kamus penuh!\n");
        return;
    }
    
    char firstChar = toupper(baru.indo[0]);
    snprintf(baru.kode, sizeof(baru.kode), "%c", firstChar);
    enqueue(&tambahQueue, baru);
    processTambah();

    // Kumpulkan indeks untuk kelompok huruf yang sama (termasuk yang baru)
    char hurufAwal = toupper(baru.indo[0]);
    int indeksKelompok[MAX_KATA];
    int count = 0;

    for (int i = 0; i < jumlahKata; i++) {
        if (toupper(kamusList[i].indo[0]) == hurufAwal) {
            indeksKelompok[count++] = i;
        }
    }

    // Urutkan indeks kelompok berdasarkan kata Indonesia
    for (int i = 0; i < count-1; i++) {
        for (int j = i+1; j < count; j++) {
            char *kata1 = kamusList[indeksKelompok[i]].indo;
            char *kata2 = kamusList[indeksKelompok[j]].indo;
            if (strcasecmp(kata1, kata2) > 0) {
                int temp = indeksKelompok[i];
                indeksKelompok[i] = indeksKelompok[j];
                indeksKelompok[j] = temp;
            }
        }
    }

    // Update kode untuk setiap kata dalam kelompok
    for (int i = 0; i < count; i++) {
        int idx = indeksKelompok[i];
        snprintf(kamusList[idx].kode, sizeof(kamusList[idx].kode), "%c%03d", hurufAwal, i+1);
    }

    // Perbarui struktur data
    freeAndRebuild();
    tulisFile("kamus.txt");

    printf("    Kata '%s' berhasil ditambahkan dengan kode %s.\n", baru.indo, kamusList[jumlahKata-1].kode);
}

void deleteMenu() {
    puts("");
    printf("(\\ \n");
    printf("\\'\\ \n");
    printf(" \\'\\     __________  \n");
    printf("/ '|    ()_________) \n");
    printf(" \\ '/    \\ ~~~~~~~~ \\ \n");
    printf("   \\       \\ ~~~~~~   \\ \n");
    printf("   ==).      \\__________\\ \n");
    printf("  (__)        ()__________) \n");
    printf("\n\n");
    printf("\n  ================ HAPUS KATA  ================\n");
    printf("    Masukkan kata (Indonesia): ");
    char key[50];
    fgets(key, sizeof(key), stdin);
    key[strcspn(key, "\n")] = '\0';

    int idx = hashSearch(key);
    if(idx == -1) {
        printf("    Kata '%s' tidak ditemukan\n", key);
        return;
    }

    Kamus data = kamusList[idx];
    if(undoTop < MAX_OP_STACK) {
        Operation op = {OP_DEL, data, idx};
        undoStack[undoTop++] = op;
        redoTop = 0;
    }

    enqueue(&hapusQueue, data);
    processHapus();
}


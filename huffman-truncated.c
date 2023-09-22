/*
Source code is from https://www.programiz.com/dsa/huffman-coding#google_vignette
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#define TABLESIZE 256
#define MAX_TREE_HT 50
#define BUFFERSIZE 4096

uint8_t inputBuffer[BUFFERSIZE];
uint8_t outputBuffer[BUFFERSIZE];
int outputBufferPos = 0, inputBufferPos = 0, totalChars = 0, bitsToPrint, numBitsToPrint;

void storeFreq();

/**
* writes the current byte to buffer
*/
void writeToBuffer(uint8_t *buffer, int bufferPos, int bytesToWrite, int numBytes) {
  int i;
  for (i = 0; i < numBytes; i++) {
    *(buffer + bufferPos) = (bytesToWrite >> (i*8)) & 0xff;
    bufferPos++;
  }
}

void printBufferAsInt(uint8_t *buffer, int bufferSize) {
  for (int i = 0; i < bufferSize; i++) {
    printf("%d\n", buffer[i]);
  }
}

void printBufferAsChar(uint8_t *buffer, int bufferSize) {
  for (int i = 0; i < bufferSize; i++) {
    printf("%c\n", (char)buffer[i]);
  }
}

struct MinHNode {
  char item;
  unsigned freq;
  struct MinHNode *left, *right;
};

struct MinHeap {
  unsigned size;
  unsigned capacity;
  struct MinHNode **array;
};

// Create nodes
struct MinHNode *newNode(char item, unsigned freq) {
  struct MinHNode *temp = (struct MinHNode *)malloc(sizeof(struct MinHNode));

  temp->left = temp->right = NULL;
  temp->item = item;
  temp->freq = freq;

  return temp;
}

// Create min heap
struct MinHeap *createMinH(unsigned capacity) {
  struct MinHeap *minHeap = (struct MinHeap *)malloc(sizeof(struct MinHeap));

  minHeap->size = 0;

  minHeap->capacity = capacity;

  minHeap->array = (struct MinHNode **)malloc(minHeap->capacity * sizeof(struct MinHNode *));
  return minHeap;
}

// Function to swap
void swapMinHNode(struct MinHNode **a, struct MinHNode **b) {
  struct MinHNode *t = *a;
  *a = *b;
  *b = t;
}

// Heapify
void minHeapify(struct MinHeap *minHeap, int idx) {
  int smallest = idx;
  int left = 2 * idx + 1;
  int right = 2 * idx + 2;

  if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
    smallest = left;

  if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
    smallest = right;

  if (smallest != idx) {
    swapMinHNode(&minHeap->array[smallest], &minHeap->array[idx]);
    minHeapify(minHeap, smallest);
  }
}

// Check if size if 1
int checkSizeOne(struct MinHeap *minHeap) {
  return (minHeap->size == 1);
}

// Extract min
struct MinHNode *extractMin(struct MinHeap *minHeap) {
  struct MinHNode *temp = minHeap->array[0];
  minHeap->array[0] = minHeap->array[minHeap->size - 1];

  --minHeap->size;
  minHeapify(minHeap, 0);

  return temp;
}

// Insertion function
void insertMinHeap(struct MinHeap *minHeap, struct MinHNode *minHeapNode) {
  ++minHeap->size;
  int i = minHeap->size - 1;

  while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
    minHeap->array[i] = minHeap->array[(i - 1) / 2];
    i = (i - 1) / 2;
  }
  minHeap->array[i] = minHeapNode;
}

void buildMinHeap(struct MinHeap *minHeap) {
  int n = minHeap->size - 1;
  int i;

  for (i = (n - 1) / 2; i >= 0; --i)
    minHeapify(minHeap, i);
}

int isLeaf(struct MinHNode *root) {
  return !(root->left) && !(root->right);
}

struct MinHeap *createAndBuildMinHeap(char item[], int freq[], int size) {
  struct MinHeap *minHeap = createMinH(size);

  for (int i = 0; i < size; ++i)
    minHeap->array[i] = newNode(item[i], freq[i]);

  minHeap->size = size;
  buildMinHeap(minHeap);

  return minHeap;
}

struct MinHNode *buildHuffmanTree(char item[], int freq[], int size) {
  struct MinHNode *left, *right, *top;
  struct MinHeap *minHeap = createAndBuildMinHeap(item, freq, size);

  while (!checkSizeOne(minHeap)) {
    left = extractMin(minHeap);
    right = extractMin(minHeap);

    top = newNode('$', left->freq + right->freq);

    top->left = left;
    top->right = right;

    insertMinHeap(minHeap, top);
  }
  return extractMin(minHeap);
}

void printHCodes(struct MinHNode *root, int arr[], int top) {
  int i;
  if (root->left) {
    arr[top] = 0;
    printHCodes(root->left, arr, top + 1);
  }
  if (root->right) {
    arr[top] = 1;
    printHCodes(root->right, arr, top + 1);
  }
  if (isLeaf(root)) {
    writeToBuffer(outputBuffer, outputBufferPos, root->item, 1); //writes char
    writeToBuffer(outputBuffer, outputBufferPos, top, 1); //writes len of bits
    int decimal = 0;
    int base = 0;
    for (i = top - 1; i >=0 ; i--) {
      decimal += arr[i] * (1 << base);
      base++;
    }
    writeToBuffer(outputBuffer, outputBufferPos, decimal, 1); //writes the code in dec format
  }
}


/**
* Prints the encoded tree into the memory buffer
*/
void HuffmanCodes(char item[], int freq[], int size) {
  struct MinHNode *root = buildHuffmanTree(item, freq, size);

  int arr[MAX_TREE_HT], top = 0;

  printHCodes(root, arr, top);
}

/*

        Start of TableArr - Need to change to Hashtable in the future to store char literals

*/ 
struct DataItem {
   int data;   
   int key;
};

struct DataItem* tableArray[TABLESIZE]; 
struct DataItem* item;

/*
        BEGIN COMPRESSION FROM FILE
*/

struct DataItem **unsortedFrequencyList;
int freqIndex;

struct DataItem **mostFrequent;
int *freqArraySplit;
char *characters;

/**
 * Gets the key from char to add to hashtable
*/
int getKey(char c) {
    return (int) c;
}


/**
 * Stores structs of chars with their frequencies to a list
*/
void storeFreq() {
    freqIndex = 0;
    for (int i = 0; i < TABLESIZE; i++) {
        if (tableArray[i] != NULL) {
            unsortedFrequencyList[freqIndex] = tableArray[i];
            freqIndex++;
        }
    }
}

/**
 * Gets the frequencies of all chars in a string and stores the struct in the unsortedFrequencyList array
*/
void getFreqOfCharSeqInBuffer(uint8_t *buffer, int bufferSize, int seqLen) {
    for (int i = 0; i < bufferSize; i++) {
        int key = buffer[i];
        if (tableArray[key] == NULL) {
            struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
            item->data = 1;  
            item->key = key;
            tableArray[key] = item;
        } else {
             tableArray[key]->data++;
        }
    }
    storeFreq();
}

void loadStringIntoBuffer(char *string) {
  for (int i = 0; string[i] != '\0'; i++) {
    inputBuffer[inputBufferPos++] = (uint8_t)string[i];
  }
}


/**
 * Compares DataItem structs; sorts from greatest to least to get most frequent characters
*/
int compare(const void* a, const void* b){
    struct DataItem *left = *(struct DataItem **)a;
    struct DataItem *right = *(struct DataItem **)b;
    
    if (left == NULL && right == NULL) {
        return 0;
    }
    if (left != NULL && right == NULL) {
        return -1;
    }
    if (left == NULL && right != NULL) {
        return 1;
    }
    return right->data - left->data;
}

/**
 * Sorts the unsorted this, then populates the mostFrequent list with k members.
 * The (k+1)th member is assigned '\' and all other char frequencies are added to it
*/
void storeMostFrequent(int kFreqChars) {
    qsort(unsortedFrequencyList, TABLESIZE, sizeof(struct DataItem*), compare);
    struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
    item->data = 0;
    item->key = (int) ('\\');
    mostFrequent[kFreqChars - 1] = item;
    int i = 0;
    while (i < freqIndex) {
        if (i < kFreqChars - 1) {
            mostFrequent[i] = unsortedFrequencyList[i];
        } else {
            mostFrequent[kFreqChars - 1]->data += unsortedFrequencyList[i]->data;
        }
        i++;
    }
}

/**
 * Splits the mostFrequent array into two arrays, one containing chars
 * and the other containing the frequency of the chars
*/
void splitStructIntoArray(int kFreqChars) {
    for (int i = 0; i < kFreqChars; i++) {
        freqArraySplit[i] = mostFrequent[i]->data;
        characters[i] = (char)mostFrequent[i]->key;
    }
}

/**
 * Wrapper function to compress the header and encoded input to outputBuffer
*/
void compressToBuffer(int kFreqChars) {
  writeToBuffer(outputBuffer, outputBufferPos, kFreqChars, 2);
  writeToBuffer(outputBuffer, outputBufferPos, totalChars, 2);
  storeMostFrequent(kFreqChars);
  splitStructIntoArray(kFreqChars);
  HuffmanCodes(characters, freqArraySplit, kFreqChars);
}

/**
 * Initializes the arrays with the specified size of most frequent chars
*/
void init(int kFreqChars) {
  unsortedFrequencyList = malloc(TABLESIZE*sizeof(struct DataItem*));
  freqArraySplit = malloc(kFreqChars*sizeof(int));
  characters = malloc(kFreqChars*sizeof(char));
  mostFrequent = malloc(kFreqChars*sizeof(struct DataItem*));
  if (mostFrequent == NULL || characters == NULL || freqArraySplit == NULL || unsortedFrequencyList == NULL) exit(1);
}

/*
* START DECOMPRESS
*/
struct MinHNode *rebuildTreeFromEncoded(uint8_t* buffer) {
  uint16_t numUniqueChars = *(uint16_t*)buffer;
  printf("%d\n", numUniqueChars);
}

FILE *fileptr;

int main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("Evocation: <Input File> <Number of Frequent Characters>");
    exit(1);
  }

  char *fileName = argv[1];
  int argFreqChars = atoi(argv[2]) + 1;

  init(argFreqChars);
  fileptr = fopen(fileName, "r");
  if (fileptr == NULL) {
    printf("Error: File Not Found");
    exit(1);
  }
  char string[200];
  while(fgets(string, 200, fileptr)) {
    loadStringIntoBuffer(string);
  }
  fclose(fileptr);
  getFreqOfCharSeqInBuffer(inputBuffer, inputBufferPos, 1);
  compressToBuffer(argFreqChars);
  return 0;
}

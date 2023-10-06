/*
Source code is from https://www.programiz.com/dsa/huffman-coding#google_vignette
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>

#define TABLESIZE 256
#define MAX_TREE_HT 50
#define BUFFERSIZE 4096

uint8_t inputBuffer[BUFFERSIZE];
uint8_t outputBuffer[BUFFERSIZE];
uint8_t decodeOutputBuffer[BUFFERSIZE];
int outputBufferPos = 0, inputBufferPos = 0, totalChars = 0, compressedSize = 0, kFreqChars;
int bitsToPrint = 0, numBitsToPrint = 0;
struct MinHNode *treeRoot;

char *codeCharacters;
int *codeValues, *codeLength;
int codeIndex = 0;

/**
* writes the current bytes to input buffer in little endian
*/
void writeToInputBuffer(int bytesToWrite, int numBytes) {
  for (int i = 0; i < numBytes; i++) {
    *(inputBuffer + inputBufferPos) = (bytesToWrite >> (i*8)) & 0xff;
    inputBufferPos++;
  }
}

/**
* writes the current bytes to output buffer in little endian
*/
void writeToOutputBuffer(int bytesToWrite, int numBytes) {
  for (int i = numBytes - 1; i >= 0; i--) {
    *(outputBuffer + outputBufferPos) = (bytesToWrite >> (i*8)) & 0xff;
    outputBufferPos++;
  }
}

/**
 * Writes the input buffer into an output file; if fp is null,
 * defaults to a output.txt file
*/
void writeBufferIntoFile(uint8_t *buffer, int bufferSize, FILE *fp) {
  if (fp == NULL) {
    fp = fopen("output.txt", "w");
  }
  for (int i = 0; i < bufferSize; i++) {
    fputc(buffer[i], fp);
  }
  fclose(fp);
}

/**
 * Writes the input buffer into an output file; if fp is null,
 * defaults to a output.txt file
*/
void writeBinaryBufferIntoFile(uint8_t *buffer, int bufferSize, FILE *fp) {
  if (fp == NULL) {
    fp = fopen("output.txt", "w");
  }
  for (int i = 0; i < bufferSize; i++) {
    unsigned int mask=1<<((sizeof(int) << 1)-1);
    while(mask) {
      fputc((buffer[i] & mask ? 1 : 0), fp);
      mask >>= 1;
    }
    
  }
  fclose(fp);
}

void binprintf(int v)
{
    unsigned int mask = 1 << ((sizeof(int) << 1)-1);
    while(mask) {
      printf("%d", (v & mask ? 1 : 0));
      mask >>= 1;
    }
}

void printOutput() {
  printf("Compressed Output:\n\n");
  for (int i = 0; i < outputBufferPos; i++) {
    binprintf(outputBuffer[i]);
  }
  printf("\n\n");
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

/**
 * Writes the Huffman codes with their characters, length of bits, and 
 * code of character in the output buffer; also updates the code arrays
*/
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
    writeToOutputBuffer(root->item, 1); //writes char
    writeToOutputBuffer(top, 1); //writes len of bits
    int decimal = 0;
    int base = 0;
    for (i = top - 1; i >=0 ; i--) {
      decimal += arr[i] * (1 << base);
      base++;
    }
    writeToOutputBuffer(decimal, 1); //writes the code in dec format
    codeCharacters[codeIndex] = root->item;
    codeValues[codeIndex] = decimal;
    codeLength[codeIndex] = top;
    codeIndex++;
  }
}

void printTree(struct MinHNode *root, int depth) {
  if (root == NULL) return;
  printTree(root->right, depth + 1);
  for (int i = 0; i < depth; i++)
      printf("   ");
  printf("%c\n", root->item);
  printTree(root->left, depth + 1);
}

void printArray(int arr[], int n) {
  int i;
  for (i = 0; i < n; ++i)
    printf("%d", arr[i]);

  printf("\n");
}

void printCodes(struct MinHNode *root, int arr[], int top) {
  if (root->left) {
    arr[top] = 0;
    printCodes(root->left, arr, top + 1);
  }
  if (root->right) {
    arr[top] = 1;
    printCodes(root->right, arr, top + 1);
  }
  if (isLeaf(root)) {
    printf("  %c   | ", root->item);
    printArray(arr, top);
  }
}


/**
* Prints the encoded tree into the memory buffer
*/
void HuffmanCodes(char item[], int freq[], int size) {
  treeRoot = buildHuffmanTree(item, freq, size);

  int arr[MAX_TREE_HT], top = 0;

  printHCodes(treeRoot, arr, top);
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
 * Gets the key from char sequence to add to hashtable
*/
int getKey(char c) {
    return (int) c;
}

/**
 * Gets the Huffman code from the current Huffman tree as an int
*/
int getCode(char c) {
  for (int i = 0; i < codeIndex; i++) {
    if (codeCharacters[i] == c) {
      return codeValues[i];
    }
  }
  return -1;
}
int getCodeLength(char c) {
  for (int i = 0; i < codeIndex; i++) {
    if (codeCharacters[i] == c) {
      return codeLength[i];
    }
  }
  return -1;
}


/**
 * Reads n number of bytes as one integer using little endian
*/
int getBytesAsInt(uint8_t *buffer, int bufferOffset, int n) {
  int res = 0;
  for (int i = n - 1; i >= 0; i--) {
    res |= buffer[bufferOffset] << (i * 8);
    bufferOffset++;
  }
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
void storeMostFrequent() {
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
void splitStructIntoArray() {
    for (int i = 0; i < kFreqChars; i++) {
        freqArraySplit[i] = mostFrequent[i]->data;
        characters[i] = (char)mostFrequent[i]->key;
    }
}

bool inFreqArray(char c) {
  for (int i = 0; i < kFreqChars; i++) {
    if (characters[i] == c) {
      return true;
    }
  }
  return false;
}

/**
 * Encodes the values in the input buffer and prints them to the output buffer
*/
void encodeInput() {
  for (int i = 0; i < inputBufferPos; i++) {
    int bits;
    int numBits;
    if (!inFreqArray(inputBuffer[i])) {
      bits = getCode('\\') << 8;
      bits |= inputBuffer[i];
      numBits = getCodeLength('\\') + 8;
    } else { 
      bits = getCode(inputBuffer[i]);
      numBits = getCodeLength(inputBuffer[i]);
    }
    while (numBits > 0) {
      int availableBits = 8 - numBitsToPrint;
      if (numBits < availableBits) {
        bitsToPrint |= (bits << (availableBits - numBits));
        numBitsToPrint += numBits;
        numBits = 0;
      } else {
        int leftover = numBits - availableBits;
        bitsToPrint |= (bits >> leftover);
        writeToOutputBuffer(bitsToPrint, 1);
        bits &= ((1 << leftover) - 1);
        numBits -= availableBits;
        bitsToPrint = numBitsToPrint = 0;
      }
    }
  }
  if (numBitsToPrint > 0) {
    writeToOutputBuffer(bitsToPrint, 1);
  }
}

/**
 * Wrapper function to compress the header and encoded input to outputBuffer
*/
void compressToBuffer() {
  writeToOutputBuffer(kFreqChars, 2);
  writeToOutputBuffer(totalChars, 2); //total number of characters in file; useful if 
  storeMostFrequent(kFreqChars);
  splitStructIntoArray(kFreqChars);
  HuffmanCodes(characters, freqArraySplit, kFreqChars);
  encodeInput();
}

/**
 * Initializes the arrays with the specified size of most frequent chars
*/
void init() {
  unsortedFrequencyList = malloc(TABLESIZE*sizeof(struct DataItem*));
  freqArraySplit = malloc(kFreqChars*sizeof(int));
  characters = malloc(kFreqChars*sizeof(char));
  mostFrequent = malloc(kFreqChars*sizeof(struct DataItem*));
  codeCharacters = malloc(kFreqChars*sizeof(char));
  codeValues = malloc(kFreqChars*sizeof(int));
  codeLength = malloc(kFreqChars*sizeof(int));
}

/*
* START DECOMPRESS
*/

int outputIter = 0, decodePos = 0; totalC;

/**
 * Gets the nth bit in a byte
*/
int getNthBit(int byte, int n) {
  return (byte >> (n-1)) & 0b1;
}

/**
 * Uses the inputs from the canonical representation of the tree
 * to rebuild it
*/
struct MinHNode *rebuildFromCanonical(char *charac, int *codesLengths, int *codes, int kFreq) {
  struct MinHNode *root = newNode('$', 0);
  struct MinHNode *curr = root;

  for (int i = 0; i < kFreq; i++) {
    char symbol = charac[i];
    int codeLength = codesLengths[i];
    int code = codes[i];
    for (int j = codeLength; j > 0; j--) {
      int bit = getNthBit(code, j);
      if (bit == 1) {
        if (curr->right == NULL) {
          curr->right = newNode('$', 0);
        }
        curr = curr->right;
      } else {
        if (curr->left == NULL) {
          curr->left = newNode('$', 0);
        }
        curr = curr->left;
      }
    }
    curr->item = symbol;
    curr = root;
  }
  return root;
}

/**
 * Extracts necessary information from the header and rebuilds the tree
*/
void rebuildTreeFromEncoded(uint8_t* buffer) {
  int kFreq = getBytesAsInt(buffer, outputIter, 2);
  outputIter += 2;
  totalC = getBytesAsInt(buffer, outputIter, 2);
  outputIter += 2;

  char charac[kFreq];
  int codesLengths[kFreq];
  int codes[kFreq];
  int headerIndex = 0;
  for (; outputIter < 4 + (3 * kFreq);) {
    char c = getBytesAsInt(buffer, outputIter, 1);
    charac[headerIndex] = c;
    outputIter++;
    int len = getBytesAsInt(buffer, outputIter, 1);
    codesLengths[headerIndex] = len;
    outputIter++;
    int code = getBytesAsInt(buffer, outputIter, 1);
    codes[headerIndex] = code;
    outputIter++;
    headerIndex++;
  }
  treeRoot = rebuildFromCanonical(charac, codesLengths, codes, kFreq);
}


/**
 * Decodes the output buffer according to the current tree
*/
void decodeBuffer() {
  int currByte = outputBuffer[outputIter];
  int currBytePos = 8;
  struct MinHNode *curr;
  while (totalC > 0) {
    curr = treeRoot;
    while (!isLeaf(curr)) {
      int bit = getNthBit(currByte, currBytePos);
      currBytePos--;
      if (bit == 1) {
        curr = curr->right;
      } else {
        curr = curr->left;
      }
      if (currBytePos < 1) {
        outputIter++;
        currByte = outputBuffer[outputIter];
        currBytePos = 8;
      }
    }
    if (curr->item == '\\') {
      int charByte = 0;
      charByte |= (currByte << (8 - currBytePos));
      outputIter++;
      currByte = outputBuffer[outputIter];
      charByte |= currByte >> currBytePos;
      decodeOutputBuffer[decodePos++] = charByte;
    }else {
      decodeOutputBuffer[decodePos++] = curr->item;
    }
    totalC--;
  }
}



void printFileCompressionRatio() {
  int compressedSize = outputBufferPos * 8;
  printf("Compressed Size: %d bits\n", compressedSize);
  int uncompressedSize = inputBufferPos * 8;
  printf("Uncompressed Size: %d bits\n", uncompressedSize);
  printf("Compression Ratio: %.2f\n", (double)uncompressedSize / compressedSize);
}

FILE *fileptr;

int main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("Evocation: <Input File> <Number of Frequent Characters>");
    exit(1);
  }

  char *fileName = argv[1];
  kFreqChars = atoi(argv[2]) + 1;

  init();
  fileptr = fopen(fileName, "r");
  if (fileptr == NULL) {
    printf("Error: File Not Found");
    exit(1);
  }
  char string[200];
  while(fgets(string, 200, fileptr)) {
    loadStringIntoBuffer(string);
    totalChars += strlen(string);
  }
  fclose(fileptr);

  getFreqOfCharSeqInBuffer(inputBuffer, inputBufferPos, 1);
  compressToBuffer();
  FILE *fp = fopen("encoded-output.txt", "w");
  writeBinaryBufferIntoFile(outputBuffer, outputBufferPos, fp);

  int arr[16];

  printFileCompressionRatio();
  printOutput();

  rebuildTreeFromEncoded(outputBuffer);
  decodeBuffer();
  fp = fopen("decoded-output.txt", "w");
  writeBufferIntoFile(decodeOutputBuffer, decodePos, fp);

  return 0;
}

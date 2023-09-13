/*
Source code is from https://www.programiz.com/dsa/huffman-coding#google_vignette
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 64
#define MAX_TREE_HT 50


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
  if (root->left) {
    arr[top] = 0;
    printHCodes(root->left, arr, top + 1);
  }
  if (root->right) {
    arr[top] = 1;
    printHCodes(root->right, arr, top + 1);
  }
  if (isLeaf(root)) {
    printf("  %c   | ", root->item);
    printArray(arr, top);
  }
}


// Wrapper function
void HuffmanCodes(char item[], int freq[], int size) {
  struct MinHNode *root = buildHuffmanTree(item, freq, size);

  int arr[MAX_TREE_HT], top = 0;

  printHCodes(root, arr, top);
}

// Print the array
void printArray(int arr[], int n) {
  int i;
  for (i = 0; i < n; ++i)
    printf("%d", arr[i]);

  printf("\n");
}


/*

        Start of Hashtable

*/ 
struct DataItem {
   int data;   
   int key;
};

struct DataItem* hashArray[SIZE]; 
struct DataItem* dummyItem;
struct DataItem* item;

int hashCode(int key) {
   return key % SIZE;
}

int collisionCode(int key) {
   return (((key / SIZE) % (SIZE / 2)) * 2) + 1;
}

struct DataItem *search(int key) {
   //get the hash 
   int hashIndex = hashCode(key);  
   int skipSize = collisionCode(key);
   //move in array until an empty 
   while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->key == key)
         return hashArray[hashIndex]; 
			
      //go to next cell
      hashIndex += skipSize;
		
      //wrap around the table
      hashIndex %= SIZE;
   }        
	
   return NULL;        
}

void insert(int key, int data) {

    struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
    item->data = data;  
    item->key = key;

    //get the hash 
    int hashIndex = hashCode(key);
    int skipSize = collisionCode(key);
    //move in array until an empty or deleted cell
    while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1) {
      //go to next cell
      hashIndex += skipSize;
		
      //wrap around the table
      hashIndex %= SIZE;
    }
	
   hashArray[hashIndex] = item;
}

struct DataItem* delete(struct DataItem* item) {
   int key = item->key;

    //get the hash 
    int hashIndex = hashCode(key);
    int skipSize = collisionCode(key);
    //move in array until an empty
    while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->key == key) {
         struct DataItem* temp = hashArray[hashIndex]; 
			
         //assign a dummy item at deleted position
         hashArray[hashIndex] = dummyItem; 
         return temp;
      }
		
        //go to next cell
        hashIndex += skipSize;
            
        //wrap around the table
        hashIndex %= SIZE;
    }      
	
    return NULL;        
}

void display() {
   int i = 0;
	
   for(i = 0; i<SIZE; i++) {
	
      if(hashArray[i] != NULL)
         printf(" (%d,%d)",hashArray[i]->key,hashArray[i]->data);
      else
         printf(" ~~ ");
   }
	
   printf("\n");
}

/*
        BEGIN CUSTOM
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
    for (int i = 0; i < SIZE; i++) {
        if (hashArray[i] != NULL) {
            unsortedFrequencyList[freqIndex] = hashArray[i];
            freqIndex++;
        }
    }
}


/**
 * Gets the frequencies of all chars in a string and stores the struct in the unsortedFrequencyList array
*/
void getFreqOfString(char *string) {
    int length = strlen(string);

    for (int i = 0; i < length; i++) {
        int key = getKey(string[i]);
        item = search(key);
        if (item == NULL) {
             insert(key, 1);
        } else {
             item->data++;
        }
    }
    storeFreq();
}

/**
 * Compares DataItem structs
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
    qsort(unsortedFrequencyList, SIZE, sizeof(struct DataItem*), compare);
    item->data = 0;
    item->key = (int) ('\\');
    mostFrequent[kFreqChars] = item;
    int i = 0;
    while (i < freqIndex) {
        if (i < kFreqChars) {
            mostFrequent[i] = unsortedFrequencyList[i];
        } else {
            mostFrequent[kFreqChars]->data += unsortedFrequencyList[i]->data;
        }
        i++;
    }
}

/**
 * Splits the mostFrequent array into two arrays, one containing chars
 * and the other containing the frequency
*/
void splitStructIntoArray(int kFreqChars) {
    for (int i = 0; i < kFreqChars + 1; i++) {
        freqArraySplit[i] = mostFrequent[i]->data;
        characters[i] = (char)mostFrequent[i]->key;
    }
}

/**
 * Initializes the arrays with the specified size of most frequent chars
*/
void initArrays(int kFreqChars) {
  unsortedFrequencyList = malloc(SIZE*sizeof(struct DataItem*));
  freqArraySplit = malloc((kFreqChars+1)*sizeof(int));
  characters = malloc((kFreqChars+1)*sizeof(char));
  mostFrequent = malloc((kFreqChars+1)*sizeof(struct DataItem*));
}

void storeTreeWrapper() {
  //int size = sizeof(characters) / sizeof(characters[0]);
  //struct MinHNode *root = buildHuffmanTree(characters, freqArraySplit, size);
  //int arr[MAX_TREE_HT], top = 0;
}

int main(int argc, char *argv[]) {

  if(argc != 3) {
    printf("Evocation: <Input File> <Number of Frequent Characters>");
    exit(1);
  }

  int argFreqChars = atoi(argv[2]);
  initArrays(argFreqChars);
  char string[] = "lskjheuhrnmscbxmzvioqwerjksdlfhqwpeuothgpqeurgnjaklsdbg4rgohalsjkdfnc vbhewuaipsjFKVBgowyqeiufhbahqowieufdkjlasdbkajsdbfkjbqmewnfAHFOISLAKJFHpqwuerhsjkdafnlkQKEHTPOQWIEJDkjasdnlfxvmbqweiurpgfbsadjkKJASDBFLBoiyasdbglqjhrhgqebpiwasdjkflqbwpieurbfaskjldfbqpweuibfqwiekdlsafbqepriubg";
  getFreqOfString(string);
  
  qsort(unsortedFrequencyList, SIZE, sizeof(struct DataItem*), compare);
  storeMostFrequent(argFreqChars);
  splitStructIntoArray(argFreqChars);
  HuffmanCodes(characters, freqArraySplit, argFreqChars + 1);
  
  exit(0);
}

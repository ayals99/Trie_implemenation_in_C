#include <stdlib.h>
#include <stdbool.h>

#define TRIE_ALPHABET_SIZE 26
#define A_LOWERCASE_CHAR 'a'
#define Z_LOWERCASE_CHAR 'z'
#define STRING_END_CHAR '\0'

/** Struct forward declarations: */
typedef struct Node Node;

/** Structs: */
typedef struct Node{
    Node* pointerArray[TRIE_ALPHABET_SIZE];
    bool isEndNode;
} Node;

typedef struct{
    Node* root;
} Trie;

/** Trie functions forward declarations: */
Trie* trieCreate();
void trieFree(Trie* obj);
void trieInsert(Trie* obj, char* word);
void trieRemove(Trie* obj, char* word);
bool trieStartsWith(Trie* obj, char* prefix);
bool trieSearch(Trie* obj, char* word);
Node* getLastNodeOfWord(Trie* obj, char* word);

/** Node functions forward declarations: */
Node* nodeCreate();
void nodeFree(Node* nodeToFree);
Node* getNextNode(Node* currentNode ,char letter);
bool nodeIsEmpty(Node* node);

/**  Node function implementations: */
Node* nodeCreate(){
    Node* newNode = malloc(sizeof(Node));
    if (newNode != NULL){
        for(short i = 0; i < TRIE_ALPHABET_SIZE; ++i){
            (newNode->pointerArray)[i] = NULL;
        }
        newNode->isEndNode = false;
    }
    return newNode;
}

Node* getNextNode(Node* currentNode ,char letter){
    if (currentNode == NULL){
        return NULL;
    }
    Node** array = currentNode->pointerArray;
    if (array[letter - A_LOWERCASE_CHAR] != NULL){
        return array[letter - A_LOWERCASE_CHAR];
    }
    else{
        return NULL;
    }
}

// NOTE: nodeFree is recursive and will make sure all nodes under it are freed as well, to avoid memory leaks
void nodeFree(Node* nodeToFree){
    if(nodeToFree == NULL){
        return;
    }
    Node** array = nodeToFree->pointerArray;
    for(short i = 0; i < TRIE_ALPHABET_SIZE; i++){
        if(array[i] != NULL){
            nodeFree(array[i]);
        }
    }
    free(nodeToFree);
}


bool nodeIsEmpty(Node* node){
    for(short i = 0; i < TRIE_ALPHABET_SIZE; ++i){
        if ( (node->pointerArray)[i] != NULL){
            return false;
        }
    }
    return true;
}


/**  Trie function implementations: */
Trie* trieCreate() {
    Trie* newTrie = malloc(sizeof(Trie));
    if (newTrie == NULL){
        return NULL;
    }
    newTrie->root = NULL;
    Node* newNode = nodeCreate();
    if(newNode == NULL){
        trieFree(newTrie);
        return NULL;
    }
    newTrie->root = newNode;
    return newTrie;
}

bool isValidLowercaseWord(char* word){
    if(word == NULL){
        return false;
    }
    short i = 0;
    while(word[i] != STRING_END_CHAR){
        if(word[i] > Z_LOWERCASE_CHAR || word[i] < A_LOWERCASE_CHAR){
            return false;
        }
        i++;
    }
    return true;
}

void trieInsert(Trie* obj, char* word) {
    if(!isValidLowercaseWord(word)){
        return;
    }
    short index = 0;
    Node* currentNode = obj->root;
    while(word[index] != STRING_END_CHAR){
        char letter = word[index];
        short letterArrayIndex = (letter - A_LOWERCASE_CHAR);
        Node* nextNode = getNextNode(currentNode, letter);
        if (nextNode == NULL){
            Node* newNode = nodeCreate();
            if(newNode == NULL){
                return;
            }
            (currentNode->pointerArray)[letterArrayIndex] = newNode;
        }
        if (word[index + 1] == STRING_END_CHAR){ // this letter is the last letter in the string, need to check if it's the end of the word.
            (currentNode->pointerArray)[letterArrayIndex]->isEndNode = true;
        }
        currentNode = (currentNode->pointerArray)[letterArrayIndex];
        index++;
    }
}

Node* getLastNodeOfWord(Trie* obj, char* word){
    short index = 0;
    Node* lastNode = obj->root;
    while(word[index] != STRING_END_CHAR){
        char letter = word[index];
        Node* nextNode = getNextNode(lastNode, letter);
        if (nextNode == NULL){
            return NULL;
        }
        lastNode = getNextNode(lastNode, letter);
        index++;
    }
    return lastNode;
}

bool trieSearch(Trie* obj, char* word) {
    if(word == NULL || obj == NULL || obj->root == NULL){
        return false;
    }
    Node* lastLetterNode = getLastNodeOfWord(obj, word);
    if (lastLetterNode == NULL || lastLetterNode->isEndNode == false){
        return false;
    }
    return true;
}

Node* recursiveRemove(Node* tempRoot, char* wordToDelete, short depth){
    if (tempRoot == NULL){
        return NULL;
    }
    char currentLetter = wordToDelete[depth];
    if (currentLetter == STRING_END_CHAR){
        // If reached last letter in the word to delete and the node exists,
        // then the node should no longer be an End node.
        if(tempRoot->isEndNode){
            tempRoot->isEndNode = false;
        }
        if(nodeIsEmpty(tempRoot)){
            nodeFree(tempRoot);
            tempRoot = NULL;
        }
        return tempRoot;
    }

    Node* nextNode = getNextNode(tempRoot, currentLetter);
    tempRoot->pointerArray[currentLetter - A_LOWERCASE_CHAR] = recursiveRemove(nextNode, wordToDelete, depth+1);

    if(nodeIsEmpty(tempRoot) && !(tempRoot->isEndNode)){
        nodeFree(tempRoot);
        tempRoot = NULL;
    }

    return tempRoot;
}

// trieRemove also updates the root node if necessary.
void trieRemove(Trie* obj, char* word){
    if(word == NULL || obj == NULL || obj->root == NULL){
        return;
    }
    obj->root = recursiveRemove(obj->root, word, 0);
}

bool trieStartsWith(Trie* obj, char* prefix) {
    if(obj == NULL || obj->root == NULL){
        return false;
    }
    if(*prefix == STRING_END_CHAR){
        // technically speaking, this Trie always starts with the empty word
        return true;
    }
    if (getLastNodeOfWord(obj, prefix) == NULL){
        return false;
    }
    return true;
}

void trieFree(Trie* obj) {
    if(obj == NULL){
        return;
    }
    nodeFree(obj->root);
    free(obj);
}
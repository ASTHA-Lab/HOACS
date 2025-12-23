#ifndef RNC_TREE
#define RNC_TREE

#include "RNC.h"
#include <stdlib.h>

typedef struct _RNC_Node
{
    // Key - value pair
    Encoded key;
    Encoded val;
    struct _RNC_Node* lChild;
    struct _RNC_Node* rChild;
} RNC_Node;

RNC_Node*   createNode(Encoded Key, Encoded val);

// XXX RNC_Tree functions
RNC_Node*   rncTreeIn(RNC_Node* root, Encoded index, Encoded value, UINT_TYPE m1, UINT_TYPE m2);

void        rncTreeDel(RNC_Node** root);

int         rncTreeRm(RNC_Node** root, Encoded index, UINT_TYPE m1, UINT_TYPE m2);

const Encoded*    rncTreeAt(const RNC_Node* root, Encoded index, UINT_TYPE m1, UINT_TYPE m2);

RNC_Node*   rncTreeFromArr(const Encoded* arr, size_t startIndex, size_t partSize, UINT_TYPE m1, UINT_TYPE m2);

unsigned    rncTreeGetDepth(const RNC_Node* root);

void rncTreePrint(const RNC_Node* root, UINT_TYPE m1, UINT_TYPE m2, FILE* out);
void rncTreeInOrderPrint(const RNC_Node* root, UINT_TYPE m1, UINT_TYPE m2, FILE* out);
void rncTreeInOrderPrintHex(const RNC_Node* root, UINT_TYPE m1, UINT_TYPE m2, FILE* out);
void rncTreeDot(const RNC_Node* root, UINT_TYPE m1, UINT_TYPE m2, FILE* out, char* name);



#endif
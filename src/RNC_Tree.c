#include "RNC_Tree.h"

RNC_Node*   createNode(Encoded keyVal, Encoded value)
{
    RNC_Node* ret = (RNC_Node*)malloc(sizeof(RNC_Node));
    
    // ret = (RNC_Node*)malloc(sizeof(RNC_Node));

    ret->key = keyVal;
    ret->val = value;

    ret->lChild = NULL;
    ret->rChild = NULL;

    return ret;
}


// XXX RNC_Tree functions
RNC_Node*   rncTreeIn(RNC_Node* root, Encoded index, Encoded value, UINT_TYPE m1, UINT_TYPE m2)
{
    if(root == NULL)
    {
        root = createNode(index,value);
    }
    else if(EqEnc(index,root->key,m1,m2)) // Node already exists
    {
        // root->val = encode(255,m1,m2); //check for repeated inserts
        root->val = value;
    }
    else if(LessEnc(index,root->key,m1,m2))
    {
        root->lChild = rncTreeIn(root->lChild,index,value,m1,m2);
    }
    else
    {
        root->rChild = rncTreeIn(root->rChild,index,value,m1,m2);
    }
    
    return root;
}

void rncTreeDel(RNC_Node** root) 
{
    if(root == NULL)
    {
        return;
    }
    else if(*root == NULL)
    {
        return;
    }
    else
    {
        if((*root)->lChild != NULL) // FIXME Segfaults here
        {
            rncTreeDel(&((*root)->lChild));
            (*root)->lChild = NULL;
        }

        if((*root)->rChild != NULL)
        {
            rncTreeDel(&((*root)->rChild));
            (*root)->rChild = NULL;
        }
        free(*root);
        *root = NULL;
    }
}

int rncTreeRm(RNC_Node** root, Encoded index, UINT_TYPE m1, UINT_TYPE m2)
{
    if(root == NULL)
    {
        return 1;
    }
    else if((*root) != NULL)
    {
        if(EqEnc((*root)->key,index,m1,m2))
        {
            if(((*root)->lChild != NULL) && ((*root)->rChild == NULL))
            {
                RNC_Node* temp = (*root);
                (*root) = (*root)->lChild;
                free(temp);
                return 0;
            }
            else if(((*root)->lChild == NULL))
            {
                RNC_Node* temp = (*root);
                (*root) = (*root)->rChild;
                free(temp);
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else if(LessEnc(index,(*root)->key,m1,m2))
        {
            return rncTreeRm(&((*root)->lChild),index,m1,m2);
        }
        else
        {
            return rncTreeRm(&((*root)->rChild),index,m1,m2);
        }
        return 1;
    }
    else
    {
        return 1;
    }
}

const Encoded*    rncTreeAt(const RNC_Node* root, Encoded index, UINT_TYPE m1, UINT_TYPE m2)
{
    if(root != NULL)
    {
        if(EqEnc(root->key,index,m1,m2))
        {
            return &(root->val);
        }
        else if(LessEnc(index,root->key,m1,m2))
        {
            return rncTreeAt(root->lChild,index,m1,m2);
        }
        else
        {
            return rncTreeAt(root->rChild,index,m1,m2);
        }
    }
    return NULL;
}

RNC_Node*   rncTreeFromArr(const Encoded* arr, size_t startIndex, size_t partSize, UINT_TYPE m1, UINT_TYPE m2)
{
    RNC_Node* root = NULL;
    if(partSize != 0)
    {
        size_t midSize = partSize/2;
        size_t midIndex = startIndex + midSize;

        Encoded k = encode(midIndex,m1,m2);
        Encoded v = arr[midIndex];

        root = rncTreeIn(root,k,v,m1,m2);

        if(partSize > 1)
        {
            root->lChild = rncTreeFromArr(arr,startIndex, midSize,m1,m2);
            root->rChild = rncTreeFromArr(arr,midIndex+1, partSize - (midSize +1),m1,m2);
        }


    }
    return root;
} 

unsigned rncTreeGetDepth(const RNC_Node* root)
{
    if(root == NULL)
    {
        return 0;
    }
    else
    {
        unsigned d = MAX(rncTreeGetDepth(root->lChild),rncTreeGetDepth(root->rChild)) +1;
        return d;
    }
}

void rncTreePrint(const RNC_Node* root, UINT_TYPE m1, UINT_TYPE m2, FILE* out)
{
    if(root == NULL)
    {
        fputc('.',out);
    }
    else
    {
        fputc('{',out);
        rncTreePrint(root->lChild,m1,m2,out);
        fputc('|',out);

        Encoded k = root->key;
        Encoded v = root->val;

        // fprintf(out,"{%u,%u}:{%u,%u}",k.u1,k.u2,v.u1,v.u2);

        UINT_TYPE kd = decode(k,m1,m2,NULL,NULL);
        UINT_TYPE vd = decode(v,m1,m2,NULL,NULL);
        fprintf(out,"%u:%u",kd,vd);
        fputc('|',out);

        rncTreePrint(root->rChild,m1,m2,out);
        fputc('}',out);
    }
}

void rncTreeInOrderPrint(const RNC_Node* root, UINT_TYPE m1, UINT_TYPE m2, FILE* out)
{
    if(root == NULL)
    {
        return;
    }
    else
    {
        rncTreeInOrderPrint(root->lChild,m1,m2,out);

        Encoded v = root->val;
        UINT_TYPE vd = decode(v,m1,m2,NULL,NULL);
        fprintf(out,"%u",vd);
        fputc(',',out);
        rncTreeInOrderPrint(root->rChild,m1,m2,out);
    }
}

void rncTreeInOrderPrintHex(const RNC_Node* root, UINT_TYPE m1, UINT_TYPE m2, FILE* out)
{
    if(root == NULL)
    {
        return;
    }
    else
    {
        rncTreeInOrderPrintHex(root->lChild,m1,m2,out);

        Encoded v = root->val;
        UINT_TYPE vd = decode(v,m1,m2,NULL,NULL);
        fprintf(out,"%.2x",vd);
        fputc(' ',out);
        rncTreeInOrderPrintHex(root->rChild,m1,m2,out);
    }
}

int rncTreeDotRec(const RNC_Node* root, UINT_TYPE m1, UINT_TYPE m2, FILE* out)
{
    if(root != NULL)
    {
        int key = decode(root->key,m1,m2,NULL,NULL);

        if(root->lChild == NULL)
        {
            fprintf(out, "\tnode%03uL \t[label=\"NULL\"]\n",key);
            fprintf(out, "\tnode%03u  \t-> node%03uL\n",key,key);
        }
        else
        {
            int lNodeKey = rncTreeDotRec(root->lChild,m1,m2,out);
            fprintf(out, "\tnode%03u  \t-> node%03d\n",key,lNodeKey);
        }

        fprintf(out, "\tnode%03u \t[label=\"%u\"]\n",key,key);

        if(root->rChild == NULL)
        {
            fprintf(out, "\tnode%03uR \t[label=\"NULL\"]\n",key);
            fprintf(out, "\tnode%03u  \t-> \tnode%03uR\n",key,key);
        }
        else
        {
            int rNodeKey = rncTreeDotRec(root->rChild,m1,m2,out);
            fprintf(out, "\tnode%03u  \t-> node%03d\n",key,rNodeKey);
            
        }
        return key;
    }
    return -1;
}

void rncTreeDot(const RNC_Node* root, UINT_TYPE m1, UINT_TYPE m2, FILE* out, char* name)
{
    fprintf(out,"digraph %s\n{\n",name);
    rncTreeDotRec(root,m1,m2,out);
    fputs("}\n",out);
}
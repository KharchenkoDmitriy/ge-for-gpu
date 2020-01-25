#include <string.h>
struct node{
    char key[exprMAXSIZE];
    int val;
    struct node *next;
};
struct table{
    int size;
    struct node **list;
};
unsigned long hash_str(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
struct table *createTable(int size){
    struct table *t = (struct table*)malloc(sizeof(struct table));
    t->size = size;
    t->list = (struct node**)malloc(sizeof(struct node*)*size);
    int i;
    for(i=0;i<size;i++)
        t->list[i] = NULL;
    return t;
}
int hashCode(struct table *t, char* keyS){
    int key = hash_str(keyS);
    if(key<0)
        return -(key%t->size);
    return key%t->size;
}
void insert(struct table *t, char *keyS,int val){
    int pos = hashCode(t,keyS);
    struct node *list = t->list[pos];
    struct node *newNode = (struct node*)malloc(sizeof(struct node));
    struct node *temp = list;
    while(temp){
        if(!strcmp(temp->key,keyS)){
            temp->val = val;
            return;
        }
        temp = temp->next;
    }

    strcpy(newNode->key, keyS);
    newNode->val = val;
    newNode->next = list;
    t->list[pos] = newNode;
}
int lookup(struct table *t,char* key){
    int pos = hashCode(t,key);
    struct node *list = t->list[pos];
    struct node *temp = list;
    while(temp){
        if(!strcmp(temp->key,key)){
            return temp->val;
        }
        temp = temp->next;
    }
    return -1;
}


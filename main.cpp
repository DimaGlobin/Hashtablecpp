#include <iostream>
#include <cassert>
#include <cstring>
#include <stdlib.h>

#define DELFAIL -1
#define DELOK 1

class CList;
class Node;
class CHashTable;

typedef const char* Elem_t;

class CHashTable {

public:
    size_t HashSize_;
    CList** Table_;
    int NumOfNodes_;

    explicit CHashTable (size_t HashSize);
    ~CHashTable ();
    unsigned int hash_func (Elem_t elem);
    void hash_ins (Elem_t elem);
    void dump_hash_table ();
    void hash_resize ();
};

unsigned int CHashTable::hash_func(Elem_t elem) {

    unsigned int h = 0;
    while (*elem) {
        h += (unsigned char )(*elem);
        h -= (h >> 19) | (h << 13);
        elem++;
    }

    return h;
}

class Node {

public:
    Elem_t data_;

    Node* next_;
    Node* prev_;

    CList* List_;

    void node_dump ();
};

class CList{

public:
    Node* head_;
    Node* tail_;

    int num_of_elem_;

    explicit CList ();
    ~CList ();
    Node* node_ins (Node* elem, Elem_t data);
    int node_del (Node* elem);
    void CList_dump();
};

Node* CList::node_ins(Node* elem, Elem_t data) {

    Node* new_elem = new Node;

    assert(new_elem != nullptr);

    new_elem -> data_ = data;

    this -> num_of_elem_++;

    if (elem == nullptr) {
        this -> head_ = new_elem;
        if (this -> tail_ == nullptr)
            this -> tail_ = new_elem;
        new_elem -> next_ = nullptr;
        new_elem -> prev_ = nullptr;
        new_elem -> List_ = this;
        return new_elem;
    }

    new_elem -> prev_ = elem;
    new_elem -> next_ = elem -> next_;

    if (elem -> next_ != nullptr)
        elem -> next_ -> prev_ = new_elem;
    else
        this -> tail_ = new_elem;

    elem -> next_ = new_elem;

    new_elem -> List_ = elem -> List_;

    return new_elem;
}

int CList::node_del (Node* elem){

    if (elem == nullptr)
        return DELFAIL;

    if (elem -> next_ == nullptr && elem -> prev_ == nullptr) {
        delete elem;
        this -> head_ = nullptr;
        this -> tail_ = nullptr;
        this -> num_of_elem_--;
        return DELOK;
    }

    if (elem -> prev_ != nullptr)
        elem -> prev_ -> next_ = elem -> next_;
    else
        this -> head_ = elem -> next_;

    if (elem -> next_ != nullptr)
        elem -> next_ -> prev_ = elem -> prev_;
    else
        this -> tail_ = elem -> prev_;

    delete elem;

    this -> num_of_elem_--;

    return DELOK;
}

void Node::node_dump (){

    fprintf(stderr, "Node: 0x%p\n", this);
    fprintf(stderr, "Node data: %s\n", this -> data_);
    fprintf(stderr, "Node next: 0x%p\n", this -> next_);
    fprintf(stderr, "Node prev: 0x%p\n", this -> prev_);
    fprintf(stderr, "Node CList: 0x%p\n", this -> List_);
    fprintf(stderr, "------------------------\n");
}

CList::CList() :
        head_ (nullptr),
        tail_ (nullptr),
        num_of_elem_ (0)
{

}

CList::~CList() {

    assert (this != nullptr);

    Node* elem = this -> tail_;
    Node* save_prev = nullptr;

    if (elem  != nullptr) {
        while (elem -> prev_ != nullptr) {
            save_prev = elem -> prev_;
            free((void* )elem);
            elem = save_prev;
        }
    }
}

void CList::CList_dump() {

    Node* elem = nullptr;

    if (this != nullptr)
        elem = this -> head_;

    fprintf(stderr, "CList: 0x%p\n", this);

    if (this != nullptr) {
        fprintf(stderr, "CList head: 0x%p\n", this -> head_);
        fprintf(stderr, "CList tail: 0x%p\n", this -> tail_);
        fprintf(stderr, "num_of_elem: %d\n\n", this -> num_of_elem_);
    }

    int i = 0;

    while (elem != nullptr) {
        elem->Node::node_dump();
        elem = elem->next_;
    }
}

CHashTable::CHashTable(size_t Input_HashSize) :

        HashSize_ (Input_HashSize),
        NumOfNodes_ (0)

{

    this->Table_ = (CList **) calloc(Input_HashSize, sizeof(CList *));
    for (int i = 0; i < Input_HashSize; i++) {
        this -> Table_[i] = new CList;
    }

}

CHashTable::~CHashTable() {

    assert (this != nullptr);

    CList* save_list = nullptr;
    for (int i = 0; i < this -> HashSize_; i++) {

        save_list = this -> Table_[i];
        if (save_list != nullptr)
            delete (save_list);
    }

    free (this -> Table_);
}

void CHashTable::hash_resize() {

    size_t NewSize = this -> HashSize_ * 2;
    CList** NewTable = (CList**)calloc(NewSize, sizeof(CList*));
    for (int i = 0; i < NewSize; i++) {

        NewTable[i] = new CList;
    }

    CHashTable* h1 = new CHashTable(NewSize);

    h1 -> HashSize_ = NewSize;
    h1 -> Table_ = NewTable;

    for (int i = 0; i < this -> HashSize_; i++) {

        CList* list = this -> Table_[i];
        Node* elem = list -> head_;
        while (elem != nullptr) {
            h1 -> hash_ins(elem -> data_);
            elem = elem -> next_;
        }
    }

    this -> HashSize_ = h1 -> HashSize_;
    this -> Table_ = h1 -> Table_;
    this -> NumOfNodes_ = h1 -> NumOfNodes_;
}

void CHashTable::hash_ins(Elem_t elem) {

    if (this -> NumOfNodes_ >= this -> HashSize_ * 2)
        this->hash_resize();

    unsigned int hash_num = hash_func(elem);
    hash_num = hash_num % this -> HashSize_;

    const char* copystr = strdup(elem);

    this -> Table_[hash_num] ->node_ins(this -> Table_[hash_num] -> tail_, copystr);

    this -> NumOfNodes_++;

}

void CHashTable::dump_hash_table() {

    fprintf(stderr, "\nHash table: [0x%p] {\n");
    fprintf(stderr, "Hash table size: %d\n", this -> HashSize_);
    fprintf(stderr, "Number of nodes: %d\n\n", this -> NumOfNodes_);

    for (int i = 0; i < this -> HashSize_; i++) {

        fprintf(stderr, "List number: %d\n", i);
        Table_[i] -> CList_dump();
    }

    fprintf(stderr, "}\n\n");
}

int main() {

    /*CList l1;
    Node* head = l1.node_ins(nullptr, 10);
    Node* elem1 = l1.node_ins(head, 20);
    Node* elem2 = l1.node_ins(elem1, 30);
    Node* elem3 = l1.node_ins(elem2, 40);
    l1.node_del(elem2); */

    /*FILE* dms = fopen("dmszsr.txt", "w");
    fprintf(dms, "digraph {\n");
    fprintf(dms, "Node_%p [shape = record, label = \"{ Data = %d | Next = 0x%p | Prev = 0x%p }\"]\n", head, head -> data_, head-> next_, head -> prev_);
    fprintf(dms, "}");
    fclose(dms);
    system("dot -Tpng -odmszsr.png dmszsr.txt");
    system("start dmszsr.png"); */

    /*Node* tail = head -> node_ins(20);
    Node* elem1 = head -> node_ins(30);
    Node* elem2 = head -> node_ins(40);
    l1.CList_dump();*/

    CHashTable h1(5);
    h1.hash_ins("Dimas");
    h1.hash_ins("Dimas");
    h1.hash_ins("Lexa");
    h1.hash_ins("Lexa");
    h1.hash_ins("Zhenya");
    h1.hash_ins("Ded");
    h1.hash_ins("Kris");
    h1.hash_ins("Vlad");
    h1.hash_ins("1Vlad");
    h1.hash_ins("Poltorashka");
    h1.hash_ins("1Poltorashka");
    h1.dump_hash_table();

    return 0;
}

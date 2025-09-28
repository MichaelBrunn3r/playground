module;

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <algorithm>

export module gc;

export enum ObjectType {
    OBJ_INT,
    OBJ_PAIR
};

#define FLAG_MARKED 1
export struct Object {
    union {
        // OBJ_INT
        long long value; 

        // OBJ_PAIR
        struct {
            Object* fst;
            Object* snd;
        };   
    };
    Object* next;
    ObjectType type;
    unsigned char flags;

    static Object* create(ObjectType type) {
        Object* obj = (Object*) malloc(sizeof(Object));
        obj->type = type;
        obj->flags = 0;
        obj->next = NULL;
        return obj;
    }

    void mark() {
        if(this->isMarked()) return;

        this->flags |= FLAG_MARKED;

        if(this->type == ObjectType::OBJ_PAIR) {
            this->fst->mark();
            this->snd->mark();
        }
    }

    void unmark() {
        this->flags &= ~FLAG_MARKED;
    }

    bool isMarked() {
        return this->flags & FLAG_MARKED;
    }

    bool isInt() {
        return this->type == ObjectType::OBJ_INT;
    }

    bool isPair() {
        return this->type == ObjectType::OBJ_PAIR;
    }
};

#define VM_MAX_STACK_SIZE 256
#define INITIAL_GC_THRESHOLD 64
export struct VM {
    Object* stack[VM_MAX_STACK_SIZE];
    Object* first;
    int numObjs;
    int maxObjs;
    int stackSize;

    VM() {
        this->stackSize=0;
        this->first = NULL;
        this->numObjs = 0;
        this->maxObjs = INITIAL_GC_THRESHOLD;
    }

    void push(Object* obj) {
        assert(this->stackSize < VM_MAX_STACK_SIZE);
        this->stack[this->stackSize++] = obj;
    }

    Object* pop() {
        assert(this->stackSize > 0);
        return this->stack[--this->stackSize];
    }

    Object* newObject(ObjectType type) {
        if(this->numObjs >= this->maxObjs) this->gc();
        
        Object* obj = Object::create(type);
        obj->next = this->first;
        this->first = obj;
        
        this->numObjs++;
        return obj;
    }

    void pushInt(long long val) {
        Object* obj = this->newObject(ObjectType::OBJ_INT);
        obj->value = val;
        this->push(obj);
    }

    Object* pushPair() {
        Object* obj = this->newObject(ObjectType::OBJ_PAIR);
        obj->fst = this->pop();
        obj->snd = this->pop();
        this->push(obj);
        return obj;
    }

    void markAll() {
        for(int i=0; i<this->stackSize; i++) {
            this->stack[i]->mark();
        }
    }

    void sweep() {
        int freed = 0;
        Object** obj = &this->first;
        while(*obj) {
            if(!((*obj)->isMarked())) {
                freed++;
                Object* unreached = *obj;
                *obj = unreached->next;
                this->numObjs--;
                free(unreached);
            } else {
                (*obj)->unmark();
                obj = &(*obj)->next;
            }
        }

        std::cout << "GC: Freed=" << freed << std::endl;
    }

    void gc() {        
        this->markAll();
        this->sweep();

        
        int newMaxObjs = this->maxObjs * 1.2;
        newMaxObjs = std::min({newMaxObjs, 2048});
        newMaxObjs = std::max({newMaxObjs, INITIAL_GC_THRESHOLD});
        
        std::cout << "Max objects: " << this->maxObjs << " -> " << newMaxObjs << std::endl;
        this->maxObjs = newMaxObjs;
    }

    int countLiveOjbs() {
        int cnt = 0;
        Object** obj = &this->first;
        while(*obj) {
            cnt++;
            obj = &(*obj)->next;
        }
        return cnt;
    }
};

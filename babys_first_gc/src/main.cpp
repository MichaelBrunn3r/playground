#include <iostream>
#include <random>
#include <cassert>

import gc;

std::mt19937 rng(12345);

void fibHelper(VM* vm) {
    Object* prev = vm->pop();
    int prevFst = prev->fst->value;
    int prevSnd = prev->snd->value;
    
    // TODO: Segmentation fault when not using temporary variables
    vm->pushInt(prevFst + prevSnd);
    vm->pushInt(prevSnd);
    vm->pushPair();
}

long long fib(VM* vm, int n) {
    vm->pushInt(1);
    vm->pushInt(1);
    vm->pushPair();

    for(int i=2; i<n; i++) {
        fibHelper(vm);
    }

    Object* result = vm->pop();
    return result->snd->value;
}

void fibRecHelper(VM* vm, int n) {
    if(n <= 2) {
        vm->pushInt(1);
        return;
    }

    fibRecHelper(vm, n-1);
    fibRecHelper(vm, n-2);
    
    Object* prev2 = vm->pop();
    Object* prev1 = vm->pop();

    vm->pushInt(prev1->value + prev2->value);
}

long long fibRec(VM* vm, int n) {
    fibRecHelper(vm, n);
    Object* result = vm->pop();
    return result->value;
}

int main() {
    VM vm = VM();
    std::cout << fibRec(&vm, 40) << std::endl;
    // for(int i=0; i<1000; i++) {
    //     std::cout << fib(&vm, 100) << std::endl;
    // }
}
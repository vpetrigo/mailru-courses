#include <iostream>
#include <string>

class StringPointer {
 private:
    std::string *ptr;
    bool empty{false};
    
 public:
    StringPointer(std::string *p) : ptr{p} {
        if (ptr == nullptr) {
            empty = true;
            ptr = new std::string;
        }
    }
    
    ~StringPointer() {
        if (empty) {
            delete ptr;
        }
    }
    
    operator std::string*() {
        return ptr;
    }
    
    std::string& operator*() {        
        return *ptr;
    }
    std::string *operator->() {
        return ptr;
    }
};

void foo(std::string& s) {
    StringPointer sp1{&s};
    
    std::cout << sp1->length() << std::endl;
    std::cout << *sp1 << std::endl;
}

std::string *bar() {
    std::string *s = new std::string{"bar"};
    StringPointer sp{s};
    
    std::cout << sp->length() << std::endl;
    std::cout << *sp << std::endl;
    
    return s;
}

int main() {
    std::string s1 = "Hello, world!";

    StringPointer sp1(&s1);
    StringPointer sp2(NULL);

    foo(s1);
    auto s = bar();
    std::cout << s->length() << std::endl;
    std::cout << *s << std::endl;
    
    std::cout << sp1->length() << std::endl;
    std::cout << *sp1 << std::endl;
    std::cout << sp2->length() << std::endl;
    std::cout << *sp2 << std::endl;
    
    return 0;
}
#include <iostream>
#include <cstring>
#include <cassert>

using namespace std;

class SmallAllocator {
private:
    union Header {
        using Align = long;

        struct {
            Header *next;
            size_t size;
        } s;

        Align x;
    };

    static constexpr unsigned int memory_size = 1048576;
    static constexpr unsigned int memory_block = 1024;
    static constexpr auto header_size = sizeof(Header);
    // allocate block for our needs. This function allocates memory block that
    // can fit <to_alloc> Headers, because we want to be able to cut even 1 byte
    // (if sizeof(Header) == 16 then for 1 byte we need 16 + 16 bytes)
    Header *alloc_in_mem(unsigned int to_alloc) {
        // because of allocation is a quite costly operation, we try to allocate big block at first
        // and then cut it for our needs
        if (to_alloc < memory_block) {
            to_alloc = memory_block;
        }

        unsigned int al = byte_size(to_alloc);

        if (allocated + al > memory_size) {
            return nullptr;
        }

        Header *block = new (Memory + offset) Header;
        offset += al;
        allocated += al;
        block->s.next = nullptr;
        block->s.size = to_alloc;

        return block;
    }

    const ptrdiff_t offset_calc(const void * const mem1, const void * const mem2) const {
        return static_cast<const char *> (mem1) - static_cast<const char *> (mem2);
    }

    const unsigned int byte_size(unsigned int to_alloc) const {
        return to_alloc * header_size;
    }

    // Our base Header member
    Header base;
    // Pointer to the last non-full memory block
    Header *freep;
    unsigned int allocated;
    unsigned int offset;
    char Memory[memory_size];

    // Prohibit copying
    SmallAllocator(const SmallAllocator& a) = delete;
    SmallAllocator& operator=(const SmallAllocator& a) = delete;

public:
    SmallAllocator() : base{&base, 0}, freep{&base}, allocated{0}, offset{0} {}

    void *Alloc(unsigned int Size) {
        Header *prevp;
        unsigned int new_alloc = (Size + header_size - 1) / header_size + 1;
        // start from the beginig and look for the suitable memory block
        for (Header *p = &base; ; prevp = p, p = p->s.next) {
            if (p->s.size >= new_alloc) {
                // if the current block equal to the allocated memory block
                // take it and forward the next pointer of the previous block to the block after current block
                if (p->s.size == new_alloc) {
                    prevp->s.next = p->s.next;
                }
                // in another case separate current block for two
                else {
                    p->s.size -= new_alloc;
                    p += p->s.size;
                    p->s.size = new_alloc;
                }

                freep = prevp;

                return static_cast<void *> (p + 1);
            }
            // we use the memory pool of 1MB, so if the current block does not suiteable
            // we have to skip it in memory
            if (p != &base) {
               offset += offset_calc(p + byte_size(p->s.size), Memory);
            }
            // if there is no place to put in data
            if (p == freep) {
                p->s.next = alloc_in_mem(new_alloc);

                if (!p->s.next) {
                    return nullptr;
                }
            }
        }
    }

    void *ReAlloc(void *Pointer, unsigned int Size) {
        if (!Pointer) {
            return Alloc(Size);
        }

        Header *base_ptr = static_cast<Header *> (Pointer) - 1;
        unsigned int new_alloc = (Size + header_size - 1) / header_size + 1;

        // in case we want the same size or a little bit smaller
        if (base_ptr->s.size >= new_alloc) {
            return static_cast<void *> (base_ptr + 1);
        }

        void *new_place = Alloc(Size);
        memcpy(new_place, Pointer, byte_size(base_ptr->s.size));
        Free(Pointer);

        return new_place;
    }

    void Free(void *Pointer) {
        if (!Pointer) {
            return;
        }
        // get the header of a block
        Header *base_ptr = static_cast<Header *> (Pointer) - 1;
        Header *p = &base;

        offset = offset_calc(base_ptr, Memory);
        allocated -= byte_size(base_ptr->s.size);
        // start from the begining of the list and search for the memory block's place
        while (!(base_ptr >= p && base_ptr <= p->s.next)) {
            if (p >= p->s.next && (base_ptr >= p || base_ptr <= p->s.next)) {
                break;
            }

            p = p->s.next;
        }
        // if it is right before the "upper bound" block
        if (base_ptr + base_ptr->s.size == p->s.next) {
            base_ptr->s.size += p->s.next->s.size;
            base_ptr->s.next = p->s.next->s.next;
        }
        else {
            base_ptr->s.next = p->s.next;
        }
        // if it is right before the "lower bound" block
        if (p + p->s.size == base_ptr) {
            p->s.size += base_ptr->s.size;
            p->s.next = base_ptr->s.next;
        }
        else {
            p->s.next = base_ptr;
        }

        freep = p->s.next;
    }

    void dump() const {
        cout << "===========================" << endl;
        for (auto ptr = &base; ptr != nullptr; ptr = ptr->s.next) {
          cout << "Address of the block: " << ptr << endl;
          cout << "Size of the block: " << ptr->s.size << endl;
        }
        cout << "Freep address: " << freep << endl;
        cout << "===========================" << endl;
    }
};

void check_block(const int * const block, const size_t size, const int value) {
    for (auto i = 0; i < size; ++i) {
        assert(block[i] == value);
    }
}

int main() {
    SmallAllocator S1;
    constexpr auto var1 = 10;
    constexpr auto var2 = 11;
    constexpr auto var3 = 12;
    constexpr auto var4 = 13;
    constexpr auto size1 = 5248;
    constexpr auto size2 = 1024;
    constexpr auto size3 = 128;
    constexpr auto size4 = size1 * 2;

    int *bigblock1 = static_cast<int *> (S1.Alloc(size1 * sizeof(int)));
    int *bigblock2 = static_cast<int *> (S1.Alloc(size1 * sizeof(int)));
    int *bigblock3 = static_cast<int *> (S1.Alloc(size1 * sizeof(int)));

    if (bigblock1) {
        fill(bigblock1, bigblock1 + size1, var1);
    }

    if (bigblock2) {
        fill(bigblock2, bigblock2 + size1, var2);
    }

    if (bigblock3) {
        fill(bigblock3, bigblock3 + size1, var3);
    }
    S1.dump();
    check_block(bigblock1, size1, var1);
    check_block(bigblock2, size1, var2);
    check_block(bigblock3, size1, var3);

    S1.Free(bigblock2);
    S1.dump();
    int *biggestblock = static_cast<int *> (S1.Alloc(size4 * sizeof(int)));

    if (biggestblock) {
        fill(biggestblock, biggestblock + size4, var2);
    }

    check_block(bigblock1, size1, var1);
    check_block(bigblock3, size1, var3);

    int *mediumblock1 = static_cast<int *> (S1.Alloc(size2 * sizeof(int)));

    if (mediumblock1) {
      fill(mediumblock1, mediumblock1 + size2, var4);
    }

    check_block(mediumblock1, size2, var4);
    check_block(bigblock1, size1, var1);
    check_block(bigblock3, size1, var3);

    cout << "mediumblock1 addr: " << mediumblock1 << endl;
    cout << "bigblock1 addr: " << bigblock1 << endl;
    cout << "bigblock3 addr: " << bigblock3 << endl;

    return 0;
}

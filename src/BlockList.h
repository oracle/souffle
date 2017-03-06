//
// Created by Patrick Nappa on 7/12/16.
//

#pragma once

#include "Types.h"
#include <vector>
#include <exception>

#include <iostream>

/**
 * A class that is designed to mimic std::list, but with better destructor speed
 * It achieves this by allocating comparatively large chunks of memory as it needs
 */
template<class T>
class BlockList {

    /** stores T in BLOCKSIZE sized arrays */
    std::vector<T*> listData;
    mutable size_t m_size = 0;

public:
    BlockList() : listData() {

    }

    /** copy constructor */
    BlockList(const BlockList& other) {
        size_t othblocks = other.listData.size();
        for (size_t i = 0; i < othblocks; ++i) {
            listData.push_back(new T[BLOCKSIZE]);
            memcpy(listData.at(i), other.listData.at(i), BLOCKSIZE);
        }
        this->m_size = other.m_size;
    }

    /** move constructor */
    BlockList(BlockList&& other) noexcept : BlockList(){
        for (size_t i = 0; i < listData.size(); ++i) delete[] listData.at(i);
        this->listData.clear();
        size_t othblocks = other.listData.size();
        for (size_t i = 0; i < othblocks; ++i) {
            listData.push_back(other.listData.at(i));
            other.listData.at(i) = nullptr;
        }
        this->m_size = other.size();
    }

    BlockList& operator=(BlockList other) {
        std::swap(this->listData, other.listData);
        std::swap(this->m_size, other.m_size);
        return *this;
    }

    ~BlockList() {
        for (size_t i = 0 ; i < listData.size(); ++i) {
            delete[] listData.at(i);
        }
    }

    /**
     * Size of List
     * @return the number of elements currently stored
     */
    inline size_t size() const { return m_size; };

    /**
     * Add a value to the blocklist
     * @param val : value to be added
     */
    void add(const T& val) {

        //store it in the vector
        size_t blocknum = m_size >> BLOCKBITS;
        size_t blockindex = m_size & (BLOCKSIZE - 1);

        //we need to add a new block
        if (blockindex == 0) {
            this->listData.push_back(new T[BLOCKSIZE]);
        }

        this->listData[blocknum][blockindex] = T(val);

        ++m_size;
    }

    /**
     * Retrieve a reference to the stored value at index
     * @param index position to search
     * @return the value at index
     */
    T& get(const size_t index) const {
        if (index < 0 || index >= this->size()) throw std::runtime_error("out of bounds");

        size_t blocknum = index >> BLOCKBITS;
        size_t blockindex = index & (BLOCKSIZE - 1);

        return listData[blocknum][blockindex];
    }
    
    /**
     * Clear all elements from the BlockList
     */
    void clear() {
        for (size_t i = 0 ; i < listData.size(); ++i) {
            delete[] listData.at(i);
        }
        listData.clear();
        m_size = 0;
    }

    /**
     * Remove the last element from the data
     * and destroying the block if its the last one in it
     * @return the copied value
     */
     T pop() {
        if (m_size == 0) throw std::runtime_error("pop called on empty blocklist");

        --m_size;
        size_t blocknum = m_size >> BLOCKBITS;
        size_t blockindex = m_size & (BLOCKSIZE - 1);

        // we need to destroy the block - we're the last one standing
        if (blockindex == 0) {
            T retVal = listData[blocknum][blockindex];

            delete[] listData[blocknum];

            listData.pop_back();

            return retVal;

        } else {
            return listData[blocknum][blockindex];
        }
    }

    /**
     * Move the other list's elements here
     * REMOVING them from the other
     * Similar to std::list.splice()
     * @param other the other list
     */
    void niptuck(BlockList& other) {

        if (other.size() == 0) return;

        // fill up the remainder of this BLs last block with elements from the end of other's
        size_t requiredEmpties = BLOCKSIZE - (m_size & (BLOCKSIZE-1));
        //we ignore the case of having to remove the entire block - its more efficient to do later
        if (requiredEmpties != BLOCKSIZE) {
            //pop and add
            for (size_t rem = requiredEmpties; rem > 0 && other.size() > 0; --rem) {
                this->add(other.pop());
            }
        }

        //negative values mess up the shift
        if (other.size() == 0) {
            other.listData.clear();
            other.m_size = 0;
            return;
        }

        //if we have remainder, we've got to make sure that we simply plonk in the pointers
        // size-1, because BLOCKSIZE >> BLOCKBITS should equal 1
        size_t otherblocks = (other.m_size-1) >> BLOCKBITS;
        ++otherblocks;
        for (size_t i = 0; i < otherblocks; ++i) {
            listData.push_back(other.listData[i]);
        }

        this->m_size += other.m_size;
        other.listData.clear();
        other.m_size = 0;
    }
    
    class iterator : std::iterator<std::forward_iterator_tag, T> {
        size_t cIndex = 0;
        BlockList* bl;
    public:
        
        // default ctor, to silence
        iterator() {};
        
        /* begin iterator for iterating over all elements */
        iterator(BlockList* bl) : bl(bl) {};
        /* ender iterator for marking the end of the iteration */
        iterator(BlockList* bl, size_t beginInd) : bl(bl), cIndex(beginInd) {};
        
        T operator*() { return bl->get(cIndex); };
        const T operator*() const { return bl->get(cIndex); };
        
        iterator& operator++(int) {
            ++cIndex;
            return *this;
        };
        
        iterator operator++() {
            iterator ret(*this);
            ++cIndex;
            return ret;
        };
        
        friend bool operator==(const iterator& x, const iterator& y) {
            return x.cIndex == y.cIndex && x.bl == y.bl;
        };

        friend bool operator!=(const iterator& x, const iterator& y) {
            return !(x == y);
        };


    };
    
    iterator begin() { return iterator(this); };
    iterator end() { return iterator(this, size()); };
};

/** this is necessary for atomics, as we cannot use the copy ctor */
template<>
inline void BlockList<std::atomic<block_t>>::add(const std::atomic<block_t>& val) {
    
    //store it in the vector
    size_t blocknum = m_size >> BLOCKBITS;
    size_t blockindex = m_size & (BLOCKSIZE - 1);
    
    //we need to add a new block
    if (blockindex == 0) {
        this->listData.push_back(new std::atomic<block_t>[BLOCKSIZE]);
    }
    // we need to assign inplace for atomics
    this->listData[blocknum][blockindex] = val.load();
    
    ++m_size;

}



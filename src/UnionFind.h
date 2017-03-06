//
// Created by Patrick Nappa on 6/12/16.
//

#pragma once

#include <atomic>
#include <vector>
#include <list>
#include <unordered_map>
#include <iterator>
#include <limits>
#include <exception>


#include "BlockList.h"
#include "Types.h"

/*
 * This is a C++ implementation of a parallel union-find listData structure
 * It also allows insertion of arbitrary nodes during run-time
 *
 * Author:
 *      Patrick Nappa
 *
 * Reference:
 *      Richard J. Anderson and Heather Woll. 1991.
 *      Wait-free parallel algorithms for the union-find problem.
 *      In Proceedings of the twenty-third annual ACM symposium on
 *      Theory of computing (STOC '91). ACM, New York, NY, USA,
 *      370-380. DOI=http://dx.doi.org/10.1145/103418.103458
 */


/**
 * TODO:
 *      - implement rule of 5 properly (might be difficult with the atomics, but to hell with it)
 */

/**
 * Structure that emulates a Disjoint Set, i.e. a data structure that supports efficient union-find operations
 */
class DisjointSet {

    /* store blocks of atomics */
    BlockList<std::atomic<block_t>> a_blocks;

    std::mutex insertMutex;

    /* whether the generated iterator needs to be updated */
    std::atomic<bool> isStale;
    std::atomic<bool> mapStale;
    
    /* a map which keeps representatives and their nodes in the disjoint set */
    std::unordered_map<parent_t, BlockList<parent_t>> repToSubords;

public:
    DisjointSet() : isStale(true), mapStale(true) {};

    inline size_t size() const { return a_blocks.size(); };

    inline bool staleList() const { return isStale; };
    inline bool staleMap() const { return mapStale; };

    /**
     * Yield reference to the node by its node index
     * @param node node to be searched
     * @return the parent block of the specified node
     */
    inline std::atomic<block_t>& get(parent_t node) const {
        return a_blocks.get(node);
    };

    /**
     * Equivalent to the find() function in union/find
     * Find the highest ancestor of the provided node - flattening as we go
     * @param x the node to find the parent of, whilst flattening its set-tree
     * @return The parent of x
     */
    parent_t findNode(parent_t x, bool isStrong = true) {
        isStale.store(true);
        mapStale.store(true);
        // while x's parent is not itself
        while (x != b2p(get(x))) {
            
            block_t xState = get(x);
            //yield x's parent's parent
            parent_t newParent = b2p(get(b2p(xState)));
            //construct block out of the original rank and the new parent
            block_t newState = pr2b(newParent, b2r(xState));
            
            if (isStrong) this->get(x).compare_exchange_strong(xState, newState);
            else this->get(x).compare_exchange_weak(xState, newState);
            
            x = newParent;
        }
        return x;
    }
    
    /**
     * Read only version of findNode
     * i.e. it doesn't compress the tree when searching - it only finds the top representative
     * @param x the node to find the rep of
     * @return the representative that is found
     */
    parent_t readOnlyFindNode(parent_t x) const {
        if (x == b2p(get(x))) return x;
        return readOnlyFindNode(b2p(get(x)));
    }

private:
    /**
     * Update the root of the tree of which x is, to have y as the base instead
     * @param x : old root
     * @param oldrank : old root rank
     * @param y : new root
     * @param newrank : new root rank
     * @return Whether the update succeeded (fails if another root update/union has been perfomed in the interim)
     */
    bool updateRoot(const parent_t x, const rank_t oldrank, const parent_t y, const rank_t newrank, bool isStrong) {
        isStale.store(true);
        mapStale.store(true);
        
        block_t oldState = get(x);
        parent_t nextN = b2p(oldState);
        rank_t rankN = b2r(oldState);
        
        if (nextN != x || rankN != oldrank) return false;
        //set the parent and rank of the new record
        block_t newVal = pr2b(y, newrank);
        
        if (isStrong) return this->get(x).compare_exchange_strong(oldState, newVal);
        return this->get(x).compare_exchange_weak(oldState, newVal);
    }

public:
    
    /** iterator stuff */
    class iterator : public std::iterator<std::forward_iterator_tag, parent_t> {
        
        DisjointSet* ds;
        
        /* iterator object to iterate over the rep, children parings */
        std::unordered_map<parent_t, BlockList<parent_t>>::iterator repIter;
        /* iterator for iterating over elements within a single disjoint set */
        BlockList<parent_t>::iterator memberIterator;
        
        enum iterType { iterSubReps, iterReps };
        // what type of iterator this is
        iterType cType;
        
    private:
        void advance() {
            switch (cType) {
                case iterSubReps:
                    ++memberIterator;
                    break;
                case iterReps:
                    ++repIter;
                    break;
                default:
                    throw "invalid iterator type";
            }
        }
        
        parent_t yield() const {
            if (ds->staleList() || ds->staleMap()) throw "iterator modification exception";
            
            switch (cType) {
                case iterSubReps:
                    return *memberIterator;
                case iterReps:
                    return (*repIter).first;
                default:
                    throw "invalid iterator type";
            }
        }
    
    public:
        /* iterate over all nodes with this node as the representative */
        iterator(DisjointSet* ds, parent_t representative) : ds(ds), memberIterator(ds->repToSubords[representative].begin()), cType(iterSubReps) {};
        iterator(DisjointSet* ds, parent_t representative, bool) : ds(ds), memberIterator(ds->repToSubords[representative].end()), cType(iterSubReps) {};
        /* iterator over all representatives */
        iterator(DisjointSet* ds, bool isRepIter) : ds(ds), repIter(ds->repToSubords.begin()), cType(iterReps) {};
        iterator(DisjointSet* ds, bool isRepIter, bool) : ds(ds), repIter(ds->repToSubords.end()), cType(iterReps) {};
//        /* iterator over all nodes */
//        iterator(DisjointSet* ds) : repIter(ds->repToSubords.begin()) {
//            // TODO do we need this?
//            // create an iterator to point to the current member children
//        }
        
        iterator& operator++(int) {
            advance();
            return *this;
        }
        
        iterator operator++() {
            iterator ret(*this);
            advance();
            return ret;
        }
        
        parent_t operator*() {
            return yield();
        }
        
        const parent_t operator*() const {
            return yield();
        }
        
        friend bool operator==(const iterator& x, const iterator& y) {
            switch (x.cType) {
                case iterSubReps:
                    return y.cType == iterSubReps && x.memberIterator == y.memberIterator;

                case iterReps:
                    return y.cType == iterReps && x.repIter == y.repIter;

                default:
                    throw "invalid iterator type";
            }
        };
        
        friend bool operator!=(const iterator& x, const iterator& y) {
            return !(x == y);
        };

    };
    
public:
    
    // iterating over all nodes
//    iterator begin();
//    iterator end();
    
    // iterator over all representatives
    iterator beginReps() { this->genMap(); return iterator(this, true); };
    iterator endReps() { this->genMap(); return iterator(this, true, true); };
    
    //iterator over all nodes with this as their representative
    iterator begin(parent_t rep) { this->genMap(); return iterator(this, rep); };
    iterator end(parent_t rep) { this->genMap(); return iterator(this, rep, true); };
    
    
    /**
     * Clears the DisjointSet of all nodes
     * Invalidates all iterators
     */
    void clear() {
        std::lock_guard<std::mutex> lock(insertMutex);
        isStale = true;
        mapStale = true;
        
        repToSubords.clear();
        
        a_blocks.clear();
    }
    
    /**
     * Check whether the two indices are in the same set
     * @param x node to be checked
     * @param y node to be checked
     * @return where the two indices are in the same set
     */
    bool sameSet(parent_t x, parent_t y) {
        while(true) {
            x = findNode(x, false);
            y = findNode(y, false);
            if (x == y) return true;
            //if x's parent is itself, they are not the same set
            if (b2p(get(x)) == x) return false;
        }
    }

    /**
     * Union the two specified index nodes
     * @param x node to be unioned
     * @param y node to be unioned
     */
    void unionNodes(parent_t x, parent_t y) {
        while (true) {
            x = findNode(x, false);
            y = findNode(y, false);
            
            // no need to union if both already in same set
            if (x == y) return;
            
            isStale.store(true);
            mapStale.store(true);
            
            rank_t xrank = b2r(get(x));
            rank_t yrank = b2r(get(y));
            
            //if x comes before y (better rank or earlier & equal node)
            if (xrank > yrank || ((xrank == yrank) && x > y)) {
                std::swap(x, y);
                std::swap(xrank, yrank);
            }
            //join the trees together
            //perhaps we can optimise the use of compare_exchange_strong here, as we're in a pessimistic loop
            if (!updateRoot(x, xrank, y, yrank, true)) continue;
            if (xrank == yrank) updateRoot(y, yrank, y, yrank + 1, false);
            break;
        }
    }

    /**
     * Performs a find operation on every node s.t. all nodes have a direct ref to their set's representative
     * This is only performed if necessary.
     */
    void findAll() {
        if (isStale) {
            
            //TODO: check whether we should use strong=false/true in findNode args
            for (parent_t i = 0; i < size(); ++i) findNode(i);
            
            isStale.store(false);
        }
    }

    /**
     * Create a node with its parent as itself, rank 0
     * @return the newly created block
     */
    inline block_t makeNode(){

        std::lock_guard<std::mutex> lock(insertMutex);

        isStale.store(true);
        mapStale.store(true);

        //its parent is itself (size indicates the position in the listData structure)
        parent_t xpar = (parent_t) size();
        rank_t xrank = 0;

        block_t x = pr2b(xpar, xrank);
        
        a_blocks.add(x);

        return x;
    };

    /**
     * Generate representative->group map for all elements in the disjoint set
     * Keys of the map are the representatives
     * Values are the representative's children
     */
    void genMap() {
        if (mapStale) {
            
            findAll();
            
            repToSubords.clear();
            mapStale.store(false);
            
            for (parent_t i = 0; i < size(); ++i) {
                repToSubords[b2p(this->get(i))].add(i);
            }
        }
    }

    size_t numInSet(parent_t rep) {
        //we may not have an up to date map underneath
        genMap();
        
        return repToSubords.at(rep).size();
    };

    /**Ï
     * Extract parent from block
     * @param inblock the block to be masked
     * @return The parent_t contained in the upper half of block_t
     */
    static inline parent_t b2p(const block_t inblock) {
        return (parent_t) (inblock >> split_size);
    };

    /**
     * Extract rank from block
     * @param inblock the block to be masked
     * @return the rank_t contained in the lower half of block_t
     */
    static inline rank_t b2r(const block_t inblock) {
        return (rank_t) (inblock & rank_mask);
    };

    /**
     * Yield a block given parent and rank
     * @param parent the top half bits
     * @param rank the lower half bits
     * @return the resultant block after merge
     */
    static inline block_t pr2b(const parent_t parent, const rank_t rank) {
        return (((block_t) parent) << split_size) | rank;
    };

};

template<typename SparseDomain>
class SparseDisjointSet {
    // lock on write operations - STL containers only support 1 writer at all times
    std::mutex modLock;
    
    DisjointSet ds;
    //values stored in here to those in the dense disjoint set
    std::unordered_map<SparseDomain, parent_t> sparseToDenseMap;
    std::vector<SparseDomain> denseToSparseMap;
    
private:
    /**
     * Retrieve dense encoding, adding it in if non-existent
     * @param in the sparse value
     * @return the corresponding dense value
     */
    parent_t toDense(const SparseDomain in) {
        auto it = sparseToDenseMap.find(in);
        if (it != sparseToDenseMap.end()) {
            return it->second;
        } else {
            std::lock_guard<std::mutex> lock(modLock);
            
            size_t j = denseToSparseMap.size();
            //check if we create a dense value outside of the bounds that can be stored
            if (j > std::numeric_limits<parent_t>::max()) throw std::runtime_error("out of bounds dense value");
            parent_t jClipped = static_cast<parent_t>(j);
            
            //we create the node
            ds.makeNode();
            denseToSparseMap.push_back(in);
            sparseToDenseMap[in] = jClipped;
            
            return jClipped;
        }
    }
    
public:
    
    /** iterator stuff */
    class iterator : public std::iterator<std::forward_iterator_tag, SparseDomain> {
        // we offload all iteration jobs to the underlying disjoint set's iterable
        // and simply convert upon dereference
        DisjointSet::iterator maskIter;
        
        SparseDisjointSet* sds;
        
    private:
        SparseDomain yield() const {
            return sds->toSparse(*maskIter);
        }
        
    public:
        // iterate over all nodes in representative's disjoint set
        iterator(SparseDisjointSet* sds, const SparseDomain rep) : sds(sds), maskIter(sds->ds.begin(sds->toDense(rep))) {};
        // end() equivalent
        iterator(SparseDisjointSet* sds, const SparseDomain rep, bool) : sds(sds), maskIter(sds->ds.end(sds->toDense(rep))) {};
        
        // iterate over all representatives
        iterator(SparseDisjointSet* sds, bool isRepIter) : sds(sds), maskIter(sds->ds.beginReps()) {};
        // end() equiv
        iterator(SparseDisjointSet* sds, bool isRepIter, bool) : sds(sds), maskIter(sds->ds.endReps()) {};
        
        iterator& operator++(int) {
            ++maskIter;
            return *this;
        }
        
        iterator operator++() {
            iterator ret(*this);
            ++maskIter;
            return ret;
        }
        
        const SparseDomain operator*() const {
            return yield();
        }
        
        SparseDomain operator*() {
            return yield();
        }
        
        friend bool operator==(const iterator& x, const iterator& y) {
            return x.sds == y.sds && x.maskIter == y.maskIter;
        };
        
        friend bool operator!=(const iterator& x, const iterator& y) {
            return !(x == y);
        };
    };
    
    iterator begin(const SparseDomain rep) { return iterator(this, rep); };
    iterator end(const SparseDomain rep) { return iterator(this, rep, true); };
    
    iterator beginReps() { return iterator(this, true); };
    iterator endReps() { return iterator(this, true, true); };
    
    /**
     * For the given dense value, return the associated sparse value
     * @param in the supplied dense value
     * @return the sparse value from the denseToSparseMap
     */
    inline const SparseDomain toSparse(const parent_t in) const { return denseToSparseMap[in]; };
    
    /* a wrapper to enable checking in the sparse set - however also adds them if not already existing */
    inline bool sameSet(SparseDomain x, SparseDomain y) { return ds.sameSet(toDense(x), toDense(y)); };
    /* simply a wrapper to findNode, that does not affect the structure of the disjoint set */
    inline SparseDomain readOnlyFindNode(SparseDomain x) { return toSparse(ds.readOnlyFindNode(toDense(x))); };
    /* finds the node in the underlying disjoint set, adding the node if non-existent */
    inline SparseDomain findNode(SparseDomain x) { return toSparse(ds.findNode(toDense(x))); };
    /* union the nodes, add if not existing */
    inline void unionNodes(SparseDomain x, SparseDomain y) { ds.unionNodes(toDense(x), toDense(y)); };
    
    inline std::size_t size() { return ds.size(); };
    
    
    // TODO: documentation
    void clear() {
        //we should clear this first, as we want to reduce how many locks are blocking at one given moment
        ds.clear();
        
        std::lock_guard<std::mutex> lg(modLock);
        
        sparseToDenseMap.clear();
        denseToSparseMap.clear();
    }
    
    /**
     * Gets the number of the items in the underlying dense set (mapping from sparse->dense)
     * @param in the node which we count the size of it's representative map
     * @return the size of the disjoint set
     */
    inline std::size_t sizeOfRepresentativeSet(SparseDomain in) {
        parent_t inD = toDense(in);
        return ds.numInSet(ds.readOnlyFindNode(inD));
    }
    
    /* wrapper for node creation */
    inline void makeNode(SparseDomain val) { toDense(val); };
    
    /* whether we the supplied node exists */
    inline bool nodeExists(const SparseDomain val) const { return sparseToDenseMap.find(val) != sparseToDenseMap.end(); };
    
    void genMap() { ds.genMap(); }
};

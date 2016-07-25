/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file BTree.h
 *
 * An implementation of a generic B-tree data structure including
 * interfaces for utilizing instances as set or multiset containers.
 *
 ***********************************************************************/

#pragma once

#include <cassert>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

#include "Util.h"
#include "ParallelUtils.h"

namespace souffle {

namespace detail {

    // ---------- comparators --------------

    /**
     * A generic comparator implementation as it is used by
     * a b-tree based on types that can be less-than and
     * equality comparable.
     */
    template<typename T>
    struct comparator {
        /**
         * Compares the values of a and b and returns
         * -1 if a<b, 1 if a>b and 0 otherwise
         */
        int operator()(const T& a, const T& b) const {
            return (a>b) - (a<b);
        }
        bool less(const T& a, const T& b) const {
            return a < b;
        }
        bool equal(const T& a, const T& b) const {
            return a == b;
        }
    };


    // ---------- search strategies --------------

    /**
     * A common base class for search strategies in b-trees.
     */
    struct search_strategy {};

    /**
     * A linear search strategy for looking up keys in b-tree nodes.
     */
    struct linear_search : public search_strategy {

        /**
         * Required user-defined default constructor.
         */
        linear_search() {}

        /**
         * Obtains an iterator referencing an element equivalent to the
         * given key in the given range. If no such element is present,
         * a reference to the first element not less than the given key
         * is returned.
         */
        template<typename Key, typename Iter, typename Comp>
        inline Iter operator()(const Key& k, Iter a, Iter b, Comp& comp) const {
            return lower_bound(k,a,b,comp);
        }

        /**
         * Obtains a reference to the first element in the given range that
         * is not less than the given key.
         */
        template<typename Key, typename Iter, typename Comp>
        inline Iter lower_bound(const Key& k, Iter a, Iter b, Comp& comp) const {
            auto c = a;
            while(c<b) {
                auto r = comp(*c,k);
                if (r >= 0) return c;
                ++c;
            }
            return b;
        }

        /**
         * Obtains a reference to the first element in the given range that
         * such that the given key is less than the referenced element.
         */
        template<typename Key, typename Iter, typename Comp>
        inline Iter upper_bound(const Key& k, Iter a, Iter b, Comp& comp) const {
            auto c = a;
            while(c<b) {
                if (comp(*c,k)>0) return c;
                ++c;
            }
            return b;
        }

    };

    /**
     * A binary search strategy for looking up keys in b-tree nodes.
     */
    struct binary_search : public search_strategy {

        /**
         * Required user-defined default constructor.
         */
        binary_search() {}

        /**
         * Obtains an iterator pointing to some element within the given
         * range that is equal to the given key, if available. If multiple
         * elements are equal to the given key, an undefined instance will
         * be obtained (no guaranteed lower or upper boundary).  If no such
         * element is present, a reference to the first element not less than
         * the given key will be returned.
         */
        template<typename Key, typename Iter, typename Comp>
        Iter operator()(const Key& k, Iter a, Iter b, Comp& comp) const {
            Iter c;
            auto count = b-a;
            while(count > 0) {
                auto step = count >> 1;
                c = a + step;
                auto r = comp(*c,k);
                if (r == 0) return c;
                if(r<0) {
                    a = ++c;
                    count -= step+1;
                } else {
                    count = step;
                }
            }
            return a;
        }

        /**
         * Obtains a reference to the first element in the given range that
         * is not less than the given key.
         */
        template<typename Key, typename Iter, typename Comp>
        Iter lower_bound(const Key& k,  Iter a, Iter b, Comp& comp) const {
            Iter c;
            auto count = b-a;
            while(count > 0) {
                auto step = count >> 1;
                c = a + step;
                if (comp(*c,k) < 0) {
                    a = ++c;
                    count -= step + 1;
                } else {
                    count = step;
                }
            }
            return a;
        }

        /**
         * Obtains a reference to the first element in the given range that
         * such that the given key is less than the referenced element.
         */
        template<typename Key, typename Iter, typename Comp>
        Iter upper_bound(const Key& k, Iter a, Iter b, Comp& comp) const {
            Iter c;
            auto count = b-a;
            while(count > 0) {
                auto step = count >> 1;
                c = a + step;
                if (comp(k,*c) >= 0) {
                    a = ++c;
                    count -= step + 1;
                } else {
                    count = step;
                }
            }
            return a;
        }

    };


    // ---------- search strategies selection --------------

    /**
     * A template-meta class to select search strategies for b-trees
     * depending on the key type.
     */
    template<typename S>
    struct strategy_selection {
        typedef S type;
    };

    struct linear : public strategy_selection<linear_search> {};
    struct binary : public strategy_selection<binary_search> {};

    // by default every key utilizes binary search
    template<typename Key>
    struct default_strategy : public binary {};

    template<>
    struct default_strategy<int> : public linear {};

    template<typename ... Ts>
    struct default_strategy<std::tuple<Ts...>> : public linear {};


    /**
     * The actual implementation of a b-tree data structure.
     *
     * @tparam Key             .. the element type to be stored in this tree
     * @tparam Comparator     .. a class defining an order on the stored elements
     * @tparam Allocator     .. utilized for allocating memory for required nodes
     * @tparam blockSize    .. determines the number of bytes/block utilized by leaf nodes
     * @tparam SearchStrategy .. enables switching between linear, binary or any other search strategy
     * @tparam isSet        .. true = set, false = multiset
     */
    template<
        typename Key,
        typename Comparator,
        typename Allocator,            // is ignored so far - TODO: add support
        unsigned blockSize,
        typename SearchStrategy,
        bool isSet
    >
    class btree {

    public:

        class iterator;
        typedef iterator const_iterator;

        typedef Key key_type;
        typedef range<iterator> chunk;

    private:

        /* ------------- static utilities ----------------- */

        const static SearchStrategy search;


        /* ---------- comparison utilities ---------------- */

        mutable Comparator comp;

        bool less(const Key& a, const Key& b) const {
            return comp.less(a,b);
        }

        bool equal(const Key& a, const Key& b) const {
            return comp.equal(a,b);
        }


        /* -------------- the node type ----------------- */

        typedef std::size_t size_type;
        typedef uint8_t field_index_type;
        typedef OptimisticReadWriteLock lock_type;

        struct node;

        /**
         * The base type of all node types containing essential
         * book-keeping information.
         */
        struct base {

#ifdef IS_PARALLEL

            // the parent node
            node* volatile parent;

            // a lock for synchronizing parallel operations on this node
            lock_type lock;

            // the number of keys in this node
            volatile size_type numElements;

            // the position in the parent node
            volatile field_index_type position;
#else
            // the parent node
            node* parent;

            // the number of keys in this node
            size_type numElements;

            // the position in the parent node
            field_index_type position;
#endif


            // a flag indicating whether this is a inner node or not
            const bool inner;

            /**
             * A simple constructor for nodes
             */
            base(bool inner) : parent(nullptr), numElements(0), position(0), inner(inner) {}

            bool isLeaf() const {
                return !inner;
            }

            bool isInner() const {
                return inner;
            }

            node* getParent() const {
                return parent;
            }

            field_index_type getPositionInParent() const {
                return position;
            }

            size_type getNumElements() const {
                return numElements;
            }

        };

        struct inner_node;

        /**
         * The actual, generic node implementation covering the operations
         * for both, inner and leaf nodes.
         */
        struct node : public base {

            enum {

                /**
                 * The number of keys/node desired by the user.
                 */
                desiredNumKeys = ((blockSize > sizeof(base)) ? blockSize - sizeof(base) : 0)/sizeof(Key),

                /**
                 * The actual number of keys/node corrected by functional requirements.
                  */
                maxKeys = (desiredNumKeys > 3) ? desiredNumKeys : 3
            };

            // the keys stored in this node
            Key keys[maxKeys];

            // a simple constructor
            node(bool inner) : base(inner) {}

            // a destructor cleaning up nodes
            ~node() {
                if (this->inner) asInnerNode().cleanup();
            }

            /**
             * A deep-copy operation creating a clone of this node.
             */
            node* clone() const {

                // create a clone of this node
                node* res = (this->isInner())
                        ? static_cast<node*>(new inner_node())
                        : static_cast<node*>(new leaf_node());

                // copy basic fields
                res->position = this->position;
                res->numElements = this->numElements;

                for(size_type i = 0; i<this->numElements; ++i) {
                    res->keys[i] = this->keys[i];
                }

                // if this is a leaf we are done
                if (this->isLeaf()) return res;

                // copy child nodes recursively
                inner_node* ires = (inner_node*) res;
                for(size_type i =0; i<= this->numElements; ++i) {
                    ires->children[i] = this->getChild(i)->clone();
                    ires->children[i]->parent = res;
                }

                // that's it
                return res;
            }

            /**
             * A utility function providing a reference to this node as
             * an inner node.
             */
            inner_node& asInnerNode() {
                assert(this->inner && "Invalid cast!");
                return *static_cast<inner_node*>(this);
            }

            /**
             * A utility function providing a reference to this node as
             * a const inner node.
             */
            const inner_node& asInnerNode() const {
                assert(this->inner && "Invalid cast!");
                return *static_cast<const inner_node*>(this);
            }

            /**
             * Computes the number of nested levels of the tree rooted
             * by this node.
             */
            size_type getDepth() const {
                if (this->isLeaf()) return 1;
                return getChild(0)->getDepth() + 1;
            }

            /**
             * Counts the number of nodes contained in the sub-tree rooted
             * by this node.
             */
            size_type countNodes() const {
                if (this->isLeaf()) return 1;
                size_type sum = 1;
                for(unsigned i=0; i<=this->numElements; ++i) {
                    sum += getChild(i)->countNodes();
                }
                return sum;
            }

            /**
             * Determines the amount of memory used by the sub-tree rooted
             * by this node.
             */
            size_type getMemoryUsage() const {
                if (this->isLeaf()) return sizeof(leaf_node);
                size_type res = sizeof(inner_node);
                for(unsigned i=0; i<=this->numElements; ++i) {
                    res += getChild(i)->getMemoryUsage();
                }
                return res;
            }

            /**
             * Obtains a pointer to the array of child-pointers
             * of this node -- if it is an inner node.
             */
            node** getChildren() {
                return asInnerNode().children;
            }

            /**
             * Obtains a pointer to the array of const child-pointers
             * of this node -- if it is an inner node.
             */
            node* const* getChildren() const {
                return asInnerNode().children;
            }

            /**
             * Obtains a reference to the child of the given index.
             */
            node* getChild(size_type s) const {
                return asInnerNode().children[s];
            }

            /**
             * Checks whether this node is empty -- can happen due to biased insertion.
             */
            bool isEmpty() const {
                return this->numElements == 0;
            }

            /**
             * Checks whether this node is full.
             */
            bool isFull() const {
                return this->numElements == maxKeys;
            }

            /**
             * Obtains the point at which full nodes should be split.
             * Conventional b-trees always split in half. However, in cases
             * where in-order insertions are frequent, a split assigning
             * larger portions to the right fragment provide higher performance
             * and a better node-filling rate.
             */
            int getSplitPoint(int) {
                return std::min(3*maxKeys/4,maxKeys-2);
            }

            /**
             * Splits this node.
             *
             * @param root .. a pointer to the root-pointer of the enclosing b-tree
             *                 (might have to be updated if the root-node needs to be split)
             * @param idx  .. the position of the insert causing the split
             */
            void split(node** root, lock_type& root_lock, int idx) {
#ifdef IS_PARALLEL
                assert(this->lock.is_write_locked());
                assert(!this->parent || this->parent->lock.is_write_locked());
                assert((this->parent != nullptr) || root_lock.is_write_locked());
#endif
                assert(this->numElements == maxKeys);

                // get middle element
                int split_point = getSplitPoint(idx);

                // create a new sibling node
                node* sibling = (this->inner)
                        ? static_cast<node*>(new inner_node())
                        : static_cast<node*>(new leaf_node());

#ifdef IS_PARALLEL
                // lock sibling
                sibling->lock.start_write();
#endif

                // move data over to the new node
                for(unsigned i = split_point+1, j = 0; i<maxKeys; ++i, ++j) {
                    sibling->keys[j] = keys[i];
                }

                // move child pointers
                if (this->inner) {

                    // move pointers to sibling
                    inner_node* other = static_cast<inner_node*>(sibling);
                    for(unsigned i = split_point+1, j=0; i<=maxKeys; ++i,++j) {
                        other->children[j] = getChildren()[i];
                        other->children[j]->parent = other;
                        other->children[j]->position = j;
                    }

                }

                // update number of elements
                this->numElements = split_point;
                sibling->numElements = maxKeys - split_point - 1;

                // update parent
                grow_parent(root, root_lock, sibling);

#ifdef IS_PARALLEL
                // unlock sibling
                sibling->lock.end_write();
#endif

            }

            /**
             * Moves keys from the this node to one of its siblings or splits
             * this node to make some space for the insertion of an element at
             * position idx.
             *
             * Returns the number of elements moved to the left side, 0 in case
             * of a split. The number of moved elements will be <= the given idx.
             *
             * @param root .. the root node of the b-tree being part of
             * @param idx  .. the position of the insert triggering this operation
             */
            // TODO: remove root_lock ... no longer needed
            int rebalance_or_split(node** root, lock_type& root_lock, int idx) {
#ifdef IS_PARALLEL
                assert(this->lock.is_write_locked());
                assert(!this->parent || this->parent->lock.is_write_locked());
                assert((this->parent != nullptr) || root_lock.is_write_locked());
#endif

                // this node is full ... and needs some space
                assert(this->numElements == maxKeys);

                // get snap-shot of parent
                auto parent = this->parent;
                auto pos = this->position;

                // Option A) re-balance data
                if (parent && pos > 0) {

                    node* left = parent->getChild(pos-1);

#ifdef IS_PARALLEL
                    // lock access to left sibling
                    if (!left->lock.try_start_write()) {
                        // left node is currently updated => skip balancing and split
                        split(root, root_lock, idx);
                        return 0;
                    }
#endif

                    // compute number of elements to be movable to left
                    //    space available in left vs. insertion index
                    size_type num = std::min<int>(maxKeys - left->numElements, idx);

                    // if there are elements to move ..
                    if (num > 0) {
                        Key* splitter = &(parent->keys[this->position-1]);

                        // .. move keys to left node
                        left->keys[left->numElements] = *splitter;
                        for(size_type i=0; i<num-1; ++i) {
                            left->keys[left->numElements+1+i] = keys[i];
                        }
                        *splitter = keys[num-1];

                        // shift keys in this node to the left
                        for(size_type i=0; i<this->numElements-num;++i) {
                            keys[i] = keys[i+num];
                        }

                        // .. and children if necessary
                        if (this->isInner()) {
                            inner_node* ileft = static_cast<inner_node*>(left);
                            inner_node* iright = static_cast<inner_node*>(this);

                            // move children
                            for(size_type i=0; i<num; ++i) {
                                ileft->children[left->numElements + i + 1] = iright->children[i];
                            }

                            // update moved children
                            for(size_type i=0; i<num; ++i) {
                                iright->children[i]->parent = ileft;
                                iright->children[i]->position = left->numElements + i + 1;
                            }

                            // shift child-pointer to the left
                            for(size_type i=0; i<this->numElements-num+1; ++i) {
                                iright->children[i] = iright->children[i+num];
                            }

                            // update position of children
                            for(size_type i=0; i<this->numElements-num+1; ++i) {
                                iright->children[i]->position = i;
                            }

                        }

                        // update node sizes
                        left->numElements += num;
                        this->numElements -= num;

#ifdef IS_PARALLEL
                        left->lock.end_write();
#endif

                        // done
                        return num;
                    }

#ifdef IS_PARALLEL
                    left->lock.abort_write();
#endif
                }

                // Option B) split node
                split(root, root_lock, idx);
                return 0;         // = no re-balancing
            }

        private:

            /**
             * Inserts a new sibling into the parent of this node utilizing
             * the last key of this node as a separation key. (for internal
             * use only)
             *
             * @param root .. a pointer to the root-pointer of the containing tree
             * @param sibling .. the new right-sibling to be add to the parent node
             */
            void grow_parent(node** root, lock_type& root_lock, node* sibling) {
#ifdef IS_PARALLEL
                assert(this->lock.is_write_locked());
                assert(!this->parent || this->parent->lock.is_write_locked());
                assert((this->parent != nullptr) || root_lock.is_write_locked());
#endif

                if(this->parent == nullptr) {
                    assert(*root == this);

                    // create a new root node
                    inner_node* new_root = new inner_node();
                    new_root->numElements = 1;
                    new_root->keys[0] = keys[this->numElements];

                    new_root->children[0] = this;
                    new_root->children[1] = sibling;

                    // link this and the sibling node to new root
                    this->parent = new_root;
                    sibling->parent = new_root;
                    sibling->position = 1;

                    // switch root node
                    *root = new_root;

                } else {
                    // insert new element in parent element
                    auto parent = this->parent;
                    auto pos = this->position;

                    parent->insert_inner(root, root_lock, pos, this, keys[this->numElements], sibling);
                }

            }

            /**
             * Inserts a new element into an inner node (for internal use only).
             *
             * @param root .. a pointer to the root-pointer of the containing tree
             * @param pos  .. the position to insert the new key
             * @param key  .. the key to insert
             * @param newNode .. the new right-child of the inserted key
             */
            void insert_inner(node** root, lock_type& root_lock, unsigned pos, node* predecessor, const Key& key, node* newNode) {
#ifdef IS_PARALLEL
                assert(this->lock.is_write_locked());
#endif

                // check capacity
                if (this->numElements >= maxKeys) {

#ifdef IS_PARALLEL
                    assert(!this->parent || this->parent->lock.is_write_locked());
                    assert((this->parent) || root_lock.is_write_locked());
#endif

                    // split this node
                    pos -= rebalance_or_split(root, root_lock, pos);

                    // complete insertion within new sibling if necessary
                    if (pos > this->numElements) {

                        // correct position
                        pos = pos - this->numElements-1;

                        // get new sibling
                        auto other = this->parent->getChild(this->position+1);

#ifdef IS_PARALLEL
                        // lock other side
                        other->lock.start_write();

                        // search for new position (since other may have been altered in the meanwhile)
                        size_type i=0;
                        for(; i<=other->numElements; ++i)
                            if (other->getChild(i) == predecessor) break;

                        pos = (i > other->numElements) ? 0 : i;
#endif
                        other->insert_inner(root, root_lock, pos, predecessor, key, newNode);
#ifdef IS_PARALLEL
                        other->lock.end_write();
#endif
                        return;
                    }

                }

                // move bigger keys one forward
                for(int i = this->numElements-1; i>= (int)pos ; --i) {
                    keys[i+1] = keys[i];
                    getChildren()[i+2] = getChildren()[i+1];
                    ++getChildren()[i+2]->position;
                }

                // ensure proper position
                assert(getChild(pos) == predecessor);

                // insert new element
                keys[pos] = key;
                getChildren()[pos+1] = newNode;
                newNode->parent = this;
                newNode->position = pos+1;
                ++this->numElements;
            }

        public:

            /**
             * Prints a textual representation of this tree to the given output stream.
             * This feature is mainly intended for debugging and tuning purposes.
             *
             * @see btree::printTree
             */
            void printTree(std::ostream& out, const std::string& prefix) const {

                // print the header
                out << prefix << "@" << this << "[" << ((int)(this->position)) << "] - "
                        << (this->inner ? "i":"") << "node : "
                        << this->numElements << "/"  << maxKeys << " [";

                // print the keys
                for(unsigned i=0; i<this->numElements; i++) {
                    out << keys[i];
                    if (i != this->numElements-1) {
                        out << ",";
                    }
                }
                out << "]";

                // print references to children
                if (this->inner) {
                    out << " - [";
                    for(unsigned i=0; i<=this->numElements; i++) {
                        out << getChildren()[i];
                        if (i != this->numElements) {
                            out << ",";
                        }
                    }
                    out << "]";
                }

#ifdef IS_PARALLEL
                // print the lock state
                if (this->lock.is_write_locked()) std::cout << " locked";
#endif

                out << "\n";

                // print the children recursively
                if (this->inner) {
                    for(unsigned i=0; i<this->numElements+1; ++i) {
                        static_cast<const inner_node*>(this)->children[i]->printTree(out, prefix + "    ");
                    }
                }
            }


            /**
             * A function decomposing the sub-tree rooted by this node into approximately equally
             * sized chunks. To minimize computational overhead, no strict load balance nor limit
             * on the number of actual chunks is given.
             *
             * @see btree::getChunks()
             *
             * @param res   .. the list of chunks to be extended
             * @param num   .. the number of chunks to be produced
             * @param begin .. the iterator to start the first chunk with
             * @param end   .. the iterator to end the last chunk with
             * @return the handed in list of chunks extended by generated chunks
             */
            std::vector<chunk>& collectChunks(std::vector<chunk>& res, size_type num, const iterator& begin, const iterator& end) const {

                assert(num > 0);

                // special case: this node is empty
                if (isEmpty()) {
                    if (begin!=end) res.push_back(chunk(begin,end));
                    return res;
                }

                // special case: a single chunk is requested
                if (num == 1) {
                    res.push_back(chunk(begin,end));
                    return res;
                }

                // cut-off
                if (this->isLeaf() || num < (this->numElements+1)) {
                    auto step = this->numElements / num;
                    if (step == 0) step = 1;

                    size_type i = 0;

                    // the first chunk starts at the begin
                    res.push_back(chunk(begin, iterator(this,step-1)));

                    // split up the main part
                    for(i = step-1; i<this->numElements-step; i+=step) {
                        res.push_back(chunk(iterator(this,i), iterator(this,i+step)));
                    }

                    // the last chunk runs to the end
                    res.push_back(chunk(iterator(this,i),end));

                    // done
                    return res;
                }

                // else: collect chunks of sub-set elements

                auto part = num / (this->numElements + 1);
                assert(part > 0);
                getChild(0)->collectChunks(res,part,begin,iterator(this,0));
                for(size_type i =1; i<this->numElements; i++) {
                    getChild(i)->collectChunks(res, part, iterator(this,i-1),iterator(this,i));
                }
                getChild(this->numElements)->collectChunks(res,num - (part * this->numElements),iterator(this,this->numElements-1), end);

                // done
                return res;
            }

            /**
             * A function to verify the consistency of this node.
             *
             * @param root ... a reference to the root of the enclosing tree.
             * @return true if valid, false otherwise
             */
            template<typename Comp>
            bool check(Comp& comp, const node* root) const {
                bool valid = true;

                // check fill-state
                if (this->numElements > maxKeys) {
                    std::cout << "Node with " << this->numElements << "/" << maxKeys << " encountered!\n";
                    valid = false;
                }

                // check root state
                if (root == this) {
                    if (this->parent != nullptr) {
                        std::cout << "Root not properly linked!\n";
                        valid = false;
                    }
                } else {

                    // check parent relation
                    if (!this->parent) {
                        std::cout << "Invalid null-parent!\n";
                        valid = false;
                    } else {
                        if (this->parent->getChildren()[this->position] != this) {
                            std::cout << "Parent reference invalid!\n";
                            std::cout << "   Node:     " << this << "\n";
                            std::cout << "   Parent:   " << this->parent << "\n";
                            std::cout << "   Position: " << ((int)this->position) << "\n";
                            valid = false;
                        }

                        // check parent key
                        if (valid && this->position != 0 && !(comp(this->parent->keys[this->position-1], keys[0]) < ((isSet)?0:1))) {
                            std::cout << "Left parent key not lower bound!\n";
                            std::cout << "   Node:     " << this << "\n";
                            std::cout << "   Parent:   " << this->parent << "\n";
                            std::cout << "   Position: " << ((int)this->position) << "\n";
                            std::cout << "   Key:   " << (this->parent->keys[this->position]) << "\n";
                            std::cout << "   Lower: " << (keys[0]) << "\n";
                            valid = false;
                        }

                        // check parent key
                        if (valid && this->position != this->parent->numElements && !(comp(keys[this->numElements-1], this->parent->keys[this->position]) < ((isSet)?0:1))) {
                            std::cout << "Right parent key not lower bound!\n";
                            std::cout << "   Node:     " << this << "\n";
                            std::cout << "   Parent:   " << this->parent << "\n";
                            std::cout << "   Position: " << ((int)this->position) << "\n";
                            std::cout << "   Key:   " << (this->parent->keys[this->position]) << "\n";
                            std::cout << "   Upper: " << (keys[0]) << "\n";
                            valid = false;
                        }

                    }

                }

                // check element order
                if (this->numElements > 0) {
                    for(unsigned i = 0; i<this->numElements-1; i++) {
                        if (valid && !( comp(keys[i],keys[i+1]) < ((isSet)?0:1))) {
                            std::cout << "Element order invalid!\n";
                            std::cout << " @" << this << " key " << i << " is " << keys[i] << " vs " << keys[i+1] << "\n";
                            valid = false;
                        }
                    }
                }

                // check state of sub-nodes
                if (this->inner) {
                    for(unsigned i = 0; i<=this->numElements; i++) {
                        valid &= getChildren()[i]->check(comp, root);
                    }
                }

                return valid;
            }
        };


        /**
         * The data type representing inner nodes of the b-tree. It extends
         * the generic implementation of a node by the storage locations
         * of child pointers.
         */
        struct inner_node : public node {

            // references to child nodes owned by this node
            node* children[node::maxKeys+1];

            // a simple default constructor initializing member fields
            inner_node() : node(true) {}

            // a destruction operation clearing up child nodes recursively
            void cleanup() {
                for(unsigned i = 0; i <= this->numElements; ++i) {
                    delete children[i];
                }
            }

        };

        /**
         * The data type representing leaf nodes of the b-tree. It does not
         * add any capabilities to the generic node type.
         */
        struct leaf_node : public node {
            // a simple default constructor initializing member fields
            leaf_node() : node(false) {}
        };


        // ------------------- iterators ------------------------

    public:

        /**
         * The iterator type to be utilized for scanning through btree instances.
         */
        class iterator : public std::iterator<std::forward_iterator_tag,Key> {

            // a pointer to the node currently referred to
            node const * cur;

            // the index of the element currently addressed within the referenced node
            field_index_type pos;

        public:

            // default constructor -- creating an end-iterator
            iterator() : cur(nullptr), pos(0) {}

            // creates an iterator referencing a specific element within a given node
            iterator(node const * cur, field_index_type pos) : cur(cur), pos(pos) {}

            // a copy constructor
            iterator(const iterator& other) : cur(other.cur), pos(other.pos) {}

            // an assignment operator
            iterator& operator=(const iterator& other) {
                cur = other.cur;
                pos = other.pos;
                return *this;
            }

            // the equality operator as required by the iterator concept
            bool operator==(const iterator& other) const {
                return cur == other.cur && pos == other.pos;
            }

            // the not-equality operator as required by the iterator concept
            bool operator!=(const iterator& other) const {
                return !(*this == other);
            }

            // the deref operator as required by the iterator concept
            const Key& operator*() const {
                return cur->keys[pos];
            }

            // the increment operator as required by the iterator concept
            iterator& operator++() {

                // the quick mode -- if in a leaf and there are elements left
                if(cur->isLeaf() && ++pos < cur->getNumElements()) {
                    return *this;
                }

                // otherwise it is a bit more tricky

                // A) currently in an inner node => go to the left-most child
                if (cur->isInner()) {
                    cur = cur->getChildren()[pos+1];
                    while(!cur->isLeaf()) {
                        cur = cur->getChildren()[0];
                    }
                    pos = 0;

                    // nodes may be empty due to biased insertion
                    if (!cur->isEmpty()) return *this;
                }

                // B) we are at the right-most element of a leaf => go to next inner node
                assert(cur->isLeaf());
                assert(pos == cur->getNumElements());

                while(cur != nullptr && pos == cur->getNumElements()) {
                    pos = cur->getPositionInParent();
                    cur = cur->getParent();
                }
                return *this;

            }

            // prints a textual representation of this iterator to the given stream (mainly for debugging)
            void print(std::ostream& out = std::cout) const {
                out << cur << "[" << (int)pos << "]";
            }

        };

        /**
         * A collection of operation hints speeding up some of the involved operations
         * by exploiting temporal locality.
         */
        struct operation_hints {

            // the node where the last insertion terminated
            node* last_insert;

            // the node where the last find-operation terminated
            node* last_find_end;

            // the node where the last lower-bound operation terminated
            node* last_lower_bound_end;

            // the node where the last upper-bound operation terminated
            node* last_upper_bound_end;

            // default constructor
            operation_hints()
                : last_insert(nullptr), last_find_end(nullptr),
                  last_lower_bound_end(nullptr), last_upper_bound_end(nullptr) {}

            // resets all hints (to be triggered e.g. when deleting nodes)
            void clear() {
                last_insert = nullptr;
                last_find_end = nullptr;
                last_lower_bound_end = nullptr;
                last_upper_bound_end = nullptr;
            }
        };

    private:

#ifdef IS_PARALLEL
        // the total number of elements in this tree
        volatile size_type numElements;

        // a pointer to the root node of this tree
        node* volatile root;

        // a lock to synchronize update operations on the root pointer
        lock_type root_lock;
#else
        // the total number of elements in this tree
        size_type numElements;

        // a pointer to the root node of this tree
        node* root;

        // required to not duplicate too much code
        lock_type root_lock;
#endif

        // a pointer to the left-most node of this tree (initial note for iteration)
        leaf_node* leftmost;

    public:

        enum {
            // the maximum number of keys stored per node
            max_keys_per_node = node::maxKeys
        };

        // -- ctors / dtors --

        // the default constructor creating an empty tree
        btree(const Comparator& comp = Comparator())
            : comp(comp), numElements(0), root(nullptr), leftmost(nullptr) {}

        // a constructor creating a tree from the given iterator range
        template<typename Iter>
        btree(const Iter& a, const Iter& b)
            : numElements(0), root(nullptr), leftmost(nullptr) {
            insert(a,b);
        }

        // a move constructor
        btree(btree&& other)
            : comp(other.comp), numElements(other.numElements), root(other.root), leftmost(other.leftmost) {
            other.numElements = 0;
            other.root = nullptr;
            other.leftmost = nullptr;
        }

        // a copy constructor
        btree(const btree& set) : comp(set.comp), numElements(0), root(nullptr), leftmost(nullptr) {
            // use assignment operator for a deep copy
            *this = set;
        }

    protected:

        /**
         * An internal constructor enabling the specific creation of a tree
         * based on internal parameters.
         */
        btree(size_type size, node* root, leaf_node* leftmost)
            : numElements(size), root(root), leftmost(leftmost) {}

    public:

        // the destructor freeing all contained nodes
        ~btree() { clear(); }


        // -- mutators and observers --

        // emptiness check
        bool empty() const {
            return numElements==0;
        }

        // determines the number of elements in this tree
        size_type size() const {
            return numElements;
        }

        /**
         * Inserts the given key into this tree.
         */
        bool insert(const Key& k) {
            operation_hints hints;
            return insert(k, hints);
        }

        /**
         * Inserts the given key into this tree.
         */
        bool insert(const Key& k, operation_hints& hints) {
#ifdef IS_PARALLEL

            // special handling for inserting first element
            while (numElements == 0) {

                // try obtaining root-lock
                if (!root_lock.try_start_write()) {
                    // somebody else was faster => re-check
                    continue;
                }

                // check loop condition again
                if (numElements != 0) {
                    // somebody else was faster => normal insert
                    root_lock.end_write();
                    break;
                }

                // create new node
                leftmost = new leaf_node();
                leftmost->numElements = 1;
                leftmost->keys[0] = k;
                root = leftmost;

                // increment number of elements (atomically)
                __sync_fetch_and_add(&numElements, 1);

                // operation complete => we can release the root lock
                root_lock.end_write();

                hints.last_insert = leftmost;

                return true;
            }

            // insert using iterative implementation

            node* cur = nullptr;

            // test last insert
            lock_type::Lease cur_lease;

            if (hints.last_insert) {
                // get a read lease on indicated node
                auto hint_lease = hints.last_insert->lock.start_read();
                // check whether it covers the key
                if (covers(hints.last_insert, k)) {
                    // and if there was no concurrent modification
                    if (hints.last_insert->lock.validate(hint_lease)) {
                        // use hinted location
                        cur = hints.last_insert;
                        // and keep lease
                        cur_lease = hint_lease;
                    }
                }
            }

            // if there is no valid hint ..
            if (!cur) {

                do {
                    // get root - access lock
                    auto root_lease = root_lock.start_read();

                    // start with root
                    cur = root;

                    // get lease of the next node to be accessed
                    cur_lease = cur->lock.start_read();

                    // check validity of root pointer
                    if (root_lock.end_read(root_lease)) break;

                } while (true);

            }

            while(true) {

                // handle inner nodes
                if(cur->inner) {

                    auto a = &(cur->keys[0]);
                    auto b = &(cur->keys[cur->numElements]);

                    auto pos = search.lower_bound(k,a,b,comp);
                    auto idx = pos-a;

                    // early exit for sets
                    if(isSet && pos != b && equal(*pos,k)) {
                        // validate results
                        if (!cur->lock.validate(cur_lease)) {
                            // start over again
                            return insert(k,hints);
                        }
                        // we found the element => no check of lock necessary
                        return false;
                    }

                    // get next pointer
                    auto next = cur->getChild(idx);

                    // get lease on next level
                    auto next_lease = next->lock.start_read();

                    // check whether there was a write
                    if (!cur->lock.end_read(cur_lease)) {
                        // start over
                        return insert(k,hints);
                    }

                    // go to next
                    cur = next;

                    // move on lease
                    cur_lease = next_lease;

                    continue;
                }

                // the rest is for leaf nodes
                assert(!cur->inner);

                // -- insert node in leaf node --

                auto a = &(cur->keys[0]);
                auto b = &(cur->keys[cur->numElements]);

                auto pos = search.upper_bound(k,a,b,comp);
                auto idx = pos-a;

                // early exit for sets
                if(isSet && pos != a && equal(*(pos-1),k)) {
                    // validate result
                    if (!cur->lock.validate(cur_lease)) {
                        // start over again
                        return insert(k,hints);
                    }
                    // we found the element => done
                    return false;
                }

                // upgrade to write-permission
                if (!cur->lock.try_upgrade_to_write(cur_lease)) {
                    // something has changed => restart
                    hints.last_insert = cur;
                    return insert(k,hints);
                }

                if (cur->numElements >= node::maxKeys) {

                    // -- lock parents --
                    auto priv = cur;
                    auto parent = priv->parent;
                    std::vector<node*> parents;
                    do {
                        if (parent) {
                            parent->lock.start_write();
                            while(true) {
                                // check whether parent is correct
                                if (parent == priv->parent) break;
                                // switch parent
                                parent->lock.abort_write();
                                parent = priv->parent;
                                parent->lock.start_write();
                            }
                        } else {
                            // lock root lock => since cur is root
                            root_lock.start_write();
                        }

                        // record locked node
                        parents.push_back(parent);

                        // stop at "sphere of influence"
                        if (!parent || !parent->isFull()) break;

                        // go one step higher
                        priv = parent;
                        parent = parent->parent;

                    } while (true);

                    // split this node
                    auto old_root = root;
                    idx -= cur->rebalance_or_split(const_cast<node**>(&root), root_lock, idx);

                    // release parent lock
                    for(auto it = parents.rbegin(); it != parents.rend(); ++it) {
                        auto parent = *it;

                        // release this lock
                        if (parent) {
                            parent->lock.end_write();
                        } else {
                            if (old_root != root) {
                                root_lock.end_write();
                            } else {
                                root_lock.abort_write();
                            }
                        }
                    }

                    // insert element in right fragment
                    if (((size_type)idx) > cur->numElements) {

                        // release current lock
                        cur->lock.end_write();

                        // insert in sibling
                        return insert(k,hints);
                    }
                }

                // ok - no split necessary
                assert(cur->numElements < node::maxKeys && "Split required!");

                // move keys
                for(int j = cur->numElements; j>idx; --j) {
                    cur->keys[j] = cur->keys[j-1];
                }

                // insert new element
                cur->keys[idx] = k;
                cur->numElements++;

                // release lock on current node
                cur->lock.end_write();

                // new element has been inserted
                __sync_fetch_and_add(&numElements, 1);

                // remember last insertion position
                hints.last_insert = cur;
                return true;
            }

#else
            // special handling for inserting first element
            if (empty()) {

                // create new node
                leftmost = new leaf_node();
                leftmost->numElements = 1;
                leftmost->keys[0] = k;
                root = leftmost;

                // increment number of elements
                ++numElements;

                hints.last_insert = leftmost;

                return true;
            }

            // insert using iterative implementation
            node* cur = root;

            // test last insert
            if(hints.last_insert && covers(hints.last_insert,k)) {
                cur = hints.last_insert;
            }

            while(true) {

                // handle inner nodes
                if(cur->inner) {

                    auto a = &(cur->keys[0]);
                    auto b = &(cur->keys[cur->numElements]);

                    auto pos = search.lower_bound(k,a,b,comp);
                    auto idx = pos-a;

                    // early exit for sets
                    if(isSet && pos != b && equal(*pos,k)) return false;

                    cur = cur->getChild(idx);
                    continue;
                }

                // the rest is for leaf nodes
                assert(!cur->inner);

                // -- insert node in leaf node --

                auto a = &(cur->keys[0]);
                auto b = &(cur->keys[cur->numElements]);

                auto pos = search.upper_bound(k,a,b,comp);
                auto idx = pos-a;

                // early exit for sets
                if(isSet && pos != a && equal(*(pos-1),k)) return false;

                if (cur->numElements >= node::maxKeys) {
                    // split this node
                    idx -= cur->rebalance_or_split(&root,root_lock,idx);

                    // insert element in right fragment
                    if (((size_type)idx) > cur->numElements) {
                        idx -= cur->numElements + 1;
                        cur = cur->parent->getChild(cur->position+1);
                    }
                }

                // ok - no split necessary
                assert(cur->numElements < node::maxKeys && "Split required!");

                // move keys
                for(int j = cur->numElements; j>idx; --j) {
                    cur->keys[j] = cur->keys[j-1];
                }

                // insert new element
                cur->keys[idx] = k;
                cur->numElements++;

                // new element has been inserted
                ++numElements;

                // remember last insertion position
                hints.last_insert = cur;
                return true;
            }
#endif
        }

        /**
         * Inserts the given range of elements into this tree.
         */
        template<typename Iter>
        void insert(const Iter& a, const Iter& b) {
            operation_hints hints;
            // a naive insert so far .. seems to work fine
            for(auto it = a; it != b; ++it) {
                // use insert with hint
                insert(*it, hints);
            }
        }

        /**
         * Inserts all elements of the given b-tree into this tree.
         * This can be a more effective alternative to the ordered insertion
         * of elements utilizing iterators.
         */
        void insertAll(const btree& other) {

            // shortcut for non-sense operation
            if (this == &other) return;

            // make sure bigger tree is inserted in smaller tree
            if ((size() + 10000) < other.size()) {
                // switch sides
                btree tmp = other;
                tmp.insertAll(*this);
                swap(tmp);
                return;
            }

            // by default use the iterator based insertion
            insert(other.begin(), other.end());
        }

        // Obtains an iterator referencing the first element of the tree.
        iterator begin() const {
            return iterator(leftmost, 0);
        }

        // Obtains an iterator referencing the position after the last element of the tree.
        iterator end() const {
            return iterator();
        }

        /**
         * Partitions the full range of this set into up to a given number of chunks.
         * The chunks will cover approximately the same number of elements. Also, the
         * number of chunks will only approximate the desired number of chunks.
         *
         * @param num .. the number of chunks requested
         * @return a list of chunks partitioning this tree
         */
        std::vector<chunk> getChunks(size_type num) const {
            std::vector<chunk> res;
            if (empty()) return res;
            return root->collectChunks(res, num,begin(), end());
        }

        /**
         * Determines whether the given element is a member of this tree.
         */
        bool contains(const Key& k) const {
            operation_hints hints;
            return contains(k, hints);
        }

        /**
         * Determines whether the given element is a member of this tree.
         */
        bool contains(const Key& k, operation_hints& hints) const {
            return find(k,hints) != end();
        }

        /**
         * Locates the given key within this tree and returns an iterator
         * referencing its position. If not found, an end-iterator will be returned.
         */
        iterator find(const Key& k) const {
            operation_hints hints;
            return find(k, hints);
        }

        /**
         * Locates the given key within this tree and returns an iterator
         * referencing its position. If not found, an end-iterator will be returned.
         */
        iterator find(const Key& k, operation_hints& hints) const {

            if (empty()) return end();

            node* cur = root;

            // test last location searched (temporal locality)
            if (hints.last_find_end && covers(hints.last_find_end,k)) {
                cur = hints.last_find_end;
            }

            // an iterative implementation (since 2/7 faster than recursive)

            while(true) {

                auto a = &(cur->keys[0]);
                auto b = &(cur->keys[cur->numElements]);

                auto pos = search(k, a, b, comp);

                if (pos < b && equal(*pos,k)) {
                    hints.last_find_end = cur;
                    return iterator(cur,pos-a);
                }

                if (!cur->inner) {
                    hints.last_find_end = cur;
                    return end();
                }

                // continue search in child node
                cur = cur->getChild(pos-a);
            }
        }

        /**
         * Obtains a lower boundary for the given key -- hence an iterator referencing
         * the smallest value that is not less the given key. If there is no such element,
         * an end-iterator will be returned.
         */
        iterator lower_bound(const Key& k) const {
            operation_hints hints;
            return lower_bound(k, hints);
        }

        /**
         * Obtains a lower boundary for the given key -- hence an iterator referencing
         * the smallest value that is not less the given key. If there is no such element,
         * an end-iterator will be returned.
         */
        iterator lower_bound(const Key& k, operation_hints& hints) const {

            if (empty()) return end();

            node* cur = root;

            // test last searched node
            if (hints.last_lower_bound_end && covers(hints.last_lower_bound_end,k)) {
                cur = hints.last_lower_bound_end;
            }

            iterator res = end();
            while(true) {

                auto a = &(cur->keys[0]);
                auto b = &(cur->keys[cur->numElements]);

                auto pos = search.lower_bound(k,a, b,comp);
                auto idx = pos-a;

                if(!cur->inner) {
                    hints.last_lower_bound_end = cur;
                    return (pos!=b) ? iterator(cur,idx) : res;
                }

                if(isSet && pos != b && equal(*pos,k)) return iterator(cur,idx);

                if(pos!=b) res = iterator(cur,idx);

                cur = cur->getChild(idx);
            }
        }

        /**
         * Obtains an upper boundary for the given key -- hence an iterator referencing
         * the first element that the given key is less than the referenced value. If
         * there is no such element, an end-iterator will be returned.
         */
        iterator upper_bound(const Key& k) const {
            operation_hints hints;
            return upper_bound(k, hints);
        }

        /**
         * Obtains an upper boundary for the given key -- hence an iterator referencing
         * the first element that the given key is less than the referenced value. If
         * there is no such element, an end-iterator will be returned.
         */
        iterator upper_bound(const Key& k, operation_hints& hints) const {

            if (empty()) return end();

            node* cur = root;

            // test last search node
            if (hints.last_upper_bound_end && coversUpperBound(hints.last_upper_bound_end,k)) {
                cur = hints.last_upper_bound_end;
            }

            iterator res = end();
            while(true) {

                auto a = &(cur->keys[0]);
                auto b = &(cur->keys[cur->numElements]);

                auto pos = search.upper_bound(k,a, b,comp);
                auto idx = pos-a;

                if(!cur->inner) {
                    hints.last_upper_bound_end = cur;
                    return (pos!=b) ? iterator(cur,idx) : res;
                }

                if(pos!=b) res = iterator(cur,idx);

                cur = cur->getChild(idx);

            }
        }

        /**
         * Clears this tree.
         */
        void clear() {
            numElements = 0;
            if(root) delete root;
            root = nullptr;
            leftmost = nullptr;
        }

        /**
         * Swaps the content of this tree with the given tree. This
         * is a much more efficient operation than creating a copy and
         * realizing the swap utilizing assignment operations.
         */
        void swap(btree& other) {
            // swap the content
            std::swap(numElements, other.numElements);
            std::swap(root, other.root);
            std::swap(leftmost, other.leftmost);
        }

        // Implementation of the assignment operation for trees.
        btree& operator=(const btree& other) {
            // check identity
            if (this == &other) return *this;

            // create a deep-copy of the content of the other tree
            // shortcut for empty sets
            if (other.empty()) return *this;

            // clone content (deep copy)
            numElements = other.size();
            root = other.root->clone();

            // update leftmost reference
            auto tmp = root;
            while(!tmp->isLeaf()) {
                tmp = tmp->getChild(0);
            }
            leftmost = static_cast<leaf_node*>(tmp);

            // done
            return *this;
        }

        // Implementation of an equality operation for trees.
        bool operator==(const btree& other) const {
            // check identity
            if (this == &other) return true;

            // check size
            if (size() != other.size()) return false;
            if (size() < other.size()) return other == *this;

            // check content
            for(const auto& key : other) {
                if (!contains(key)) return false;
            }
            return true;
        }

        // Implementation of an inequality operation for trees.
        bool operator!=(const btree& other) const {
            return !(*this == other);
        }


        // -- for debugging --

        // Determines the number of levels contained in this tree.
        size_type getDepth() const {
            return (empty()) ? 0 : root->getDepth();
        }

        // Determines the number of nodes contained in this tree.
        size_type getNumNodes() const {
            return (empty()) ? 0 : root->countNodes();
        }

        // Determines the amount of memory used by this data structure
        size_type getMemoryUsage() const {
            return sizeof(*this) + (empty() ? 0 : root->getMemoryUsage());
        }

        /*
         * Prints a textual representation of this tree to the given
         * output stream (mostly for debugging and tuning).
         */
        void printTree(std::ostream& out = std::cout) const {
            out << "B-Tree with " << size() << " elements:\n";
            if (empty()) {
                out << " - empty - \n";
            } else {
                root->printTree(out, "");
            }
        }

        /**
         * Prints a textual summary of statistical properties of this
         * tree to the given output stream (for debugging and tuning).
         */
        void printStats(std::ostream& out = std::cout) const {

            auto nodes = getNumNodes();

            out << "\n";
            out << "---------------------------------\n";
            out << "Table Statistics:\n";
            out << "---------------------------------\n";
            out << "  Elements: " << size() << "\n";
            out << "  Depth:    " << (empty()?0:root->getDepth()) << "\n";
            out << "  Nodes:    " << nodes << "\n";
            out << "---------------------------------\n";
            out << "  Size of inner node: " << sizeof(inner_node) << "\n";
            out << "  Size of leaf node:  " << sizeof(leaf_node) << "\n";
            out << "  Size of Key:        " << sizeof(Key) << "\n";
            out << "  max keys / node:  " << node::maxKeys << "\n";
            out << "  avg keys / node:  " << (size() / (double)nodes) << "\n";
            out << "  avg filling rate: " << ((size() / (double)nodes) / node::maxKeys) << "\n";
            out << "---------------------------------\n";

        }

        /**
         * Checks the consistency of this tree.
         */
        bool check() {
            auto ok = empty() || root->check(comp,root);
            if (!ok) printTree();
            return ok;
        }

        /**
         * A static member enabling the bulk-load of ordered data into an empty
         * tree. This function is much more efficient in creating a index over
         * an ordered set of elements than an iterative insertion of values.
         *
         * @tparam Iter .. the type of iterator specifying the range
         *                     it must be a random-access iterator
         */
        template<typename R, typename Iter>
        static
        typename std::enable_if<
            std::is_same<
                typename std::iterator_traits<Iter>::iterator_category,
                std::random_access_iterator_tag
            >::value, R
        >::type
        load(const Iter& a, const Iter& b) {

            // quick exit - empty range
            if (a == b) return R();

            // resolve tree recursively
            auto root = buildSubTree(a,b-1);

            // find leftmost node
            node* leftmost = root;
            while(!leftmost->isLeaf()) {
                leftmost = leftmost->getChild(0);
            }

            // build result
            return R(b-a, root, static_cast<leaf_node*>(leftmost));
        }

    private:

        /**
         * Determines whether the range covered by the given node is also
         * covering the given key value.
         */
        bool covers(const node* node, const Key& k) const {
            if (isSet) {
                // in sets we can include the ends as covered elements
                return !node->isEmpty() && !less(k,node->keys[0]) && !less(node->keys[node->numElements-1],k);
            }
            // in multi-sets the ends may not be completely covered
            return !node->isEmpty() && less(node->keys[0],k) && less(k,node->keys[node->numElements-1]);
        }

        /**
         * Determines whether the range covered by this node covers
         * the upper bound of the given key.
         */
        bool coversUpperBound(const node* node, const Key& k) const {
            // ignore edges
            return !node->isEmpty() && !less(k,node->keys[0]) && less(k,node->keys[node->numElements-1]);
        }

        // Utility function for the load operation above.
        template<typename Iter>
        static node* buildSubTree(const Iter& a, const Iter& b) {
            const int N = node::maxKeys;

            // divide range in N+1 sub-ranges
            int length = (b-a)+1;

            // terminal case: length is less then maxKeys
            if (length <= N) {
                // create a leaf node
                node* res = new leaf_node();
                res->numElements = length;

                for(int i=0; i<length; ++i) {
                    res->keys[i] = a[i];
                }

                return res;
            }


            // recursive case - compute step size
            int numKeys = N;
            int step = ((length-numKeys) / (numKeys+1));

            while(numKeys > 1 && (step < N/2)) {
                numKeys--;
                step = ((length-numKeys) / (numKeys+1));
            }

            // create inner node
            node* res = new inner_node();
            res->numElements = numKeys;

            Iter c = a;
            for(int i=0; i<numKeys; i++) {

                // get dividing key
                res->keys[i] = c[step];

                // get sub-tree
                auto child = buildSubTree(c,c+(step-1));
                child->parent = res;
                child->position = i;
                res->getChildren()[i] = child;

                c = c+(step+1);
            }

            // and the remaining part
            auto child = buildSubTree(c,b);
            child->parent = res;
            child->position = numKeys;
            res->getChildren()[numKeys] = child;

            // done
            return res;

        }

    };

    // Instantiation of static member search.
    template<typename Key, typename Comparator, typename Allocator, unsigned blockSize, typename SearchStrategy, bool isSet>
    const SearchStrategy btree<Key,Comparator,Allocator,blockSize,SearchStrategy,isSet>::search;

} // end namespace detail

/**
 * A b-tree based set implementation.
 *
 * @tparam Key             .. the element type to be stored in this set
 * @tparam Comparator     .. a class defining an order on the stored elements
 * @tparam Allocator     .. utilized for allocating memory for required nodes
 * @tparam blockSize    .. determines the number of bytes/block utilized by leaf nodes
 * @tparam SearchStrategy .. enables switching between linear, binary or any other search strategy
 */
template<
    typename Key,
    typename Comparator = detail::comparator<Key>,
    typename Allocator = std::allocator<Key>,            // is ignored so far
    unsigned blockSize = 256,
    typename SearchStrategy = typename detail::default_strategy<Key>::type
>
class btree_set : public detail::btree<Key,Comparator,Allocator,blockSize,SearchStrategy,true> {

    typedef detail::btree<Key,Comparator,Allocator,blockSize,SearchStrategy,true> super;

    friend class detail::btree<Key,Comparator,Allocator,blockSize,SearchStrategy,true>;

public:

    /**
     * A default constructor creating an empty set.
     */
    btree_set(const Comparator& comp = Comparator()) : super(comp) {}

    /**
     * A constructor creating a set based on the given range.
     */
    template<typename Iter>
    btree_set(const Iter& a, const Iter& b) {
        this->insert(a,b);
    }

    // A copy constructor.
    btree_set(const btree_set& other) : super(other) {}

    // A move constructor.
    btree_set(btree_set&& other) : super(std::move(other)) {}

private:

    // A constructor required by the bulk-load facility.
    template<typename s, typename n, typename l>
    btree_set(s size, n* root, l* leftmost)
        : super(size, root, leftmost) {}

public:

    // Support for the assignment operator.
    btree_set& operator=(const btree_set& other) {
        super::operator=(other);
        return *this;
    }

    // Support for the bulk-load operator.
    template<typename Iter>
    static btree_set load(const Iter& a, const Iter& b) {
        return super::template load<btree_set>(a,b);
    }

};

/**
 * A b-tree based multi-set implementation.
 *
 * @tparam Key             .. the element type to be stored in this set
 * @tparam Comparator     .. a class defining an order on the stored elements
 * @tparam Allocator     .. utilized for allocating memory for required nodes
 * @tparam blockSize    .. determines the number of bytes/block utilized by leaf nodes
 * @tparam SearchStrategy .. enables switching between linear, binary or any other search strategy
 */
template<
    typename Key,
    typename Comparator = detail::comparator<Key>,
    typename Allocator = std::allocator<Key>,            // is ignored so far
    unsigned blockSize = 256,
    typename SearchStrategy = typename detail::default_strategy<Key>::type
>
class btree_multiset : public detail::btree<Key,Comparator,Allocator,blockSize,SearchStrategy,false> {

    typedef detail::btree<Key,Comparator,Allocator,blockSize,SearchStrategy,false> super;

    friend class detail::btree<Key,Comparator,Allocator,blockSize,SearchStrategy,false>;

public:

    /**
     * A default constructor creating an empty set.
     */
    btree_multiset(const Comparator& comp = Comparator()) : super(comp) {}

    /**
     * A constructor creating a set based on the given range.
     */
    template<typename Iter>
    btree_multiset(const Iter& a, const Iter& b) {
        this->insert(a,b);
    }

    // A copy constructor.
    btree_multiset(const btree_multiset& other) : super(other) {}

    // A move constructor.
    btree_multiset(btree_multiset&& other) : super(std::move(other)) {}

private:

    // A constructor required by the bulk-load facility.
    template<typename s, typename n, typename l>
    btree_multiset(s size, n* root, l* leftmost)
        : super(size, root, leftmost) {}

public:

    // Support for the assignment operator.
    btree_multiset& operator=(const btree_multiset& other) {
        super::operator=(other);
        return *this;
    }

    // Support for the bulk-load operator.
    template<typename Iter>
    static btree_multiset load(const Iter& a, const Iter& b) {
        return super::template load<btree_multiset>(a,b);
    }
};

} // end of namespace souffle



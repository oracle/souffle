/*
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All Rights reserved
 * 
 * The Universal Permissive License (UPL), Version 1.0
 * 
 * Subject to the condition set forth below, permission is hereby granted to any person obtaining a copy of this software,
 * associated documentation and/or data (collectively the "Software"), free of charge and under any and all copyright rights in the 
 * Software, and any and all patent rights owned or freely licensable by each licensor hereunder covering either (i) the unmodified 
 * Software as contributed to or provided by such licensor, or (ii) the Larger Works (as defined below), to deal in both
 * 
 * (a) the Software, and
 * (b) any piece of software and/or hardware listed in the lrgrwrks.txt file if one is included with the Software (each a “Larger
 * Work” to which the Software is contributed by such licensors),
 * 
 * without restriction, including without limitation the rights to copy, create derivative works of, display, perform, and 
 * distribute the Software and make, use, sell, offer for sale, import, export, have made, and have sold the Software and the 
 * Larger Work(s), and to sublicense the foregoing rights on either these or other terms.
 * 
 * This license is subject to the following condition:
 * The above copyright notice and either this complete permission notice or at a minimum a reference to the UPL must be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/************************************************************************
 *
 * @file IterUtils.h
 *
 * A (growing) collection of generic iterator utilities.
 *
 ***********************************************************************/

#pragma once

/**
 * A wrapper for an iterator obtaining pointers of a certain type,
 * dereferencing values before forwarding them to the consumer.
 *
 * @tparam Iter ... the type of wrapped iterator
 * @tparam T    ... the value to be accessed by the resulting iterator
 */
template<
    typename Iter,
    typename T = typename std::remove_pointer<typename Iter::value_type>::type
>
struct IterDerefWrapper : public std::iterator<std::forward_iterator_tag,T> {

    /* The nested iterator. */
    Iter iter;

public:

    // some constructores
    IterDerefWrapper() {}
    IterDerefWrapper(const Iter& iter) : iter(iter) {}

    // defaulted copy and move constructors
    IterDerefWrapper(const IterDerefWrapper&) = default;
    IterDerefWrapper(IterDerefWrapper&&) = default;

    // default assignment operators
    IterDerefWrapper& operator=(const IterDerefWrapper&) = default;
    IterDerefWrapper& operator=(IterDerefWrapper&&) = default;

    /* The equality operator as required by the iterator concept. */
    bool operator==(const IterDerefWrapper& other) const {
        return iter == other.iter;
    }

    /* The not-equality operator as required by the iterator concept. */
    bool operator!=(const IterDerefWrapper& other) const {
        return iter != other.iter;
    }

    /* The deref operator as required by the iterator concept. */
    const T& operator*() const {
        return **iter;
    }

    /* Support for the pointer operator. */
    const T* operator->() const {
        return &(**iter);
    }

    /* The increment operator as required by the iterator concept. */
    IterDerefWrapper& operator++() {
        ++iter;
        return *this;
    }

};

/**
 * A factory function enabling the construction of a dereferencing
 * iterator utilizing the automated deduction of template parameters.
 */
template<typename Iter>
IterDerefWrapper<Iter> derefIter(const Iter& iter) {
    return IterDerefWrapper<Iter>(iter);
}



// ---------------------------------------------------------------------
//                        Single-Value-Iterator
// ---------------------------------------------------------------------


/**
 * An iterator to be utilized if there is only a single element to iterate over.
 */
template<typename T>
class SingleValueIterator : public std::iterator<std::forward_iterator_tag,T> {

    T value;

    bool end;

public:

    SingleValueIterator() : end(true) {}

    SingleValueIterator(const T& value) : value(value), end(false) {}

    // a copy constructor
    SingleValueIterator(const SingleValueIterator& other) = default;

    // an assignment operator
    SingleValueIterator& operator=(const SingleValueIterator& other) =default;

    // the equality operator as required by the iterator concept
    bool operator==(const SingleValueIterator& other) const {
        // only equivalent if pointing to the end
        return end && other.end;
    }

    // the not-equality operator as required by the iterator concept
    bool operator!=(const SingleValueIterator& other) const {
        return !(*this == other);
    }

    // the deref operator as required by the iterator concept
    const T& operator*() const {
        return value;
    }

    // support for the pointer operator
    const T* operator->() const {
        return &value;
    }

    // the increment operator as required by the iterator concept
    SingleValueIterator& operator++() {
        end = true;
        return *this;
    }

};

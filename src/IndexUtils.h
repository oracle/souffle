#pragma once

#include "Util.h"
#include <deque>
#include <map>
#include <set>
#include <vector>

namespace souffle {

namespace index {

/** A class mapping an object to an index. */
template <typename Object>
class ObjectToIndex {
protected:
    /** The table mapping from an object to an index. */
    std::map<Object, size_t> objectToIndex;

public:
    /** Check if there is an index for the given object. */
    const bool has(const Object& object) const {
        return objectToIndex.find(object) != objectToIndex.end();
    }

    /** Get the index for the given object. */
    const size_t getIndex(const Object& object) const {
        assert(this->has(object));
        return objectToIndex.at(object);
    }

    /** Set the index for the given object. */
    virtual void setIndex(const Object& object, const size_t index) {
        objectToIndex[object] = index;
    }

    /** Remove the index for the given object. */
    virtual void removeIndex(const Object& object) {
        if (!this->has(object)) return;
        objectToIndex.erase(object);
    }
};

/** A class mapping an index to an object. */
template <typename Object>
class IndexToObject {
private:
    /** A set of indices pending erasure. */
    std::set<size_t, std::greater<size_t>> pending;

protected:
    /** The table mapping from an index to an object. */
    std::vector<Object> indexToObject;

public:
    /** Check if there is an object for the given index. */
    const bool hasIndex(const size_t index) const {
        return index >= 0 && index < indexToObject.size() && pending.find(index) == pending.end();
    }

    /** Get the object for the given index. */
    const Object& get(const size_t index) const {
        assert(this->hasIndex(index));
        return indexToObject.at(index);
    }

    /* Set the object for the given index. */
    virtual void set(const size_t index, const Object& object) {
        assert(index <= indexToObject.size());
        if (index == indexToObject.size()) {
            indexToObject.push_back(object);
        } else {
            auto it = pending.find(index);
            if (it != pending.end()) pending.erase(it);
            indexToObject[index] = object;
        }
    }

    /* Remove the object for the given index. */
    virtual void remove(const size_t index) {
        if (!this->hasIndex(index)) return;
        pending.insert(index);
        auto iter = pending.begin();
        while (*iter == indexToObject.size() - 1) {
            indexToObject.erase(indexToObject.begin() + *iter);
            iter++;
        }
        pending.erase(pending.begin(), iter);
    }
};

/** A class mapping an index to a collection of objects. */
template <typename Object, template <typename...> class Container>
class IndexToObjects {
private:
    /** A set of indices pending erasure. */
    std::set<size_t, std::greater<size_t>> pending;

protected:
    /** The table mapping from an index to a collection of objects. */
    std::vector<Container<Object>> indexToObject;

public:
    /** Check if there is a collection of objects for the given index. */
    const bool hasIndex(const size_t index) const {
        return index >= 0 && index < indexToObject.size() && pending.find(index) == pending.end();
    }

    /** Get the collection of objects for the given index. */
    const Container<Object>& get(const size_t index) const {
        this->hasIndex(index);
        return indexToObject.at(index);
    }

    /* Set the collection of objects for the given index. */
    virtual void set(const size_t index, const Container<Object>& objects = Container<Object>()) {
        assert(index <= indexToObject.size());
        if (index == indexToObject.size()) {
            indexToObject.push_back(objects);
        } else {
            auto it = pending.find(index);
            if (it != pending.end()) pending.erase(it);
            indexToObject[index] = objects;
        }
    }

    /* Remove the collection of objects for the given index. */
    virtual void remove(const size_t index) {
        if (!this->hasIndex(index)) return;
        pending.insert(index);
        auto iter = pending.begin();
        while (*iter == indexToObject.size() - 1) {
            indexToObject.erase(indexToObject.begin() + *iter);
            iter++;
        }
        pending.erase(pending.begin(), iter);
    }
};

/** A class mapping between index and an object. */
template <typename Object>
class IndexTable : ObjectToIndex<Object>, IndexToObject<Object> {
public:
    /** Set the index for the given object. */
    virtual void setIndex(const Object& object, const size_t index) {
        ObjectToIndex<Object>::setIndex(object, index);
        IndexToObject<Object>::set(index, object);
    }

    /* Set the object for the given index. */
    virtual void set(const size_t index, const Object& object) {
        IndexToObject<Object>::set(index, object);
        ObjectToIndex<Object>::setIndex(index, object);
    }

    /** Remove the index for the given object. */
    virtual void removeIndex(const Object& object) {
        if (!this->has(object)) return;
        size_t index = this->getIndex(object);
        ObjectToIndex<Object>::removeIndex(object);
        IndexToObject<Object>::remove(index);
    }

    /* Remove the object for the given index. */
    virtual void remove(const size_t index) {
        if (!this->hasIndex(index)) return;
        const Object& object = this->get(index);
        IndexToObject<Object>::remove(index);
        ObjectToIndex<Object>::removeIndex(object);
    }
};

/** An abstract class mapping between index and a collection of objects. */
template <typename Object, template <typename...> class Container>
class CollectionIndexTable : public ObjectToIndex<Object>, public IndexToObjects<Object, Container> {
protected:
    /** Virtual append method, used for extending classes. */
    virtual void virtualAppend(const size_t index, const Object& object) = 0;

    /** Virtual prepend method, used for extending classes. */
    virtual void virtualPrepend(const size_t index, const Object& object) = 0;

public:
    /** Set the index for the given object. */
    void setIndex(const Object& object, const size_t index) {
        this->set(index);
        virtualAppend(index, object);
        this->objectToIndex[object] = index;
    }

    /** Set the collection of objects for the given index. */
    void set(const size_t index, const Container<Object>& objects = Container<Object>()) {
        this->remove(index);
        for (const Object& object : objects) ObjectToIndex<Object>::setIndex(object, index);
        IndexToObjects<Object, Container>::set(index, objects);
    }

    /** Remove the index for the given object. */
    void removeIndex(const Object& object) {
        if (!this->has(object)) return;
        size_t index = this->getIndex(object);
        ObjectToIndex<Object>::removeIndex(object);
        IndexToObjects<Object, Container>::remove(index);
    }

    /* Remove the collection of objects for the given index. */
    void remove(const size_t index) {
        if (!this->hasIndex(index)) return;
        const Container<Object> objects = this->get(index);
        IndexToObjects<Object, Container>::remove(index);
        for (const Object& object : objects) ObjectToIndex<Object>::removeIndex(object);
    }

    /** Append the object to the sequence at the given index. */
    void append(const size_t index, const Object& object) {
        if (!this->hasIndex(index)) {
            this->setIndex(object, index);
        } else {
            virtualAppend(index, object);
            this->objectToIndex[object] = index;
        }
    }

    /** Append the collection of objects to the collection at the given index. */
    template <template <typename...> class T>
    void append(const size_t index, const T<Object>& objects) {
        if (!this->hasIndex(index)) this->set(index);
        for (const auto& object : objects) this->append(index, object);
    }

    /** Move the collection of objects at the 'from' index, appending them to the collection at the 'to'
     * index. */
    void moveAppend(const size_t fromIndex, const size_t toIndex) {
        if (!this->hasIndex(fromIndex)) return;
        assert(this->hasIndex(toIndex));
        this->append(toIndex, this->get(fromIndex));
        this->remove(fromIndex);
    }

    /** Prepend the object to the sequence at the given index. */
    void prepend(const size_t index, const Object& object) {
        if (!this->hasIndex(index)) {
            this->setIndex(object, index);
        } else {
            virtualPrepend(index, object);
            this->objectToIndex[object] = index;
        }
    }

    /** Prepend the collection of objects to the collection at the given index. */
    template <template <typename...> class T>
    void prepend(const size_t index, const T<Object>& objects) {
        if (!this->hasIndex(index)) this->set(index);
        std::vector<Object> vectorOfObjects;
        vectorOfObjects.insert(vectorOfObjects.end(), objects.begin(), objects.end());
        for (auto it = vectorOfObjects.end() - 1; it != vectorOfObjects.begin() - 1; --it)
            this->prepend(index, *it);
    }

    /** Move the collection of objects at the 'from' index, prepending them to the collection at the 'to'
     * index. */
    void movePrepend(const size_t fromIndex, const size_t toIndex) {
        if (!this->hasIndex(fromIndex)) return;
        assert(this->hasIndex(toIndex));
        this->prepend(toIndex, this->get(fromIndex));
        this->remove(fromIndex);
    }
};

/** A class mapping between index and a set of objects. */
template <typename Object>
class SetTable : public CollectionIndexTable<Object, std::set> {
protected:
    /** Insert the object into the set at the given index. */
    virtual void virtualAppend(const size_t index, const Object& object) {
        this->indexToObject[index].insert(object);
    }

    /** Insert the object into the set at the given index. */
    virtual void virtualPrepend(const size_t index, const Object& object) {
        this->indexToObject[index].insert(object);
    }
};

/** A class mapping between index and a sequence of objects. */
template <typename Object>
class SeqTable : public CollectionIndexTable<Object, std::deque> {
protected:
    /** Append the object into the sequence at the given index. */
    virtual void virtualAppend(const size_t index, const Object& object) {
        this->indexToObject[index].push_back(object);
    }

    /** Prepend the object into the sequence at the given index. */
    virtual void virtualPrepend(const size_t index, const Object& object) {
        this->indexToObject[index].push_front(object);
    }
};
}
}
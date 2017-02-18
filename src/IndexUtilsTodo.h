#pragma once

#include "Util.h"
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
        assert(this->has(object));
        objectToIndex.erase(object);
    }
};

/** A class mapping an index to an object. */
template <typename Object>
class IndexToObject {
protected:
    /** The table mapping from an index to an object. */
    std::vector<Object> indexToObject;

public:
    /** Check if there is an object for the given index. */
    const bool hasIndex(const size_t index) const {
        return index >= 0 && index < indexToObject.size() && indexToObject.at(index) != nullptr;
    }

    /** Get the object for the given index. */
    const Object& get(const size_t index) const {
        assert(this->hasIndex(index));
        return indexToObject.at(index);
    }

    /* Set the object for the given index. */
    virtual void set(const size_t index, const Object& object) {
        assert(index <= indexToObject.size());
        if (index == indexToObject.size())
            indexToObject.push_back(object);
        else
            indexToObject[index] = object;
    }

    /* Remove the object for the given index. */
    virtual void remove(const size_t index) {
        assert(this->hasIndex(index));
        if (index == indexToObject.size() - 1)
            indexToObject.erase(indexToObject.begin() + index);
        else
            indexToObject[index] = nullptr;
    }
};

/** A class mapping an index to a collection of objects. */
template <typename Object, template <typename...> typename Container>
class IndexToObjects {
protected:
    /** The table mapping from an index to a collection of objects. */
    std::vector<Container<Object>> indexToObject;

public:
    /** Check if there is a collection of objects for the given index. */
    const bool hasIndex(const size_t index) const {
        return index >= 0 && index < indexToObject.size() && !indexToObject.at(index).empty();
    }

    /** Get the collection of objects for the given index. */
    const Container<Object>& get(const size_t index) const {
        this->hasIndex(index);
        return indexToObject.at(index);
    }

    /* Set the collection of objects for the given index. */
    virtual void set(const size_t index, const Container<Object>& objects) {
        assert(!objects.empty());
        assert(index <= indexToObject.size());
        if (index == indexToObject.size())
            indexToObject.push_back(objects);
        else
            indexToObject[index] = objects;
    }

    /* Remove the collection of objects for the given index. */
    virtual void remove(const size_t index) {
        this->hasIndex(index);
        if (index == indexToObject.size() - 1)
            indexToObject.erase(indexToObject.begin() + index);
        else
            indexToObject.at(index).clear();
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
        size_t index = this->getIndex(object);
        ObjectToIndex<Object>::removeIndex(object);
        IndexToObject<Object>::remove(index);
    }

    /* Remove the object for the given index. */
    virtual void remove(const size_t index) {
        const Object& object = this->get(object);
        IndexToObject<Object>::remove(index);
        ObjectToIndex<Object>::removeIndex(object);
    }
};

/** An abstract class mapping between index and a collection of objects. */
template <typename Object, template <typename...> typename Container>
class AbstractCollectionIndexTable : public ObjectToIndex<Object>, public IndexToObjects<Object, Container> {
public:
    /** Set the index for the given object, this functionality is superseded by the methods of extending
     * classes. */
    virtual void setIndex(const Object& object, const size_t index) const = delete;

    /** Set the collection of objects for the given index, this functionality is superseded by the methods of
     * extending classes. */
    virtual void set(const size_t index, const Container<Object>& objects) const = delete;

    /** Remove the index for the given object. */
    virtual void removeIndex(const Object& object) {
        size_t index = this->getIndex(object);
        ObjectToIndex<Object>::removeIndex(object);
        IndexToObjects<Object, Container>::remove(index);
    }

    /* Remove the collection of objects for the given index. */
    virtual void remove(const size_t index) {
        const Container<Object> objects = this->get(index);
        IndexToObjects<Object, Container>::remove(index);
        for (const Object& object : objects) ObjectToIndex<Object>::removeIndex(object);
    }

    /** Append an object to the collection at the given index. */
    virtual void append(const size_t index, const Object& object) = 0;

    /** Append the collection of objects to the collection at the given index. */
    template <template <typename...> typename T>
    void append(const size_t index, const T<Object>& objects) {
        for (const auto& object : objects) this->append(index, object);
    }

    /** Move the collection of objects at the 'from' index, appending them to the collection at the 'to'
     * index. */
    void moveAppend(const size_t fromIndex, const size_t toIndex) {
        assert(this->hasIndex(fromIndex) && this->hasIndex(toIndex));
        for (auto object : this->indexToObject.at(fromIndex)) this->append(toIndex, object);
        this->remove(fromIndex);
    }

    /** Prepend an object to the collection at the given index. */
    virtual void prepend(const size_t index, const Object& object) = 0;

    /** Prepend the collection of objects to the collection at the given index. */
    template <template <typename...> typename T>
    void prepend(const size_t index, const T<Object>& objects) {
        for (const auto& object : objects) this->prepend(index, object);
    }

    /** Move the collection of objects at the 'from' index, prepending them to the collection at the 'to'
     * index. */
    void movePrepend(const size_t fromIndex, const size_t toIndex) {
        assert(this->hasIndex(fromIndex) && this->hasIndex(toIndex));
        for (auto object : this->indexToObject.at(fromIndex)) this->prepend(toIndex, object);
        this->remove(fromIndex);
    }
};

template <typename Object, template <typename...> typename Container>
class CollectionIndexTable : public AbstractCollectionIndexTable<Object, Container> {};

template <typename Object>
class CollectionIndexTable<Object, std::set> : public AbstractCollectionIndexTable<Object, std::set> {
public:
    void append(const size_t index, const Object& object) {
        assert(index <= this->indexToObject.size());
        if (index == this->indexToObject.size())
            this->indexToObject.push_back(std::set<Object>());
        this->indexToObject[index].insert(object);
        this->objectToIndex[object] = index;
    }

    void prepend(const size_t index, const Object& object) {
        this->append(index, object);
    }
};

template <typename Object>
class CollectionIndexTable<Object, std::deque> : public AbstractCollectionIndexTable<Object, std::deque> {
public:
    void append(const size_t index, const Object& object) {
        assert(index <= this->indexToObject.size());
        if (index == this->indexToObject.size())
            this->indexToObject.push_back(std::deque<Object>());
        this->indexToObject[index].push_back(object);
        this->objectToIndex[object] = index;
    }

    void prepend(const size_t index, const Object& object) {
        assert(index <= this->indexToObject.size());
        if (index == this->indexToObject.size())
            this->indexToObject.push_back(std::deque<Object>());
        this->indexToObject[index].push_front(object);
        this->objectToIndex[object] = index;
    }
};
}
}
#ifndef LIBDS_ADT_QUEUE_HPP
#define LIBDS_ADT_QUEUE_HPP

#include <libds/adt/abstract_data_type.hpp>
#include <libds/amt/explicit_sequence.hpp>
#include <libds/amt/implicit_sequence.hpp>

#include <libds/utils.hpp>


namespace ds::adt {


template<typename T>
class Queue : virtual public ADT {
public:
    virtual void push(const T &element) = 0;

    virtual T &peek() = 0;

    virtual const T &peek() const = 0;

    virtual void pop() = 0;
};

//----------

template<typename T>
class ImplicitQueue : public Queue<T>, public ADS<T> {
public:
    ImplicitQueue();

    ImplicitQueue(const ImplicitQueue &other);

    ImplicitQueue(std::size_t capacity);

    void push(const T &element) override;

    T &peek() override;

    const T &peek() const override;

    void pop() override;

    ADT &assign(const ADT &other) override;

    void clear() override;

    std::size_t size() const override;

    bool equals(const ADT &other) const override;

    std::size_t getCapacity() const;

    void reserveCapacity(std::size_t newCapacity);

private:
    amt::CyclicImplicitSequence<T> *getSequence() const;

private:
    static const int INIT_CAPACITY = 100;

private:
    std::size_t m_insertionIndex;
    std::size_t m_removalIndex;
    std::size_t m_size;
};

//----------

template<typename T>
class ExplicitQueue : public Queue<T>, public ADS<T> {
public:
    ExplicitQueue();

    ExplicitQueue(const ExplicitQueue &other);

    void push(const T &element) override;

    T &peek() override;

    const T &peek() const override;

    void pop() override;

    ADT &assign(const ADT &other) override;

    bool equals(const ADT &other) const override;

private:
    amt::SinglyLinkedSequence<T> *getSequence() const;
};

//----------

template<typename T>
ImplicitQueue<T>::ImplicitQueue() : ImplicitQueue(INIT_CAPACITY) {
}

template<typename T>
ImplicitQueue<T>::ImplicitQueue(std::size_t capacity) :
    ADS<T>(new amt::CyclicImplicitSequence<T>(capacity, true)),
    m_insertionIndex(0),
    m_removalIndex(0),
    m_size(0) {
}

template<typename T>
ImplicitQueue<T>::ImplicitQueue(const ImplicitQueue &other) :
    ADS<T>(new amt::CyclicImplicitSequence<T>(*other.getSequence())),
    m_insertionIndex(other.m_insertionIndex),
    m_removalIndex(other.m_removalIndex),
    m_size(other.m_size) {
}

template<typename T>
void ImplicitQueue<T>::push(const T &element) {
    if (this->size() == this->getSequence()->size()) {
        throw std::runtime_error("Implicit queue is full.");
    }
    this->getSequence()->access(m_insertionIndex)->data = element;
    m_insertionIndex = this->getSequence()->indexOfNext(m_insertionIndex);
    ++m_size;
}

template<typename T>
T &ImplicitQueue<T>::peek() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return this->getSequence()->access(m_removalIndex)->data;
}

template<typename T>
const T &ImplicitQueue<T>::peek() const {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return this->getSequence()->access(m_removalIndex)->data;
}

template<typename T>
void ImplicitQueue<T>::pop() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    m_removalIndex = this->getSequence()->indexOfNext(m_removalIndex);
    --m_size;
}

template<typename T>
ADT &ImplicitQueue<T>::assign(const ADT &other) {
    if (this == &other) {
        return *this;
    }

    if (this->getSequence()->size() < other.size()) {
        throw std::invalid_argument("Not enough capacity!");
    }

    const ImplicitQueue *o = dynamic_cast<const ImplicitQueue*>(&other);
    if (o == nullptr) {
        throw utils::type_missmatch();
    }

    this->getSequence()->assign(*o->getSequence());

    m_insertionIndex = o->m_insertionIndex;
    m_removalIndex = o->m_removalIndex;
    m_size = o->m_size;

    return *this;
}

template<typename T>
void ImplicitQueue<T>::clear() {
    m_insertionIndex = m_removalIndex;
    m_size = 0;
}

template<typename T>
std::size_t ImplicitQueue<T>::size() const {
    return m_size;
}

template<typename T>
bool ImplicitQueue<T>::equals(const ADT &other) const {
    if (this == &other) {
        return true;
    }

    if (this->size() != other.size()) {
        return false;
    }

    const ImplicitQueue *o = dynamic_cast<const ImplicitQueue*>(&other);
    if (o == nullptr) {
        return false;
    }

    std::size_t myIndex = m_removalIndex;
    std::size_t otherIndex = o->m_removalIndex;
    for (std::size_t i = 0; i < m_size; ++i) {
        if (this->getSequence()->access(myIndex)->data
            != o->getSequence()->access(otherIndex)->data) {
            return false;
        }
        myIndex = this->getSequence()->indexOfNext(myIndex);
        otherIndex = o->getSequence()->indexOfNext(otherIndex);
    }

    return true;
}

template<typename T>
std::size_t ImplicitQueue<T>::getCapacity() const {
    return this->getSequence()->size();
}

template<typename T>
void ImplicitQueue<T>::reserveCapacity(std::size_t newCapacity) {
    return this->getSequence()->reserveCapacity(newCapacity);
}

template<typename T>
amt::CIS<T> *ImplicitQueue<T>::getSequence() const {
    return dynamic_cast<amt::CyclicImplicitSequence<T>*>(
        this->m_memoryStructure);
}

//----------

template<typename T>
ExplicitQueue<T>::ExplicitQueue() : ADS<T>(new amt::SinglyLinkedSequence<T>()) {
}

template<typename T>
ExplicitQueue<T>::ExplicitQueue(const ExplicitQueue &other) :
    ADS<T>(new amt::SinglyLinkedSequence<T>(*other.getSequence())) {
}

template<typename T>
void ExplicitQueue<T>::push(const T &element) {
    this->getSequence()->emplaceLast().data = element;
}

template<typename T>
T &ExplicitQueue<T>::peek() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return this->getSequence()->accessFirst()->data;
}

template<typename T>
const T &ExplicitQueue<T>::peek() const {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return this->getSequence()->accessFirst()->data;
}

template<typename T>
void ExplicitQueue<T>::pop() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    this->getSequence()->removeFirst();
}

template<typename T>
ADT &ExplicitQueue<T>::assign(const ADT &other) {
    const ExplicitQueue *o = dynamic_cast<const ExplicitQueue*>(&other);
    if (o == nullptr) {
        throw utils::type_missmatch();
    }
    this->getSequence()->assign(*o->getSequence());
    return *this;
}

template<typename T>
bool ExplicitQueue<T>::equals(const ADT &other) const {
    const ExplicitQueue *o = dynamic_cast<const ExplicitQueue*>(&other);
    if (o == nullptr) {
        return false;
    }
    return this->getSequence()->equals(*o->getSequence());
}

template<typename T>
amt::SinglyLS<T> *ExplicitQueue<T>::getSequence() const {
    return dynamic_cast<amt::SinglyLinkedSequence<T>*>(this->m_memoryStructure);
}


} // namespace ds::adt

#endif

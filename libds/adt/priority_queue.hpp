#ifndef LIBDS_ADT_PRIORITY_QUEUE_HPP
#define LIBDS_ADT_PRIORITY_QUEUE_HPP

#include <libds/adt/abstract_data_type.hpp>
#include <libds/amt/explicit_sequence.hpp>
#include <libds/amt/implicit_hierarchy.hpp>
#include <libds/amt/implicit_sequence.hpp>

#include <cmath>

namespace ds::adt {


template<typename P, typename T>
struct PriorityQueueItem {
    P priority;
    T data;

    bool operator==(const PriorityQueueItem &other) const = default;
    bool operator!= (const PriorityQueueItem &other) const = default;
};

//----------

template<typename P, typename T>
class PriorityQueue : virtual public ADT {
public:
    virtual void push(const P &priority, const T &data) = 0;

    virtual T &peek() = 0;

    virtual const T &peek() const = 0;

    virtual void pop() = 0;
};

//----------

template<typename P, typename T, typename SequenceType>
class SequencePriorityQueue :
    public PriorityQueue<P, T>,
    public ADS<PriorityQueueItem<P, T>> {
public:
    SequencePriorityQueue();

    SequencePriorityQueue(const SequencePriorityQueue &other);

    bool equals(const ADT &other) const override;

protected:
    void assignSafe(const SequencePriorityQueue *other);

    SequenceType *getSequence() const;
};

//----------

template<typename P, typename T, typename SequenceType>
class UnsortedSequencePriorityQueue :
    public SequencePriorityQueue<P, T, SequenceType> {
public:
    T &peek() override;

    const T &peek() const override;

protected:
    using BlockType = typename SequenceType::BlockType;

protected:
    BlockType *findHighestPriorityBlock() const;
};

//----------

template<typename P, typename T, typename SequenceType>
class SortedSequencePriorityQueue :
    public SequencePriorityQueue<P, T, SequenceType> {
public:
    T &peek() override;

    const T &peek() const override;

    void pop() override;

protected:
    virtual std::size_t indexOfHighestPriorityBlock() const = 0;

private:
    using BlockType = typename SequenceType::BlockType;
};

//----------

template<typename P, typename T>
class UnsortedImplicitSequencePriorityQueue :
    public UnsortedSequencePriorityQueue<
        P,
        T,
        amt::ImplicitSequence<PriorityQueueItem<P, T>>
    > {
public:
    void push(const P &priority, const T &data) override;

    void pop() override;

    ADT &assign(const ADT &other) override;

private:
    using SequenceType = amt::ImplicitSequence<PriorityQueueItem<P, T>>;
    using BlockType = typename SequenceType::BlockType;
};

//----------

template<typename P, typename T>
class UnsortedExplicitSequencePriorityQueue :
    public UnsortedSequencePriorityQueue<
        P,
        T,
        amt::SinglyLinkedSequence<PriorityQueueItem<P, T>>
    > {
public:
    void push(const P &priority, const T &data) override;

    void pop() override;

    ADT &assign(const ADT &other) override;

private:
    using SequenceType = amt::SinglyLinkedSequence<PriorityQueueItem<P, T>>;
    using BlockType = typename SequenceType::BlockType;
};

//----------

template<typename P, typename T>
class SortedImplicitSequencePriorityQueue :
    public SortedSequencePriorityQueue<
        P,
        T,
        amt::ImplicitSequence<PriorityQueueItem<P, T>>
    > {
public:
    void push(const P &priority, const T &data) override;

    ADT &assign(const ADT &other) override;

protected:
    std::size_t indexOfHighestPriorityBlock() const override;

private:
    using SequenceType = amt::ImplicitSequence<PriorityQueueItem<P, T>>;
    using BlockType = typename SequenceType::BlockType;
};

//----------

template<typename P, typename T>
class SortedExplicitSequencePriorityQueue :
    public SortedSequencePriorityQueue<
        P,
        T,
        amt::SinglyLinkedSequence<PriorityQueueItem<P, T>>
    > {
public:
    void push(const P &priority, const T &data) override;

    ADT &assign(const ADT &other) override;

protected:
    std::size_t indexOfHighestPriorityBlock() const override;

private:
    using SequenceType = amt::SinglyLinkedSequence<PriorityQueueItem<P, T>>;
    using BlockType = typename SequenceType::BlockType;
};

//----------

template<typename P, typename T>
class TwoLists :
    public ADMS<PriorityQueueItem<P, T>>,
    public PriorityQueue<P, T>
{
public:
    TwoLists(std::size_t expectedSize);

    TwoLists(const TwoLists &other);

    ~TwoLists();

    void push(const P &priority, const T &data) override;

    T &peek() override;

    const T &peek() const override;

    void pop() override;

    ADT &assign(const ADT &other) override;

    void clear() override;

    std::size_t size() const override;

    bool isEmpty() const override;

    bool equals(const ADT &other) const override;

private:
    using ItemType = PriorityQueueItem<P, T>;
    using ShortSequenceType = amt::ImplicitSequence<ItemType>;
    using LongSequenceType = amt::SinglyLinkedSequence<ItemType>;
    using ShortBlockType = typename ShortSequenceType::BlockType;
    using LongBlockType = typename LongSequenceType::BlockType;

private:
    ShortSequenceType *m_shortSequence;
    LongSequenceType *m_longSequence;
};

//----------

template<typename P, typename T>
class BinaryHeap : public PriorityQueue<P, T>, public ADS<PriorityQueueItem<P, T>> {
public:
    BinaryHeap();

    BinaryHeap(const BinaryHeap &other);

    void push(const P &priority, const T &data) override;

    T &peek() override;

    const T &peek() const override;

    void pop() override;

    ADT &assign(const ADT &other) override;

    bool equals(const ADT &other) const override;

private:
    using HierarchyType = amt::BinaryImplicitHierarchy<PriorityQueueItem<P, T>>;
    using BlockType = typename HierarchyType::BlockType;

private:
    HierarchyType *getHierarchy() const;
};

//----------

template<typename P, typename T, typename SequenceType>
SequencePriorityQueue<P, T, SequenceType>::SequencePriorityQueue() :
    ADS<PriorityQueueItem<P, T>>(new SequenceType()) {
}

template<typename P, typename T, typename SequenceType>
SequencePriorityQueue<P, T, SequenceType>::SequencePriorityQueue(
    const SequencePriorityQueue &other
) :
    ADS<PriorityQueueItem<P, T>>(new SequenceType(&other.getSequence())) {
}

template<typename P, typename T, typename SequenceType>
bool SequencePriorityQueue<P, T, SequenceType>::equals(const ADT &) const {
    throw utils::unavailable_function_call();
}

template<typename P, typename T, typename SequenceType>
void SequencePriorityQueue<P, T, SequenceType>::assignSafe(
    const SequencePriorityQueue *other
) {
    if (other == nullptr) {
        throw utils::type_missmatch();
    }

    if (this == other) {
        return;
    }

    this->getSequence()->assign(*other->getSequence());
}

template<typename P, typename T, typename SequenceType>
SequenceType *SequencePriorityQueue<P, T, SequenceType>::getSequence() const {
    return dynamic_cast<SequenceType*>(this->m_memoryStructure);
}

//----------

template<typename P, typename T, typename SequenceType>
T &UnsortedSequencePriorityQueue<P, T, SequenceType>::peek() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return this->findHighestPriorityBlock()->data.data;
}

template<typename P, typename T, typename SequenceType>
const T &UnsortedSequencePriorityQueue<P, T, SequenceType>::peek() const {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return this->findHighestPriorityBlock()->data.data;
}

template<typename P, typename T, typename SequenceType>
typename SequenceType::BlockType*
UnsortedSequencePriorityQueue<P, T, SequenceType>::findHighestPriorityBlock() const {
    BlockType *bestBlock = this->getSequence()->accessFirst();
    this->getSequence()->processBlocksForward(
        this->getSequence()->accessNext(*bestBlock),
        [&](BlockType *b) {
            if (bestBlock->data.priority > b->data.priority) {
                bestBlock = b;
            }
        }
    );
    return bestBlock;
}

//----------

template<typename P, typename T, typename SequenceType>
T &SortedSequencePriorityQueue<P, T, SequenceType>::peek() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return this->getSequence()
        ->access(this->indexOfHighestPriorityBlock())
        ->data.data;
}

template<typename P, typename T, typename SequenceType>
const T &SortedSequencePriorityQueue<P, T, SequenceType>::peek() const {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return this->getSequence()
        ->access(this->indexOfHighestPriorityBlock())
        ->data.data;
}

template<typename P, typename T, typename SequenceType>
void SortedSequencePriorityQueue<P, T, SequenceType>::pop() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    this->getSequence()->remove(this->indexOfHighestPriorityBlock());
}

//----------

template<typename P, typename T>
void UnsortedImplicitSequencePriorityQueue<P, T>::push(
    const P &priority,
    const T &data
) {
    PriorityQueueItem<P, T> &item = this->getSequence()->emplaceLast().data;
    item.priority = priority;
    item.data = data;
}

template<typename P, typename T>
void UnsortedImplicitSequencePriorityQueue<P, T>::pop() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    BlockType *bestBlock = this->findHighestPriorityBlock();
    BlockType *lastBlock = this->getSequence()->accessLast();
    if (bestBlock != lastBlock) {
        std::swap(bestBlock->data, lastBlock->data);
    }
    this->getSequence()->removeLast();
}

template<typename P, typename T>
ADT &UnsortedImplicitSequencePriorityQueue<P, T>::assign(
    const ADT &other
) {
    this->assignSafe(dynamic_cast<const UnsortedImplicitSequencePriorityQueue*>(&other));
    return *this;
}

//----------

template<typename P, typename T>
void UnsortedExplicitSequencePriorityQueue<P, T>::push(
    const P &priority,
    const T &data
) {
    PriorityQueueItem<P, T> &item = this->getSequence()->emplaceFirst().data;
    item.priority = priority;
    item.data = data;
}

template<typename P, typename T>
void UnsortedExplicitSequencePriorityQueue<P, T>::pop() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    BlockType *bestBlock = this->findHighestPriorityBlock();
    BlockType *firstBlock = this->getSequence()->accessFirst();
    if (bestBlock != firstBlock) {
        std::swap(bestBlock->data, firstBlock->data);
    }
    this->getSequence()->removeFirst();
}

template<typename P, typename T>
ADT &UnsortedExplicitSequencePriorityQueue<P, T>::assign(
    const ADT &other
) {
    this->assignSafe(dynamic_cast<const UnsortedExplicitSequencePriorityQueue*>(&other));
    return *this;
}

//----------

template<typename P, typename T>
void SortedImplicitSequencePriorityQueue<P, T>::push(
    const P &priority,
    const T &data
) {
    PriorityQueueItem<P, T> *item = nullptr;

    if (this->isEmpty() || priority <= this->getSequence()->accessLast()->data.priority) {
        item = &this->getSequence()->emplaceLast().data;
    } else if (priority >= this->getSequence()->accessFirst()->data.priority) {
        item = &this->getSequence()->emplaceFirst().data;
    } else {
        BlockType *block = this->getSequence()->findBlockWithProperty(
            [&priority](BlockType *b) -> bool {
                return b->data.priority <= priority;
            }
        );
        item = &this->getSequence()->emplaceBefore(*block).data;
    }

    item->priority = priority;
    item->data = data;
}

template<typename P, typename T>
ADT &SortedImplicitSequencePriorityQueue<P, T>::assign(
    const ADT &other
) {
    this->assignSafe(dynamic_cast<const SortedImplicitSequencePriorityQueue*>(&other));
    return *this;
}

template<typename P, typename T>
std::size_t
SortedImplicitSequencePriorityQueue<P, T>::indexOfHighestPriorityBlock() const {
    return this->size() - 1;
}

//----------

template<typename P, typename T>
void SortedExplicitSequencePriorityQueue<P, T>::push(
    const P &priority,
    const T &data
) {
    PriorityQueueItem<P, T> *item = nullptr;

    if (this->isEmpty() || priority <= this->getSequence()->accessFirst()->data.priority) {
        item = &this->getSequence()->emplaceFirst().data;
    } else if (priority >= this->getSequence()->accessLast()->data.priority) {
        item = &this->getSequence()->emplaceLast().data;
    } else {
        BlockType *block = this->getSequence()->findPreviousToBlockWithProperty(
            [priority](BlockType *b) -> bool {
                return b->data.priority >= priority;
            }
        );
        item = &this->getSequence()->emplaceAfter(*block).data;
    }

    item->priority = priority;
    item->data = data;
}

template<typename P, typename T>
ADT &SortedExplicitSequencePriorityQueue<P, T>::assign(
    const ADT &other
) {
    this->assignSafe(dynamic_cast<const SortedExplicitSequencePriorityQueue*>(&other));
    return *this;
}

template<typename P, typename T>
std::size_t
SortedExplicitSequencePriorityQueue<P, T>::indexOfHighestPriorityBlock() const {
    return 0;
}

//----------

template<typename P, typename T>
TwoLists<P, T>::TwoLists(std::size_t expectedSize) :
    m_shortSequence(new ShortSequenceType(
        static_cast<std::size_t>(std::ceil(std::sqrt(expectedSize))),
        false)),
    m_longSequence(new LongSequenceType()) {
}

template<typename P, typename T>
TwoLists<P, T>::TwoLists(const TwoLists<P, T> &other) :
    m_shortSequence(new ShortSequenceType(*other.m_shortSequence)),
    m_longSequence(new LongSequenceType(*other.m_longSequence)) {
}

template<typename P, typename T>
TwoLists<P, T>::~TwoLists() {
    delete m_shortSequence;
    delete m_longSequence;
}

template<typename P, typename T>
void TwoLists<P, T>::push(const P &priority, const T &data) {
    (void)priority;
    (void)data;
    throw std::runtime_error("Not implemented yet.");
}

template<typename P, typename T>
T &TwoLists<P, T>::peek() {
    if (m_shortSequence->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return m_shortSequence->accessLast()->data.data;
}

template<typename P, typename T>
const T &TwoLists<P, T>::peek() const {
    if (m_shortSequence->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return m_shortSequence->accessLast()->data.data;
}

template<typename P, typename T>
void TwoLists<P, T>::pop() {
    throw std::runtime_error("Not implemented yet.");
}

template<typename P, typename T>
ADT &TwoLists<P, T>::assign(const ADT &other) {
    if (this == &other) {
        return *this;
    }

    const TwoLists *o = dynamic_cast<const TwoLists*>(&other);
    if (o == nullptr) {
        throw utils::type_missmatch();
    }

    m_shortSequence->assign(*o->m_shortSequence);
    m_longSequence->assign(*o->m_longSequence);

    return *this;
}

template<typename P, typename T>
void TwoLists<P, T>::clear() {
    m_shortSequence->clear();
    m_longSequence->clear();
}

template<typename P, typename T>
std::size_t TwoLists<P, T>::size() const {
    return m_shortSequence->size() + m_longSequence->size();
}

template<typename P, typename T>
bool TwoLists<P, T>::isEmpty() const {
    return m_shortSequence->isEmpty();
}

template<typename P, typename T>
bool TwoLists<P, T>::equals(const ADT &other) const {
    (void)other;
    throw utils::unavailable_function_call();
}

//----------

template<typename P, typename T>
BinaryHeap<P, T>::BinaryHeap() :
    ADS<PriorityQueueItem<P, T>>(
        new amt::BinaryImplicitHierarchy<PriorityQueueItem<P, T>>()) {
}

template<typename P, typename T>
BinaryHeap<P, T>::BinaryHeap(const BinaryHeap &other) :
    ADS<PriorityQueueItem<P, T>>(
        new amt::BinaryImplicitHierarchy<PriorityQueueItem<P, T>>(
            *other.getHierarchy())) {
}

template<typename P, typename T>
void BinaryHeap<P, T>::push(const P &priority, const T &data) {
    PriorityQueueItem<P, T> &item = this->getHierarchy()->emplaceLastLeaf().data;
    item.priority = priority;
    item.data = data;

    BlockType *current = this->getHierarchy()->accessLastLeaf();
    BlockType *parent = this->getHierarchy()->accessParent(*current);

    while (parent != nullptr && current->data.priority < parent->data.priority) {
        std::swap(current->data, parent->data);
        current = parent;
        parent = this->getHierarchy()->accessParent(*current);
    }
}

template<typename P, typename T>
T &BinaryHeap<P, T>::peek() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return this->getHierarchy()->accessRoot()->data.data;
}

template<typename P, typename T>
const T &BinaryHeap<P, T>::peek() const {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }
    return this->getHierarchy()->accessRoot()->data.data;
}

template<typename P, typename T>
void BinaryHeap<P, T>::pop() {
    // The queue is empty
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty.");
    }

    // Pop root
    HierarchyType *hierarchy = this->getHierarchy();
    BlockType *current = hierarchy->accessRoot();
    std::swap(current->data, hierarchy->accessLastLeaf()->data);
    hierarchy->removeLastLeaf();

    // Nothing to correct
    if (this->isEmpty()) {
        return;
    }

    // Correct possible violation
    while (true) {
        BlockType *left = hierarchy->accessLeftSon(*current);
        BlockType *right = hierarchy->accessRightSon(*current);
        BlockType *better = left;

        if (left == nullptr) {
            break;
        }

        if (right != nullptr && right->data.priority < left->data.priority) {
            better = right;
        }

        if (better->data.priority >= current->data.priority) {
            break;
        }

        std::swap(current->data, better->data);
        current = better;
    }
}

template<typename P, typename T>
ADT &BinaryHeap<P, T>::assign(const ADT &other) {
    if (this == &other) {
        return *this;
    }

    const BinaryHeap *o = dynamic_cast<const BinaryHeap*>(&other);
    if (o == nullptr) {
        throw utils::type_missmatch();
    }

    this->getHierarchy()->assign(*o->getHierarchy());

    return *this;
}

template<typename P, typename T>
bool BinaryHeap<P, T>::equals(const ADT &other) const {
    (void)other;
    throw utils::type_missmatch();
}

template<typename P, typename T>
typename BinaryHeap<P, T>::HierarchyType*
BinaryHeap<P, T>::getHierarchy() const {
    return dynamic_cast<HierarchyType*>(this->m_memoryStructure);
}


} // namespace ds::adt

#endif

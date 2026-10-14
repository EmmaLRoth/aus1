#ifndef LIBDS_ADT_STACK_HPP
#define LIBDS_ADT_STACK_HPP

#include <libds/adt/abstract_data_type.hpp>
#include <libds/amt/explicit_sequence.hpp>
#include <libds/amt/implicit_sequence.hpp>


namespace ds::adt {


template<typename T>
class Stack : virtual public ADT {
public:
    virtual void push(const T &element) = 0;

    virtual T &peek() = 0;

    virtual const T &peek() const = 0;

    virtual void pop() = 0;
};

//----------

template<typename T>
class ImplicitStack : public Stack<T>, public ADS<T> {
public:
    ImplicitStack();

    ImplicitStack(const ImplicitStack &other);

    void push(const T &element) override;

    T &peek() override;

    const T &peek() const override;

    void pop() override;

    ADT &assign(const ADT &other) override;

    bool equals(const ADT &other) const override;

private:
    amt::ImplicitSequence<T> *getSequence() const;
};

//----------

template<typename T>
class ExplicitStack : public Stack<T>, public ADS<T> {
public:
    ExplicitStack();

    ExplicitStack(const ExplicitStack &other);

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
ImplicitStack<T>::ImplicitStack() : ADS<T>(new amt::ImplicitSequence<T>()) {
}

template<typename T>
ImplicitStack<T>::ImplicitStack(const ImplicitStack &other) :
    ADS<T>(new amt::ImplicitSequence<T>(*other.getSequence())) {
}

template<typename T>
void ImplicitStack<T>::push(const T &element) {
    this->getSequence()->emplaceLast().data = element;
}

template<typename T>
T &ImplicitStack<T>::peek() {
    if (this->isEmpty()) {
        throw std::out_of_range("Stack is empty.");
    }
    return this->getSequence()->accessLast()->data;
}

template<typename T>
const T &ImplicitStack<T>::peek() const {
    if (this->isEmpty()) {
        throw std::out_of_range("Stack is empty.");
    }
    return this->getSequence()->accessLast()->data;
}

template<typename T>
void ImplicitStack<T>::pop() {
    if (this->isEmpty()) {
        throw std::out_of_range("Stack is empty.");
    }
    this->getSequence()->removeLast();
}

template<typename T>
ADT &ImplicitStack<T>::assign(const ADT &other) {
    const ImplicitStack *o = dynamic_cast<const ImplicitStack*>(&other);
    if (o == nullptr) {
        throw utils::type_missmatch();
    }
    this->getSequence()->assign(*o->getSequence());
    return *this;
}

template<typename T>
bool ImplicitStack<T>::equals(const ADT &other) const {
    const ImplicitStack *o = dynamic_cast<const ImplicitStack*>(&other);
    if (o == nullptr) {
        return false;
    }
    return this->getSequence()->equals(*o->getSequence());
}

template<typename T>
amt::IS<T> *ImplicitStack<T>::getSequence() const {
    return dynamic_cast<amt::ImplicitSequence<T>*>(this->m_memoryStructure);
}

//----------

template<typename T>
ExplicitStack<T>::ExplicitStack() : ADS<T>(new amt::SinglyLinkedSequence<T>()) {
}

template<typename T>
ExplicitStack<T>::ExplicitStack(const ExplicitStack &other) :
    ADS<T>(new amt::SinglyLinkedSequence<T>(*other.getSequence())) {
}

template<typename T>
void ExplicitStack<T>::push(const T &element) {
    this->getSequence()->emplaceFirst().data = element;
}

template<typename T>
T &ExplicitStack<T>::peek() {
    if (this->isEmpty()) {
        throw std::out_of_range("Stack is empty.");
    }
    return this->getSequence()->accessFirst()->data;
}

template<typename T>
const T &ExplicitStack<T>::peek() const {
    if (this->isEmpty()) {
        throw std::out_of_range("Stack is empty.");
    }
    return this->getSequence()->accessFirst()->data;
}

template<typename T>
void ExplicitStack<T>::pop() {
    if (this->isEmpty()) {
        throw std::out_of_range("Stack is empty.");
    }
    this->getSequence()->removeFirst();
}

template<typename T>
ADT &ExplicitStack<T>::assign(const ADT &other) {
    const ExplicitStack *o = dynamic_cast<const ExplicitStack*>(&other);
    if (o == nullptr) {
        throw utils::type_missmatch();
    }
    this->getSequence()->assign(*o->getSequence());
    return *this;
}

template<typename T>
bool ExplicitStack<T>::equals(const ADT &other) const {
    const ExplicitStack *o = dynamic_cast<const ExplicitStack*>(&other);
    if (o == nullptr) {
        return false;
    }
    return this->getSequence()->equals(*o->getSequence());
}

template<typename T>
amt::SinglyLS<T> *ExplicitStack<T>::getSequence() const {
    return dynamic_cast<amt::SinglyLS<T> *>(this->m_memoryStructure);
}


} // namespace ds::adt

#endif

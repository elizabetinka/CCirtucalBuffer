#pragma once

#include <iostream>
#include <memory>
#include <limits>
#include <iterator>

template<class T, class Allocator= std::allocator<T>>
class CCirtucalBufferBase;

template<class T, class Allocator = std::allocator<T>>
class Iterator;

template<class T, class Allocator=std::allocator<T>>
class ConstIterator;

template<class T, class Allocator>
class Iterator {
    friend CCirtucalBufferBase<T, Allocator>;
    friend ConstIterator<T, Allocator>;
public:
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef size_t size_type;
    typedef std::random_access_iterator_tag iterator_category;
    typedef Iterator self;

    difference_type operator-(const self& lhs) const {
        if (mass != lhs.mass) {
            return mass + ind_ - lhs.mass - lhs.ind_;
        }
        if (ind_ == lhs.ind_) {
            if (IamBeg == lhs.IamBeg) {
                return 0;
            }
            if (!IamBeg && lhs.IamBeg) {
                return getSize();
            }
            if (IamBeg && !lhs.IamBeg) {
                return -getSize();
            }
        }
        if (ind_ == end_index) {
            size_t pred_ind = end_index;
            if (pred_ind == 0) {
                pred_ind = capacity_;
            }
            --pred_ind;
            return Iterator(capacity_, mass, beg_index, end_index, pred_ind, empty_, true) - lhs + 1;
        }
        if (ind_ >= beg_index && lhs.ind_ >= beg_index) {
            if (ind_ == lhs.ind_ && beg_index == ind_ && beg_index == end_index) {
                if (!IamBeg && lhs.IamBeg) {
                    return getSize();
                }
                if (IamBeg && !lhs.IamBeg) {
                    return -getSize();
                }
                return 0;
            }
            return ind_ - lhs.ind_;
        }
        if (ind_ >= beg_index) {
            return -(lhs.ind_ + capacity_ - ind_);
        }

        return (ind_ + capacity_ - lhs.ind_);

    }

    bool operator==(const self& lhs) const {
        if ((mass + ind_) == (lhs.mass + lhs.ind_)) {
            if (beg_index == end_index && ind_ == beg_index) {
                if (IamBeg == lhs.IamBeg) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return true;
            }
        }
        return false;
    }

    bool operator!=(const self& lhs) const {
        return !(*this == lhs);
    }

    bool operator<(const self& lhs) const {
        return ((lhs - *this) > 0);
    }

    bool operator>(const self& lhs) const {
        return ((*this - lhs) > 0);
    }

    bool operator>=(const self& lhs) const {
        return !(*this < lhs);
    }

    bool operator<=(const self& lhs) const {
        return !(*this > lhs);
    }

    //унарный плюс
    pointer operator+() {
        return mass + ind_;
    }

    reference operator->() {
        return *(mass + ind_);
    }

    virtual reference operator*() {
        return *(mass + ind_);
    }

    //префиксная версия возвращает значение после инкремента
    self& operator++() {
        if (empty_ or capacity_ == 0) {
            return *this;
        }
        if (beg_index >= end_index) {
            ind_ = (ind_ + 1) % capacity_;
        } else {
            ind_ = ind_ + 1;
            if (ind_ > end_index) {
                ind_ = beg_index + end_index - ind_ - 1;
            }
        }
        if (ind_ == end_index) {
            IamBeg = !IamBeg;
        }
        return *this;
    }

    //постфиксная версия возвращает значение до инкремента
    self operator++(int) {
        if (empty_ or capacity_ == 0) {
            return *this;
        }
        Iterator oldValue(capacity_, mass, beg_index, end_index, ind_, empty_);
        ++(*this);
        return oldValue;
    }

    //префиксная версия возвращает значение после инкремента
    self& operator--() {
        if (empty_ or capacity_ == 0) {
            return *this;
        }
        if (ind_ == beg_index && IamBeg) {
            IamBeg = !IamBeg;
            return *this;
        }

        if (beg_index == end_index) {
            if (ind_ == 0) {
                ind_ = capacity_ - 1;
            } else {
                ind_ = ind_ - 1;
            }

            if (ind_ == beg_index) {
                IamBeg = true;
                return *this;
            }

        } else if (beg_index > end_index) {
            if (ind_ == 0) {
                ind_ = capacity_ - 1;
            } else if (ind_ == beg_index) {
                ind_ = end_index;
            } else {
                ind_ = ind_ - 1;
            }
        } else {
            ind_ = ind_ - 1;
            if (ind_ < beg_index) {
                ind_ = end_index - (beg_index - ind_ - 1);
            }
        }
        if (ind_ == end_index) {
            IamBeg = !IamBeg;
        }
        return *this;
    }

    //постфиксная версия возвращает значение до инкремента
    self operator--(int) {
        if (empty_) {
            return *this;
        }
        Iterator oldValue(capacity_, mass, beg_index, end_index, ind_, empty_);
        --(*this);
        return oldValue;
    }

    self& operator+=(difference_type lhs) {
        if (empty_ or capacity_ == 0) {
            return *this;
        }
        if (lhs == 0) {
            return *this;
        }

        lhs = lhs % getSize();
        if (lhs == 0) {
            if (ind_ == end_index && end_index == beg_index) {
                IamBeg = !IamBeg;
            }
            return *this;
        }

        if (beg_index == end_index) {
            ind_ = (ind_ + lhs) % capacity_;
        } else if (beg_index > end_index) {
            ind_ = (ind_ + lhs) % capacity_;
            if (ind_ > end_index && ind_ < beg_index) {
                ind_ = beg_index - end_index + ind_ - 1;
            }
        } else {
            ind_ = ind_ + lhs;
            if (ind_ > end_index) {
                ind_ = beg_index + end_index - ind_ - 1;
            }
        }
        if (ind_ == end_index && end_index == beg_index) {
            IamBeg = !IamBeg;
        }
        return *this;
    }

    self operator+(difference_type lhs) const {
        return Iterator<T>(capacity_, mass, beg_index, end_index, ind_, empty_, IamBeg) += lhs;
    }

    self& operator-=(difference_type lhs) {
        if (empty_ or capacity_ == 0) {
            return *this;
        }

        lhs = lhs % getSize();
        if (lhs == 0) {
            if (ind_ == end_index && end_index == beg_index) {
                IamBeg = !IamBeg;
            }
            return *this;
        }
        if (beg_index == end_index) {
            if (ind_ < lhs) {
                lhs = lhs - ind_;
                ind_ = capacity_ - lhs;
            } else {
                ind_ = ind_ - lhs;
            }
        } else if (beg_index > end_index) {
            if (ind_ < lhs) {
                lhs = lhs - ind_;
                ind_ = capacity_ - lhs;
                if (ind_ < beg_index) {
                    ind_ = end_index - (beg_index - ind_ - 1);
                }
            } else if (ind_ < (beg_index + lhs) && ind_ >= beg_index) {
                lhs = lhs - (ind_ - beg_index);
                lhs = lhs - 1;
                if ((lhs) > end_index) {
                    lhs = lhs - end_index;
                    ind_ = capacity_ - lhs;
                } else {
                    ind_ = end_index - (lhs);
                }

            } else {
                ind_ = ind_ - lhs;
            }
        } else {
            lhs = lhs % (end_index - beg_index);
            if (ind_ < (beg_index + lhs) && ind_ >= beg_index) {
                lhs = lhs - (ind_ - beg_index);
                lhs = lhs - 1;
                ind_ = end_index - lhs;

            } else {
                ind_ = ind_ - lhs;
            }
        }

        if (ind_ == end_index && end_index == beg_index) {
            IamBeg = !IamBeg;
        }

        return *this;
    }


    virtual reference operator[](difference_type idx) const {
        Iterator oldValue(capacity_, mass, beg_index, end_index, ind_, empty_, IamBeg);
        oldValue += idx;
        return *oldValue;
    }

    self& operator=(const self& other) {
        mass = other.mass;
        capacity_ = other.capacity_;
        end_index = other.end_index;
        beg_index = other.beg_index;
        empty_ = other.empty_;
        ind_ = other.ind_;
        return *this;
    }

protected:
    Iterator() = default;

    size_t capacity_;
    size_t end_index;
    size_t beg_index;
    size_t ind_;
    bool empty_;
    pointer mass;
    bool IamBeg = true;

    difference_type getSize() const {
        if (end_index > beg_index) {
            return end_index - beg_index;
        } else if (end_index < beg_index) {
            return capacity_ - beg_index + end_index;
        } else if (empty_) {
            return 0;
        } else {
            return capacity_;
        }
    }

    Iterator(size_t capacity, T* ptr, size_t beg, size_t end, size_t ind, bool empty, bool isBeg) : capacity_(capacity),
                                                                                                    mass(ptr),
                                                                                                    ind_(ind),
                                                                                                    beg_index(beg),
                                                                                                    end_index(end),
                                                                                                    empty_(empty) {
        if (!empty_ && beg_index == end_index) {
            IamBeg = isBeg; // буфер полный и нам важно итератор начальный или конечный
        }
    };
};

template<class T, class Allocator>
class ConstIterator : public Iterator<T, Allocator> {
    friend CCirtucalBufferBase<T, Allocator>;
public:
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef size_t size_type;
    typedef std::random_access_iterator_tag iterator_category;
    typedef ConstIterator self;

    const reference operator*() const {
        return *(this->mass + this->ind_);
    }

    const reference operator[](difference_type idx) const {
        Iterator oldValue(this->capacity_, this->mass, this->beg_index, this->end_index, this->beg_index, this->empty_,
                          this->IamBeg);
        return oldValue[idx];
    }

protected:
    ConstIterator(size_t capacity, T* ptr, size_t beg, size_t end, size_t ind, bool empty, bool isBeg)
            : Iterator<T, Allocator>(
            capacity, ptr, beg, end, ind, empty, isBeg) {}
};

template<class T, class Allocator>
class CCirtucalBufferBase {
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef Iterator<T> iterator;
    typedef ConstIterator<T> const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Allocator allocator_type;
    typedef CCirtucalBufferBase self;
    using Traits = std::allocator_traits<Allocator>;


    size_t size() const {
        if (end_index > beg_index) {
            return end_index - beg_index;
        } else if (end_index < beg_index) {
            return capacity_ - beg_index + end_index;
        } else if (empty_) {
            return 0;
        } else {
            return capacity_;
        }
    }

    size_t max_size() const {
        return allocator_.max_size();
    }

    void reserve(size_type new_cap) {
        if (new_cap > max_size()) {
            return;
        }
        if (new_cap <= capacity_) {
            return;
        }
        size_t sizes = size();
        size_t old_cap = capacity_;
        capacity_ = new_cap;
        T* mass2 = Traits::allocate(allocator_,capacity_);

        for (uint64_t i = 0; i < sizes; i++) {
            Traits::construct(allocator_,mass2+i,(*this)[i]);
        }

        pointer temp = mass;
        uint64_t old_beg = beg_index;
        mass = mass2;
        beg_index = 0;
        end_index = sizes;
        if (end_index == new_cap) {
            end_index = 0;
        }

        if (old_cap != 0) {
            for (uint64_t i = 0; i < sizes; i++) {
                Traits::destroy(allocator_,temp + old_beg);
                old_beg = (old_beg + 1) % old_cap;
            }
            Traits::deallocate(allocator_,temp, old_cap);
        }
    }

    size_t capacity() const {
        return capacity_;
    }

    bool empty() const {
        return empty_;
    }

    void clear() {
        uint64_t j = beg_index;
        for (uint64_t i = 0; i < size(); i++) {
            Traits::destroy(allocator_,mass + j);
            j = (j + 1) % capacity_;
        }
        beg_index = 0;
        end_index = 0;
        empty_ = true;
    }

    void pop_front() {
        if (empty_) {
            return;
        }
        Traits::destroy(allocator_,mass + beg_index);
        beg_index = (beg_index + 1) % capacity_;
        if (beg_index == end_index) {
            empty_ = true;
        }
        return;
    }

    void pop_back() {
        if (empty_) {
            return;
        }
        if (end_index == 0) {
            end_index = capacity_ - 1;
            Traits::destroy(allocator_,mass + end_index);
            return;
        } else {
            end_index = end_index - 1;
        }
        if (beg_index == end_index) {
            empty_ = true;
        }
        Traits::destroy(allocator_,mass + end_index);
        return;
    }

    Iterator<value_type> begin() {
        return Iterator<value_type, allocator_type>(capacity_, mass, beg_index, end_index, beg_index, empty_, true);
    }

    Iterator<value_type> end() {
        return Iterator<value_type, allocator_type>(capacity_, mass, beg_index, end_index, end_index, empty_, false);
    }

    ConstIterator<value_type> cbegin() const {
        return ConstIterator<value_type, allocator_type>(capacity_, mass, beg_index, end_index, beg_index, empty_,
                                                         true);
    }

    ConstIterator<value_type> cend() const {
        return ConstIterator<value_type, allocator_type>(capacity_, mass, beg_index, end_index, end_index, empty_,
                                                         false);
    }

    reverse_iterator rbegin() {
        Iterator<value_type, allocator_type> i(capacity_, mass, beg_index, end_index, end_index, empty_, false);
        return std::reverse_iterator<Iterator<value_type, allocator_type>>(i);
    }

    reverse_iterator rend() {
        Iterator<value_type, allocator_type> i(capacity_, mass, beg_index, end_index, beg_index, empty_, true);
        return std::reverse_iterator<Iterator<value_type, allocator_type>>(i);
    }

    const_reverse_iterator rcbegin() const {
        ConstIterator<value_type, allocator_type> i(capacity_, mass, beg_index, end_index, end_index, empty_, false);
        return std::reverse_iterator<ConstIterator<value_type, allocator_type>>(i);
    }

    const_reverse_iterator rcend() const {
        ConstIterator<value_type, allocator_type> i(capacity_, mass, beg_index, end_index, end_index, empty_, true);
        return std::reverse_iterator<ConstIterator<value_type, allocator_type>>(i);
    }

    value_type& operator[](size_t idx) {
        return *(begin() + idx);
    }

    const T& operator[](size_t idx) const {
        return cbegin()[idx];
    }

    const T& front() const {
        return mass[(beg_index)];
    }

    const T& back() const {
        if (end_index == 0) {
            return mass[capacity_ - 1];
        }
        return mass[end_index - 1];
    }

    self& operator=(const self& other) {
        uint64_t j = beg_index;
        for (uint64_t i = 0; i < size(); i++) {
            allocator_.destroy(mass + j);
            j = (j + 1) % capacity_;
        }
        if (capacity_ > 0) {
            allocator_.deallocate(mass, capacity_);
        }

        capacity_ = other.capacity_;
        end_index = other.end_index;
        beg_index = other.beg_index;
        empty_ = other.empty_;
        allocator_ = other.allocator_;

        if (capacity_ != 0) {
            mass = allocator_.allocate(capacity_);
            for (uint64_t i = 0; i < other.size(); i++) {
                allocator_.construct(mass + i + beg_index, other[i]);
            }
        }
        return *this;
    }

    bool operator==(const self& lhs) const {
        if (empty_ != lhs.empty_ or size() != lhs.size()) {
            return false;
        }
        for (uint64_t i = 0; i < size(); i++) {
            if ((*this)[i] != lhs[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const self& lhs) const {
        return !(*this == lhs);
    }

    void swap(self& lhs) {
        std::swap<self>(*this, lhs);
    }

    iterator erase(iterator q) {
        if (size() == 0) {
            return end();
        }
        if (size() == 1) {
            empty_ = true;
        }
        size_t iter_ind = q.ind_;
        int dist1 = q - begin();
        int dist2 = end() - q - 1;
        if (dist1 <= dist2) {
            while (q > begin()) {
                iterator q_copy = q;
                --q;
                *q_copy = *q;
            }
            Traits::destroy(allocator_,mass + beg_index);
            beg_index = (beg_index + 1) % capacity_;
        } else {
            while (q < end()) {
                iterator q_copy = q;
                ++q;
                *q_copy = *q;
            }
            Traits::destroy(allocator_,mass + end_index);
            if (end_index == 0) {
                end_index = capacity_;
            }
            --end_index;
        }
        iter_ind = (iter_ind + 1) % capacity_;
        return Iterator<value_type>(capacity_, mass, beg_index, end_index, iter_ind, empty_, false);
    }

    iterator erase(iterator q1, iterator q2) {
        int n = q2 - q1;
        if (n > size()) {
            return end();
        }
        if (n == size()) {
            empty_ = true;
        }
        size_t q2_ind = q2.ind_;
        --q2;
        iterator ans = Iterator<value_type>(capacity_, mass, beg_index, end_index, q2_ind, empty_, false);
        int dist1 = q1 - begin();
        int dist2 = end() - q2 - 1;
        if (dist1 <= dist2) {
            for (uint64_t i = 0; i < dist1; i++) {
                --q1;
                *q2 = *q1;
                --q2;
            }

            for (uint64_t i = 0; i < n; i++) {
                Traits::destroy(allocator_,mass + beg_index);
                beg_index = (beg_index + 1) % capacity_;
            }

        } else {
            for (uint64_t i = 0; i < dist2; i++) {
                ++q2;
                *q1 = *q2;
                ++q1;
            }

            for (uint64_t i = 0; i < n; i++) {
                if (end_index == 0) {
                    end_index = capacity_;
                }
                --end_index;
                Traits::destroy(allocator_,mass + end_index);
            }
        }
        return ans;
    }

    CCirtucalBufferBase() : capacity_(0), beg_index(0), end_index(0), empty_(true) {};

    CCirtucalBufferBase(const CCirtucalBufferBase& other) : allocator_(other.allocator_) {
        setСapacity(other.capacity_);
        end_index = other.end_index;
        beg_index = other.beg_index;
        empty_ = other.empty_;
        for (uint64_t i = 0; i < other.size(); i++) {
            Traits::construct(allocator_,mass + (beg_index + i) % capacity_, other[i]);
        }
    }

    CCirtucalBufferBase(size_t n, const T& value) {
        setСapacity(n);
        for (uint64_t i = 0; i < capacity_; i++) {
            Traits::construct(allocator_,mass + i, value);
            empty_ = false;
        }
    }

    template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
    CCirtucalBufferBase(InputIterator first, InputIterator last): capacity_(0), beg_index(0), end_index(0),
                                                                  empty_(true) {
        uint64_t n = std::distance(first, last);
        setСapacity(n);
        empty_ = false;
        for (uint64_t i = 0; i < capacity_; i++) {
            Traits::construct(allocator_,mass + i, *first);
            ++first;
        }
        return;
    }

    CCirtucalBufferBase(const std::initializer_list<value_type>& list) : CCirtucalBufferBase(list.begin(),
                                                                                             list.end()) {}

    CCirtucalBufferBase(size_t capacity) {
        setСapacity(capacity);
    }

    virtual ~CCirtucalBufferBase() {
        uint64_t j = beg_index;
        for (uint64_t i = 0; i < size(); i++) {
            Traits::destroy(allocator_,mass + j);
            j = (j + 1) % capacity_;
        }
        if (capacity_ > 0) {
            Traits::deallocate(allocator_,mass, capacity_);
        }
    }

protected:
    T* mass;
    size_t capacity_; // количество доступных элементов
    size_t end_index; // указывает на следующий индекс после последнего элемента
    size_t beg_index;
    bool empty_;
    Allocator allocator_;

    iterator CreateIterator(size_t capacity, T* ptr, size_t beg, size_t end, size_t ind, bool empty, bool isBeg) {
        return Iterator<value_type, Allocator>(capacity, ptr, beg, end, ind, empty, isBeg);
    }

    const_iterator
    CreateConstIterator(size_t capacity, T* ptr, size_t beg, size_t end, size_t ind, bool empty, bool isBeg) {
        return ConstIterator<value_type, Allocator>(capacity, ptr, beg, end, ind, empty, isBeg);
    }

    // вызывается только в конструкторах
    void setСapacity(size_t capacity) {
        capacity_ = capacity;
        mass = Traits::allocate(allocator_,capacity);
        beg_index = 0;
        end_index = 0;
        empty_ = true;
    }
};

template<class T, class Allocator = std::allocator<T>>
class CCirtucalBuffer : public CCirtucalBufferBase<T, Allocator> {
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef Iterator<T> iterator;
    typedef ConstIterator<T> const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Allocator allocator_type;
    typedef CCirtucalBuffer self;
    using Traits = std::allocator_traits<Allocator>;

    CCirtucalBuffer() : CCirtucalBufferBase<T, Allocator>() {};

    void push_back(const T& value) {
        if (this->capacity_ == 0) {
            return;
        }
        this->allocator_.construct(this->mass + this->end_index, value_type());
        if (!this->empty_ && this->end_index == this->beg_index) {
            Traits::construct(this->allocator_,this->mass + this->beg_index, value);
            this->beg_index++;
            this->end_index++;
            return;
        }

        Traits::construct(this->allocator_,this->mass + this->end_index, value);
        this->end_index = (this->end_index + 1) % this->capacity_;
        this->empty_ = false;
    }

    void push_front(const T& value) {
        if (this->capacity_ == 0) {
            return;
        }

        // при недостатке места последние добавленные в конце не переписываются
        if (!this->empty_ && this->end_index == this->beg_index) {
            return;
        }

        if (this->beg_index == 0) {
            this->beg_index = this->capacity_;
        }
        --this->beg_index;
        Traits::construct(this->allocator_,this->mass + this->beg_index, value);

        this->empty_ = false;
        return;
    }

    void resize(size_type new_size) {
        if (new_size >= this->capacity_) {
            return;
        }
        if (new_size == 0) {
            this->empty_ = true;
        }

        if (new_size <= this->size()) {
            uint64_t old_end = this->end_index;
            uint64_t diff = this->size() - new_size - 1;
            this->end_index = (this->beg_index + new_size) % this->capacity_;
            uint64_t copy_end = this->end_index;

            for (uint64_t i = 0; i < diff; i++) {
                Traits::destroy(this->allocator_,this->mass + copy_end);
                copy_end = (copy_end + 1) % this->capacity_;
            }
            return;
        }

        for (uint64_t i = this->size(); i < new_size; i++) {
            Traits::construct(this->allocator_,this->mass + i, value_type());
            this->end_index++;
        }
        return;
    }

    CCirtucalBuffer(const std::initializer_list<value_type>& list) : CCirtucalBufferBase<T, Allocator>(list) {};

    CCirtucalBuffer(size_t capacity) : CCirtucalBufferBase<T, Allocator>(capacity) {}

    CCirtucalBuffer(const CCirtucalBuffer& other) : CCirtucalBufferBase<T, Allocator>(other) {};

    CCirtucalBuffer(size_t n, const T& value) : CCirtucalBufferBase<T, Allocator>(n, value) {}

    template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
    CCirtucalBuffer(InputIterator first, InputIterator last) : CCirtucalBufferBase<T, Allocator>(first, last) {}

    iterator insert(iterator q, size_t n, const T& value) {
        if (this->capacity_ == 0 || n == 0) {
            return q;
        }
        if ((this->size() + n) > this->capacity_) {
            return q;
        }
        uint64_t sizes = this->size();

        uint64_t ind_ans = 0;
        bool be = false;
        auto iter = this->begin();
        auto iter_end = this->end();

        //пустой
        if (iter == iter_end) {
            for (uint64_t k = 0; k < n; k++) {
                Traits::construct(this->allocator_,this->mass + k, value);

            }
            this->end_index = n % this->capacity_;
            this->beg_index = 0;
            this->empty_ = false;
            ind_ans = 0;
            return this->begin();
        }

        // вставка в конец
        if (q == iter_end) {
            for (uint64_t k = 0; k < n; k++) {
                Traits::construct(this->allocator_,this->mass + (k + this->end_index) % this->capacity_, value);
            }
            ind_ans = this->end_index;
            this->end_index = (this->end_index + n) % this->capacity_;
            return this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, ind_ans,
                                        this->empty_, true);
        }

        pointer mass2 = Traits::allocate(this->allocator_,this->capacity_);
        uint64_t j = 0;
        while (iter != iter_end) {
            if (iter == q) {
                for (uint64_t k = 0; k < n; k++) {
                    Traits::construct(this->allocator_,mass2 + j + k, value);
                }
                be = true;
                ind_ans = j;
                j = j + n;
                Traits::construct(this->allocator_,mass2 + j, *iter);
                ++iter;
                ++j;
            } else {
                Traits::construct(this->allocator_,mass2 + j, *iter);
                ++iter;
                ++j;
            }
        }


        if (!be) {
            for (uint64_t i = 0; i < j; i++) {
                Traits::destroy(this->allocator_,mass2 + i);
            }
            Traits::deallocate(this->allocator_, mass2,this->capacity_);
            return q;
        }
        this->end_index = j % this->capacity_;
        uint64_t old_beg = this->beg_index;
        this->beg_index = 0;
        pointer temp = this->mass;
        this->mass = mass2;

        j = old_beg;
        for (uint64_t i = 0; i < sizes; i++) {
            Traits::destroy(this->allocator_,temp + j);
            j = (j + 1) % this->capacity_;
        }
        Traits::deallocate(this->allocator_,temp, this->capacity_);
        return this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, ind_ans,
                                    this->empty_, true);
    }

    iterator insert(iterator q, const T& value) {
        return insert(q, 1, value);
    }

    template<typename _InputIterator, typename = std::_RequireInputIter<_InputIterator>>
    iterator insert(iterator q, _InputIterator q1, _InputIterator q2) {
        if (this->capacity_ == 0) {
            return q;
        }
        size_t n = std::distance(q1, q2);
        size_t sizes = this->size();

        if ((this->size() + n) > this->capacity_) {
            return q;
        }

        uint64_t ind_ans = 0;
        bool be = false;
        auto iter = this->begin();
        auto iter_end = this->end();

        // пустой
        if (iter == iter_end) {
            for (uint64_t k = 0; k < n; k++) {
                Traits::construct(this->allocator_,this->mass + k, *q1);
                ++q1;
            }
            this->beg_index = 0;
            this->end_index = n % this->capacity_;
            this->empty_ = false;
            ind_ans = 0;
            return this->begin();
        }
        // вставка в конец
        if (q == iter_end) {
            for (uint64_t k = 0; k < n; k++) {
                Traits::construct(this->allocator_,this->mass + (k + this->end_index) % this->capacity_, *q1);
                ++q1;
            }
            ind_ans = this->end_index;
            this->end_index = (this->end_index + n) % this->capacity_;
            return this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, ind_ans,
                                        this->empty_, true);
        }
        pointer mass2 = Traits::allocate(this->allocator_,this->capacity_);
        uint64_t j = 0;
        while (iter != iter_end) {
            if (iter == q) {
                for (uint64_t k = 0; k < n; k++) {
                    Traits::construct(this->allocator_,mass2 + j + k, *q1);
                    ++q1;
                }
                be = true;
                ind_ans = j;
                j = j + n;
                Traits::construct(this->allocator_,mass2 + j, *iter);
                ++iter;
                ++j;
            } else {
                Traits::construct(this->allocator_,mass2 + j, *iter);
                ++iter;
                ++j;
            }
        }


        if (!be) {
            for (uint64_t i = 0; i < j; i++) {
                Traits::destroy(this->allocator_,mass2 + j);
            }
            Traits::deallocate(this->allocator_,mass2, this->capacity_);
            return q;
        }
        this->end_index = j % this->capacity_;
        size_t old_beg = this->beg_index;
        this->beg_index = 0;
        pointer temp = this->mass;
        this->mass = mass2;

        j = old_beg;
        for (uint64_t i = 0; i < sizes; i++) {
            Traits::destroy(this->allocator_,temp + j);
            j = (j + 1) % this->capacity_;
        }
        Traits::deallocate(this->allocator_,temp, this->capacity_);
        return this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, ind_ans,
                                    this->empty_, true);
    }

    iterator insert(iterator q, const std::initializer_list<value_type>& list) {
        return insert(q, list.begin(), list.end());
    }

    void assign(size_t n, const T& value) {
        size_t j = this->beg_index;
        for (uint64_t i = 0; i < this->size(); i++) {
            Traits::destroy(this->allocator_,this->mass + j);
            j = (j + 1) % this->capacity_;
        }
        //запишет сколько сможет
        if (n >= this->capacity_) {
            this->end_index = 0;
        } else {
            this->end_index = n;
        }
        this->beg_index = 0;
        this->empty_ = false;
        for (uint64_t i = 0; i < n; i++) {
            if (i >= this->capacity_) {
                return;
            }
            Traits::construct(this->allocator_,this->mass + i, value);
            //this->mass[i] = value;
        }
        return;
    }

    template<typename _InputIterator, typename = std::_RequireInputIter<_InputIterator>>
    void assign(_InputIterator q1, _InputIterator q2) {
        size_t j = this->beg_index;
        for (uint64_t i = 0; i < this->size(); i++) {
            Traits::destroy(this->allocator_,this->mass + j);
            j = (j + 1) % this->capacity_;
        }
        this->beg_index = 0;
        this->empty_ = false;
        uint64_t i = 0;
        while (q1 != q2) {
            Traits::construct(this->allocator_,this->mass + i, *q1);
            ++q1;
            i++;
            if (i >= this->capacity_) { // заменяет сколько может
                return;
            }
        }
        this->end_index = i % this->capacity_;
        return;
    }

    void assign(const std::initializer_list<value_type>& list) {
        assign(list.begin(), list.end());
    }

    template<typename... _Args>
    iterator emplace(iterator position, _Args&& ... __args) {
        return insert(position, T(std::forward<_Args>(__args)...));
    }

    template<typename... _Args>
    void emplace_front(_Args&& ... __args) {
        push_front(T(std::forward<_Args>(__args)...));
    }

    template<typename... _Args>
    void emplace_back(_Args&& ... __args) {
        push_back(T(std::forward<_Args>(__args)...));
    }

    ~CCirtucalBuffer() = default;
};

template<class T, class Allocator = std::allocator<T>>
class CCirtucalBufferExt : public CCirtucalBufferBase<T, Allocator> {
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef Iterator<T> iterator;
    typedef ConstIterator<T> const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Allocator allocator_type;
    typedef CCirtucalBufferExt self;
    using Traits = std::allocator_traits<Allocator>;

    CCirtucalBufferExt() : CCirtucalBufferBase<T, Allocator>() {};

    CCirtucalBufferExt(const CCirtucalBufferExt& other) : CCirtucalBufferBase<T, Allocator>(other) {};


    CCirtucalBufferExt(size_t capacity) : CCirtucalBufferBase<T, Allocator>(capacity) {};

    CCirtucalBufferExt(size_t n, const T& value) : CCirtucalBufferBase<T, Allocator>(n, value) {};

    template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
    CCirtucalBufferExt(InputIterator first, InputIterator last):CCirtucalBufferBase<T, Allocator>(first, last) {};

    CCirtucalBufferExt(const std::initializer_list<value_type>& list) : CCirtucalBufferBase<T, Allocator>(list) {};

    void resize(size_type new_size) {
        if (new_size > this->capacity_) {
            this->reserve(new_size);
        }

        if (new_size == 0) {
            this->empty_ = true;
        }

        if (new_size <= this->size()) {
            uint64_t old_end = this->end_index;
            uint64_t diff = this->size() - new_size - 1;
            this->end_index = (this->beg_index + new_size) % this->capacity_;
            uint64_t copy_end = this->end_index;

            for (uint64_t i = 0; i < diff; i++) {
                Traits::destroy(this->allocator_,this->mass + copy_end);
                copy_end = (copy_end + 1) % this->capacity_;
            }
            return;
        }
        for (uint64_t i = this->size(); i < new_size; i++) {
            Traits::construct(this->allocator_,this->mass + i, value_type());
            this->end_index++;
        }

        return;
    }

    void push_back(const T& value) {
        if (this->size() == this->capacity_) {
            if (this->capacity_ == 0) {
                this->reserve(1);
            } else {
                this->reserve(2 * this->capacity_);
            }
        }
        Traits::construct(this->allocator_,this->mass + this->end_index, value);
        this->end_index = (this->end_index + 1) % this->capacity_;
        this->empty_ = false;
    }

    void push_front(const T& value) {
        if (this->size() == this->capacity_) {
            if (this->capacity_ == 0) {
                this->reserve2(2, false);
            } else {
                this->reserve2(2 * this->capacity_, false);
            }
        }

        if (this->beg_index == 0) {
            this->beg_index = this->capacity_;
        }
        --this->beg_index;
        Traits::construct(this->allocator_,this->mass + this->beg_index, value);
        this->empty_ = false;
        return;
    }

    iterator insert(iterator q, size_t n, const T& value) {
        auto iterF = this->begin();
        uint64_t i = 0;
        while (iterF != q) {
            if (iterF == this->end()) {
                return q; //нет такого итератора
            }
            ++i;
            ++iterF;
        }
        size_t Iter_ind = i;

        size_t sizes = this->size();
        if ((sizes + n) > this->capacity_) {
            this->reserve(std::max(2 * this->capacity_, sizes + n));
        }

        uint64_t ind_ans = 0;
        bool be = false;
        auto iter = this->begin();
        auto iter_end = this->end();
        // из-за reserve итератор мог стать невалидным
        q = this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, Iter_ind,
                                 this->empty_, true);

        //пустой
        if (iter == iter_end) {
            for (uint64_t k = 0; k < n; k++) {
                Traits::construct(this->allocator_,this->mass + k, value);
                //this->mass[k] = value;
            }
            this->end_index = n % this->capacity_;
            this->beg_index = 0;
            this->empty_ = false;
            ind_ans = 0;
            return this->begin();
            //return this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, ind_ans,this->empty_, true);
        }

        // вставка в конец
        if (q == iter_end) {
            for (uint64_t k = 0; k < n; k++) {
                Traits::construct(this->allocator_,this->mass + (k + this->end_index) % this->capacity_, value);
                //this->mass[(k + this->end_index) % this->capacity_] = value;
            }
            ind_ans = this->end_index;
            this->end_index = (this->end_index + n) % this->capacity_;
            return this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, ind_ans,
                                        this->empty_, true);
        }

        pointer mass2 = this->allocator_.allocate(this->capacity_);
        uint64_t j = 0;
        while (iter != iter_end) {
            if (iter == q) {
                for (uint64_t k = 0; k < n; k++) {
                    Traits::construct(this->allocator_,mass2 + j + k, value);
                    //mass2[j + k] = value;
                }
                be = true;
                ind_ans = j;
                j = j + n;
                Traits::construct(this->allocator_,mass2 + j, *iter);
                //mass2[j] = *iter;
                ++iter;
                ++j;
            } else {
                Traits::construct(this->allocator_,mass2 + j, *iter);
                //mass2[j] = *iter;
                ++iter;
                ++j;
            }
        }


        if (!be) {
            for (uint64_t i = 0; i < j; i++) {
                Traits::destroy(this->allocator_,mass2 + j);
            }
            Traits::deallocate(this->allocator_,mass2, this->capacity_);
            return q;
        }
        this->end_index = j % this->capacity_;
        size_t old_beg = this->beg_index;
        this->beg_index = 0;
        pointer temp = this->mass;
        this->mass = mass2;

        j = old_beg;
        for (uint64_t i = 0; i < sizes; i++) {
            Traits::destroy(this->allocator_,temp + j);
            j = (j + 1) % this->capacity_;
        }
        Traits::deallocate(this->allocator_,temp, this->capacity_);

        return this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, ind_ans,
                                    this->empty_, true);
    }

    iterator insert(iterator q, const T& value) {
        return insert(q, 1, value);
    }

    template<typename _InputIterator, typename = std::_RequireInputIter<_InputIterator>>
    iterator insert(iterator q, _InputIterator q1, _InputIterator q2) {
        size_t n = std::distance(q1, q2);

        auto iterF = this->begin();
        uint64_t i = 0;
        while (iterF != q) {
            if (iterF == this->end()) {
                return q; //нет такого итератора
            }
            ++i;
            ++iterF;
        }
        size_t Iter_ind = i;

        size_t sizes = this->size();

        if ((sizes + n) > this->capacity_) {
            this->reserve(std::max(2 * this->capacity_, sizes + n));
        }

        uint64_t ind_ans = 0;
        bool be = false;
        auto iter = this->begin();
        auto iter_end = this->end();
        q = this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, Iter_ind,
                                 this->empty_, true);
        // пустой
        if (iter == iter_end) {
            for (uint64_t k = 0; k < n; k++) {
                Traits::construct(this->allocator_,this->mass + k, *q1);
                //this->mass[k] = *q1;
                ++q1;
            }
            this->beg_index = 0;
            this->end_index = n % this->capacity_;
            this->empty_ = false;
            ind_ans = 0;
            return this->begin();
        }
        // вставка в конец
        if (q == iter_end) {
            for (uint64_t k = 0; k < n; k++) {
                Traits::construct(this->allocator_,this->mass + (k + this->end_index) % this->capacity_, *q1);
                ++q1;
            }
            ind_ans = this->end_index;
            this->end_index = (this->end_index + n) % this->capacity_;
            return this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, ind_ans,
                                        this->empty_, true);
        }
        pointer mass2 = Traits::allocate(this->allocator_,this->capacity_);
        uint64_t j = 0;
        while (iter != iter_end) {
            if (iter == q) {
                for (uint64_t k = 0; k < n; k++) {
                    Traits::construct(this->allocator_,mass2 + j + k, *q1);
                    ++q1;
                }
                be = true;
                ind_ans = j;
                j = j + n;
                Traits::construct(this->allocator_,mass2 + j, *iter);
                ++iter;
                ++j;
            } else {
                Traits::construct(this->allocator_,mass2 + j, *iter);
                ++iter;
                ++j;
            }
        }


        if (!be) {
            for (uint64_t i = 0; i < j; i++) {
                Traits::destroy(this->allocator_,mass2 + j);
            }
            Traits::deallocate(this->allocator_,mass2, this->capacity_);
            return q;
        }
        this->end_index = j % this->capacity_;
        size_t old_beg = this->beg_index;
        this->beg_index = 0;
        pointer temp = this->mass;
        this->mass = mass2;

        j = old_beg;
        for (uint64_t i = 0; i < sizes; i++) {
            Traits::destroy(this->allocator_,temp + j);
            j = (j + 1) % this->capacity_;
        }
        Traits::deallocate(this->allocator_,temp, this->capacity_);
        return this->CreateIterator(this->capacity_, this->mass, this->beg_index, this->end_index, ind_ans,
                                    this->empty_, true);
    }

    iterator insert(iterator q, const std::initializer_list<value_type>& list) {
        return insert(q, list.begin(), list.end());
    }

    void assign(size_t n, const T& value) {
        if (n > this->capacity_) {
            this->reserve(std::max(this->capacity_ * 2, n));
        }
        this->end_index = n;
        if (this->capacity_ == n) {
            this->end_index = 0;
        }
        this->beg_index = 0;
        this->empty_ = false;
        for (uint64_t i = 0; i < n; i++) {
            Traits::construct(this->allocator_,this->mass + i, value);
        }
        return;
    }

    template<typename _InputIterator, typename = std::_RequireInputIter<_InputIterator>>
    void assign(_InputIterator q1, _InputIterator q2) {
        this->empty_ = false;
        size_t n = std::distance(q1, q2);

        if (n > this->capacity_) {
            this->reserve(std::max(this->capacity_ * 2, n));
        }
        this->end_index = n;
        if (this->capacity_ == n) {
            this->end_index = 0;
        }
        for (uint64_t i = 0; i < n; i++) {
            Traits::construct(this->allocator_,this->mass + i, *q1);
            this->mass[i] = *q1;
            ++q1;
        }
        return;
    }

    void assign(const std::initializer_list<value_type>& list) {
        assign(list.begin(), list.end());
    }

    template<typename... _Args>
    iterator emplace(iterator position, _Args&& ... __args) {
        return insert(position, T(std::forward<_Args>(__args)...));
    }

    template<typename... _Args>
    void emplace_front(_Args&& ... __args) {
        push_front(T(std::forward<_Args>(__args)...));
    }

    template<typename... _Args>
    void emplace_back(_Args&& ... __args) {
        push_back(T(std::forward<_Args>(__args)...));
    }

    ~CCirtucalBufferExt() = default;

private:
    // только для увеличения при добавлении в начало
    void reserve2(size_type new_cap, bool norm = true) {
        if (new_cap > this->max_size()) {
            return;
        }
        if (new_cap <= this->capacity_) {
            return;
        }
        size_t sizes = this->size();
        size_t old_cap = this->capacity_;
        this->capacity_ = new_cap;
        T* mass2 = Traits::allocate(this->allocator_,this->capacity_);

        for (uint64_t i = 0; i < sizes; i++) {
            Traits::construct(this->allocator_,mass2 + i + 1, (*this)[i]);
        }

        pointer temp = this->mass;
        this->mass = mass2;
        uint64_t old_beg = this->beg_index;
        this->beg_index = 1;
        this->end_index = sizes + 1;
        if (this->end_index == new_cap) {
            this->end_index = 0;
        }
        if (old_cap != 0) {
            for (uint64_t i = 0; i < sizes; i++) {
                Traits::destroy(this->allocator_,temp + old_beg);
                old_beg = (old_beg + 1) % old_cap;
            }
            Traits::deallocate(this->allocator_,temp, old_cap);
        }
    }
};

template<class T>
void swap(CCirtucalBufferBase<T>& lhs, CCirtucalBufferBase<T>& rhs) {
    lhs.swap(rhs);
}

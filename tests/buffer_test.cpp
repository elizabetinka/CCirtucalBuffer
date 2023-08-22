#include <lib/CCirtucalBuffer.h>
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

class Myclass {
public:
    Myclass() = default;

    Myclass(int myClassInt_, const std::string& myClassString_, const std::initializer_list<double>& myClassDoubles_)
            : myClassInt(myClassInt_), myClassString(myClassString_), myClassDoubles(myClassDoubles_) {};
    int myClassInt;
    std::string myClassString;
    std::vector<double> myClassDoubles;

    Myclass& operator=(const Myclass& other) = default;

    Myclass(const Myclass& other) : myClassInt(other.myClassInt), myClassString(other.myClassString),
                                    myClassDoubles(other.myClassDoubles) {};


    bool operator==(Myclass& lhs) const {
        return myClassInt == lhs.myClassInt && myClassString == lhs.myClassString &&
               myClassDoubles == lhs.myClassDoubles;
    }

    ~Myclass() = default;
};

template<class T>
class InpIter {
public:
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef size_t size_type;
    typedef std::input_iterator_tag iterator_category;
    typedef InpIter self;

    InpIter(pointer p) : mass(p) {}

    bool operator==(const self& lhs) const {
        return mass == lhs.mass;
    }

    bool operator!=(const self& lhs) const {
        return !(*this == lhs);
    }

    reference operator->() { //pointer?
        return *mass;
    }

    reference operator*() {
        return *mass;
    }

    //префиксная версия возвращает значение после инкремента
    self& operator++() {
        ++mass;
        return *this;
    }

    //постфиксная версия возвращает значение до инкремента
    self operator++(int) {
        InpIter oldValue(mass);
        ++(*this);
        return oldValue;
    }

protected:
    pointer mass;
};

TEST(CCirtucalBufferTestSuite, EmptyTest) {
    CCirtucalBuffer<int> bufer;

    ASSERT_TRUE(bufer.empty());
    ASSERT_TRUE(bufer.begin() == bufer.end());
    bufer.push_back(32); // проверка на то, что программа не упадет
    bufer.pop_front(); // проверка на то, что программа не упадет
    ASSERT_TRUE(bufer.empty()); //проверка что ничего нее добавилось

    bufer.reserve(10);
    bufer.push_back(32);
    ASSERT_FALSE(bufer.empty());
    bufer.clear();
    ASSERT_TRUE(bufer.empty());
}

TEST(CCirtucalBufferTestSuite, ConstructorTest) {
    CCirtucalBuffer<int> bufer{1, 1, 1, 1, 1, 1};
    std::vector<int> bufer_copy{1, 1, 1, 1, 1, 1, 1, 1};

    int buf_c[6] = {1, 1, 1, 1, 1, 1};

    CCirtucalBuffer<int> anotherBufer(bufer);
    CCirtucalBuffer<int> anotherBufer2(6, 1);
    CCirtucalBuffer<int> anotherBufer3(bufer.begin(), bufer.end());
    CCirtucalBuffer<int> anotherBufer4(anotherBufer2.begin(), anotherBufer2.end());
    CCirtucalBuffer<int> anotherBufer5(bufer_copy.begin() + 1, bufer_copy.end() - 1);
    CCirtucalBuffer<int> anotherBufer6(InpIter<int>(buf_c), InpIter<int>(buf_c + 6));

    InpIter<int> a(buf_c);
    InpIter<int> b(buf_c + 6);

    ASSERT_TRUE(std::distance(a, b) == std::distance(a, b));
    ASSERT_FALSE(a == b);


    ASSERT_TRUE((anotherBufer2.end() - anotherBufer2.begin()) == 6);
    ASSERT_TRUE(bufer == anotherBufer);
    ASSERT_TRUE(bufer == anotherBufer2);
    ASSERT_TRUE(bufer == anotherBufer3);
    ASSERT_TRUE(bufer == anotherBufer4);
    ASSERT_TRUE(bufer == anotherBufer5);
    ASSERT_TRUE(bufer == anotherBufer6);
}

TEST(CCirtucalBufferTestSuite, EqualityTest) {
    CCirtucalBuffer<int> bufer;
    CCirtucalBuffer<int> bufer2;
    ASSERT_TRUE(bufer == bufer);
    ASSERT_TRUE(bufer == bufer2 && bufer2 == bufer);

    CCirtucalBuffer<int> bufer3(5);
    bufer3.push_back(100);
    ASSERT_FALSE(bufer == bufer3);
    ASSERT_TRUE(bufer != bufer3);
}

TEST(CCirtucalBufferTestSuite, IteratorTest) {
    CCirtucalBuffer<int> bufer2_{1, 2, 3, 4, 5, 6, 6, 8};
    const CCirtucalBuffer<int> answer{1, 2, 100, 4, 5, 6, 6, 8};

    ASSERT_TRUE(std::distance(bufer2_.begin(), bufer2_.end()) == bufer2_.size());
    ASSERT_TRUE(bufer2_.begin() < bufer2_.end());

    Iterator<int> first_elem = bufer2_.begin();
    uint64_t i = 0;
    while (first_elem < bufer2_.end()) {
        ASSERT_TRUE(*first_elem == answer[i]);
        ++i;
        ++first_elem;
        if (i == 2) {
            *first_elem = 100;
        }
    }


    i = 0;
    for (int j: bufer2_) {
        ASSERT_TRUE(j == bufer2_[i]);
        ++i;
    }

    bufer2_.push_back(100);
    ASSERT_TRUE(bufer2_.begin() < bufer2_.end()); // тут поменялись местами итераторы
    ASSERT_TRUE(bufer2_[8] == bufer2_[0] && bufer2_[0] == 2); // тут перезаписалось первое значение
    ASSERT_TRUE(*(--bufer2_.end()) == 100);
    ASSERT_TRUE(bufer2_.back() == 100);

}

TEST(CCirtucalBufferTestSuite, constIteratorTest) {
    const CCirtucalBuffer<int> bufer{1, 2, 3, 4, 5, 6, 6, 8};

    std::vector<int> diff_bufer{1, 2, 3, 4, 5, 6, 6, 8}; // для сравнения

    ConstIterator<int> first_elem = bufer.cbegin();

    uint64_t i = 0; // для сравнения
    while (first_elem < bufer.cend()) {
        ASSERT_TRUE(*first_elem == diff_bufer[i]);
        ++i;
        ++first_elem;
    }

    for (i = 0; i < bufer.size(); ++i) {
        ASSERT_TRUE(bufer[i] == diff_bufer[i]);
    }

    ASSERT_TRUE(bufer.cbegin() < bufer.cend());
}

TEST(CCirtucalBufferTestSuite, ReverseIteratorTest) {
    CCirtucalBuffer<int> bufer{1, 2, 3, 4, 5, 6, 6, 8};
    const CCirtucalBuffer<int> buferConst{8, 1, 3, 7, 5, 9, 6, 9};

    auto first_elem = bufer.rbegin();
    auto first_elem_const = buferConst.rcbegin();

    uint64_t i = bufer.size() - 1;
    while (first_elem < bufer.rend()) {
        ASSERT_TRUE(*first_elem == bufer[i] && buferConst[i] == *first_elem_const);
        ++first_elem;
        ++first_elem_const;
        --i;
    }
}

TEST(CCirtucalBufferTestSuite, PushPopTest) {
    CCirtucalBuffer<int> bufer2_{1, 2, 3, 4, 5, 6, 6, 8};

    bufer2_.push_front(199); // проверка на то, что в начало не вставляется, если не хватает capacity
    ASSERT_TRUE(bufer2_.back() == 8);
    ASSERT_TRUE(bufer2_.front() == 1);

    bufer2_.push_back(20000); // проверка на то, что если не хватает capacity перезаписывается начало
    ASSERT_TRUE(bufer2_.back() == 20000);
    ASSERT_TRUE(bufer2_.front() == 2);

    bufer2_.pop_front();
    ASSERT_TRUE(bufer2_.front() == 3);
    ASSERT_TRUE(bufer2_.back() == 20000);

    bufer2_.pop_back();
    ASSERT_TRUE(bufer2_.back() == 8);
    ASSERT_TRUE(bufer2_.front() == 3);

    bufer2_.push_front(199);
    ASSERT_TRUE(bufer2_.back() == 8);
    ASSERT_TRUE(bufer2_.front() == 199);
}

TEST(CCirtucalBufferTestSuite, ReservResizeTest) {
    CCirtucalBuffer<int> bufer2_{1, 2, 3, 4, 5, 6, 6, 8};

    bufer2_.reserve(10); // new_cap>cur_cap
    bufer2_.push_back(666);
    bufer2_.push_back(999);

    ASSERT_TRUE(bufer2_.back() == 999);
    ASSERT_TRUE(bufer2_.front() == 1);

    bufer2_.resize(8);
    ASSERT_TRUE(bufer2_.back() == 8);
    ASSERT_TRUE(bufer2_.front() == 1);

    bufer2_.reserve(bufer2_.max_size() + 1); // проверка на то, что программа не упадет
}

TEST(CCirtucalBufferTestSuite, SwapTest) {
    const CCirtucalBuffer<int> bufer{1, 2, 3, 4, 5};
    const CCirtucalBuffer<int> anotherBufer{333, 666, 999};

    CCirtucalBuffer<int> bufer2(bufer);
    CCirtucalBuffer<int> anotherBufer2(anotherBufer);

    bufer2.swap(anotherBufer2);
    ASSERT_TRUE(bufer2 == anotherBufer && anotherBufer2 == bufer);

    swap(bufer2, anotherBufer2);
    ASSERT_TRUE(bufer2 == bufer && anotherBufer2 == anotherBufer);
}

TEST(CCirtucalBufferTestSuite, EraseTest) {
    CCirtucalBuffer<int> bufer1{1, 2, 3, 4, 5, 6, 6, 8};
    CCirtucalBuffer<int> bufer2{1, 2, 3, 5, 6, 6, 8};

    auto iter5 = bufer1.erase(bufer1.begin() + 3);

    ASSERT_TRUE(bufer1 == bufer2);
    ASSERT_TRUE(*iter5 == 5);

    CCirtucalBuffer<int> bufer3{1, 2, 6, 6, 8};
    auto iter6 = bufer1.erase(bufer1.begin() + 2, bufer1.begin() + 4);


    ASSERT_TRUE(bufer1 == bufer3);
    ASSERT_TRUE(*iter6 == 6);

    bufer3.pop_front();
    iter5 = bufer1.erase(bufer1.begin());
    ASSERT_TRUE(bufer1 == bufer3);
    ASSERT_TRUE(*iter5 == 2);

    bufer1.erase(bufer1.begin(), bufer1.end());
    ASSERT_TRUE(bufer1.empty());
}

TEST(CCirtucalBufferTestSuite, AssignTest) {
    CCirtucalBuffer<int> bufer2{1, 2, 3, 888, 9, 7};
    CCirtucalBuffer<int> bufer(5, 20);

    bufer2.assign(5, 20);
    ASSERT_TRUE(bufer2 == bufer);

    CCirtucalBuffer<int> bufer3{0, 0, 34};
    bufer2.assign(bufer3.begin(), bufer3.end());
    ASSERT_TRUE(bufer2 == bufer3);

    std::vector<int> buf3{13, 2};
    bufer2.assign(buf3.begin(), buf3.end());
    for (uint64_t i = 0; i < bufer2.size(); i++) {
        ASSERT_TRUE(bufer2[i] == buf3[i]);
    }

    bufer2.clear();
    bufer2.assign({1, 4, 5, 700});
    CCirtucalBuffer<int> bufer5{1, 4, 5, 700};
    ASSERT_TRUE(bufer2 == bufer5);

}

TEST(CCirtucalBufferTestSuite, InsertTest) {
    CCirtucalBuffer<int> mass{1, 2, 3, 6, 4};
    mass.reserve(10);

    const CCirtucalBuffer<int> answer{1, 2, 10000, 3, 6, 4};
    auto it = mass.insert(mass.begin() + 2, 10000);
    ASSERT_TRUE(mass == answer);
    ASSERT_TRUE(*it = 10000);

    const CCirtucalBuffer<int> answer2({1, 2, 10000, 8, 8, 8, 3, 6, 4});
    it = mass.insert(mass.begin() + 3, 3, 8);
    ASSERT_TRUE(mass == answer2);
    ASSERT_TRUE(*it = 8);

    it = mass.insert(mass.begin() + 3, 3, 7); // закончилось место
    ASSERT_TRUE(mass == answer2);
    ASSERT_TRUE(*it = 8);

    mass.reserve(100);

    std::vector<int> another_bufer{444, 666};
    const CCirtucalBuffer<int> answer3({1, 2, 10000, 8, 444, 666, 8, 8, 3, 6, 4});
    it = mass.insert(mass.begin() + 4, another_bufer.begin(), another_bufer.end());
    ASSERT_TRUE(mass == answer3);
    ASSERT_TRUE(*it == 444);

    const CCirtucalBuffer<int> answer4({1, 2, 10000, 8, 444, 666, 8, 8, 3, 6, 4, 18, 33});
    it = mass.insert(mass.end(), {18, 33});
    ASSERT_TRUE(mass == answer4);
    ASSERT_TRUE(*it == 18);

    const CCirtucalBuffer<int> answer5({11, 13, 1, 2, 10000, 8, 444, 666, 8, 8, 3, 6, 4, 18, 33});
    it = mass.insert(mass.begin(), {11, 13});
    ASSERT_TRUE(mass == answer5);
    ASSERT_TRUE(*it == 11);
}

TEST(CCirtucalBufferTestSuite, EmplaceTest) {
    CCirtucalBuffer<Myclass> mass;
    mass.reserve(10);

    Myclass first(10, "Liza", {2.0, 3.0, 5.0});
    Myclass second(22, "Sasha", {10.4, 30.5, 80.0});
    Myclass third(1, "Masha", {3.4, 1.5, 9.0});
    Myclass fouth(1000, "Kolya", {7.4, 30.4, 66.6});

    mass.push_back(first);

    std::initializer_list<double> list{10.4, 30.5, 80.0};
    mass.emplace(mass.begin(), 22, "Sasha", list);
    ASSERT_TRUE(mass[0] == second);

    list = {3.4, 1.5, 9.0};
    mass.emplace_back(1, "Masha", list);
    ASSERT_TRUE(mass.back() == third);

    list = {7.4, 30.4, 66.6};
    mass.emplace_front(1000, "Kolya", list);
    ASSERT_TRUE(mass.front() == fouth);
}

TEST(CCirtucalBufferExtTestSuite, EmptyTest) {
    CCirtucalBufferExt<std::string> bufer;

    bufer.pop_front(); // проверка на то, что программа не упадет
    bufer.pop_back(); // проверка на то, что программа не упадет

    ASSERT_TRUE(bufer.empty());
    ASSERT_TRUE(bufer.begin() == bufer.end());

    bufer.push_back("Liza");
    ASSERT_FALSE(bufer.empty());
    bufer.clear();
    ASSERT_TRUE(bufer.empty());

    bufer.push_front("Liza");
    ASSERT_FALSE(bufer.empty());
    bufer.clear();
    ASSERT_TRUE(bufer.empty());
}

TEST(CCirtucalBufferExtTestSuite, ConstructorTest) {
    CCirtucalBufferExt<int> bufer{1, 1, 1, 1, 1, 1};
    CCirtucalBufferExt<int> anotherBufer(bufer);
    CCirtucalBufferExt<int> anotherBufer2(6, 1);
    CCirtucalBufferExt<int> anotherBufer3(bufer.begin(), bufer.end());
    CCirtucalBufferExt<int> anotherBufer4(anotherBufer2.begin(), anotherBufer2.end());

    ASSERT_TRUE((anotherBufer2.end() - anotherBufer2.begin()) == 6);
    ASSERT_TRUE(bufer == anotherBufer);
    ASSERT_TRUE(bufer == anotherBufer2);
    ASSERT_TRUE(bufer == anotherBufer3);
    ASSERT_TRUE(bufer == anotherBufer4);
}

TEST(CCirtucalBufferExtTestSuite, constIteratorTest) {
    const CCirtucalBufferExt<int> bufer{1, 2, 3, 4, 5, 6, 6, 8};
    std::vector<int> diff_bufer{1, 2, 3, 4, 5, 6, 6, 8}; // для сравнения

    auto first_elem = bufer.cbegin();

    uint64_t i = 0; // для сравнения
    while (first_elem < bufer.cend()) {
        ASSERT_TRUE(*first_elem == diff_bufer[i]);
        ++i;
        ++first_elem;
    }

    for (i = 0; i < bufer.size(); ++i) {
        ASSERT_TRUE(bufer[i] == diff_bufer[i]);
    }

    ASSERT_TRUE(bufer.cbegin() < bufer.cend());
}

TEST(CCirtucalBufferExtTestSuite, SwapTest) {
    const CCirtucalBufferExt<int> bufer{1, 2, 3, 4, 5};
    const CCirtucalBufferExt<int> anotherBufer{333, 666, 999};

    CCirtucalBufferExt<int> bufer2(bufer);
    CCirtucalBufferExt<int> anotherBufer2(anotherBufer);

    bufer2.swap(anotherBufer2);
    ASSERT_TRUE(bufer2 == anotherBufer && anotherBufer2 == bufer);

    swap(bufer2, anotherBufer2);
    ASSERT_TRUE(bufer2 == bufer && anotherBufer2 == anotherBufer);
}

TEST(CCirtucalBufferExtTestSuite, PushPopTest) {
    CCirtucalBufferExt<int> mass{1, 2, 3, 4, 5, 6, 7, 8, 0};

    CCirtucalBufferExt<int> answer{1, 2, 3, 4, 5, 6, 7, 8, 0, 300};
    mass.push_back(300);
    ASSERT_TRUE(mass == answer);
    ASSERT_TRUE(mass.capacity() == 18);

    CCirtucalBufferExt<int> answer2{500, 1, 2, 3, 4, 5, 6, 7, 8, 0, 300};
    mass.push_front(500);
    ASSERT_TRUE(mass == answer2);
    ASSERT_TRUE(mass.capacity() == 18);


    CCirtucalBufferExt<int> answer3{1000, 500, 1, 2, 3, 4, 5, 6, 7, 8, 0, 300};
    answer2.push_front(1000);
    ASSERT_TRUE(answer3 == answer2);
    ASSERT_TRUE(answer2.capacity() == 22);

    CCirtucalBufferExt<int> answer4{1000, 500, 1, 2, 3, 4, 5, 6, 7, 8, 0, 300, 444};
    answer2.push_back(444);
    ASSERT_TRUE(answer4 == answer2);
    ASSERT_TRUE(answer2.capacity() == 22);


}

TEST(CCirtucalBufferExtTestSuite, InsertTest) {
    CCirtucalBufferExt<int> mass{1, 2, 3, 4, 5, 6, 7, 8, 0};

    CCirtucalBufferExt<int> answer{1, 2, 3, 4, 5, 8, 8, 8, 6, 7, 8, 0};
    auto it = mass.insert(mass.begin() + 5, 3, 8);
    ASSERT_TRUE(mass == answer);
    ASSERT_TRUE(*it == 8);

    CCirtucalBufferExt<int> answer2{1, 2, 3, 4, 5, 8, 8, 8, 6, 7, 4, 5, 6, 8, 8, 0};
    it = mass.insert(mass.begin() + 10, {4, 5, 6, 8});
    ASSERT_TRUE(mass == answer2);
    ASSERT_TRUE(*it == 4);

    std::vector<int> ya_vector{55555, 7777, 8888, 9999};
    CCirtucalBufferExt<int> answer3{1, 7777, 8888, 9999, 2, 3, 4, 5, 8, 8, 8, 6, 7, 4, 5, 6, 8, 8, 0};
    it = mass.insert(mass.begin() + 1, ya_vector.begin() + 1, ya_vector.end());
    ASSERT_TRUE(answer3 == mass);
    ASSERT_TRUE(*it == 7777);

    it = mass.insert(answer3.begin(), ya_vector.begin() + 1, ya_vector.end()); //итератор не отсюда
    ASSERT_TRUE(*it == answer3[0]);
    ASSERT_TRUE(answer3 == mass);

}

TEST(CCirtucalBufferExtTestSuite, AssignTest) {
    CCirtucalBufferExt<int> mass{1, 2};
    CCirtucalBufferExt<int> answer{20, 20, 20};

    mass.assign(3, 20);
    ASSERT_TRUE(answer == mass);

    std::vector<int> friends{222, 444, 888, 111, 555};
    CCirtucalBufferExt<int> answer2{444, 888, 111, 555};
    mass.assign(friends.begin() + 1, friends.end());
    ASSERT_TRUE(mass == answer2);

    mass.assign({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
    CCirtucalBufferExt<int> answer3{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    ASSERT_TRUE(mass == answer3);
}

TEST(CCirtucalBufferExtTestSuite, ResizeTest) {
    CCirtucalBufferExt<int> mass{1, 2, 3, 4, 5, 6, 7, 8, 0, 10};

    ASSERT_TRUE(mass.size() == 10);
    mass.resize(8);
    ASSERT_TRUE(mass.size() == 8);
    ASSERT_TRUE(mass.back() == 8);

    mass.resize(20);
    ASSERT_TRUE(mass.size() == 20);
}

TEST(CCirtucalBufferExtTestSuite, EmplaceTest) {
    CCirtucalBufferExt<Myclass> mass;

    Myclass first(10, "Liza", {2.0, 3.0, 5.0});
    Myclass second(22, "Sasha", {10.4, 30.5, 80.0});
    Myclass third(1, "Masha", {3.4, 1.5, 9.0});
    Myclass fouth(1000, "Kolya", {7.4, 30.4, 66.6});

    mass.push_back(first);

    std::initializer_list<double> list{10.4, 30.5, 80.0};
    mass.emplace(mass.begin(), 22, "Sasha", list);
    ASSERT_TRUE(mass[0] == second);

    list = {3.4, 1.5, 9.0};
    mass.emplace_back(1, "Masha", list);
    ASSERT_TRUE(mass.back() == third);

    list = {7.4, 30.4, 66.6};
    mass.emplace_front(1000, "Kolya", list);
    ASSERT_TRUE(mass.front() == fouth);
}

TEST(CCirtucalBufferExtTestSuite, VectorTest) {
    CCirtucalBufferExt<std::vector<int>> mass;

    mass.push_back({7, 8, 9, 0, 4});
    mass.push_back({8, 8, 8, 8, 8});
    mass.push_front({});

    std::vector<int> a{300, 200};


    mass[0].push_back(300);
    mass[0].push_back(200);

    ASSERT_TRUE(mass[0] == a);
    ASSERT_TRUE(mass.front() == a);

    a = {8, 8, 8, 8, 8};

    ASSERT_TRUE(mass.back() == a);
}

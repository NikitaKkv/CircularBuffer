#include <circular_buffer.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <vector>

template <typename T>
class CircularBufferIntTest : public testing::Test {};

using CircularBufferIntTypes = testing::Types<
    circular_buffer<int, false>
    #ifdef RUN_EXT_TESTS
    , circular_buffer<int, true>
    #endif
>;
TYPED_TEST_SUITE(CircularBufferIntTest, CircularBufferIntTypes);

template <typename T>
class CircularBufferStringTest : public testing::Test {};

using CircularBufferStringTypes = testing::Types<
    circular_buffer<std::string, false>
    #ifdef RUN_EXT_TESTS
    , circular_buffer<std::string, true>
    #endif
>;
TYPED_TEST_SUITE(CircularBufferStringTest, CircularBufferStringTypes);

TYPED_TEST(CircularBufferIntTest, alternatingPush) {
    TypeParam cb(6);
    for (int i = 0; i < 6; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }

    ASSERT_THAT(cb, testing::ElementsAre(5, 3, 1, 0, 2, 4));
}

TYPED_TEST(CircularBufferStringTest, pushingComplicatedObjects) {
    TypeParam cb(3);
    cb.push_back("aaa");
    cb.push_back("bbb");
    cb.push_back("ccc");
    ASSERT_THAT(cb, testing::ElementsAre("aaa", "bbb", "ccc"));
}

TYPED_TEST(CircularBufferIntTest, simplePopTest) {
    TypeParam cb(5);

    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    cb.push_front(4);
    cb.push_front(5);

    cb.pop_back();
    cb.pop_front();

    ASSERT_THAT(cb, testing::ElementsAre(4, 1, 2));
}

TYPED_TEST(CircularBufferIntTest, popFromEmpty) {
    TypeParam cb(1);
    cb.push_back(1);
    cb.pop_back();

    try {
        cb.pop_back();
        FAIL();
    } catch (...) {}

    try {
        cb.pop_front();
        FAIL();
    } catch (...) {}

    SUCCEED();
}

TYPED_TEST(CircularBufferIntTest, eraseOneElement) {
    TypeParam cb = {1, 2, 3, 4, 5};

    cb.erase(cb.cbegin() + 2);

    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 4, 5));
}

TYPED_TEST(CircularBufferIntTest, eraseSequence) {
    TypeParam cb(5);
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }
    cb.erase(cb.cbegin() + 1, cb.cend() - 1);

    ASSERT_THAT(cb, testing::ElementsAre(3, 4));
}

TYPED_TEST(CircularBufferIntTest, nValues) {
    TypeParam cb(5);

    cb.assign(static_cast<size_t>(5), 10);

    ASSERT_THAT(cb, testing::ElementsAre(10, 10, 10, 10, 10));
}

TYPED_TEST(CircularBufferIntTest, assignIterator) {
    TypeParam cb(5);
    std::vector<int> v = {1, 2, 3, 4, 5};

    cb.assign(v.begin() + 1, v.end() - 1);

    ASSERT_THAT(cb, testing::ElementsAre(2, 3, 4));
}

TYPED_TEST(CircularBufferIntTest, initializerList) {
    TypeParam cb = {5, 4, 3, 2, 1};

    cb.assign({1, 2, 3, 4, 5});

    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 3, 4, 5));
}

TYPED_TEST(CircularBufferIntTest, simpleTest) {
    TypeParam cb(5); // {3, 1, 0, 2, 4}
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }

    ASSERT_EQ(cb.front(), 3);
}

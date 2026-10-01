#include <circular_buffer.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cstdlib>

class NodeTag {};

class SomeObj {
public:
    static inline int ConstructorCalled = 0;
    static inline int DestructorCalled = 0;

    SomeObj() {
        ++ConstructorCalled;
    }

    ~SomeObj() {
        ++DestructorCalled;
    }
};


template<class Alloc>
concept AllocatorRequirements = requires(Alloc alloc, std::size_t n)
{
    { *alloc.allocate(n) } -> std::same_as<typename Alloc::value_type&>;
    { alloc.deallocate(alloc.allocate(n), n) };
} && std::copy_constructible<Alloc>
  && std::equality_comparable<Alloc>;


template<typename T>
class TestAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using size_type = size_t;
    using is_always_equal = std::true_type;

    static inline int AllocationCount = 0;
    static inline int ElementsAllocated = 0;

    TestAllocator() = default;

    template<typename U>
    TestAllocator(const TestAllocator<U>& other) {
    }

    pointer allocate(size_type sz) {
        if constexpr (std::is_same_v<T, SomeObj>) {
            ++TestAllocator<SomeObj>::AllocationCount;
            TestAllocator<SomeObj>::ElementsAllocated += sz;
        } else {
            ++TestAllocator<NodeTag>::AllocationCount;
            TestAllocator<NodeTag>::ElementsAllocated += sz;
        }
        return static_cast<pointer>(std::aligned_alloc(alignof(T), sizeof(T) * sz));
    }

    void deallocate(pointer p, std::size_t) {
        std::free(p);
    }

    bool operator==(const TestAllocator& other) const {
        return true;
    }

};


static_assert(AllocatorRequirements<TestAllocator<SomeObj>>);

class WorkWithAllocatorTest : public testing::Test {
public:
    void SetUp() override {
        SomeObj::ConstructorCalled = 0;
        SomeObj::DestructorCalled = 0;

        TestAllocator<SomeObj>::AllocationCount = 0;
        TestAllocator<SomeObj>::ElementsAllocated = 0;

        TestAllocator<NodeTag>::AllocationCount = 0;
        TestAllocator<NodeTag>::ElementsAllocated = 0;
    }

};

/*
    В тесте задаётся Capacity = 5, Extended = false.

    Ожидается, что будет:
        1. 1 аллокация буфера
*/
TEST_F(WorkWithAllocatorTest, reserve) {
    TestAllocator<SomeObj> allocator;
    circular_buffer<SomeObj, false, TestAllocator<SomeObj>> buffer(5, allocator);

    ASSERT_EQ(TestAllocator<SomeObj>::AllocationCount, 1);
    ASSERT_EQ(TestAllocator<SomeObj>::ElementsAllocated, 5);

    ASSERT_EQ(SomeObj::ConstructorCalled, 0);
    ASSERT_EQ(SomeObj::DestructorCalled, 0);
}

/*
    В тесте задаётся Capacity = 5, Extended = false, а далее добавляется 5 элементов.

    Ожидается, что будет:
        1. 1 аллокация буфера
        2. 5 конструкторов и деструкторов у SomeObj
*/
TEST_F(WorkWithAllocatorTest, simplePushBack) {
    TestAllocator<SomeObj> allocator;
    circular_buffer<SomeObj, false, TestAllocator<SomeObj>> buffer(5, allocator);
    for (int i = 0; i < 5; ++i) {
        buffer.push_back(SomeObj{});
    }

    ASSERT_EQ(TestAllocator<SomeObj>::AllocationCount, 1);
    ASSERT_EQ(TestAllocator<SomeObj>::ElementsAllocated, 5);

    ASSERT_EQ(SomeObj::ConstructorCalled, 5);
    ASSERT_EQ(SomeObj::DestructorCalled, 5);
}

/*
    В тесте задаётся Capacity = 5, Extended = true, а далее добавляется 6 элементов.

    Ожидается, что будет:
        1. 2 аллокации буфера
        2. 6 конструкторов и деструкторов у SomeObj
*/
#ifdef RUN_EXT_TESTS
TEST_F(WorkWithAllocatorTest, extendedPushBack) {
    TestAllocator<SomeObj> allocator;
    circular_buffer<SomeObj, true, TestAllocator<SomeObj>> buffer(5, allocator);
    for (int i = 0; i < 6; ++i) {
        buffer.push_back(SomeObj{});
    }

    ASSERT_EQ(TestAllocator<SomeObj>::AllocationCount, 2);
    ASSERT_EQ(TestAllocator<SomeObj>::ElementsAllocated, 15);

    ASSERT_EQ(SomeObj::ConstructorCalled, 6);
    ASSERT_EQ(SomeObj::DestructorCalled, 11);
}
#endif

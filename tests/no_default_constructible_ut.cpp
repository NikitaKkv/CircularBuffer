#include <circular_buffer.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

struct NoDefaultConstructible {
    NoDefaultConstructible() = delete;

    NoDefaultConstructible(int) {}
};

/*
    Тест проверяет, что ваш контейнер умеет работать с типами, у которых нет конструктора по-умолчанию
*/
TEST(NoDefaultConstructible, canConstruct) {
    circular_buffer<NoDefaultConstructible> buffer(1);
    buffer.push_back(NoDefaultConstructible(1));
    ASSERT_EQ(buffer.size(), 1);
}

#include <stdexcept>

#include <circular_buffer.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


class throwing_int {
public:
    static int counter_;
    static int throw_after_;
    static int alive_;

    int value;

    throwing_int(int v = 0) : value(v) { ++alive_;}

    throwing_int(const throwing_int& other) : value(other.value) {
        if (++counter_ == throw_after_)
            throw std::runtime_error("copy");
         ++alive_;
    }

    throwing_int& operator=(const throwing_int&) = default;

    static void reset(int t) {
        counter_ = 0;
        throw_after_ = t;
    }

    bool operator==(const throwing_int& other) const {
        return value == other.value;
    }

    ~throwing_int() {
      --alive_;
    }
};

int throwing_int::counter_ = 0;
int throwing_int::throw_after_ = -1;
int throwing_int::alive_ = 0;



class exception_test : public testing::Test {
   protected:
      void SetUp() override {
         ext_cb_ = {0, 1, 2, 3};
         not_ext_cb_ = {0, 1, 2, 3};
      }

   circular_buffer<throwing_int, true>ext_cb_;
   circular_buffer<throwing_int, false>not_ext_cb_;
};

//EXTANDABLE BUFFER

TEST_F(exception_test, PushBackStrongGuarantee) {
    circular_buffer<throwing_int, true> cb;

    throwing_int::reset(1);  

    try {
        cb.push_back(throwing_int(10));
    }
    catch (...) {}

    EXPECT_TRUE(cb.empty());
}

TEST_F(exception_test, PushFrontStrongGuarantee) {
    circular_buffer<throwing_int, true> cb;

    throwing_int::reset(1);  

    try {
        cb.push_front(throwing_int(10));
    }
    catch (...) {}

    EXPECT_TRUE(cb.empty());
}


TEST_F(exception_test, InsertStrongGuarantee1) {
    
   throwing_int::reset(1);  

   try {
      ext_cb_.insert(ext_cb_.begin() + 3, throwing_int(10));
   }
   catch (...) {}

   EXPECT_THAT(ext_cb_, ::testing::ElementsAre(0,1,2,3));
}

TEST_F(exception_test, InsertStrongGuarantee2) {
    
   throwing_int::reset(5);  

   throwing_int a[5] = {11, 12, 13, 14, 15};

   try {
      ext_cb_.insert(ext_cb_.begin() + 2, a, a + 5);
   }
   catch (...) {}

   EXPECT_THAT(ext_cb_, ::testing::ElementsAre(0,1,2,3));
}

TEST_F(exception_test, InsertStrongGuarantee3) {
    
   throwing_int::reset(3);  

   throwing_int a[5] = {11, 12, 13, 14, 15};

   try {
      ext_cb_.insert(ext_cb_.begin() + 2, 5, 1);
   }
   catch (...) {}

   EXPECT_THAT(ext_cb_, ::testing::ElementsAre(0,1,2,3));
}

TEST_F(exception_test, ConstructStrongGuarantee3) {
    
   throwing_int::reset(3);  

   throwing_int a[5] = {11, 12, 13, 14, 15};

   try {
      circular_buffer<throwing_int, true>(a,a + 5);
   }
   catch (...) {}

   EXPECT_EQ(8 + 5, throwing_int::alive_);
}


//NOT EXTANDABLE


TEST_F(exception_test, PushBackBasicGuaranteeNotExt) {

    throwing_int::reset(1);  

    try {
        not_ext_cb_.push_back(throwing_int(10));
    }
    catch (...) {}

    EXPECT_THAT(not_ext_cb_, ::testing::ElementsAre(1,2,3));
    EXPECT_EQ(not_ext_cb_.size(), 3);
}

TEST_F(exception_test, PushFrontBasicGuaranteeNotExt) {

    throwing_int::reset(1);  

    try {
        not_ext_cb_.push_front(throwing_int(10));
    }
    catch (...) {}

   EXPECT_THAT(not_ext_cb_, ::testing::ElementsAre(0,1,2));
   EXPECT_EQ(not_ext_cb_.size(), 3);
}


TEST_F(exception_test, InsertStringGuarantee1NotExt) {
    
   throwing_int::reset(1);  

   try {
      not_ext_cb_.insert(not_ext_cb_.begin() + 3, throwing_int(10));
   }
   catch (...) {}

   EXPECT_THAT(not_ext_cb_, ::testing::ElementsAre(0,1,2,3));
}

TEST_F(exception_test, InsertStrongGuarantee2NotExt) {
    
   throwing_int::reset(5);  

   throwing_int a[5] = {11, 12, 13, 14, 15};

   try {
      not_ext_cb_.insert(not_ext_cb_.begin() + 2, a, a + 5);
   }
   catch (...) {}

   EXPECT_THAT(not_ext_cb_, ::testing::ElementsAre(0,1,2,3));
}

TEST_F(exception_test, InsertStrongGuarantee3NotExt) {
    
   throwing_int::reset(3);  

   throwing_int a[5] = {11, 12, 13, 14, 15};

   try {
      not_ext_cb_.insert(not_ext_cb_.begin() + 2, 5, 1);
   }
   catch (...) {}

   EXPECT_THAT(not_ext_cb_, ::testing::ElementsAre(0,1,2,3));
}

TEST_F(exception_test, ConstructStrongGuarantee3NotExt) {
    
   throwing_int::reset(3);  

   throwing_int a[5] = {11, 12, 13, 14, 15};

   try {
      circular_buffer<throwing_int, false>(a,a + 5);
   }
   catch (...) {}

   EXPECT_EQ(8 + 5, throwing_int::alive_);
}
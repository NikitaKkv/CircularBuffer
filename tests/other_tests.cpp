#include <circular_buffer.h>

#include <compare>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

class other_test : public testing::Test {
   protected:
      void SetUp() override {
         ext_cb_ = {0, 1, 2, 3};
         not_ext_cb_ = {0, 1, 2, 3};
      }

   circular_buffer<int, true>ext_cb_;
   circular_buffer<int, false>not_ext_cb_;
};

template<class C>
void expect_equal(const C& a, const C& b) {
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
        EXPECT_EQ(a[i], b[i]);
}

TEST_F(other_test, OtherObj) {
   circular_buffer<int, true> cb(ext_cb_);

   circular_buffer<int, false> cb2;
   cb2 = not_ext_cb_;

   expect_equal(cb2, not_ext_cb_);
   expect_equal(cb, ext_cb_);
}

TEST_F(other_test, CheckAlloc) {
   std::allocator<int> alloc;
   EXPECT_EQ(alloc, ext_cb_.get_allocator());
}


TEST_F(other_test, ReverseIterators) { //error
   circular_buffer<int> cb = {0, 1, 2, 3};
    
   EXPECT_EQ(*cb.rbegin(), 3);
   EXPECT_EQ(*(cb.rbegin() + 1), 2);
   EXPECT_EQ(*(++cb.rbegin()), 2);
   EXPECT_EQ(*(cb.rbegin() += 2), 1);
   EXPECT_EQ(*(cb.rend() - 2), 1);
   EXPECT_EQ(cb.rend() - cb.rbegin(), 4);
   
   const auto& const_cb = cb;
   EXPECT_EQ(*const_cb.rbegin(), 3);
   EXPECT_EQ(*(const_cb.rbegin() + 2), 1);
   
   EXPECT_EQ(*cb.crbegin(), 3);
   EXPECT_EQ(*(cb.crbegin() + 3), 0);
   EXPECT_EQ(*(cb.crend() - 1), 0);
   EXPECT_EQ(cb.crend() - cb.crbegin(), 4);
   
   auto rit = cb.rbegin();
   EXPECT_EQ(rit + 3, 3 + rit);
   EXPECT_EQ(*(rit++), 3);
   EXPECT_EQ(*(rit--), 2);
   EXPECT_EQ(*((++rit) += 1), 1);


   rit = cb.rbegin();
   auto rit2 = cb.rbegin();
   EXPECT_TRUE(rit == rit2);
   EXPECT_TRUE(rit != ++rit2);
   EXPECT_FALSE(rit < rit2);
   EXPECT_EQ(rit[0], 3);
   EXPECT_EQ(rit[1], 2);

   rit += 2;
   rit -= 1;
   EXPECT_EQ(*rit, 2);
   EXPECT_EQ(rit2 - rit, 0);
   EXPECT_TRUE((rit <=> rit2) == std::strong_ordering::equal); 

   decltype(rit) default_it;                 
   default_it = rit;
   --default_it;
   EXPECT_EQ(*default_it, 3);
}

TEST_F(other_test, InsertInEmpty) {
   circular_buffer<int, true> cb;

   int arr[5] = {0, 1, 2, 3, 4};

   cb.insert(cb.begin(), arr, arr + 5);

   ASSERT_THAT(cb, testing::ElementsAre(0, 1, 2, 3, 4));
}

TEST_F(other_test, InsertInBeginRangeNotExt) {

   int arr[5] = {0, 1, 2, 3, 4};

   not_ext_cb_.insert(not_ext_cb_.begin(), arr, arr + 5);

   ASSERT_THAT(not_ext_cb_, testing::ElementsAre(0, 1, 2, 3));
}

TEST_F(other_test, InsertInBeginElNotExt) {

   int arr[5] = {0, 1, 2, 3, 4};

   not_ext_cb_.insert(not_ext_cb_.begin());

   ASSERT_THAT(not_ext_cb_, testing::ElementsAre(0, 1, 2, 3));
}

TEST_F(other_test, Reserve) {

   not_ext_cb_.reserve(6);

   EXPECT_EQ(not_ext_cb_.capacity(), 6);
   EXPECT_EQ(not_ext_cb_.size(), 4);
   ASSERT_THAT(not_ext_cb_, testing::ElementsAre(0, 1, 2, 3));
}

TEST_F(other_test, Front) {
   EXPECT_EQ(not_ext_cb_.front(), not_ext_cb_.cfront());
   EXPECT_EQ(not_ext_cb_.front(), 0);
}

TEST_F(other_test, Back) {
   EXPECT_EQ(not_ext_cb_.back(), not_ext_cb_.cback());
   EXPECT_EQ(not_ext_cb_.back(), 3);
}


TEST_F(other_test, GetValue) {
   const auto cext_cb = ext_cb_;
   EXPECT_EQ(cext_cb[3], ext_cb_[3]);
   ext_cb_[3] = 5;
   EXPECT_EQ(5, ext_cb_[3]);
   EXPECT_FALSE(ext_cb_[3] == cext_cb[3]);
   EXPECT_FALSE(ext_cb_.at(3) == cext_cb.at(3));
}

TEST_F(other_test, At) {
   const auto cext_cb = ext_cb_;
   EXPECT_ANY_THROW(ext_cb_.at(10));
   EXPECT_ANY_THROW(cext_cb.at(-10));
}

TEST_F(other_test, Equiv) {
   auto cext_cb = ext_cb_;
   EXPECT_TRUE(cext_cb == ext_cb_);
   EXPECT_FALSE(cext_cb != ext_cb_);
   EXPECT_FALSE(cext_cb < ext_cb_);
}







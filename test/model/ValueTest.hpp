#pragma once

#include <gtest/gtest.h>

#include "model/Value.hpp"
#include "persistence/BufferStream.hpp"

namespace ursql {

class ValueTest : public testing::Test {
protected:
    template<typename... Args>
    void doTest(ValueType expectedValType, std::string_view expectedStr,
                Args&&... args) {
        Value original(std::forward<Args>(args)...);
        validateValue(original, expectedValType, expectedStr);

        Value restored = saveAndRestore(original);
        validateValue(restored, expectedValType, expectedStr);
    }

private:
    static Value saveAndRestore(const Value& original) {
        BufferWriter writer(block_.getData(), Block::size);
        writer << original;
        BufferReader reader(block_.getData(), Block::size);
        return reader.read<Value>();
    }

    static void validateValue(const Value& val, ValueType valueType,
                              std::string_view str) {
        ASSERT_EQ(valueType, val.getType());
        ASSERT_EQ(str, val.toString());
    }

    static Block block_;
};

Block ValueTest::block_;

TEST_F(ValueTest, null) {
    doTest(ValueType::null_type, "NULL");
}

TEST_F(ValueTest, int) {
    for (int i : { 0, 1, 4, 5, 101284, 2147483647 }) {
        doTest(ValueType::int_type, std::to_string(i), i);
    }
}

TEST_F(ValueTest, float) {
    for (float f : { 000.00f, 123.45f, 1000.000002f, 673841.63274f, 128097.0f })
    {
        doTest(ValueType::float_type, std::to_string(f), f);
    }
}

TEST_F(ValueTest, bool) {
    for (bool b : { true, false }) {
        doTest(ValueType::bool_type, b ? "t" : "f", b);
    }
}

TEST_F(ValueTest, varchar) {
    for (std::string s : { "asd", "aioj123", "annie", "hanhan" }) {
        doTest(ValueType::varchar_type, s, s);
    }
}

}  // namespace ursql

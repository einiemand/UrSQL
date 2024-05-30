#pragma once

#include "Value.hpp"

namespace ursql {

class Attribute : public Storable {
public:
    explicit Attribute() = default;
    ~Attribute() override = default;

    URSQL_DEFAULT_COPY_CTOR(Attribute);
    URSQL_DISABLE_COPY_ASSIGN(Attribute);

    void serialize(BufferWriter& writer) const override;
    void deserialize(BufferReader& reader) override;

    void setName(std::string name);
    void setValueType(ValueType valueType);
    void setDefaultValue(Value defaultValue);
    void setNotNullable();
    void setPrimary();
    void setAutoInc();

    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] ValueType getType() const;
    [[nodiscard]] const Value& getDefaultValue() const;
    [[nodiscard]] bool isNullable() const;
    [[nodiscard]] bool isPrimary() const;
    [[nodiscard]] bool isAutoInc() const;

    [[nodiscard]] bool mustBeSpecified() const;

    static Attribute parse(TokenStream& ts);

private:
    std::string name_;
    ValueType valueType_ = ValueType::null_type;
    Value defaultValue_;
    bool isNullable_ = true;
    bool isPrimary_ = false;
    bool isAutoInc_ = false;
};

}  // namespace ursql

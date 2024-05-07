#pragma once

#include "Storable.hpp"

namespace ursql {

class TOC : public LazySaveMonoStorable {
public:
    explicit TOC();
    ~TOC() override = default;

    URSQL_DISABLE_COPY(TOC);

    [[nodiscard]] BlockType expectedBlockType() const override;

    void serialize(BufferWriter& aWriter) const override;
    void deserialize(BufferReader& aReader) override;

    [[nodiscard]] bool entityExists(const std::string& entityName) const;
    [[nodiscard]] std::size_t getEntityPosByName(
      const std::string& entityName) const;

    void addEntity(const std::string& entityName, std::size_t blockNum);
    void dropEntity(const std::string& entityName);

    [[nodiscard]] std::vector<std::string> getAllEntityNames() const;

private:
    std::unordered_map<std::string, std::size_t> entityPosMap_;
};

}  // namespace ursql

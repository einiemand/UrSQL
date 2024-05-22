#include "model/TOC.hpp"

#include <format>

#include "exception/InternalError.hpp"
#include "persistence/BufferStream.hpp"

namespace ursql {

TOC::TOC() : MonoStorable(0), entityPosMap_() {}

BlockType TOC::expectedBlockType() const {
    return BlockType::toc;
}

void TOC::serialize(BufferWriter& writer) const {
    writer << entityPosMap_.size();
    for (auto& [entityName, blockNum] : entityPosMap_) {
        writer << entityName << blockNum;
    }
}

void TOC::deserialize(BufferReader& reader) {
    auto mapSize = reader.read<std::size_t>();
    for (; mapSize > 0; --mapSize) {
        auto entityName = reader.read<std::string>();
        auto blockNum = reader.read<std::size_t>();
        entityPosMap_.emplace(entityName, blockNum);
    }
}

bool TOC::entityExists(const std::string& entityName) const {
    return entityPosMap_.find(entityName) != std::end(entityPosMap_);
}

std::size_t TOC::getEntityPosByName(const std::string& entityName) const {
    auto it = entityPosMap_.find(entityName);
    URSQL_ASSERT(it != std::end(entityPosMap_),
                 std::format("entity {} doesn't exist", entityName));
    return it->second;
}

void TOC::addEntity(const std::string& entityName, std::size_t blockNum) {
    auto it = entityPosMap_.find(entityName);
    URSQL_ASSERT(it == std::end(entityPosMap_),
                 std::format("entity {} already exists", entityName));
    entityPosMap_.emplace(entityName, blockNum);
    makeDirty(true);
}

void TOC::dropEntity(const std::string& entityName) {
    auto it = entityPosMap_.find(entityName);
    URSQL_ASSERT(it != std::end(entityPosMap_),
                 std::format("entity {} doesn't exist", entityName));
    entityPosMap_.erase(it);
    makeDirty(true);
}

std::vector<std::string> TOC::getAllEntityNames() const {
    std::vector<std::string> entityNames;
    entityNames.reserve(entityPosMap_.size());
    for (auto& p : entityPosMap_) {
        entityNames.push_back(p.first);
    }
    return entityNames;
}

}  // namespace ursql

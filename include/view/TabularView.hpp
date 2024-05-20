#pragma once

#include <vector>

#include "View.hpp"
#include "model/Value.hpp"

namespace ursql {

class TabularView : public View {
public:
    explicit TabularView(std::vector<std::string> headers,
                         std::vector<std::vector<Value>> valueRows);
    ~TabularView() override = default;

    void show(std::ostream& os) const override;

private:
    const std::vector<std::string> headers_;
    const std::vector<std::vector<Value>> valueRows_;

    static void _printBreak(std::ostream& os,
                            const std::vector<std::size_t>& widths);
    void _printHeaders(std::ostream& os,
                       const std::vector<std::size_t>& widths) const;
    void _printValueRows(std::ostream& os,
                         const std::vector<std::size_t>& widths) const;
};

}  // namespace ursql

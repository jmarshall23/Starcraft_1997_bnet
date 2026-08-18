#pragma once

#include <string>

namespace staredit::formats {

class ChkDocument;

// Converts the recovered beta placement layouts to retail StarCraft CHK while
// preserving every section that does not require a dialect-specific rewrite.
[[nodiscard]] bool convert_to_retail_chk(ChkDocument& document,
                                         std::string& error) noexcept;

}  // namespace staredit::formats

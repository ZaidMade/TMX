#ifndef LM_TMX_UTILS_H
#define LM_TMX_UTILS_H

#include <iostream>
#include <stdlib.h>
#include <string>

#include "tlist.hpp"

namespace tmx {
    /**
     * Decode a base64 encoded data set.
     *
     * @param p_raw C-string holding the raw data to decode.
     * @param p_len Length of the C-string holding the raw data to decode.
     * @returns [std::string] Decoded data set.
     */
    std::string base64_decode(const char* p_raw, const size_t p_len);

    /**
     * @todo zlib decompress
     * @todo gzip decompress
     */
}

#endif

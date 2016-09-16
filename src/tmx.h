#ifndef LM_TMX_H
#define LM_TMX_H

#include "tmx_core.h"

namespace tmx {
    class tmxnode {
    public:
        tmxnode(const sNode& p_node);

        eTag tag();

        sVal attr(str_p p_attribute);
        sVal prop(str_p p_property);
    private:
        sNode _mynode;
    };
}

#endif

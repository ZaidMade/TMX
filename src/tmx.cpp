#include "tmx.h"

namespace tmx {
    tmxnode::tmxnode(const sNode& p_node) { _mynode = p_node; }

    eTag tmxnode::tag(){ return _mynode.tag; }

    sVal tmxnode::attr(str_p p_attribute) {
        return getNodeVar(_mynode, p_attribute);
    }

    sVal tmxnode::prop(str_p p_property) {
        return getNodeVar(_mynode, p_property, true);
    }
}

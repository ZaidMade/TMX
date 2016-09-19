#include "tmx.h"

namespace tmx {
    tmxnode::tmxnode(){
        _childiter = nullptr;
    }

    tmxnode::tmxnode(const sNode& p_node) {
        _mynode = p_node;
        _childiter = _mynode.nodes;
    }

    void tmxnode::setNode(const sNode& p_node){
        _mynode = p_node;
        _childiter = _mynode.nodes;
    }

    eTag tmxnode::tag(){
        return _mynode.tag;
    }

    sVal tmxnode::attr(str_p p_attribute) {
        return getNodeVar(_mynode, p_attribute);
    }

    sVal tmxnode::prop(str_p p_property) {
        return getNodeVar(_mynode, p_property, true);
    }

    sData tmxnode::data(){
        if(tag() == eTag::data)
            return *_mynode.data;
        return mkData("\"", eEnc::text);
    }

    bool tmxnode::pollChildren(tmxnode& p_to){
        // Check if the end of child nodes has been reached.
        if(_childiter == nullptr){
            // Reset iterator to beginning
            _childiter = _mynode.nodes;
            return false;
        }

        // Set passed tmxnode to next child node.
        p_to.setNode(_childiter->value());
        _childiter = _childiter->next();
        return true;
    }
}

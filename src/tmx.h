#ifndef LM_TMX_H
#define LM_TMX_H

#include "tmx_core.h"

/**============================================================================
 * Wrapper class for the core TMX functions. This allows the end user to
 * access the map data much safer and easier than using the core functions.
 *
 * @author Zaid
 * @version 1.0
 ============================================================================*/

namespace tmx {
    class tmxnode {
    public:
        tmxnode();
        tmxnode(const sNode& p_node);

        /**
         * Set the node this wrapper represents.
         *
         * @param p_node Raw TMX node.
         */
        void setNode(const sNode& p_node);

        /** @returns [eTag] Enum alias of TMX element tag */
        eTag tag();

        /**
         * Get the node's value for the given attribute.
         *
         * @param p_attribute Attribute's name.
         * @returns [sVal] Requested attribute's value.
         */
        sVal attr(str_p p_attribute);

        /**
         * Get the node's value for the given property.
         *
         * @param p_property Property name.
         * @returns [sVal] Requested property's value.
         */
        sVal prop(str_p p_property);

        sData data();

        /**
         * Poll over all this node's child nodes.
         *
         * @param p_to tmxnode to set the child node to.
         * @returns [bool] false = finished
         */
        bool pollChildren(tmxnode& p_to);
    private:
        sNode _mynode;
        TList<sNode>* _childiter;
    };
}

#endif

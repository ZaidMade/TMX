/**============================================================================
 * tmx_core.h - C++ TMX Map Loader Core
 * Copyright (c) 2016-2017, Logan Moore <logan.moore513@gmail.com>
 *
 * Aliases for commonly used values, data structures used to store the map's
 * data and core function declarations. It's recommended that you shy away
 * from functionality defined here and favor the wrapper classes found in
 * tmx.h.
 *
 * Created according to the specifications found here:
 * http://doc.mapeditor.org/reference/tmx-map-format/
 *
 * @author Logan Moore
 * @version 1.0
 ============================================================================*/

#ifndef LM_TMX_CORE_H
#define LM_TMX_CORE_H

#include <string>
#include <vector>
#include <iostream>

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "tlist.hpp"

#define TMX_UNDEFINED_ATTRIBUTE "\""

namespace tmx {
    typedef const std::string& str_p; //@- String argument type

    // Available tags in the TMX standard
    enum eTag { ignore, root, map, tileset, tileoffset, image, terrain, frame,
                layer, tile, objectgroup, object, ellipse, polygon, polyline,
                imagelayer, data };

    // Variable types.
    enum eType { str, whole, dec, boolean, points, hexcolor, error };
    // Map render-orders.
    enum eRO { rd, ru, ld, lu };
    // Map orientations.
    enum eOrient { ortho, iso, stag, hex };

    // Encoding types of raw data.
    enum eEnc { text, xml, base64, csv, png, bmp, jpg };
    // Compression types of raw data.
    enum eComp { none, gzip, zlib };

    // Value structure.
    struct sVal { std::string value; eType type; };
    // Variable structure.
    struct sNamedVal { std::string name; sVal myvalue; };
    // Raw data structure.
    struct sData { std::string value; eEnc enc; eComp comp; };

    // Base node structure.
    struct sNode {
        eTag tag;
        TList<sNamedVal>* vars;
        TList<sNode>* nodes;
        sData* data;
    };

    /**
    * Builds a variable to assign to a node.
    *
    * @param p_name The name of the variable to build.
    * @param p_value The value the new variable should hold.
    * @param p_type The value type of the variable.
    * @returns [sNamedVal] The wrapped variable.
    */
    sNamedVal mkVar(str_p p_name, str_p p_value, eType p_type);

    /**
    * Builds a raw data set wrapper to assign to a node.
    *
    * @param p_value The raw data.
    * @param p_enc The encoding of the raw data.
    * @param p_comp The compression of the raw data. Defaults to none.
    * @returns [sData] The wrapped raw data set.
    */
    sData mkData(str_p p_value, eEnc p_enc, eComp p_comp = eComp::none);

    /**
    * Builds a new node of given tag and assigns it raw data.
    *
    * !WARNING! The newly created node will have its variable and child...
    * ...node lists undefined (nullptr) by default. Make sure to define...
    * ...them before use!
    *
    * @param p_tag The tag to assign to the new node.
    * @param p_data The raw data to assign to the new node. Defaults to...
    * ... empty plaintext.
    * @returns [sNode] The newly created node.
    */
    sNode mkNode(eTag p_tag, const sData& p_data = mkData("\"", eEnc::text));

    /**
    * Builds a child node of given tag within the node passed as an argument.
    *
    * @param p_node The sNode to create the new child node in.
    * @param p_tag The tag to assign to the new child node.
    * @param p_data The raw data to assign to the new child node...
    * ... defaults to empty plaintext.
    * @returns Address to the newly created child node.
    */

    sNode* nodeMkNode(
        sNode& p_node,
        const eTag& p_tag,
        const sData& p_data = mkData("\"", eEnc::text)
    );

    /**
    * Sets the value of a variable of given name in the node passed as an
    * argument.
    *
    * @param p_node The sNode whos variable is to be set.
    * @param p_var The labeled value to serve as the variable.
    * @param p_prop Whether or not the variable should be assigned as a...
    * ...property.
    * @returns [bool] Whether or not the variable was set successfully.
    */

    bool setNodeVar(sNode& p_node, const sNamedVal& p_var, bool p_prop = false);

    /**
    * Gets the value of a variable of given name from the node passed as an
    * argument.
    *
    * @param p_node The sNode to evaluate for the variable.
    * @param p_name The name of the variable to look for.
    * @param p_prop Specifies whether this variable is a property or not.
    * @returns [sVal] The value of the variable.
    */
    sVal getNodeVar(sNode& p_node, str_p p_name, bool p_prop = false);

    /**
    * Attempts to load the TMX map file at the given file path.
    *
    * @param p_path The path to the TMX map file.
    * @returns [sNode] The first node in the generated TMX structure.
    */
    sNode load(str_p p_path);
}

#endif

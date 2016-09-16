#include "tmx_core.h"
using namespace tmx;

/**============================================================================
 *  T M X  H E L P E R  F U N C T I O N S
 ============================================================================*/

/**
 * Returns the default value specified in the TMX standard for undefined...
 * ...attributes of a given tag.
 *
 * @param p_tag The TMX tag whose default attribute value is needed.
 * @param p_attr The name of the attribute whose default value is to be found.
 * @param p_map The pointer to the root map node to pull values from...
 * ... in special cases. (<layer width="" height="">)
 * @returns [std::string] The default value of the evaluated attribute.
 */
std::string tmxAttrDefault(eTag p_tag, str_p p_attr, sNode* p_map = nullptr) {
	if (p_tag == eTag::layer ||
        p_tag == eTag::imagelayer ||
        p_tag == eTag::objectgroup
    ) {
		if (p_attr == "x")
            return "0";
		else if (p_attr == "y")
            return "0";

        // Returns the map's width & height if able to.
		if (p_map != nullptr && p_tag == eTag::layer) {
			if (p_attr == "width")
                return getNodeVar(*p_map, "width").value;
			else if (p_attr == "height")
                return getNodeVar(*p_map, "height").value;
		}
	}
	else if (p_tag == eTag::object) {
		if (p_attr == "width")
            return "0";
		else if (p_attr == "height")
            return "0";
	}

	// Global default values:
	if (p_attr == "opacity")
        return "1";
	else if (p_attr == "visible")
        return "1";
	else if (p_attr == "offsetx")
        return "0";
	else if (p_attr == "offsety")
        return "0";

    // No default found! Returning undefined attribute placeholder.
	return TMX_UNDEFINED_ATTRIBUTE;
}

/**============================================================================
 *  X M L  H E L P E R  F U N C T I O N S
 ============================================================================*/

/**
 * Get the TMX tag of given XML node or <ignore> if the XML tag doesn't fit
 * the TMX standard.
 *
 * @param p_node XML node to evaluate.
 * @returns [tmx::eTag] TMX tag of the XML node.
 */
tmx::eTag xmlEvalTag(rapidxml::xml_node<>* p_node) {
	if (!(p_node->name_size() > 0))
        return eTag::ignore;

	std::string t = p_node->name();

    // Define all TMX tags to look out for and their associated aliases.
	struct sTagDict { std::string key; eTag value; };
	std::vector<sTagDict> tagDict{
		{ "tile", eTag::tile },
        { "tileoffset", eTag::tileoffset },
        { "tileset", eTag::tileset },
		{ "object", eTag::object },
        { "objectgroup", eTag::objectgroup },
        { "layer", eTag::layer },
		{ "imagelayer", eTag::imagelayer },
        { "image", eTag::image },
        { "polygon", eTag::polygon },
		{ "ellipse", eTag::ellipse },
        { "polyline", eTag::polyline },
        { "terrain", eTag::terrain },
		{ "frame", eTag::frame },
        { "map", eTag::map }
	};

    // See if the XML tag matches any TMX tags.
	for (unsigned int i = 0; i < tagDict.size(); i++)
		if (tagDict.at(i).key == t) return tagDict.at(i).value;
    // XML tag outside of TMX specification.
	return eTag::ignore;
}

/**
 * Evaluates an XML node for a given attribute value.
 *
 * @param p_node XML node to evaluate for the attribute.
 * @param p_attr Attribute to find the value of.
 * @returns [std::string] Value of the requested attribute.
 */
std::string xmlEvalAttr(rapidxml::xml_node<>* p_node, str_p p_attr) {
    // Checks if attribute exists and returns undefined if it doesn't.
	if (p_node->first_attribute(p_attr.c_str()) == nullptr)
		return TMX_UNDEFINED_ATTRIBUTE;
    // Attribute exists, returning its value.
	return p_node->first_attribute(p_attr.c_str())->value();
}

/**
 * Loads all the properties assigned to a given TMX tag into a TMX node.
 *
 * @param p_xnode The XML node of the TMX node used to find the properties.
 * @param p_tnode The TMX node to the load the properties into.
 * @returns [bool] Whether or not the properties were loaded successfully.
 */

bool xmlLoadNodeProps(rapidxml::xml_node<>* p_xnode, sNode& p_tnode) {
    // Make sure the XML node is valid.
    if (p_xnode == nullptr)
        return false;

	rapidxml::xml_node<>* properties = p_xnode->first_node("properties");
    // Make sure the XML node has a <properties> child XML node.
	if (properties == nullptr)
        return false;

    // Iterate over each child XML <property> node and load it into TMX node.
	for (rapidxml::xml_node<>* prop = properties->first_node("property");
         prop;
         prop = prop->next_sibling("property")
    ) {
        // Retrieve the property type.
		eType tbuff = eType::str;
		std::string s = xmlEvalAttr(prop, "type");
		if (s == "int")
            tbuff = eType::whole;
		else if (s == "float")
            tbuff = eType::dec;
		else if (s == "bool")
            tbuff = eType::boolean;

        // Assign the TMX node the property.
		setNodeVar(
            p_tnode,
            {xmlEvalAttr(prop, "name"), { xmlEvalAttr(prop, "value"), tbuff }},
            true
        );
	}
	return true;
}

/**
 * Encodes TMX node's data as comma seperated values.
 *
 * @param p_xnode XML node of the TMX data node to be loaded as CSV.
 * @param p_tnode TMX node to load the CSV data into.
 * @returns [bool] Whether or not the data was encoded successfully.
 */

bool xmlLoadDataCSV(rapidxml::xml_node<>* p_xnode, sNode& p_tnode) {
	if (p_tnode.tag != eTag::data)
        return false;
	std::string enc = getNodeVar(p_tnode, "encoding").value;
	std::string comp = getNodeVar(p_tnode, "compression").value;

    // Load the data based on its encoding.
	std::string csv = "";
    // Data already encoded as CSV.
	if (enc == "csv")
		csv = p_xnode->value();
	else if (enc == "xml") {
        // Iterate over each child XML node and append each gid to the CSV.
		for (rapidxml::xml_node<>* t = p_xnode->first_node("tile");
            t;
            t = t->next_sibling("tile")
        ) {
			std::string gid = xmlEvalAttr(t, "gid");
            // Ignore if no gid found.
			if (gid == TMX_UNDEFINED_ATTRIBUTE)
                continue;
			csv += gid + ((t->next_sibling("tile")) ? "," : "");
		}
	}

    // Make sure an empty string isn't being loaded.
	if (csv == "")
        return false;

	p_tnode.data = new sData(mkData(csv, eEnc::csv, eComp::none));
	return true;
}

/**
 * Load raw data into TMX node.
 *
 * @param p_xnode XML node of TMX node used to find the data XML node.
 * @param p_tnode TMX node to load the data into.
 * @returns [bool] Whether or not the data was loaded successfully.
 */

bool xmlLoadNodeData(rapidxml::xml_node<>* p_xnode, sNode& p_tnode) {
    // Check if the XML node is valid.
	if (p_xnode == nullptr)
        return false;
    // Check if the TMX node is valid to have raw data.
	if (p_tnode.tag != eTag::image && p_tnode.tag != eTag::layer)
        return false;

	rapidxml::xml_node<>* data = p_xnode->first_node("data");
    // Check if the XML data node is valid.
	if (data == nullptr)
        return false;

    // Begin loading
	sNode* n = nodeMkNode(p_tnode, eTag::data);

	std::string encoding = xmlEvalAttr(data, "encoding");
	std::string compression = xmlEvalAttr(data, "compression");

    // Set default meta values if undefined.
	if(encoding == TMX_UNDEFINED_ATTRIBUTE)
        encoding = "xml";
	if(compression == TMX_UNDEFINED_ATTRIBUTE)
        compression = "none";

	setNodeVar(*n, mkVar("encoding", encoding, eType::str));
	setNodeVar(*n, mkVar("compression", compression, eType::str));

	if(!xmlLoadDataCSV(data, *n))
		return false;
	return true;
}

/**
 * Loads all the child TMX nodes of given XML node into given TMX node.
 *
 * @param p_xnode XML node of TMX node used to locate child TMX nodes.
 * @param p_tnode TMX node to load the child TMX nodes into.
 * @returns [bool] Whether or not the child nodes were loaded successfully,
 */

bool xmlLoadChildNodes(rapidxml::xml_node<>* p_xnode, sNode& p_tnode) {
    // Checks if the XML node is valid.
	if (p_xnode == nullptr)
        return false;

	// TMX <tileset> attributes...
	std::vector<sVal> d_tsa{
		{ "firstgid", eType::whole },
        { "source", eType::str },
        { "name", eType::str },
		{ "tilewidth", eType::whole },
        { "tileheight", eType::whole },
        { "spacing", eType::whole },
		{ "margin", eType::whole },
        { "tilecount", eType::whole },
        { "columns", eType::whole }
	};

    // TMX <layer> attributes...
	std::vector<sVal> d_la{
		{ "name", eType::str },
        { "x", eType::whole },
        { "y", eType::whole },
        { "width", eType::whole },
		{ "height", eType::whole },
        { "opacity", eType::dec },
        { "visible", eType::boolean },
		{ "offsetx", eType::whole },
        { "offsety", eType::whole }
	};

    // TMX <objectgroup> attributes...
	std::vector<sVal> d_oga{
		{ "name", eType::str },
        { "color", eType::hexcolor },
        { "x", eType::whole },
        { "y", eType::whole },
		{ "width", eType::whole },
        { "height", eType::whole },
        { "opacity", eType::dec },
		{ "visible", eType::boolean },
        { "offsetx", eType::whole },
        { "offsety", eType::whole },
		{ "draworder", eType::str }
	};

    // TMX <imagelayer> attributes...
	std::vector<sVal> d_ila{
		{ "name", eType::str },
        { "x", eType::whole },
        { "y", eType::whole },
        { "width", eType::whole },
		{ "height", eType::whole },
        { "opacity", eType::dec },
        { "visible", eType::boolean },
		{ "offsetx", eType::whole },
        { "offsety", eType::whole }
	};

	// TMX <ignore> attributes...
	std::vector<sVal> d_ignore;

    // TMX <tileoffset> attributes...
	std::vector<sVal> d_toa{
        { "x", eType::whole },
        { "y", eType::whole }
    };

    // TMX <image> attributes...
	std::vector<sVal> d_ia{
		{ "format", eType::str },
        { "id", eType::whole },
        { "source", eType::str },
		{ "trans", eType::hexcolor },
        { "width", eType::whole },
        { "height", eType::whole }
	};

    // TMX <object> attributes...
	std::vector<sVal> d_oa{
		{ "id", eType::whole },
        { "name", eType::str },
        { "type", eType::str },
		{ "x", eType::whole },
        { "y", eType::whole },
        { "width", eType::whole },
		{ "height", eType::whole },
        { "rotation", eType::dec },
        { "gid", eType::whole },
		{ "visible", eType::boolean }
	};

    // TMX <ellipse> attributes...
	std::vector<sVal> d_ea{
		{ "x", eType::whole },
        { "y", eType::whole },
        { "width", eType::whole },
        { "height", eType::whole }
	};

    // TMX <polygon>/<polyline> attributes...
	std::vector<sVal> d_pa{ { "points", eType::points } };

    // TMX <layer> > <tile> attributes...
	std::vector<sVal> d_lta{ { "id", eType::whole } };

    // TMX <tileset> > <tile> attributes...
	std::vector<sVal> d_tsta{
        { "id", eType::whole },
        { "probability", eType::dec }
    };

    // Load all of the child nodes into the given TMX node.
	for (rapidxml::xml_node<>* xmlnode = p_xnode->first_node();
        xmlnode;
        xmlnode = xmlnode->next_sibling()
    ) {
        // Get the TMX tag of the XML node.
		eTag tag = xmlEvalTag(xmlnode);

        // Ignore it if it's an <ignore> tag.
		if (tag == eTag::ignore)
            continue;

		// Determine which attributes to load based on the node's tag.
		std::vector<sVal>* attrs;
		switch (tag) {
		case eTag::layer: attrs = &d_la; break;
		case eTag::imagelayer: attrs = &d_ila; break;
		case eTag::objectgroup: attrs = &d_oga; break;
		case eTag::tileset: attrs = &d_tsa; break;
		case eTag::tileoffset: attrs = &d_toa; break;
		case eTag::image: attrs = &d_ia; break;
		case eTag::object: attrs = &d_oa; break;
		case eTag::ellipse: attrs = &d_ea; break;
		case eTag::polygon: attrs = &d_pa; break;
		case eTag::polyline: attrs = &d_pa; break;
		case eTag::tile:
			if (p_tnode.tag == eTag::layer) attrs = &d_lta;
			else if (p_tnode.tag == eTag::tileset) attrs = &d_tsta;
			else attrs = &d_ignore;
			break;
		default: attrs = &d_ignore; break;
		}

        // Create the TMX node.
		sNode* tmxnode = nodeMkNode(p_tnode, tag);

		// Load the child TMX node's attributes.
		for (unsigned int i = 0; i < attrs->size(); i++) {
			std::string attribute = xmlEvalAttr(xmlnode, attrs->at(i).value);
			std::string attribute_default = tmxAttrDefault(
                                                tmxnode->tag,
                                                attrs->at(i).value
                                            );

            // Assign the attribute its given value.
			if (attribute != TMX_UNDEFINED_ATTRIBUTE)
				setNodeVar(
                    *tmxnode,
                    mkVar(attrs->at(i).value, attribute, attrs->at(i).type)
                );
            // Assign the attribute its default value if none given.
			else if (attribute_default != TMX_UNDEFINED_ATTRIBUTE)
				setNodeVar(
                    *tmxnode,
                    mkVar(
                        attrs->at(i).value,
                        attribute_default,
                        attrs->at(i).type
                    )
                );
		}

        // Load the node's data.
		xmlLoadNodeData(xmlnode, *tmxnode);
        // Load the node's properties.
		xmlLoadNodeProps(xmlnode, *tmxnode);

		// Load the node's child nodes.
		if (xmlnode->first_node() != nullptr)
			if (!xmlLoadChildNodes(xmlnode, *tmxnode))
                return false;
	}
	return true;
}

/**============================================================================
 *  T M X  C O R E  F U N C T I O N S
 ============================================================================*/

namespace tmx {
	tmx::sNamedVal mkVar(str_p p_name, str_p p_value, eType p_type) {
        return { p_name,{ p_value, p_type } };
    }

	tmx::sData mkData(str_p p_value, eEnc p_enc, eComp p_comp) {
        return { p_value, p_enc, p_comp };
    }

	tmx::sNode mkNode(eTag p_tag, const sData& p_data) {
		return {
                p_tag,
                nullptr,
                nullptr,
                ((p_data.value == "\"") ?  nullptr : new sData(p_data))
            };
	}

	sNode* nodeMkNode(sNode& p_node, const eTag& p_tag, const sData& p_data) {
        // Initializes the TMX node's child node list if undefined.
		if (p_node.nodes == nullptr)
			return (
                    p_node.nodes = new TList<sNode>(mkNode(p_tag, p_data))
                )->valptr();
        // Appends new child node to the TMX node.
		return (
                p_node.nodes->last()->append(mkNode(p_tag, p_data))
            )->valptr();
	}

	bool setNodeVar(sNode& p_node, const sNamedVal& p_var, bool p_prop) {
        // Deliminates whether or not the variable is a property.
		std::string n = ((p_prop) ? "\'" : "") + p_var.name;

        // Initializes the TMX node's variable list if undefined.
		if (p_node.vars == nullptr) {
			p_node.vars = new TList<sNamedVal>({n, p_var.myvalue});
			return true;
		}

        // Determine whether or not we're attempting to edit a variable.
		bool EDIT_FLAG = (p_var.myvalue.type == eType::error);

        // Check to see if the variable already exists.
		for(auto it = p_node.vars; it; it = it->next())
			if (it->value().name == n) {
                // Variable exists, not editing.
				if(!EDIT_FLAG)
					return false;

                // Variable exists, editing.
				it->value().myvalue = p_var.myvalue;
				return true;
			}

        // Variable does not exist, creating variable.
		p_node.vars->last()->append({n, p_var.myvalue});
		return true;
	}

	sVal getNodeVar(sNode& p_node, str_p p_name, bool p_prop) {
        // Makes sure the TMX node's variable set is initialized
		if(p_node.vars == nullptr)
			return {"!Node sets uninitialized", eType::error};

        // Deliminates whether or not the requested variable is a property.
		std::string n = ((p_prop) ? "\'" : "") + p_name;

        // Looks for requested variable to return.
		for(auto it = p_node.vars; it; it = it->next())
			if(it->value().name == n)
				return it->value().myvalue;

        // Requested variable not found in given TMX node.
		return {
                "!Reached end of node sets without finding specified set.",
                eType::error
            };
	}

	sNode load(str_p p_path) {
        // Load the TMX map from given file path.
		rapidxml::file<> file(p_path.c_str());
		rapidxml::xml_document<> document;
        // Parse the TMX map file into a DOM structure.
		document.parse<0>(file.data());

        // Find the root <map> tag.
		rapidxml::xml_node<>* map_node = document.first_node("map");
		sNode map = mkNode(eTag::map);

		// Defining attributes to look for in the <map> tag.
		std::vector<sVal> d_ma {
			{"version", eType::str},
            {"orientation", eType::str},
            {"renderorder", eType::str},
			{"width", eType::whole},
            {"height", eType::whole},
            {"tilewidth", eType::whole},
			{"tileheight", eType::whole},
            {"hexsidelength", eType::whole},
            {"staggeraxis", eType::str},
			{"staggerindex", eType::str},
            {"backgroundcolor", eType::hexcolor},
            {"nextobjectid", eType::whole}
		};

		// Load all <map> attributes.
		for (unsigned int i = 0; i < d_ma.size(); i++) {
			std::string attribute = xmlEvalAttr(map_node, d_ma.at(i).value);
			std::string attribute_default = tmxAttrDefault(
                                                map.tag, d_ma.at(i).value
                                            );

            // Assigns the attribute its given value.
			if(attribute != TMX_UNDEFINED_ATTRIBUTE)
				setNodeVar(
                    map,
                    mkVar(d_ma.at(i).value, attribute, d_ma.at(i).type)
                );
            // Assigns the attrbute its default value if no given value.
			else if(attribute_default != TMX_UNDEFINED_ATTRIBUTE)
				setNodeVar(
                    map,
                    mkVar(d_ma.at(i).value, attribute_default, d_ma.at(i).type)
                );
		}

        // Load <map> properties.
		xmlLoadNodeProps(map_node, map);
        // Load <map> child nodes.
		xmlLoadChildNodes(map_node, map);

		return map;
	}
}

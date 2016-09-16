/*
	http://www.hcidata.info/base64.htm
	BASE64 DECODING^
*/

#include <iostream>
using namespace std;

#include "tmx.h"
#include "tmx_utils.h"
using namespace tmx;

#include "tlist.hpp"

std::string tagToStr(tmx::eTag p_tag) {
	switch (p_tag) {
		case eTag::root:		return "root";
		case eTag::map:			return "map";
		case eTag::tileset:		return "tileset";
		case eTag::tileoffset:	return "tileoffset";
		case eTag::image:		return "image";
		case eTag::terrain:		return "terrain";
		case eTag::frame:		return "frame";
		case eTag::layer:		return "layer";
		case eTag::tile:		return "tile";
		case eTag::objectgroup:	return "objectgroup";
		case eTag::object:		return "object";
		case eTag::ellipse:		return "ellipse";
		case eTag::polygon:		return "polygon";
		case eTag::polyline:	return "polyline";
		case eTag::imagelayer:	return "imagelayer";
		case eTag::data:		return "data";
		default:				return "ignore";
	}
}

void dumpNode(sNode& p_node, unsigned int s = 0) {
	for(int sh = s; sh > 0; sh--)
		std::cout << "\t";

	std::cout << "<" << tagToStr(p_node.tag) << "> = ";
	if (p_node.data != nullptr)
		std::cout << "`" << p_node.data->value << "`" << std::endl;
	else
		std::cout << std::endl;

	for(auto vars = p_node.vars; vars != nullptr; vars = vars->next()) {					// print out all node variables
		std::string n = vars->valptr()->name;

		for(int sh = s; sh > 0; sh--)
			std::cout << "\t";
		std::cout << "\t" << n << ": " << getNodeVar(p_node, n).value << endl;
	}

	for(auto child = p_node.nodes; child != nullptr; child = child->next())
		dumpNode(*child->valptr(), (s + 1));
}

int main() {
	sNode tmxmap = load("media/map.tmx");
	dumpNode(tmxmap);

	//char* c = "TWFyeSBoYWQgYSBsaXR0bGUgbGFtYg==";
	//base64_decode(c, strlen(c));

	return 0;
}

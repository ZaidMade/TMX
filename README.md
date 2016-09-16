# TMX Map Loader
TMX is a minimalist C++ library for loading TMX map files created using the [Tiled Map Editor](http://www.mapeditor.org/).
Created according to the specifications found [here](http://doc.mapeditor.org/reference/tmx-map-format/).

---

##Dependencies:
TMX was created out of frustration caused by a lack of complete C++ TMX map
loaders that aren't dependent on other (much larger) libraries.
For this reason the number of dependencies is kept to a minimum.

* RapidXML 1.13

   TMX uses RapidXML version 1.13 which can be downloaded [here](http://rapidxml.sourceforge.net/).  
   Simply extract and copy **rapidxml.hpp** and **rapidxml_utils.hpp** into *src/* before compiling.

##Compiling:
**Note:** This entire section is subject to change with the first stable release. This is just how I've been doing it.

Compiled using [GCC v6.1.1](https://gcc.gnu.org/) on Fedora 24.
```Shell
g++ src/rapidxml.hpp src/rapidxml_utils.hpp src/tmx_utils.cpp src/tmx_core.cpp src/tmx.cpp src/main.cpp
```

---

##Usage:
Everything displayed here can be used by linking the compiled library and including *tmx.h* in your C++ project.

####Loading
```C++
tmx::tmxnode map_node(tmx::load("<FILE_PATH>"));
```

####Retrieving attributes
```C++
tmx::tmxnode::attr("<ATTRIBUTE_NAME>");
```

####Retrieving properties
```C++
tmx::tmxnode::prop("<ATTRIBUTE_NAME>");
```

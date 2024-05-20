#include "inputMaps.hpp"
#include <istream>

void marishin::inputMaps(std::istream& in, BinarySearchTree< std::string, BinarySearchTree< size_t, std::string > >& maps)
{
  while (!in.eof())
  {
    in.clear();
    BinarySearchTree< size_t, std::string > tempMap;
    std::string mapName = "";
    in >> mapName;
    size_t keyNumber = 0;
    while (in >> keyNumber)
    {
      std::string value = "";
      in >> value;
      tempMap.insert(keyNumber, value);
    }
    maps.insert(mapName, tempMap);
  }
}

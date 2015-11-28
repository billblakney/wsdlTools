#ifndef ENUM_HH_
#define ENUM_HH

#include <string>
#include <map>
#include <vector>

class Enum
{
public:

  Enum();

  Enum(std::string aName);

  virtual ~Enum();

  void addValues(std::vector<std::string> aValues);

  void addValue(std::string aValue);

  std::string toString();

  std::string _Name;

  std::vector<std::string> _Values;
};

#endif /* ENUM_HH_ */

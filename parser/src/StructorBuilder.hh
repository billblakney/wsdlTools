#ifndef STRUCTORUTIL_HH_
#define STRUCTORUTIL_HH_

#include <string>
#include <map>
#include <vector>

#include "Field.hh"
#include "Structure.hh"

using namespace std;

class StructorBuilder {
public:

	StructorBuilder();
	virtual ~StructorBuilder();

	void pushField(Field field);

	vector<Field> pullFields();

	void printFields();

	void clearFields();

	void onMatch(const char *aRuleName);

	void onField(string *aName,string *aType,int aIsPointer);

	void onStruct(std::string *aStructName);

	bool isPrimitive(std::string aType);

	std::vector<std::string> getStructNames();

	Structure *getStructure(std::string aType);

	std::string getDotString(std::string aName,std::string aPrefix);

	void postProcess();

	void printSummary();

	std::map<std::string,Structure *> _Structs;

private:

	vector<Field>  _Fields;
};

#endif /* STRUCTORUTIL_HH_ */

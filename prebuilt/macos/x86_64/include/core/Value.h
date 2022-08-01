#ifndef __VALUE_H__
#define __VALUE_H__

#include "core/igeSceneMacros.h"

#include <string>
#include <vector>
#include <unordered_map>

NS_IGE_SCENE_BEGIN

class Value;

typedef std::vector<Value> ValueVector;
typedef std::unordered_map<std::string, Value> ValueMap;
typedef std::unordered_map<int, Value> ValueMapIntKey;

class Value
{
public:
	static const Value Null;
	Value();

	explicit Value(unsigned char v);
	explicit Value(int v);
	explicit Value(unsigned int v);
	explicit Value(float v);
	explicit Value(double v);
	explicit Value(bool v);
	explicit Value(const char* v);
	explicit Value(const std::string& v);
	
	explicit Value(const ValueVector& v);
	explicit Value(ValueVector&& v);

	explicit Value(const ValueMap& v);
	explicit Value(ValueMap&& v);

	explicit Value(const ValueMapIntKey& v);
	explicit Value(ValueMapIntKey&& v);

	Value(const Value& other);
	Value(Value&& other);

	~Value();

	Value& operator= (const Value& other);
	Value& operator= (Value&& other);

	Value& operator= (unsigned char v);
	Value& operator= (int v);
	Value& operator= (unsigned int v);
	Value& operator= (float v);
	Value& operator= (double v);
	Value& operator= (bool v);
	Value& operator= (const char* v);
	Value& operator= (const std::string& v);

	Value& operator= (const ValueVector& v);
	Value& operator= (ValueVector&& v);

	Value& operator= (const ValueMap& v);
	Value& operator= (ValueMap&& v);

	Value& operator= (const ValueMapIntKey& v);
	Value& operator= (ValueMapIntKey&& v);

	bool operator!= (const Value& v);
	bool operator!= (const Value& v) const;

	bool operator== (const Value& v);
	bool operator== (const Value& v) const;

	unsigned char asByte() const;
	int asInt() const;
	unsigned int asUnsignedInt() const;
	float asFloat() const;
	double asDouble() const;
	bool asBool() const;
	std::string asString() const;

	ValueVector& asValueVector();
	const ValueVector& asValueVector() const;

	ValueMap& asValueMap();
	const ValueMap& asValueMap() const;

	ValueMapIntKey& asIntKeyMap();
	const ValueMapIntKey& asIntKeyMap() const;

	bool isNull() const { return m_type == Type::NONE; }

	enum class Type
	{
		/// no value is wrapped, an empty Value
		NONE = 0,
		/// wrap byte
		BYTE,
		/// wrap integer
		INTEGER,
		/// wrap unsigned
		UNSIGNED,
		/// wrap float
		FLOAT,
		/// wrap double
		DOUBLE,
		/// wrap bool
		BOOLEAN,
		/// wrap string
		STRING,
		/// wrap vector
		VECTOR,
		/// wrap ValueMap
		MAP,
		/// wrap ValueMapIntKey
		INT_KEY_MAP
	};

	Type getType() const { return m_type; }

private:
	void clear();
	void reset(Type type);

	union
	{
		unsigned char byteVal;
		int intVal;
		unsigned int unsignedVal;
		float floatVal;
		double doubleVal;
		bool boolVal;

		std::string* strVal;
		ValueVector* vectorVal;
		ValueMap* mapVal;
		ValueMapIntKey* intKeyMapVal;
	}m_field;

	Type m_type;
};

NS_IGE_SCENE_END

#endif
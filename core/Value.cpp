#include "core/Value.h"
#include <cmath>
#include <sstream>
#include <iomanip>

#ifndef FLT_EPSILON
    #define FLT_EPSILON __FLT_EPSILON__
#endif

#ifndef DBL_EPSILON
    #define DBL_EPSILON __DBL_EPSILON__
#endif

NS_IGE_SCENE_BEGIN

const Value Value::Null;

Value::Value() : m_type(Type::NONE)
{
	memset(&m_field, 0, sizeof(m_field));
}

Value::Value(unsigned char v) : m_type(Type::BYTE)
{
	m_field.byteVal = v;
}

Value::Value(int v) : m_type(Type::INTEGER)
{
	m_field.intVal = v;
}

Value::Value(unsigned int v) : m_type(Type::UNSIGNED)
{
	m_field.unsignedVal = v;
}

Value::Value(float v) : m_type(Type::FLOAT)
{
    m_field.floatVal = v;
}

Value::Value(double v) : m_type(Type::DOUBLE)
{
    m_field.doubleVal = v;
}

Value::Value(bool v) : m_type(Type::BOOLEAN)
{
    m_field.boolVal = v;
}

Value::Value(const char* v) : m_type(Type::STRING)
{
    m_field.strVal = new std::string();
    if (v)
    {
        *m_field.strVal = v;
    }
}

Value::Value(const std::string& v) : m_type(Type::STRING)
{
    m_field.strVal = new std::string();
    *m_field.strVal = v;
}

Value::Value(const ValueVector& v) : m_type(Type::VECTOR)
{
    m_field.vectorVal = new ValueVector();
    *m_field.vectorVal = v;
}

Value::Value(ValueVector&& v) : m_type(Type::VECTOR)
{
    m_field.vectorVal = new ValueVector();
    *m_field.vectorVal = std::move(v);
}

Value::Value(const ValueMap& v) : m_type(Type::MAP)
{
    m_field.mapVal = new ValueMap();
    *m_field.mapVal = v;
}

Value::Value(ValueMap&& v) : m_type(Type::MAP)
{
    m_field.mapVal = new ValueMap();
    *m_field.mapVal = std::move(v);
}

Value::Value(const ValueMapIntKey& v) : m_type(Type::INT_KEY_MAP)
{
    m_field.intKeyMapVal = new ValueMapIntKey();
    *m_field.intKeyMapVal = v;
}

Value::Value(ValueMapIntKey&& v) : m_type(Type::INT_KEY_MAP)
{
    m_field.intKeyMapVal = new ValueMapIntKey();
    *m_field.intKeyMapVal = std::move(v);
}

Value::Value(const Value& other) : m_type(Type::NONE)
{
    *this = other;
}

Value::Value(Value&& other) : m_type(Type::NONE)
{
    *this = std::move(other);
}

Value::~Value()
{
    clear();
}

Value& Value::operator= (const Value& other)
{
    if (this != &other) {
        reset(other.m_type);

        switch (other.m_type) {
        case Type::BYTE:
            m_field.byteVal = other.m_field.byteVal;
            break;
        case Type::INTEGER:
            m_field.intVal = other.m_field.intVal;
            break;
        case Type::UNSIGNED:
            m_field.unsignedVal = other.m_field.unsignedVal;
            break;
        case Type::FLOAT:
            m_field.floatVal = other.m_field.floatVal;
            break;
        case Type::DOUBLE:
            m_field.doubleVal = other.m_field.doubleVal;
            break;
        case Type::BOOLEAN:
            m_field.boolVal = other.m_field.boolVal;
            break;
        case Type::STRING:
            if (m_field.strVal == nullptr)
            {
                m_field.strVal = new std::string();
            }
            *m_field.strVal = *other.m_field.strVal;
            break;
        case Type::VECTOR:
            if (m_field.vectorVal == nullptr)
            {
                m_field.vectorVal = new ValueVector();
            }
            *m_field.vectorVal = *other.m_field.vectorVal;
            break;
        case Type::MAP:
            if (m_field.mapVal == nullptr)
            {
                m_field.mapVal = new ValueMap();
            }
            *m_field.mapVal = *other.m_field.mapVal;
            break;
        case Type::INT_KEY_MAP:
            if (m_field.intKeyMapVal == nullptr)
            {
                m_field.intKeyMapVal = new ValueMapIntKey();
            }
            *m_field.intKeyMapVal = *other.m_field.intKeyMapVal;
            break;
        default:
            break;
        }
    }
    return *this;
}

Value& Value::operator= (Value&& other)
{
    if (this != &other)
    {
        clear();
        switch (other.m_type)
        {
        case Type::BYTE:
            m_field.byteVal = other.m_field.byteVal;
            break;
        case Type::INTEGER:
            m_field.intVal = other.m_field.intVal;
            break;
        case Type::UNSIGNED:
            m_field.unsignedVal = other.m_field.unsignedVal;
            break;
        case Type::FLOAT:
            m_field.floatVal = other.m_field.floatVal;
            break;
        case Type::DOUBLE:
            m_field.doubleVal = other.m_field.doubleVal;
            break;
        case Type::BOOLEAN:
            m_field.boolVal = other.m_field.boolVal;
            break;
        case Type::STRING:
            m_field.strVal = other.m_field.strVal;
            break;
        case Type::VECTOR:
            m_field.vectorVal = other.m_field.vectorVal;
            break;
        case Type::MAP:
            m_field.mapVal = other.m_field.mapVal;
            break;
        case Type::INT_KEY_MAP:
            m_field.intKeyMapVal = other.m_field.intKeyMapVal;
            break;
        default:
            break;
        }
        m_type = other.m_type;

        memset(&other.m_field, 0, sizeof(other.m_field));
        other.m_type = Type::NONE;
    }

    return *this;
}

Value& Value::operator= (unsigned char v)
{
    reset(Type::BYTE);
    m_field.byteVal = v;
    return *this;
}

Value& Value::operator= (int v)
{
    reset(Type::INTEGER);
    m_field.intVal = v;
    return *this;
}

Value& Value::operator= (unsigned int v)
{
    reset(Type::UNSIGNED);
    m_field.unsignedVal = v;
    return *this;
}

Value& Value::operator= (float v)
{
    reset(Type::FLOAT);
    m_field.floatVal = v;
    return *this;
}

Value& Value::operator= (double v)
{
    reset(Type::DOUBLE);
    m_field.doubleVal = v;
    return *this;
}

Value& Value::operator= (bool v)
{
    reset(Type::BOOLEAN);
    m_field.boolVal = v;
    return *this;
}

Value& Value::operator= (const char* v)
{
    reset(Type::STRING);
    *m_field.strVal = v ? v : "";
    return *this;
}

Value& Value::operator= (const std::string& v)
{
    reset(Type::STRING);
    *m_field.strVal = v;
    return *this;
}

Value& Value::operator= (const ValueVector& v)
{
    reset(Type::VECTOR);
    *m_field.vectorVal = v;
    return *this;
}

Value& Value::operator= (ValueVector&& v)
{
    reset(Type::VECTOR);
    *m_field.vectorVal = std::move(v);
    return *this;
}

Value& Value::operator= (const ValueMap& v)
{
    reset(Type::MAP);
    *m_field.mapVal = v;
    return *this;
}

Value& Value::operator= (ValueMap&& v)
{
    reset(Type::MAP);
    *m_field.mapVal = std::move(v);
    return *this;
}

Value& Value::operator= (const ValueMapIntKey& v)
{
    reset(Type::INT_KEY_MAP);
    *m_field.intKeyMapVal = v;
    return *this;
}

Value& Value::operator= (ValueMapIntKey&& v)
{
    reset(Type::INT_KEY_MAP);
    *m_field.intKeyMapVal = std::move(v);
    return *this;
}

bool Value::operator!= (const Value& v)
{
    return !(*this == v);
}
bool Value::operator!= (const Value& v) const
{
    return !(*this == v);
}

bool Value::operator== (const Value& v)
{
    const auto& t = *this;
    return t == v;
}
bool Value::operator== (const Value& v) const
{
    if (this == &v) return true;
    if (v.m_type != this->m_type) return false;
    if (this->isNull()) return true;
    switch (m_type)
    {
    case Type::BYTE:    return v.m_field.byteVal == this->m_field.byteVal;
    case Type::INTEGER: return v.m_field.intVal == this->m_field.intVal;
    case Type::UNSIGNED:return v.m_field.unsignedVal == this->m_field.unsignedVal;
    case Type::BOOLEAN: return v.m_field.boolVal == this->m_field.boolVal;
    case Type::STRING:  return *v.m_field.strVal == *this->m_field.strVal;
    case Type::FLOAT:   return std::abs(v.m_field.floatVal - this->m_field.floatVal) <= FLT_EPSILON;
    case Type::DOUBLE:  return std::abs(v.m_field.doubleVal - this->m_field.doubleVal) <= DBL_EPSILON;
    case Type::VECTOR:
    {
        const auto& v1 = *(this->m_field.vectorVal);
        const auto& v2 = *(v.m_field.vectorVal);
        const auto size = v1.size();
        if (size == v2.size())
        {
            for (size_t i = 0; i < size; i++)
            {
                if (v1[i] != v2[i]) return false;
            }
            return true;
        }
        return false;
    }
    case Type::MAP:
    {
        const auto& map1 = *(this->m_field.mapVal);
        const auto& map2 = *(v.m_field.mapVal);
        for (const auto& kvp : map1)
        {
            auto it = map2.find(kvp.first);
            if (it == map2.end() || it->second != kvp.second)
            {
                return false;
            }
        }
        return true;
    }
    case Type::INT_KEY_MAP:
    {
        const auto& map1 = *(this->m_field.intKeyMapVal);
        const auto& map2 = *(v.m_field.intKeyMapVal);
        for (const auto& kvp : map1)
        {
            auto it = map2.find(kvp.first);
            if (it == map2.end() || it->second != kvp.second)
            {
                return false;
            }
        }
        return true;
    }
    default:
        break;
    };

    return false;
}

/// Convert value to a specified type
unsigned char Value::asByte() const
{
    if (m_type == Type::BYTE)
    {
        return m_field.byteVal;
    }

    if (m_type == Type::INTEGER)
    {
        return static_cast<unsigned char>(m_field.intVal);
    }

    if (m_type == Type::UNSIGNED)
    {
        return static_cast<unsigned char>(m_field.unsignedVal);
    }

    if (m_type == Type::STRING)
    {
        return static_cast<unsigned char>(atoi(m_field.strVal->c_str()));
    }

    if (m_type == Type::FLOAT)
    {
        return static_cast<unsigned char>(m_field.floatVal);
    }

    if (m_type == Type::DOUBLE)
    {
        return static_cast<unsigned char>(m_field.doubleVal);
    }

    if (m_type == Type::BOOLEAN)
    {
        return m_field.boolVal ? 1 : 0;
    }

    return 0;
}

int Value::asInt() const
{
    if (m_type == Type::INTEGER)
    {
        return m_field.intVal;
    }

    if (m_type == Type::UNSIGNED)
    {
        return (int)m_field.unsignedVal;
    }

    if (m_type == Type::BYTE)
    {
        return m_field.byteVal;
    }

    if (m_type == Type::STRING)
    {
        return atoi(m_field.strVal->c_str());
    }

    if (m_type == Type::FLOAT)
    {
        return static_cast<int>(m_field.floatVal);
    }

    if (m_type == Type::DOUBLE)
    {
        return static_cast<int>(m_field.doubleVal);
    }

    if (m_type == Type::BOOLEAN)
    {
        return m_field.boolVal ? 1 : 0;
    }

    return 0;
}


unsigned int Value::asUnsignedInt() const
{
    if (m_type == Type::UNSIGNED)
    {
        return m_field.unsignedVal;
    }

    if (m_type == Type::INTEGER)
    {
        return static_cast<unsigned int>(m_field.intVal);
    }

    if (m_type == Type::BYTE)
    {
        return static_cast<unsigned int>(m_field.byteVal);
    }

    if (m_type == Type::STRING)
    {
        // NOTE: strtoul is required (need to augment on unsupported platforms)
        return static_cast<unsigned int>(strtoul(m_field.strVal->c_str(), nullptr, 10));
    }

    if (m_type == Type::FLOAT)
    {
        return static_cast<unsigned int>(m_field.floatVal);
    }

    if (m_type == Type::DOUBLE)
    {
        return static_cast<unsigned int>(m_field.doubleVal);
    }

    if (m_type == Type::BOOLEAN)
    {
        return m_field.boolVal ? 1u : 0u;
    }

    return 0u;
}

float Value::asFloat() const
{
    if (m_type == Type::FLOAT)
    {
        return m_field.floatVal;
    }

    if (m_type == Type::BYTE)
    {
        return static_cast<float>(m_field.byteVal);
    }

    if (m_type == Type::STRING)
    {
        return atof(m_field.strVal->c_str());
    }

    if (m_type == Type::INTEGER)
    {
        return static_cast<float>(m_field.intVal);
    }

    if (m_type == Type::UNSIGNED)
    {
        return static_cast<float>(m_field.unsignedVal);
    }

    if (m_type == Type::DOUBLE)
    {
        return static_cast<float>(m_field.doubleVal);
    }

    if (m_type == Type::BOOLEAN)
    {
        return m_field.boolVal ? 1.0f : 0.0f;
    }

    return 0.0f;
}

double Value::asDouble() const
{
    if (m_type == Type::DOUBLE)
    {
        return m_field.doubleVal;
    }

    if (m_type == Type::BYTE)
    {
        return static_cast<double>(m_field.byteVal);
    }

    if (m_type == Type::STRING)
    {
        return static_cast<double>(atof(m_field.strVal->c_str()));
    }

    if (m_type == Type::INTEGER)
    {
        return static_cast<double>(m_field.intVal);
    }

    if (m_type == Type::UNSIGNED)
    {
        return static_cast<double>(m_field.unsignedVal);
    }

    if (m_type == Type::FLOAT)
    {
        return static_cast<double>(m_field.floatVal);
    }

    if (m_type == Type::BOOLEAN)
    {
        return m_field.boolVal ? 1.0 : 0.0;
    }

    return 0.0;
}

bool Value::asBool() const
{
    if (m_type == Type::BOOLEAN)
    {
        return m_field.boolVal;
    }

    if (m_type == Type::BYTE)
    {
        return m_field.byteVal == 0 ? false : true;
    }

    if (m_type == Type::STRING)
    {
        return (*m_field.strVal == "0" || *m_field.strVal == "false") ? false : true;
    }

    if (m_type == Type::INTEGER)
    {
        return m_field.intVal == 0 ? false : true;
    }

    if (m_type == Type::UNSIGNED)
    {
        return m_field.unsignedVal == 0 ? false : true;
    }

    if (m_type == Type::FLOAT)
    {
        return m_field.floatVal == 0.0f ? false : true;
    }

    if (m_type == Type::DOUBLE)
    {
        return m_field.doubleVal == 0.0 ? false : true;
    }

    return false;
}

std::string Value::asString() const
{
    if (m_type == Type::STRING)
    {
        return *m_field.strVal;
    }

    std::stringstream ret;

    switch (m_type)
    {
    case Type::BYTE:
        ret << m_field.byteVal;
        break;
    case Type::INTEGER:
        ret << m_field.intVal;
        break;
    case Type::UNSIGNED:
        ret << m_field.unsignedVal;
        break;
    case Type::FLOAT:
        ret << std::fixed << std::setprecision(7) << m_field.floatVal;
        break;
    case Type::DOUBLE:
        ret << std::fixed << std::setprecision(16) << m_field.doubleVal;
        break;
    case Type::BOOLEAN:
        ret << (m_field.boolVal ? "true" : "false");
        break;
    default:
        break;
    }
    return ret.str();
}

ValueVector& Value::asValueVector()
{
    return *m_field.vectorVal;
}

const ValueVector& Value::asValueVector() const
{
    return *m_field.vectorVal;
}

ValueMap& Value::asValueMap()
{
    return *m_field.mapVal;
}

const ValueMap& Value::asValueMap() const
{
    return *m_field.mapVal;
}

ValueMapIntKey& Value::asIntKeyMap()
{
    return *m_field.intKeyMapVal;
}

const ValueMapIntKey& Value::asIntKeyMap() const
{
    return *m_field.intKeyMapVal;
}

static std::string getTabs(int depth)
{
    std::string tabWidth;

    for (int i = 0; i < depth; ++i)
    {
        tabWidth += "\t";
    }

    return tabWidth;
}

static std::string visit(const Value& v, int depth);

static std::string visitVector(const ValueVector& v, int depth)
{
    std::stringstream ret;

    if (depth > 0)
        ret << "\n";

    ret << getTabs(depth) << "[\n";

    int i = 0;
    for (const auto& child : v)
    {
        ret << getTabs(depth + 1) << i << ": " << visit(child, depth + 1);
        ++i;
    }

    ret << getTabs(depth) << "]\n";

    return ret.str();
}

template <class T>
static std::string visitMap(const T& v, int depth)
{
    std::stringstream ret;

    if (depth > 0)
        ret << "\n";

    ret << getTabs(depth) << "{\n";

    for (auto& iter : v)
    {
        ret << getTabs(depth + 1) << iter.first << ": ";
        ret << visit(iter.second, depth + 1);
    }

    ret << getTabs(depth) << "}\n";

    return ret.str();
}

static std::string visit(const Value& v, int depth)
{
    std::stringstream ret;

    switch (v.getType())
    {
    case Value::Type::NONE:
    case Value::Type::BYTE:
    case Value::Type::INTEGER:
    case Value::Type::UNSIGNED:
    case Value::Type::FLOAT:
    case Value::Type::DOUBLE:
    case Value::Type::BOOLEAN:
    case Value::Type::STRING:
        ret << v.asString() << "\n";
        break;
    case Value::Type::VECTOR:
        ret << visitVector(v.asValueVector(), depth);
        break;
    case Value::Type::MAP:
        ret << visitMap(v.asValueMap(), depth);
        break;
    case Value::Type::INT_KEY_MAP:
        ret << visitMap(v.asIntKeyMap(), depth);
        break;
    default:
        break;
    }

    return ret.str();
}

void Value::clear()
{
    // Free memory the old value allocated
    switch (m_type)
    {
    case Type::BYTE:
        m_field.byteVal = 0;
        break;
    case Type::INTEGER:
        m_field.intVal = 0;
        break;
    case Type::UNSIGNED:
        m_field.unsignedVal = 0u;
        break;
    case Type::FLOAT:
        m_field.floatVal = 0.0f;
        break;
    case Type::DOUBLE:
        m_field.doubleVal = 0.0;
        break;
    case Type::BOOLEAN:
        m_field.boolVal = false;
        break;
    case Type::STRING:
        if (m_field.strVal) {
            delete m_field.strVal;
            m_field.strVal = nullptr;
        }
        break;
    case Type::VECTOR:
        if (m_field.vectorVal) {
            delete m_field.vectorVal;
            m_field.vectorVal = nullptr;
        }
        break;
    case Type::MAP:
        if (m_field.mapVal) {
            delete m_field.mapVal;
            m_field.mapVal = nullptr;
        }
        break;
    case Type::INT_KEY_MAP:
        if (m_field.intKeyMapVal) {
            delete m_field.intKeyMapVal;
            m_field.intKeyMapVal = nullptr;
        }
        break;
    default:
        break;
    }

    m_type = Type::NONE;
}

void Value::reset(Type type)
{
    if (m_type == type)
        return;

    clear();

    // Allocate memory for the new value
    switch (type)
    {
    case Type::STRING:
        m_field.strVal = new std::string();
        break;
    case Type::VECTOR:
        m_field.vectorVal = new ValueVector();
        break;
    case Type::MAP:
        m_field.mapVal = new ValueMap();
        break;
    case Type::INT_KEY_MAP:
        m_field.intKeyMapVal = new ValueMapIntKey();
        break;
    default:
        break;
    }

    m_type = type;
}

#define MAX_ITOA_BUFFER_SIZE 256
static double atof(const char* str)
{
    if (str == nullptr)
    {
        return 0.0;
    }

    char buf[MAX_ITOA_BUFFER_SIZE];
    strncpy(buf, str, MAX_ITOA_BUFFER_SIZE);

    // strip string, only remain 7 numbers after '.'
    char* dot = strchr(buf, '.');
    if (dot != nullptr && dot - buf + 8 < MAX_ITOA_BUFFER_SIZE)
    {
        dot[8] = '\0';
    }

    return ::atof(buf);
}

NS_IGE_SCENE_END
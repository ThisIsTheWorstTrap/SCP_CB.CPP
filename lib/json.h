#ifndef JSON_H
#define JSON_H

#include <string>
#include <map>
#include <vector>
#include <map>
#include <fstream>

class JsonValue;
class JsonObject;
class JsonArray;
class JsonString;
class JsonNumber;
class JsonBool;
class JsonNull;
class JsonData;
class StringBuffer;

JsonData *parseToJsonData(StringBuffer &buffer);

static bool parseError = false;
static std::string parseErrorString = "";

bool hasError()
{
    return parseError;
}

std::string getError()
{
    return parseErrorString;
}

enum class JsonType
{
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOL,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_NULL
};

class StringBuffer
{
public:
    inline StringBuffer() : str(""), index(0){};

    inline StringBuffer(const std::string &str)
        : str(str), index(0){};

    inline StringBuffer(const char *str)
        : str(std::string(str)), index(0)
    {
    }

    inline char next()
    {
        if (index >= str.length())
            return '\0';
        return str[index++];
    }

    inline char peek()
    {
        if (index >= str.length())
            return '\0';
        return str[index];
    }

    inline void skipWhitespace()
    {
        while (peek() == ' ' || peek() == '\n' || peek() == '\t' || peek() == '\r')
        {
            next();
        }
    }

private:
    const std::string &str;
    uint index;
};
static StringBuffer emptyBuffer;

inline bool isWhitespace(char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

inline bool isDelimiter(char c)
{

    return isWhitespace(c) || c == '}' || c == ']' || c == ',';
};

inline std::string parseString(StringBuffer &buffer)
{

    parseError = false;

    std::string str = "";
    char stringEnd;

    if (buffer.peek() != '"' && buffer.peek() != '\'')
    {
        parseError = true;
        return str;
    }

    stringEnd = buffer.next();

    while (buffer.peek() != stringEnd && buffer.peek() != '\0')
    {
        char next = buffer.next();

        if (next == '\\')
        {
            next = buffer.next();

            if (next == '\0')
            {
                parseError = true;
                return str;
            }

            std::string escaped = "\\" + std::string(1, next);
            str += escaped;
            continue;
        }

        str += next;
    }

    if (buffer.peek() == '\0')
    {
        parseError = true;
        return str;
    }

    buffer.next();

    return str;
}

inline double parseNumber(StringBuffer &buffer)
{

    parseError = false;

    std::string str = "";

    bool isNegative = false;
    bool isDecimal = false;

    if (buffer.peek() == '-')
    {
        isNegative = true;
        buffer.next();
    }

    while (!isDelimiter(buffer.peek()) && buffer.peek() != '\0')
    {
        char next = buffer.next();

        if (next == '.')
        {
            if (isDecimal)
            {
                parseError = true;
                return 0;
            }
            isDecimal = true;
        }

        str += next;
    }

    if (isNegative)
        return -std::stod(str);

    return std::stod(str);
}

inline bool parseBool(StringBuffer &buffer)
{

    parseError = false;

    std::string str = "";

    if (buffer.peek() == 't')
    {
        str += buffer.next();
        str += buffer.next();
        str += buffer.next();
        str += buffer.next();

        if (!isDelimiter(buffer.peek()) && buffer.peek() != '\0')
        {
            parseError = true;
            parseError =
                printf("Expected value [true] followed by [delimiter] or ['\\0']. Got [%s] followed by [%c]", str.data(), buffer.peek());
            return false;
        }

        if (str == "true")
            return true;

        parseError = true;
        parseError = printf("| Expected [true] got [%s]. |", str.data());
        return false;
    }

    if (buffer.peek() == 'f')
    {
        str += buffer.next();
        str += buffer.next();
        str += buffer.next();
        str += buffer.next();
        str += buffer.next();

        if (!isDelimiter(buffer.peek()) && buffer.peek() != '\0')
        {
            parseError = true;
            parseError =
                printf("| Expected value [false] followed by [delimiter]. Got [%s] followed by [%c]. |", str.data(), buffer.peek());
            return false;
        }

        if (str == "false")
            return false;

        parseError = true;
        return false;
    }

    parseError = true;
    parseError = printf("| Expected [true] got [%s]. |", str.data());
    return false;
}

bool parseNull(StringBuffer &buffer)
{

    parseError = false;

    std::string str = "";

    str += buffer.next();
    str += buffer.next();
    str += buffer.next();
    str += buffer.next();

    if (!isDelimiter(buffer.peek()) && buffer.peek() != '\0')
    {
        parseError = true;
        return false;
    }

    if (str == "null")
        return true;

    parseError = true;
    return false;
}

class JsonData
{
public:
    JsonData() {}

    virtual std::string asString()
    {
        return "";
    };

    virtual double asNumber()
    {
        return 0;
    };

    virtual bool asBool()
    {
        return false;
    };

    virtual JsonObject *asObject()
    {
        return nullptr;
    };

    virtual std::vector<JsonData *> *asArray()
    {
        return nullptr;
    };

    virtual JsonData *operator[](const std::string &key)
    {
        return nullptr;
    };

    virtual JsonData *operator[](int index)
    {
        return nullptr;
    };

    virtual JsonData *get(const std::string &key)
    {
        return nullptr;
    };

    virtual JsonData *get(int index)
    {
        return nullptr;
    };

    virtual JsonData *set(const std::string &key, JsonData *data)
    {
        return nullptr;
    };

    virtual JsonData *set(int index, JsonData *data)
    {
        return nullptr;
    };

    virtual void push(JsonData *data){};

    virtual JsonData *pop()
    {
        return nullptr;
    };

    virtual int size()
    {
        return 0;
    };

    virtual JsonType getType()
    {
        return JsonType::JSON_NULL;
    };

    virtual std::string emit()
    {
        return "";
    }

    virtual void operator=(JsonData *data)
    {
    }

    virtual void operator=(std::string str)
    {
    }

    virtual void operator=(double num)
    {
    }

    virtual void operator=(bool b)
    {
    }

    virtual ~JsonData(){};
};

class JsonString : public JsonData
{
public:
    inline JsonString(std::string str) : buffer(emptyBuffer), str(str){};

    inline JsonString(StringBuffer &buffer) : buffer(buffer)
    {
        str = parseString(buffer);
        if (parseError)
        {
            parseErrorString = "Error parsing string";
        }
    };

    inline std::string asString() override
    {
        return str;
    };

    inline JsonType getType() override
    {
        return JsonType::JSON_STRING;
    };

    inline void operator=(std::string str) override
    {
        this->str = str;
    }

    inline std::string emit() override
    {
        return "\"" + str + "\"";
    }

private:
    StringBuffer &buffer;
    std::string str;
};

class JsonNumber : public JsonData
{
public:
    inline JsonNumber(double num) : buffer(emptyBuffer), num(num){};

    inline JsonNumber(StringBuffer &buffer) : buffer(buffer)
    {
        num = parseNumber(buffer);
        if (parseError)
        {
            parseErrorString = "Error parsing number";
        }
    };

    inline double asNumber() override
    {
        return num;
    };

    inline JsonType getType() override
    {
        return JsonType::JSON_NUMBER;
    };

    inline void operator=(double num) override
    {
        this->num = num;
    }

    inline std::string emit() override
    {
        return std::to_string(num);
    }

private:
    StringBuffer &buffer;
    double num;
};

class JsonBool : public JsonData
{
public:
    inline JsonBool(bool b) : buffer(emptyBuffer), b(b){};

    inline JsonBool(StringBuffer &buffer) : buffer(buffer)
    {
        b = parseBool(buffer);
        if (parseError)
        {
            parseErrorString = printf("| Error parsing bool. Reason: %s. |", parseErrorString.data());
        }
    };

    inline bool asBool() override
    {
        return b;
    };

    inline JsonType getType() override
    {
        return JsonType::JSON_BOOL;
    };

    inline void operator=(bool b) override
    {
        this->b = b;
    }

    inline std::string emit() override
    {
        if (b)
            return "true";
        return "false";
    }

private:
    StringBuffer &buffer;
    bool b;
};

class JsonNull : public JsonData
{

public:
    inline JsonNull() : buffer(emptyBuffer){};

    inline JsonNull(StringBuffer &buffer) : buffer(buffer)
    {
        if (!parseNull(buffer))
        {
            parseError = true;
            parseErrorString = "Error parsing null";
        }
    };

    inline JsonType getType() override
    {
        return JsonType::JSON_NULL;
    };

    inline std::string emit() override
    {
        return "null";
    }

private:
    StringBuffer &buffer;
};

class JsonArray : public JsonData
{
public:
    inline JsonArray() : buffer(emptyBuffer){};

    inline JsonArray(StringBuffer &buffer) : buffer(buffer)
    {

        buffer.skipWhitespace(); // skip whitespace

        if (buffer.next() != '[')
        {
            parseError = true;
            parseErrorString = "Error parsing array";
            return;
        }

        buffer.skipWhitespace(); // skip whitespace

        while (buffer.peek() != ']')
        {
            data.push_back(parseToJsonData(buffer));
            if (parseError)
            {
                parseErrorString = "Error parsing array";
                return;
            }
            buffer.skipWhitespace(); // skip whitespace

            if (buffer.peek() == ',')
            {
                buffer.next();
            }
            else if (buffer.peek() != ']')
            {
                parseError = true;
                parseErrorString = "Error parsing array";
                return;
            }
            else
            {
                break;
            }

            buffer.skipWhitespace();
        }

        buffer.next(); // skip ']'
    };

    inline std::vector<JsonData *> *asArray() override
    {
        return &data;
    };

    inline JsonData *operator[](int index) override
    {
        return data[index];
    };

    inline JsonData *get(int index) override
    {
        return data[index];
    };

    inline JsonType getType() override
    {
        return JsonType::JSON_ARRAY;
    };

    inline int size() override
    {
        return data.size();
    };

    inline void push(JsonData *data) override
    {
        this->data.push_back(data);
    };

    inline JsonData *pop() override
    {
        JsonData *data = this->data.back();
        this->data.pop_back();
        return data;
    };

    inline ~JsonArray() override
    {
        for (auto &d : data)
        {
            delete d;
        }
    };

    inline std::string emit() override
    {
        std::string str = "[";
        for (int i = 0; i < data.size(); i++)
        {
            str += data[i]->emit();
            if (i != data.size() - 1)
                str += ",";
        }
        str += "]";
        return str;
    }

private:
    StringBuffer &buffer;
    std::vector<JsonData *> data;
};

class JsonObject : public JsonData
{

public:
    inline JsonObject() : buffer(emptyBuffer){};

    inline JsonObject(StringBuffer &buffer) : buffer(buffer)
    {
        parseObject(buffer);
    };

    inline JsonData *operator[](const std::string &key) override
    {
        return data[key];
    };

    inline JsonData *get(const std::string &key) override
    {
        return data[key];
    };

    inline JsonData *set(const std::string &key, JsonData *value) override
    {
        data[key] = value;
        return value;
    };

    inline JsonType getType() override
    {
        return JsonType::JSON_OBJECT;
    };

    inline int size() override
    {
        return data.size();
    };

    inline JsonObject *asObject() override
    {
        return this;
    };

    inline ~JsonObject() override
    {
        for (auto &d : data)
        {
            delete d.second;
        }
    };

    inline std::string emit() override
    {
        std::string str = "{";
        int i = 0;
        for (auto &d : data)
        {
            str += "\"" + d.first + "\":" + d.second->emit();
            if (i != data.size() - 1)
                str += ",";
            i++;
        }
        str += "}";
        return str;
    }

private:
    inline void parseObject(StringBuffer &buffer)
    {

        buffer.skipWhitespace(); // skip whitespace

        if (buffer.next() != '{')
        {
            parseError = true;
            parseErrorString = "Error parsing object. Expected {";
            return;
        }

        buffer.skipWhitespace(); // skip whitespace

        while (buffer.peek() != '}')
        {
            std::string key = parseString(buffer);
            if (parseError)
            {
                parseErrorString = printf("Error parsing object. Invalid Key. | %s", parseErrorString.data());
                return;
            }

            buffer.skipWhitespace(); // skip whitespace

            if (buffer.next() != ':')
            {
                parseError = true;
                parseErrorString = "Error parsing object. Expected ':' character between key and value.";
                return;
            }

            buffer.skipWhitespace(); // skip whitespace

            data[key] = parseToJsonData(buffer);
            if (parseError)
            {
                parseErrorString = printf("Error parsing object. Value error for key=[%s]. | %s", key.data(), parseErrorString.data());
                return;
            }

            buffer.skipWhitespace(); // skip whitespace

            if (buffer.peek() == ',')
            {
                buffer.next();
            }
            else if (buffer.peek() != '}')
            {
                parseError = true;
                parseErrorString = "Error parsing object. Expected ending '}'";
                return;
            }
            else
            {
                break;
            }

            buffer.skipWhitespace();
        }

        buffer.next(); // skip '}'
    }

    StringBuffer &buffer;
    std::map<std::string, JsonData *> data;
};

#define JSON_DATA_CASE(value, type) \
    case value:                     \
        return new type(buffer);

inline JsonData *parseToJsonData(StringBuffer &buffer)
{

    parseError = false;

    buffer.skipWhitespace(); // skip whitespace

    char next = buffer.peek();

    switch (next)
    {
        JSON_DATA_CASE('"', JsonString);
        JSON_DATA_CASE('t', JsonBool);
        JSON_DATA_CASE('f', JsonBool);
        JSON_DATA_CASE('n', JsonNull);
        JSON_DATA_CASE('{', JsonObject);
        JSON_DATA_CASE('[', JsonArray);
        JSON_DATA_CASE('-', JsonNumber);
        JSON_DATA_CASE('0', JsonNumber);
        JSON_DATA_CASE('1', JsonNumber);
        JSON_DATA_CASE('2', JsonNumber);
        JSON_DATA_CASE('3', JsonNumber);
        JSON_DATA_CASE('4', JsonNumber);
        JSON_DATA_CASE('5', JsonNumber);
        JSON_DATA_CASE('6', JsonNumber);
        JSON_DATA_CASE('7', JsonNumber);
        JSON_DATA_CASE('8', JsonNumber);
        JSON_DATA_CASE('9', JsonNumber);

    default:
        parseError = true;
        parseErrorString = printf("Invalid character found: %c", next);
        return nullptr;
    }
}

inline JsonData *JSON(const std::string &str)
{
    StringBuffer buffer(str);
    return parseToJsonData(buffer);
}

inline JsonData *JSON(const char *str)
{
    std::string s(str);
    StringBuffer buffer(s);
    return parseToJsonData(buffer);
}

inline JsonData *JSON(const char *str, int len)
{

    std::string s(str, len);
    StringBuffer buffer(s);
    return parseToJsonData(buffer);
}

inline JsonData *JSON(std::ifstream &file)
{
    std::string str((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
    StringBuffer buffer(str);
    return parseToJsonData(buffer);
}

inline JsonData *JSON(std::istream &stream)
{
    std::string str((std::istreambuf_iterator<char>(stream)),
                    std::istreambuf_iterator<char>());
    StringBuffer buffer(str);
    return parseToJsonData(buffer);
}

inline JsonData *JSON_loadf(std::string filename)
{
    std::ifstream file(filename);
    return JSON(file);
}

inline void JSON_dumpf(JsonData *data, std::string filename)
{
    std::ofstream file(filename);
    file << data->emit();
    file.close();
}

inline std::string JSON_emit(JsonData *data)
{
    return data->emit();
}

JsonData *toJsonData(const std::string &str)
{
    return new JsonString(str);
}

JsonData *toJsonData(const char *str)
{
    return new JsonString(str);
}

JsonData *toJsonData(double number)
{
    return new JsonNumber(number);
}

JsonData *toJsonData(int number)
{
    return new JsonNumber(number);
}

JsonData *toJsonData(bool boolean)
{
    return new JsonBool(boolean);
}

#undef JSON_DATA_CASE

#endif
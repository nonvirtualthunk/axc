//
// Created by Sam Bock on 9/3/16.
//

#ifndef AXM_PARSER_H
#define AXM_PARSER_H

#include <string>
#include <core/ArxString.h>
#include <containers/ArxMap.h>
#include <core/ArxFile.h>
#include <Noto.h>

enum class AxmNodeType {
    Number,
    Bool,
    String,
    Array,
    Tuple,
    Object,
    Empty
};

struct AxmNode {
    AxmNodeType type;

    bool isEmpty() const { return type == AxmNodeType::Empty; }
    bool nonEmpty() const { return ! isEmpty(); }
    virtual bool asBool() const;
    virtual int asInt() const;
    virtual Arx::String asStr() const;
    virtual double asDouble() const;
    virtual float asFloat() const;
    virtual const AxmNode& child(const Arx::String& name) const;
    virtual const AxmNode& operator[](int i) const;
    virtual const int size() const;
    const AxmNode& operator()(const Arx::String& name) const {
        return child(name);
    }
    const AxmNode& operator[](const Arx::String& name) const {
        return child(name);
    }
    const AxmNode& operator>(const Arx::String& name) const {
        return child(name);
    }
    const AxmNode& operator%(const Arx::String& name) const {
        return child(name);
    }
    const AxmNode& operator%(const char *name) const {
        return child(name);
    }

    void operator>>(float& fvar) const;
    void operator>>(int& ivar) const;
    void operator>>(double& dvar) const;
    void operator>>(bool& dvar) const;
    void operator>>(Arx::String& svar) const;
    template<typename T>
    void operator>>(Arx::Sequence<T>& fvar) const {
        for (int i = 0; i < size(); ++i) {
            const AxmNode& elem = (*this)[i];
            T tmp;
            elem >> tmp;
            fvar.add(tmp);
        }
    }

    static std::shared_ptr<AxmNode> emptyNode;

    AxmNode(AxmNodeType type);
};

struct AxmValueNode : public AxmNode{
    union {
        double number;
        bool boolean;
    };

    AxmValueNode(double n);
    AxmValueNode(bool b);

    virtual bool asBool() const override;
    virtual int asInt() const override;
    virtual double asDouble() const override;
    virtual float asFloat() const override;
};

struct AxmObjectNode : public AxmNode {
    Arx::Map<std::string, std::shared_ptr<AxmNode>> children;

    AxmObjectNode();

    virtual const AxmNode& child(const Arx::String &name) const override;
};

struct AxmArrayNode : public AxmNode {
    Arx::Sequence<std::shared_ptr<AxmNode>> elements;

    AxmArrayNode();

    virtual const AxmNode& operator[](int i) const override;

    virtual const int size() const override;
};

struct AxmStringNode : public AxmNode {
    Arx::String string;

    AxmStringNode(const Arx::String &string);

    virtual Arx::String asStr() const override;
};

class ParsingContext;
class ParseState;

class AxmParser {
public:
	static std::shared_ptr<AxmObjectNode> parse(const Arx::File& file);
    static std::shared_ptr<AxmObjectNode> parse(ParsingContext& context, Arx::String raw);

    static void parsePreamble(ParseState& state);
    static void parseObject(ParseState& state, std::shared_ptr<AxmObjectNode> intoNode);
    static void parseArray(ParseState& state, std::shared_ptr<AxmArrayNode> intoNode);
    static void parseContainer(ParseState& state, std::shared_ptr<AxmNode> container, AxmNodeType type);
};


#endif //AXM_PARSER_H

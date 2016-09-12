//
// Created by Sam Bock on 9/3/16.
//

#include <core/Predef.h>
#include <Noto.h>
#include <application/ResourceManager.h>
#include <core/Parsing.h>
#include "AxmParser.h"

struct ParseState : public ParseStateBase {
    ParseState(String &raw, ParsingContext &context) : ParseStateBase(raw, context) {}

    std::shared_ptr<AxmObjectNode> topLevelNode = std::make_shared<AxmObjectNode>();
};


void AxmParser::parsePreamble(ParseState& state) {
    while (state.hasNext()) {
        auto c = state.next();

        if (c == '{') {
            return;
        } else if (c == 'i') {
            auto statement = state.takeUntil(' ',-1);
            if (statement == "include") {
                state.advanceUntil('"');
                auto includeFilePath = state.takeUntil('"',0);
                auto includeFile = state.context.referenceDirectory.relativeFile(includeFilePath);
                if (!includeFile.exists()) {
                    includeFile = state.context.rootDirectory.relativeFile(includeFilePath);
                }
                if (includeFile.exists()) {
                    Arx::String includeText = ResourceManager::loadTextFile(includeFile);

                    auto includedTopLevel = parse(state.context, includeText);
                    includedTopLevel->children.foreach([&](Arx::String k,std::shared_ptr<AxmNode> v) {
                        state.topLevelNode->children.put(k,v);
                    });
                } else {
                    state.addError(Format::format(
                            "include directive \"{}\"could not locate matching file at either\n\trootDir: {}\n\trelativeDir: {}",
                            includeFilePath, state.context.rootDirectory, state.context.referenceDirectory));
                }
            }
        } else {
            state.addError("axm file preamble contained something other than include statements and a top level object");
        }
    }
}

std::shared_ptr<AxmNode> valueToNode(Arx::String& value) {
    std::shared_ptr<AxmNode> node;
    auto dopt = value.doubleValue();
    if (dopt.isPresent()) {
        node = std::make_shared<AxmValueNode>(dopt.get());
    } else {
        auto bopt = value.boolValue();
        if (bopt.isPresent()) {
            node = std::make_shared<AxmValueNode>(bopt.get());
        } else {
            node = std::make_shared<AxmStringNode>(value);
        }
    }
    return node;
}

void AxmParser::parseContainer(ParseState& state, std::shared_ptr<AxmNode> container, AxmNodeType type) {
    Arx::String buffer;
    Arx::String activeKey;
    bool inKey = true;
    bool quoted = false;
    bool escaped = false;

    AxmObjectNode* intoNode = (type == AxmNodeType::Object) ? ((AxmObjectNode*)container.get()) : nullptr;
    AxmArrayNode* arrayNode = (type == AxmNodeType::Array) ? ((AxmArrayNode*)container.get()) : nullptr;

    auto addNode = [&](const std::shared_ptr<AxmNode>& newNode) {
        if (type == AxmNodeType::Object) {
            intoNode->children.put(activeKey, newNode);
        } else {
            arrayNode->elements.add(newNode);
        }
    };

    while (state.hasNext()) {
        auto c = state.next();

        if (escaped) {
            buffer.append(c);
            escaped = false;
        } else if (quoted) {
            if (c == '"') {
                quoted = false;
            } else {
                buffer.append(c);
            }
        } else {
            if (c == '{') {
                auto newNode = std::make_shared<AxmObjectNode>();
                parseObject(state, newNode);
                addNode(newNode);
                state.advanceUntil('\n',';');
                activeKey.clear();
                inKey = true;
            } else if (c == '}') {
                if (type == AxmNodeType::Object) {
                    return;
                } else {
                    state.addError("Object end marker found in array, likely an unpaired ] or errant {");
                }
            } else if (c == '[') {
                auto newNode = std::make_shared<AxmArrayNode>();
                parseArray(state, newNode);
                addNode(newNode);
                inKey = true;
            } else if (c == '"') {
                quoted = true;
            } else if (c == '\\') {
                escaped = true;
            } else if (c == ':' || c == '=') {
                if (! inKey) {
                    Arx::String err("'");
                    err.append(c);
                    err.append("' encountered when not parsing key");
                    state.addError(err);
                } else if (type == AxmNodeType::Array) {
                    state.addError("Key identifier found while parsing array");
                }
                activeKey = buffer.trimmed();
                buffer.clear();
                inKey = false;
            } else if (c == '\n' || c == ';' || c == ',' || c == ']') {
                if ((c == ',' || c == ']') && type != AxmNodeType::Array) {
                    state.addError("Encountered array control character while parsing non-array field");
                } else {
                    Arx::String value = buffer.trimmed();
                    if (value.nonEmpty()) {
                        auto newNode = valueToNode(value);
                        addNode(newNode);
                        activeKey.clear();
                        buffer.clear();
                    }

                    inKey = true;

                    if (c == ']' && type == AxmNodeType::Array) {
                        return;
                    }
                }
            } else {
                buffer.append(c);
            }
        }
    }
}

void AxmParser::parseObject(ParseState &state, std::shared_ptr<AxmObjectNode> intoNode) {
    parseContainer(state, intoNode, AxmNodeType::Object);
}

void AxmParser::parseArray(ParseState &state, std::shared_ptr<AxmArrayNode> intoNode) {
    parseContainer(state, intoNode, AxmNodeType::Array);
}

std::shared_ptr<AxmObjectNode> AxmParser::parse(ParsingContext& context, Arx::String raw) {
    ParseState state(raw, context);

    parsePreamble(state);
    parseObject(state, state.topLevelNode);

    return state.topLevelNode;
}

std::shared_ptr<AxmObjectNode> AxmParser::parse(const Arx::File &file) {
    ParsingContext context;
	context.referenceDirectory = file.parent();
	context.rootDirectory = file.parent();

	return parse(context, file.readAllText());
}


AxmValueNode::AxmValueNode(double n) : AxmNode(AxmNodeType::Number) {
    number = n;
}

AxmValueNode::AxmValueNode(bool b) : AxmNode(AxmNodeType::Bool) {
    boolean = b;
}

bool AxmValueNode::asBool() const {
    return type == AxmNodeType::Bool ? boolean : AxmNode::asBool();
}

int AxmValueNode::asInt() const {
    return type == AxmNodeType::Number ? (int)number : AxmNode::asInt();
}

double AxmValueNode::asDouble() const {
    return type == AxmNodeType::Number ? number : AxmNode::asDouble();
}

float AxmValueNode::asFloat() const {
    return type == AxmNodeType::Number ? (float)number : AxmNode::asFloat();
}

AxmObjectNode::AxmObjectNode() : AxmNode(AxmNodeType::Object) {}

const AxmNode& AxmObjectNode::child(const Arx::String &name) const {
    return *(children.get(name).orElse(AxmNode::emptyNode));
}

AxmStringNode::AxmStringNode(const Arx::String &string) : AxmNode(AxmNodeType::String), string(string) {}

Arx::String AxmStringNode::asStr() const {
    return string;
}



std::shared_ptr<AxmNode> AxmNode::emptyNode = std::make_shared<AxmNode>(AxmNodeType::Empty);

AxmNode::AxmNode(AxmNodeType type) : type(type) {}

#define wrongType(rtype, ret) Noto::error("Attempted to access value as {}, but was {}",#rtype, (int)type); return ret

bool AxmNode::asBool() const { wrongType(bool, false); }

int AxmNode::asInt() const { wrongType(int, 0); }

Arx::String AxmNode::asStr() const { wrongType(std::string, ""); }

double AxmNode::asDouble() const { wrongType(double, 0.0); }

float AxmNode::asFloat() const { wrongType(float, 0.0f); }

const AxmNode& AxmNode::child(const Arx::String &name) const { wrongType(Object, *emptyNode); }

const AxmNode& AxmNode::operator[](int i) const { wrongType(Array, *emptyNode); }

void AxmNode::operator>>(float &fvar) const {
    if (nonEmpty()) {
        fvar = this->asFloat();
    }
}

const int AxmNode::size() const {
    wrongType(Array, 0);
}

void AxmNode::operator>>(int &ivar) const {
    if (nonEmpty()) {
        ivar = this->asInt();
    }
}

void AxmNode::operator>>(double &dvar) const {
    if (nonEmpty()) {
        dvar = this->asDouble();
    }
}

void AxmNode::operator>>(bool &bvar) const {
    if (nonEmpty()) {
        bvar = this->asBool();
    }
}

void AxmNode::operator>>(Arx::String &svar) const {
    if (nonEmpty()) {
        svar = this->asStr();
    }
}

AxmArrayNode::AxmArrayNode() : AxmNode(AxmNodeType::Array) {}

const AxmNode& AxmArrayNode::operator[](int i) const {
    return *elements[i];
}

const int AxmArrayNode::size() const {
    return elements.size();
}

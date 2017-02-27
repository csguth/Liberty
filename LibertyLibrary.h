#ifndef LIBERTY_LIBRARY_H
#define LIBERTY_LIBRARY_H

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

namespace liberty
{

class Cell;

namespace ast
{

struct SimpleAttribute
{
    using Value = boost::variant<double, std::string, int>;
    std::string name;
    Value value;
};

struct ComplexAttribute
{
    std::string name;
    std::vector<SimpleAttribute::Value> values;
};

enum class AttributeType {
    String,
    Boolean,
    Integer,
    FloatingPoint
};

struct DefineStatement
{
    std::string attribute_name;
    std::string group_name;
    AttributeType attribute_type;
};

struct GroupStatement;
using GroupStatementAst = boost::spirit::x3::forward_ast<GroupStatement>;
struct AttributeStatement : boost::spirit::x3::variant<DefineStatement, SimpleAttribute, ComplexAttribute, GroupStatementAst>
{
    using base_type::base_type;
    using base_type::operator=;
};

struct GroupStatement
{
    std::string group_name;
    std::string name;
    std::vector<AttributeStatement> children;
};

using Library = GroupStatementAst;
class Visitor final
{
public:
    using result_type = void;
    result_type operator()(const ast::GroupStatementAst& group) const;
    result_type operator()(const ast::SimpleAttribute& attr) const;
    result_type operator()(const ast::ComplexAttribute&) const;
    result_type operator()(const ast::DefineStatement&) const;

    void onCell(std::function<void(const Cell&)>&& callback);
private:
    std::function<void(const Cell&)> onCell_;
};

}

class Cell final
{
public:
    friend class ast::Visitor;
    Cell();
    ~Cell();
    std::string name() const;
private:
    struct Impl;
    std::unique_ptr<Impl> this_;
};


}

#endif // LIBERTY_LIBRARY_H

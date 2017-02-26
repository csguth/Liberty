#ifndef LIBERTY_LIBRARY_H
#define LIBERTY_LIBRARY_H

#include <string>
#include <vector>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/io.hpp>

namespace liberty
{
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



}

struct Library
{
};

}

#endif // LIBERTY_LIBRARY_H

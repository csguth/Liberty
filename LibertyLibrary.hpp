/*
Copyright 2017 Liberty Parser

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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

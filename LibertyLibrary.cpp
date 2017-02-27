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

#include "LibertyLibrary.hpp"

namespace liberty
{

struct Cell::Impl
{
    const ast::GroupStatement* group{nullptr};
};

Cell::Cell() :
    this_(std::make_unique<Impl>())
{

}

Cell::~Cell()
{

}

std::string Cell::name() const
{
    Impl* this__ = this_.get();
    return this__->group->name;
}

ast::Visitor::result_type ast::Visitor::operator()(const ast::GroupStatementAst& group) const
{
    if(group.get().group_name == "cell")
    {
        Cell c;
        c.this_->group = group.get_pointer();
        onCell_(c);
    }
    for(ast::AttributeStatement const& child : group.get().children)
    {
        boost::apply_visitor(*this, child);
    }
}

ast::Visitor::result_type ast::Visitor::operator()(const ast::SimpleAttribute&) const
{
}

ast::Visitor::result_type ast::Visitor::operator()(const ast::ComplexAttribute&) const
{

}

ast::Visitor::result_type ast::Visitor::operator()(const ast::DefineStatement&) const
{

}

void ast::Visitor::onCell(std::function<void (const Cell &)> &&callback)
{
    onCell_ = callback;
}

}

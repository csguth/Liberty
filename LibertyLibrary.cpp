#include "LibertyLibrary.h"

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

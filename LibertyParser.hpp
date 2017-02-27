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

#ifndef LIBERTYGRAMMAR_PRIVATE_H
#define LIBERTYGRAMMAR_PRIVATE_H

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/operator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "LibertyLibrary.hpp"

BOOST_FUSION_ADAPT_STRUCT(
    liberty::ast::GroupStatement,
    (std::string, group_name)
    (std::string, name)
    (std::vector<liberty::ast::AttributeStatement>, children)
)

BOOST_FUSION_ADAPT_STRUCT(
    liberty::ast::SimpleAttribute,
    (std::string, name)
    (liberty::ast::SimpleAttribute::Value, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    liberty::ast::ComplexAttribute,
    (std::string, name)
    (std::vector<liberty::ast::SimpleAttribute::Value>, values)
)

BOOST_FUSION_ADAPT_STRUCT(
    liberty::ast::DefineStatement,
    (std::string, attribute_name)
    (std::string, group_name)
    (liberty::ast::AttributeType, attribute_type)
)

namespace liberty
{
namespace ast
{

    namespace x3 = boost::spirit::x3;
    namespace ascii = x3::ascii;

    using x3::double_;
    using x3::int_;
    using x3::lexeme;
    using x3::lit;

    using ascii::char_;

    struct attribute_type_table : x3::symbols<AttributeType> {
        attribute_type_table() {
            add ("Boolean" , AttributeType::Boolean)
                ("integer" , AttributeType::Integer)
                ("float" , AttributeType::FloatingPoint)
                ("string"   , AttributeType::String)
            ;
        }
    } const attribute;


    x3::real_parser<double, x3::strict_real_policies<double> > strictDouble;
    x3::rule<class name, std::string> name = "name";
    x3::rule<class nonQuotedString, std::string> nonQuotedString = "nonQuotedString";
    x3::rule<class quotedString, std::string> quotedString = "quotedString";
    x3::rule<class stringValue, std::string> stringValue = "stringValue";
    x3::rule<class value, SimpleAttribute::Value> value = "value";
    x3::rule<class simpleAttribute, SimpleAttribute> simpleAttribute = "simpleAttribute";
    x3::rule<class complexAttribute, ComplexAttribute> complexAttribute = "complexAttribute";
    x3::rule<class defineStatement, DefineStatement> defineStatement = "defineStatement";
    x3::rule<class attributeStatement, AttributeStatement> attributeStatement = "attribute";
    x3::rule<class attributeList, std::vector<AttributeStatement> > attributeList = "attributeList";
    x3::rule<class groupStatement, GroupStatement> groupStatement = "groupStatement";

    const auto name_def                 = (x3::char_("A-Za-z_") >> *(x3::char_("A-Za-z0-9_")));
    const auto quotedString_def         = lexeme['"' >> *(char_ - '"') >> '"'];
    const auto nonQuotedString_def      = *x3::char_("0-9") >> x3::char_("A-Za-z_") >> *x3::char_("A-Za-z-0-9_");
    const auto stringValue_def          = (quotedString | nonQuotedString);
    const auto value_def                = (strictDouble | stringValue | int_);
    const auto simpleAttribute_def      = name >> ':' >> value >> ';';
    const auto complexAttribute_def     = name >> '(' >> (value % ',')  >> ')' >> lit(";");
    const auto defineStatement_def      = lit("define") >> '(' >> name >> ',' >> name >> ',' >> attribute >> ')' >> lit(";");
    const auto attributeList_def        = ('{' >> *attributeStatement >> '}');
    const auto groupStatement_def       = (name >> ('('>> -stringValue >> ')') >> attributeList);
    const auto attributeStatement_def   = (defineStatement|simpleAttribute|complexAttribute|groupStatement);
    const auto spaceComment             = x3::rule<class spaceComment>{} = ascii::space | lexeme[lit("/*") >> *(char_ - x3::string("*/")) >> lit("*/")];

    BOOST_SPIRIT_DEFINE(name, quotedString, nonQuotedString, stringValue, value, simpleAttribute, complexAttribute, defineStatement, attributeList, attributeStatement, groupStatement)
            
    template <class Iterator>
    bool liberty_parse(Iterator& first, Iterator const& last, Library& library)
    {
        bool result = x3::phrase_parse(first, last, groupStatement, spaceComment, library.get());
        if(result && first == last)
        {
            return true;
        }
        return result;
    }
}
}


#endif // LIBERTYGRAMMAR_PRIVATE_H


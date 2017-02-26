#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <vector>
#include <utility>
#include <boost/fusion/include/std_pair.hpp>
#include "libertygrammar_private.h"


#include "test.hpp"
using namespace liberty;
using namespace spirit_test;

TEST_CASE("Name")
{
    const char input[] = "library";
    std::string name;
    CHECK(test_attr(input, ast::name, name));
    REQUIRE(name == "library");
}

TEST_CASE("Invalid name")
{
    const char input[] = "4library";
    REQUIRE(test_failure(input, ast::name));
}

TEST_CASE("Quoted string")
{
    const char input[] = "\"quoted string \"";
    std::string name;
    CHECK(test_attr(input, ast::quotedString, name));
    REQUIRE(name == "quoted string ");
}

TEST_CASE("Invalid quoted string")
{
    const char input[] = "\"quoted string";
    REQUIRE(test_failure(input, ast::quotedString));
}

TEST_CASE("Double value")
{
    const char input[] = "4.2";
    ast::SimpleAttribute::Value value;
    CHECK(test_attr(input, ast::value, value));
    REQUIRE_NOTHROW(boost::get<double>(value) == Approx(4.2));
}

TEST_CASE("Integer value")
{
    const char input[] = "4";
    ast::SimpleAttribute::Value value;
    CHECK(test_attr(input, ast::value, value));
    REQUIRE_NOTHROW(boost::get<int>(value) == 4);
}


TEST_CASE("Quoted string value")
{
    const char input[] = "\"4.2\"";
    ast::SimpleAttribute::Value value;
    CHECK(test_attr(input, ast::value, value));
    REQUIRE_NOTHROW(boost::get<std::string>(value) == "4.2");
}

TEST_CASE("Non-quoted string value")
{
    const char input[] = "1uW";
    ast::SimpleAttribute::Value value;
    CHECK(test_attr(input, ast::value, value));
    REQUIRE_NOTHROW(boost::get<std::string>(value) == "1uW");
}

TEST_CASE("Empty group statement")
{
    const char input[] = "group_name (name) { }";
    ast::GroupStatement group;
    INFO(input);
    CHECK(test_attr(input, ast::groupStatement, group, ast::x3::space));
    CHECK(group.group_name == "group_name");
    CHECK(group.name == "name");
    REQUIRE(group.children.empty());
}

TEST_CASE("Group statement without name")
{
    const char input[] = "timing() {}";
    ast::GroupStatement group;
    INFO(input);
    CHECK(test_attr(input, ast::groupStatement, group, ast::x3::space));
    CHECK(group.group_name == "timing");
    CHECK(group.name.empty());
}

TEST_CASE("Big group statement")
{
    const char input[] = "bigGroupName (bigName) { attr : 42.0; innerComplex(1.0, 1, \"cmp lex\"\); }";
    ast::GroupStatement group;
    INFO(input);
    CHECK(test_attr(input, ast::groupStatement, group, ast::x3::space));
    CHECK(group.group_name == "bigGroupName");
    CHECK(group.name == "bigName");
    REQUIRE(group.children.size() == 2);
    ast::SimpleAttribute simpleFirst;
    REQUIRE_NOTHROW(simpleFirst = boost::get<ast::SimpleAttribute>(group.children.at(0)));
    CHECK(simpleFirst.name == "attr");
    CHECK(boost::get<double>(simpleFirst.value) == Approx(42.0));
    ast::ComplexAttribute groupInner;
    REQUIRE_NOTHROW(groupInner = boost::get<ast::ComplexAttribute>(group.children.at(1)));
    CHECK(groupInner.name == "innerComplex");
    REQUIRE(groupInner.values.size() == 3);
    double first;
    int second;
    std::string third;
    REQUIRE_NOTHROW(first = boost::get<double>(groupInner.values.at(0)));
    REQUIRE_NOTHROW(second = boost::get<int>(groupInner.values.at(1)));
    REQUIRE_NOTHROW(third = boost::get<std::string>(groupInner.values.at(2)));
    REQUIRE(first == Approx(1.0));
    REQUIRE(second == 1);
    REQUIRE(third == "cmp lex");
}

TEST_CASE("Simple double attribute")
{
    const char input[] = "attr : 1.4 ;";
    ast::SimpleAttribute attr;
    CHECK(test_attr(input, ast::simpleAttribute, attr, ast::x3::space));
    CHECK(attr.name == "attr");
    CHECK_NOTHROW(boost::get<double>(attr.value) == Approx(1.4));
}

TEST_CASE("Simple quoted string attribute")
{
    const char input[] = "strattr : \"1.4\" ;";
    ast::SimpleAttribute attr;
    CHECK(test_attr(input, ast::simpleAttribute, attr, ast::x3::space));
    CHECK(attr.name == "strattr");
    CHECK_NOTHROW(boost::get<std::string>(attr.value) == "1.4");
}

TEST_CASE("Simple non-quoted string attribute")
{
    const char input[] = "nqstrattr : name ;";
    ast::SimpleAttribute attr;
    CHECK(test_attr(input, ast::simpleAttribute, attr, ast::x3::space));
    CHECK(attr.name == "nqstrattr");
    CHECK_NOTHROW(boost::get<std::string>(attr.value) == "name");
}


TEST_CASE("Complex attribute")
{
    const char input[] = "attr_name (val1, 1.2, \"va l3\", 1);";
    ast::ComplexAttribute attr;
    CHECK(test_attr(input, ast::complexAttribute, attr, ast::x3::space));
    CHECK(attr.name == "attr_name");
    CHECK(attr.values.size() == 4);
    CHECK_NOTHROW(boost::get<std::string>(attr.values.at(0)) == "val1");
    CHECK_NOTHROW(boost::get<double>(attr.values.at(1)) == 1.2);
    CHECK_NOTHROW(boost::get<std::string>(attr.values.at(2)) == "va l3");
    CHECK_NOTHROW(boost::get<int>(attr.values.at(3)) == 1);
}

TEST_CASE("Attribute type")
{
    // TODO: Set a default type for unrecognized values.
    const char input[] = "string integer Boolean float";
    std::vector<ast::AttributeType> types;
    CHECK(test_attr(input, +ast::attribute, types, ast::x3::space));
    const std::vector<ast::AttributeType> golden({
                                                     ast::AttributeType::String,
                                                     ast::AttributeType::Integer,
                                                     ast::AttributeType::Boolean,
                                                     ast::AttributeType::FloatingPoint
                                                 });
    REQUIRE(types == golden);
}

TEST_CASE("Define statement")
{
    const char input[] = "define (attribute_name, group_name, string);";
    ast::DefineStatement attr;
    CHECK(test_attr(input, ast::defineStatement, attr, ast::ascii::blank));
    CHECK(attr.attribute_name == "attribute_name");
    CHECK(attr.group_name == "group_name");
    CHECK(attr.attribute_type == ast::AttributeType::String);

}

TEST_CASE("Variant attribute statement")
{
    const char defineStatement[] = "define (attribute_name, group_name, Boolean);";
    const char simpleDoubleAttribute[] = "attr : 1.4 ;";
    const char complexAttribute[] = "attr_name (val1, 1.2, \"va l3\", 1);";
    const char groupStatement[] = "group_name (name) { }";

    ast::AttributeStatement attr;
    CHECK(test_attr(defineStatement, ast::attributeStatement, attr, ast::x3::space));
    ast::DefineStatement define;
    REQUIRE_NOTHROW(define = boost::get<ast::DefineStatement>(attr));
    CHECK(define.attribute_name == "attribute_name");
    CHECK(define.group_name == "group_name");
    CHECK(define.attribute_type == ast::AttributeType::Boolean);

    ast::SimpleAttribute simple;
    CHECK(test_attr(simpleDoubleAttribute, ast::attributeStatement, attr, ast::x3::space));
    REQUIRE_NOTHROW(simple = boost::get<ast::SimpleAttribute>(attr));
    CHECK(simple.name == "attr");
    CHECK_NOTHROW(boost::get<double>(simple.value) == Approx(1.4));

    ast::ComplexAttribute complex;
    CHECK(test_attr(complexAttribute, ast::attributeStatement, attr, ast::x3::space));
    REQUIRE_NOTHROW(complex = boost::get<ast::ComplexAttribute>(attr));
    CHECK(complex.name == "attr_name");
    CHECK(complex.values.size() == 4);
    CHECK_NOTHROW(boost::get<std::string>(complex.values.at(0)) == "val1");
    CHECK_NOTHROW(boost::get<double>(complex.values.at(1)) == 1.2);
    CHECK_NOTHROW(boost::get<std::string>(complex.values.at(2)) == "va l3");
    CHECK_NOTHROW(boost::get<int>(complex.values.at(3)) == 1);

    ast::GroupStatementAst group;
    CHECK(test_attr(groupStatement, ast::attributeStatement, attr, ast::x3::space));
    REQUIRE_NOTHROW(group = boost::get<ast::GroupStatementAst>(attr));
    CHECK(group.get().group_name == "group_name");
    CHECK(group.get().name == "name");
}

TEST_CASE("Empty attribute list")
{
    const char input[] = "{ }";
    std::vector<ast::AttributeStatement> list;
    CHECK(test_attr(input, ast::attributeList, list, ast::ascii::space));
    REQUIRE(list.empty());
}

TEST_CASE("Attribute list")
{
    const char input[] =
                "{ simple : \"quoted str\" ; double : 4.2 ; }";
    std::vector<ast::AttributeStatement> list;
    CHECK(test_attr(input, ast::attributeList, list, ast::ascii::space));
    REQUIRE(list.size() == 2);
    ast::SimpleAttribute simple;
    CHECK_NOTHROW(simple = boost::get<ast::SimpleAttribute>(list.at(0)));
    CHECK(simple.name == "simple");
    CHECK_NOTHROW(boost::get<std::string>(simple.value) == "quoted str");
    ast::SimpleAttribute double_;
    CHECK_NOTHROW(double_ = boost::get<ast::SimpleAttribute>(list.at(1)));
    CHECK(double_.name == "double");
    CHECK_NOTHROW(boost::get<double>(double_.value) == Approx(4.2));

}

TEST_CASE("Nested Group")
{
    const char input[] = "group1(name1) { group2(name2) { attr: 4.2 ; } }";
    ast::GroupStatement group;
    CHECK(test_attr(input, ast::groupStatement, group, ast::ascii::space));
    CHECK(group.group_name == "group1");
    CHECK(group.name == "name1");
    REQUIRE(group.children.size() == 1);
    ast::GroupStatementAst innerGroup;
    REQUIRE_NOTHROW(innerGroup = boost::get<ast::GroupStatementAst>(group.children.at(0)));
    CHECK(innerGroup.get().group_name == "group2");
    CHECK(innerGroup.get().name == "name2");
    REQUIRE(innerGroup.get().children.size() == 1);
    ast::SimpleAttribute attr;
    REQUIRE_NOTHROW(attr = boost::get<ast::SimpleAttribute>(innerGroup.get().children.at(0)));
    CHECK(attr.name == "attr");
    CHECK_NOTHROW(boost::get<double>(attr.value) == Approx(4.2));
}

TEST_CASE("Comment")
{
    const char input[] = "1.1 /* SHOULD SKIP THIS */ \"quoted string\"";
    std::pair<double, std::string> out;
    CHECK(test_attr(input, (ast::double_ >> ast::stringValue), out, ast::spaceComment));
    CHECK(out.first == Approx(1.1));
    CHECK(out.second == "quoted string");
}

TEST_CASE("Blank library with few comments")
{
    const char input[] = "/*  * Library File Format  * Copyright (c) 2011, Synopsys, Inc. and others. All Rights reserved.  */ library (\"contest\") { }";
    ast::GroupStatement library;
    CHECK(test_attr(input, ast::groupStatement, library, ast::spaceComment));
    CHECK(library.group_name == "library");
    CHECK(library.name == "contest");
}

TEST_CASE("Lookup Table")
{
    const char input[] = "rise_transition (\"delay_outputslew_template_7X8\") {"
         "index_1 (\"0.00,1.00,2.00,4.00,8.00,16.00,32.00\") ;"
         "index_2 (\"5.00,30.00,50.00,80.00,140.00,200.00,300.00,500.00\") ;"
         "values ("
         "\"15, 16.464, 19.656, 24.12, 30.204, 34.38, 39.54, 47.796\","
         "\"22.5, 23.04, 25.38, 30.336, 38.58, 44.472, 51.576, 61.572\","
         "\"30, 30.108, 31.692, 35.892, 45.564, 52.908, 61.992, 74.232\","
         "\"45, 45, 45.384, 48.12, 57.192, 66.924, 79.368, 96.684\","
         "\"75, 75, 75, 75.48, 81.012, 89.712, 106.536, 132.276\","
         "\"135, 135, 135, 135, 135.672, 140.004, 152.304, 185.376\","
         "\"255, 255, 255, 255, 255, 255, 258.18, 278.508\""
         ");"
         "}";
    auto it = std::begin(input);
    ast::GroupStatement lut;
    CHECK(test_attr(input, ast::groupStatement, lut, ast::ascii::space));;
    CHECK(lut.group_name == "rise_transition");
    CHECK(lut.name == "delay_outputslew_template_7X8");
    REQUIRE(lut.children.size() == 3);
    ast::ComplexAttribute index_1, index_2, values;
    CHECK_NOTHROW(index_1 = boost::get<ast::ComplexAttribute>(lut.children.at(0)));
    CHECK_NOTHROW(index_2 = boost::get<ast::ComplexAttribute>(lut.children.at(1)));
    CHECK_NOTHROW(values = boost::get<ast::ComplexAttribute>(lut.children.at(2)));
    CHECK(index_1.name == "index_1");
    REQUIRE(index_1.values.size() == 1);
    CHECK_NOTHROW(boost::get<std::string>(index_1.values.front()) == "0.00,1.00,2.00,4.00,8.00,16.00,32.00");
    CHECK(index_2.name == "index_2");
    REQUIRE(index_2.values.size() == 1);
    CHECK_NOTHROW(boost::get<std::string>(index_2.values.front()) == "5.00,30.00,50.00,80.00,140.00,200.00,300.00,500.00");
    CHECK(values.name == "values");
    CHECK(values.values.size() == 7);
    const std::vector<std::string> golden = {
        {"15, 16.464, 19.656, 24.12, 30.204, 34.38, 39.54, 47.796",
         "22.5, 23.04, 25.38, 30.336, 38.58, 44.472, 51.576, 61.572",
         "30, 30.108, 31.692, 35.892, 45.564, 52.908, 61.992, 74.232",
         "45, 45, 45.384, 48.12, 57.192, 66.924, 79.368, 96.684",
         "75, 75, 75, 75.48, 81.012, 89.712, 106.536, 132.276",
         "135, 135, 135, 135, 135.672, 140.004, 152.304, 185.376",
         "255, 255, 255, 255, 255, 255, 258.18, 278.508"}};
    std::vector<std::string> extractedValues;
    std::transform(values.values.begin(), values.values.end(), std::back_inserter(extractedValues), [](auto& value){
        return boost::get<std::string>(value);
    });

    REQUIRE(golden == extractedValues);
}


struct ast_traversal
{
    typedef void result_type;

    static std::set<std::string> cells;
    static std::multimap<std::string, std::string> pins;
    static std::map<std::string, std::string> directions;
    static std::string clock;

    static void reset()
    {
        cells.clear();
        pins.clear();
        directions.clear();
        clock.clear();
    }

    ast_traversal(const ast::GroupStatementAst* parent = nullptr) :
        parent(parent)
    {

    }

    result_type operator()(const ast::GroupStatementAst& group) const
    {
        if(group.get().group_name == "cell")
        {
            cells.insert(group.get().name);
        }
        else if(group.get().group_name == "pin")
        {
            pins.insert(std::make_pair(parent->get().name, group.get().name));
        }
        for(auto const& child: group.get().children)
        {
            child.apply_visitor(ast_traversal(&group));
//            boost::apply_visitor(child, ast_traversal(&group));
        }
    }

    result_type operator()(const ast::SimpleAttribute& attr) const
    {
        if(attr.name == "direction")
        {
            directions.insert(std::make_pair(parent->get().name, boost::get<std::string>(attr.value)));
        }
        else if(attr.name == "clock")
        {
            clock = (boost::get<std::string>(attr.value) == "true" ? parent->get().name : clock);
        }
    }

    result_type operator()(const ast::ComplexAttribute&) const
    {

    }

    result_type operator()(const ast::DefineStatement&) const
    {

    }

    const ast::GroupStatementAst* parent;
};

std::set<std::string> ast_traversal::cells;
std::multimap<std::string, std::string> ast_traversal::pins;
std::map<std::string, std::string> ast_traversal::directions;
std::string ast_traversal::clock;


#include <fstream>
#include <iterator>

TEST_CASE("Input file", "[file]")
{
#ifndef LIBERTY_TEST_RESOURCE_PATH
    static_assert(1!=1, "Please set -DLIBERTY_TEST_RESOURCE_PATH");
#endif

    const char test_filename[] = LIBERTY_TEST_RESOURCE_PATH "/sample2_Late.lib";
    std::ifstream in(test_filename, std::fstream::in);
    REQUIRE(in.good());

    std::vector<char> buffer;
    std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), std::back_inserter(buffer));
    std::replace(buffer.begin(), buffer.end(), '\n', ' ');
    std::replace(buffer.begin(), buffer.end(), '\\', ' ');

    ast::AttributeStatement group;
    CHECK(test_attr(buffer.data(), ast::attributeStatement, group, ast::spaceComment));
    ast::GroupStatementAst& groupAst = boost::get<ast::GroupStatementAst>(group);
    CHECK(groupAst.get().group_name == "library");
    CHECK(groupAst.get().name == "contest");

    group.apply_visitor(ast_traversal());

    CHECK(ast_traversal::cells.size() == 2);
    CHECK(ast_traversal::cells.count("INV_X1") == 1);
    CHECK(ast_traversal::cells.count("DFF_X80") == 1);
    CHECK(ast_traversal::pins.count("INV_X1") == 2);
    CHECK(ast_traversal::pins.count("DFF_X80") == 3);
    CHECK_NOTHROW(ast_traversal::directions.at("a") == "input");
    CHECK_NOTHROW(ast_traversal::directions.at("o") == "output");
    CHECK_NOTHROW(ast_traversal::directions.at("d") == "input");
    CHECK_NOTHROW(ast_traversal::directions.at("d") == "input");
    CHECK_NOTHROW(ast_traversal::directions.at("ck") == "input");
    CHECK_NOTHROW(ast_traversal::directions.at("q") == "output");
    CHECK(ast_traversal::clock == "ck");
    ast_traversal::reset();
}


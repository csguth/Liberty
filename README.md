# Liberty Parser

This is an open-source Parser for the Liberty Standard Cell Library format.

Please, refer to [Open Source Liberty](http://opensourceliberty.org) for format specification and reference parser.

## Usage

Use the Api function to parse a file. The function `liberty_parse` receives a pair of iterators as input and writes the AST (abstract-syntax tree) on a `liberty::ast::Library` object. e.g.,:
```c++
liberty::ast::liberty_parse(first, buffer.end(), library);
```
## To-do:

* Improve this Readme.
* Create a Wiki.
* Implement Visitor functionalities.
* Make syntax verification less abstract.
* Etc.

##
_This Library is still in development. Feel free to create an issue._ 

# Liberty Parser

This is an open-source Parser for the Liberty Standard Cell Library format.

Please, refer to [Open Source Liberty](http://opensourceliberty.org) for format specification and reference parser.

## Usage

Use the Api function to parse a file. The function `liberty_parse` receives a pair of iterators as input and writes the AST (abstract-syntax tree) on a `liberty::ast::Library` object. e.g.,:
```c++
  #include <fstream>
  #include <vector>
  #include <algorithm>
  #include <iterator>
  #include <iostream>
  #include "LibertyParser.hpp"
  
  int main() {
    const liberty::ast::Library library = [&]() {
      liberty::ast::Library library_;
      std::ifstream in("file.lib", std::fstream::in);
      const std::vector<char> buffer = []() {
        std::vector<char> buffer_(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
        std::replace(buffer_.begin(), buffer_.end(), '\n', ' ');
        std::replace(buffer_.begin(), buffer_.end(), '\\', ' ');
        return buffer_;
      } ();
      auto first = buffer.begin();
      auto result = liberty::ast::liberty_parse(first, buffer.end(), library_);
      return library_;
    } ();
    const int numberOfCells = [&]() {
      int numberOfCells_ = 0;
      liberty::ast::Visitor visitor;
      visitor.onCell([&](const liberty::Cell&) { ++numberOfCells_; });
      visitor(library);
      return numberOfCells_;
    } ();
    std::cout << "This Library has " << numberOfCells << " cells." << std::endl;
    return 0;
}
```
## To-do:

* Improve this Readme.
* Create a Wiki.
* Implement Visitor functionalities.
* Make syntax verification less abstract.
* Etc.

##
_This Library is still in development. Feel free to create an issue._ 

## Library

This is a bleeding-edge-C++ generic *Variant* implementation.

### Build

The library is **header-only**. To use the library just include `include/versatile.hpp`.
All symbols are available in `versatile` namespace.
To be used the library requires up to *C++17* features enabled.
Currently the library is only tested against latest *clang-3.8* and *libc++* (both from SVN repo's trunk).

### Testing

Supported build system is *cmake*.
To build release version of the test project contained in `variant/` directory
using `Makefile`-s on *Linux* do type in shell:

    cd variant/
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" .
    ROWS=5 COLS=5 /usr/bin/env time make # running compilation time test
    /usr/bin/env time ./versatile # running runtime test

### Benchmarking

To benchmark compilation times for multivisitation
try to vary values of `ROWS` and `COLS` defines.

To benchmark deepest template depth used during compilation
try to add `-ftemplate-depth=` option to `CXXFLAGS` and vary its value in bisection manner.

Running time is not tested at the moment.

### Features

 - `variant` class template is heap-based *discriminated union* supporting **never-empty guarantee**
 - `versatile` is storage for `variant`; it based on **union-like classes**
 and can be just only explicitly convertible from/to any of alternative types
 - `visit()` (non-unary overloading) template function for **visitation**
 - `multivisit()` template function for **multivisitation**
 - **perfect forwarding** for visitor (enables *cv-ref-qualifiers* for `operator ()`),
 visitable and non-visitable ("pass-through" arguments; only for multivisitation)
 arguments for all template functions described here; automatic unwrapping
 of wrapped visitable and non-visitable arguments
 - all the free template functions can be used in **constant expressions**
 (nor `variant`, neither even `versatile` class templates currently can be used though)
 - **automatic deducing of result type** for visitor
 (by means of `decltype(auto)` declaration of function return type)
 - `visit()` (non-unary overloading) can be modified in a simple way
 to work with `boost::variant` or `eggs::variant` (even for `constexpr` one), or on the contrary
 any *variant* variadic class template can be easily adapted to deal with `visit()`
 by means of using a properly engeneered wrapper; both ways requires an adding of a specialization
 of `is_variant` type trait
 - `recursive_wrapper` class template is a wrapper for **incomplete types**
 to enable "recursive" types (e.g. useful for *AST* definitions)
 - `aggregate_wrapper` class template is a wrapper for **aggregates**
 to be "parenthesis"-constructible as usual classes
 - `visit()` template function (unary overloading) to make **delayed visitor**
 - `compose_visitors()` template function to make **composite visitor** ("lambda-visitor")
 from *functional objects* and *lambda-functions*
 - `call()` template function to **call a callable** contained in visitable argument using provided arguments
 - **I/O operators** `operator <<` and `operator >>` to deal with `std::istream` and `std::ostream`
 - **relational operators** `operator <` and `operator ==` to compare containing values
 or to compare containing value against "free" value
 - **no dependencies** on third-party libraries, only *C++* headers
 (mostly standard, with the exception of `<experimental/optional>`)
 - partial support for `noexcept` specifiers

### Future work

 - change protocol of obtaining a type template parameter pack of alternative types
 to be usable for classes or for class templates, which template parameters
 are not exactly the set of alternative types
 - enable/disable special functions, conversion constructors, variadic constructors,
 assignment operators and conversion operators of `variant` class template in sensible way
 - enable `constexpr` for storage (`versatile` class template) and make storage to be a literal type
 - decide about `noexcept` specifiers necessity (everywhere, especially for `visit()`
 and `multivisit()` template functions)
 - automatic deducing the type of discriminator for storage (`versatile` class template)
 to fit indices for only provided number of alternative types

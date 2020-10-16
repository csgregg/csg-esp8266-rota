# 1. Naming Conventions

## 1.1 General Naming Conventions


1. Names representing types must be in mixed case starting with upper case. |
--- |
Line, SavingsAccount |
 |


2. Variable names must be in mixed case starting with lower case. |
--- |
line, savingsAccount |
|

3. Named constants (including enumeration values) must be all uppercase using underscore to separate words. |
--- |
MAX_ITERATIONS, COLOR_RED, PI |
|

4. Names representing methods or functions must be verbs and written in mixed case starting with lower case. |
--- |
getName(), computeTotalWidth() |
|

5. Names representing namespaces should be all lowercase. |
--- |
model::analyzer, io::iomanager, common::math::geometry |
|

6. Names representing template types should be a single uppercase letter, or SIZE, NUM. |
--- |
template<class T> ...
template<class C, class D> ...
template<size_t SIZE> ... |
|
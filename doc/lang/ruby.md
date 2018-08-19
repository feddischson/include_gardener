Ruby
===========
[TOC]

Ruby include and require mechanism
===========

In ruby, there are two mechanisms which might be interesting to analyze: 
`require` and `include`.

Basically, the `require` statement can be compared with C/C++'s `include`
statement.
Ruby's `include` statement is used to extend a class with other modules.
In addition, there exists also a `load` statement, which is used
to execute code.

Examples
-----------
```ruby
require "foo.rb"
require "bar"
require 'foo/bar'
require_relative '../foo/bar'
load 'foo.rb'
load 'foo/bar.rb'
```


Considerations
-----------
There exists a `$LOAD_PATH` which is considered for searching modules.
This dynamic behaviour is hard to analyze with an extenral tool.

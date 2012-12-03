 Pyruby module
=======

+ A Python module written in C that provides an embedded Ruby interpreter
  within Python to allow evaluation of Ruby code in a sandboxed sub environment.
 
+ Note that the return values of pyrb.eval are Python-ized Ruby objects
 	(Ruby arrays become Python lists, Ruby exceptions are converted to Python runtime
  exceptions and rethrown properly, Ruby's nil becomes Python's None, etc.)

+ The code won't compile unless linked against, built with, or otherwise allowed to call
  C Ruby's API.  The licensing is the same; see [Ruby's site](http://www.ruby-lang.org)
  for the source as necessary.

### Usage

```Python
import pyrb

>>> pyrb.eval_s("""def use_a_block(arg = %|Top o' the morning, gov'na!|)
...                yield(arg); end
...                use_a_block { |str| puts str.inspect }
...             """)
"Top o' the morning, gov'na!"

>>> pyrb.eval_f("RubyScriptFile.rb")
"Pretend I did something super cool, mmkay?"

>>> pyrb.eval("Run this file if it exists otherwise evaluate the string")
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
pyrb.ruby_error: #<NameError: undefined local variable or method `string' for main:Object>
```
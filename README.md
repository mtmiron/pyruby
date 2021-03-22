Pyruby module
===
A Python module written in C that provides an embedded Ruby interpreter
within Python to allow evaluation of Ruby code in a sandboxed sub environment.

 * Note that the return values of pyrb.eval are Python-ized Ruby objects
 	(Ruby arrays become Python lists, Ruby exceptions are converted to Python runtime
  exceptions and rethrown properly, Ruby's nil becomes Python's None, etc.)

 * The code won't compile unless linked against, built with, or otherwise allowed to call
  C Ruby's API.  The licensing is the same; see [Ruby's site](http://www.ruby-lang.org)
  for the source as necessary.

## But why...!?!
I was a bored undergraduate: why not?


#### Functions
* _`pyrb.eval_s`_    <center>evaluate a string</center>
* _`pyrb.eval_f`_    <center>evaluate a file</center>
* _`pyrb.eval`_      evaluate whichever it looks like

#### Usage

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

```Python
#!/usr/bin/env python2
import pyrb

ruby_script_array="""
    def make_an_array()
        array = []
        3.times { |i| array.push(i) }
        array.push(3.14159265)
        return array
    end

    make_an_array()
"""

ruby_script_exception="""
    raise "This Exception was raised in Ruby"
"""


print("\n- Executing this Ruby code:")
print(ruby_script_array)

list_from_ruby = pyrb.eval(ruby_script_array)

print("\n- The value returned by Ruby to Python is (Ruby returns the value of the last expression by default):")
print("\n    {}".format(list_from_ruby))
print("\n\n- Executing this Ruby code:")
print(ruby_script_exception)

try:
    pyrb.eval(ruby_script_exception)
except Exception as exc:
    print("\n- Caught the exception raised from Ruby in a Python try/catch block!  Re-raising for traceback...\n")
    raise exc
```
```
- Executing this Ruby code:

    def make_an_array()
        array = []
        3.times { |i| array.push(i) }
        array.push(3.14159265)
        return array
    end

    make_an_array()


- The value returned by Ruby to Python is (Ruby returns the value of the last expression by default):

    [0, 1, 2, 3.14159265]


- Executing this Ruby code:

    raise "This Exception was raised in Ruby"


- Caught the exception raised from Ruby in a Python try/catch block!  Re-raising for traceback...

Traceback (most recent call last):
  File "test.py", line 34, in <module>
    raise exc
pyrb.ruby_error: #<RuntimeError: This Exception was raised in Ruby>
```

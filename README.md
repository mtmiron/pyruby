Pyruby module
=============

* A Python module written in C that provides an embedded Ruby interpreter
  within Python to allow evaluation of Ruby code in a sandboxed sub environment.
 
* Note that the return values of pyrb.eval are Python-ized Ruby objects
 	(Ruby arrays become Python lists, Ruby exceptions are converted to Python runtime
  exceptions and thrown properly, Ruby's nil becomes Python's None, etc.)

 
 
Usage:
-----

import pyrb



pyrb.eval_s("Any valid Ruby code can be passed as a Python string.")
 
pyrb.eval_f("RubyScriptFile.rb")

pyrb.eval("If this filename exists, same as eval_f; else same as eval_s")

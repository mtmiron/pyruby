/*
 * pyrbmodule.c:
 *
 * 	A Python module that provides execution of Ruby code from
 * 	directly within the Python environment.  Note that the
 * 	return value of pyrb.eval is the Kernel#inspect() string of
 * 	the returned Ruby object (converted to a Python string).
 *
 * Usage:
 *
 * import pyrb
 * pyrb.eval_s("Any valid Ruby code can be passed as a Python string.")
 * pyrb.eval_f("RubyScriptFile.rb")
 * pyrb.eval("If this filename exists, same as eval_f; else same as eval_s")
 *
 */

#include <Python.h>
#include <ruby.h>

/* Object conversions & macros taking Ruby objects */
// RubyStringObject -> PythonStringObject
#define RBSTR2PYSTR(rbstr) Py_BuildValue("s", RSTRING_PTR(rbstr))
// `$!.to_s()' ($! == most recent Ruby exception object)
#define RB_GET_ERRSTR() rb_funcall(rb_errinfo(), id_to_s, 0)
// `$!.inspect()'
#define RB_GET_ERRINFO() rb_funcall(rb_errinfo(), id_inspect, 0)
// `obj.inspect()'
#define RB_INSPECT(obj) rb_funcall(obj, id_inspect, 0)

// Boolean value indicating file (non)existence; takes a C string, unlike the above
#define FILE_EXISTS_P(fname) RTEST(rb_funcall(rb_cFile, rb_intern("exists?"), 1, rb_str_new2(fname)))


/* Protos */
static PyObject *pyrb_eval_s(PyObject *, PyObject *);
static PyObject *pyrb_eval_f(PyObject *, PyObject *);
static PyObject *pyrb_eval(PyObject *, PyObject *);
static PyObject *pyrb_eval_cstr(PyObject *, char *);
static PyObject *pyrb_ruby_version(PyObject *);

static PyObject *rbobject_to_pyobject(VALUE);
static PyObject *rbarray_to_pylist(VALUE);


// Python exception object to indicate a Ruby interpreter exception occurred
static PyObject *pyrb_exc;

/* Ruby IDs (internalized strings), etc. */
static ID id_to_s;
static ID id_inspect;
static int rb_needs_init = 1;


// Method definition structure for Python
static PyMethodDef pyrb_methods[] = {
	{ "eval_s", pyrb_eval_s, METH_VARARGS,
	  "Evaluate a Python string as Ruby code." },
	{ "eval_f", pyrb_eval_f, METH_VARARGS,
	  "Evaluate the contents of a file as Ruby code." },
	{ "eval", pyrb_eval, METH_VARARGS,
	  "Eval contents of file if filename exists, else eval string as code." },
    { "ruby_version", (PyCFunction)pyrb_ruby_version, METH_NOARGS,
      "Return the version of the embedded Ruby interpreter" },
	{ NULL, NULL, 0, NULL },
};


// Initialize the embedded Ruby interpreters environment
static void init_ruby_env(int argc, char **argv)
{
	if (!rb_needs_init)
		return;

	ruby_init();
	ruby_init_loadpath();
	ruby_set_argv(argc, argv);

	id_to_s = rb_intern("to_s");
	id_inspect = rb_intern("inspect");

	rb_needs_init = 0;
}

// Convert a Ruby object to it's Python equivalent
static PyObject *rbobject_to_pyobject(VALUE rb_obj)
{
    switch (rb_type(rb_obj))
    {
      case T_FLOAT:
        return (PyObject*)Py_BuildValue("f", RFLOAT_VALUE(rb_obj));

      case T_BIGNUM:
      case T_FIXNUM:
        return (PyObject*)Py_BuildValue("l", NUM2LONG(rb_obj));

      case T_ARRAY:
        return rbarray_to_pylist(rb_obj);

      case T_TRUE:
        Py_RETURN_TRUE;

      case T_FALSE:
        Py_RETURN_FALSE;

      case T_NIL:
        Py_RETURN_NONE;

      case T_STRING:
        return PyString_FromString(RSTRING_PTR(rb_obj));

      default:
		return RBSTR2PYSTR(RB_INSPECT(rb_obj));
    }
}

// Convert a Ruby array to a Python list, recursively converting all elements
static PyObject *rbarray_to_pylist(VALUE ary)
{
    PyObject *list;
    int i, len;

    len = RARRAY_LEN(ary);
    list = PyList_New(len);

    for (i = 0; i < len; i++)
    {
        PyList_SET_ITEM(list, i, rbobject_to_pyobject(rb_ary_entry(ary, i)));
    }
    return list;
}

// Get the version of the embedded Ruby interpreter
static PyObject *pyrb_ruby_version(PyObject *self)
{
    return (PyObject *)pyrb_eval_cstr(self, (char *)"RUBY_VERSION");
}

// Call the Ruby interpreter to eval/exec a C string; other pyrb_evals ultimately
// call this func to perform the actual Ruby code evaluation
static PyObject *pyrb_eval_cstr(PyObject *self, char *arg)
{
	const char *rb_code = (const char *)arg;
	int state = 0;
	VALUE rb_ret;

	rb_ret = rb_eval_string_protect(rb_code, &state);
	if (state) {
		PyErr_SetString(pyrb_exc, RSTRING_PTR( RB_GET_ERRINFO() ));
		return NULL;
	}
    return rbobject_to_pyobject(rb_ret);
}


// Call the Ruby interpreter to eval/exec a Python string object
static PyObject *pyrb_eval_s(PyObject *self, PyObject *args)
{
	const char *rb_code;

	if (! PyArg_ParseTuple(args, "s", &rb_code) )
		return NULL;
	else
		return pyrb_eval_cstr(self, (char *)rb_code);
}


// Call the Ruby interpreter to eval/exec the contents of a file (name passed as Python string)
static PyObject *pyrb_eval_f(PyObject *self, PyObject *args)
{
	const char *fname;
	VALUE rb_f_contents;
	
	if (! PyArg_ParseTuple(args, "s", &fname) )
		return NULL;

	if (! FILE_EXISTS_P(fname) ) {
		PyErr_SetString(PyExc_IOError, "file not found");
		return NULL;
	}

	rb_f_contents = rb_funcall(rb_cFile, rb_intern("read"), 1, rb_str_new2(fname));
	fname = RSTRING_PTR(rb_funcall(rb_f_contents, id_to_s, 0));
	return pyrb_eval_cstr(self, (char *)fname);
}


/*
 * Runs contents of the file (Python string arg is taken as filename) if it
 * exists, otherwise runs the passed Python string as Ruby code.
 */
static PyObject *pyrb_eval(PyObject *self, PyObject *args)
{
	char *str;

	if (! PyArg_ParseTuple(args, "s", &str) )
		return NULL;

	// Check if this is an existing filename & run file contents if so
	if ( FILE_EXISTS_P(str) )
		return pyrb_eval_f(self, args);
	else
		return pyrb_eval_s(self, args);
}


// Initialize Python bindings, init the embedded Ruby interpreter, etc.
PyMODINIT_FUNC initpyrb()
{
	PyObject *pyrb_module;

	// Ruby init
	init_ruby_env(0, NULL);

	// Python inits
	pyrb_module = Py_InitModule("pyrb", pyrb_methods);

	pyrb_exc = PyErr_NewException("pyrb.ruby_error", NULL, NULL);
	Py_XINCREF(pyrb_exc);
	PyModule_AddObject(pyrb_module, "exception", pyrb_exc);
}

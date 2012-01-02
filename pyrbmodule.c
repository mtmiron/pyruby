/*
 * pyrb.c:
 *
 * 	A Python module that provides execution of Ruby code from
 * 	directly within the Python environment.
 *
 * Usage:
 *
 * import pyrb
 * pyrb.eval_s("Any valid Ruby code can be passed as a Python string.")
 * pyrb.eval_f("RubyScriptFile.rb")
 *
 */

#include <Python.h>
#include <ruby.h>

// Object conversions, etc.
#define RBSTR2PYSTR(rbstr) Py_BuildValue("s", RSTRING_PTR(rbstr))
#define RB_GET_ERRSTR() rb_funcall(rb_errinfo(), id_to_s, 0)
#define RB_GET_ERRINFO() rb_funcall(rb_errinfo(), id_inspect, 0)
#define RB_INSPECT(obj) rb_funcall(obj, id_inspect, 0)

// Protos
static PyObject *pyrb_eval_s(PyObject *, PyObject *);
static PyObject *pyrb_eval_f(PyObject *, PyObject *);
static PyObject *pyrb_eval(PyObject *, PyObject *);

// Python exception object to indicate a Ruby interpreter exception occurred
static PyObject *pyrb_exc;

// Ruby IDs (internalized strings), etc.
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
};


// Call the Ruby interpreter to eval/exec a C string; other pyrb_evals ultimately
// call this func to perform the actual Ruby code evaluation
static PyObject *pyrb_eval_cstr(PyObject *self, char *arg)
{
	const char *rb_code = (const char *)arg;
	int state = 0;
	VALUE rb_ret;

	rb_ret = RB_INSPECT(rb_eval_string_protect(rb_code, &state));
	if (state) {
		PyErr_SetString(pyrb_exc, RSTRING_PTR( RB_GET_ERRINFO() ));
		return RBSTR2PYSTR( RB_GET_ERRINFO() );
	}
	else
		return RBSTR2PYSTR(rb_ret);
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
	char *rb_code;
	VALUE rb_f_contents;
	
	if (! PyArg_ParseTuple(args, "s", &rb_code) )
		return NULL;

	rb_f_contents = rb_funcall(rb_cFile, rb_intern("read"), 1, rb_str_new2(rb_code));
	rb_code = RSTRING_PTR(rb_funcall(rb_f_contents, id_to_s, 0));
	return pyrb_eval_cstr(self, rb_code);
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
	if (RTEST( rb_funcall(rb_cFile, rb_intern("exists?"), 1, rb_str_new2(str)) ))
		return pyrb_eval_f(self, args);
	else
		return pyrb_eval_s(self, args);
}


// Initialize Python bindings, init the embedded Ruby interpreter, etc.
PyMODINIT_FUNC initpyrb()
{
	PyObject *module;

	// Ruby inits
	ruby_init();
	ruby_init_loadpath();
	ruby_set_argv(0, NULL);
	rb_needs_init = 0;

	id_to_s = rb_intern("to_s");
	id_inspect = rb_intern("inspect");

	// Python inits
	module = Py_InitModule("pyrb", pyrb_methods);

	pyrb_exc = PyErr_NewException("pyrb.error", NULL, NULL);
	Py_INCREF(pyrb_exc);
	Py_INCREF(Py_None);
	PyModule_AddObject(module, "error", pyrb_exc);
}

#include "Var.h"
#include "Log.h"
#include <cstdlib>

namespace Oryol {

Var::VarMap Var::_vars;
RWLock Var::_lock;

VarPtr Var::get(const String& name, const String& value, unsigned int flags) {
	bool missing;
	int32 index;
	{
		ScopedReadLock lock(_lock);
		index = _vars.FindIndex(name);
		missing = index == InvalidIndex;
	}
	if (missing) {
		if (value.Empty() && (flags & CV_NOTCREATEEMPTY))
			return VarPtr();
		VarPtr p(new Var(name, value, flags));
		ScopedWriteLock lock(_lock);
		_vars[name] = p;
		return p;
	}
	return _vars.ValueAtIndex(index);
}

Var::Var(const String& name, const String& value, unsigned int flags) :
		_name(name), _flags(flags), _value(value), _dirty(false) {
	_intValue = atoi(_value.AsCStr());
	_longValue = atol(_value.AsCStr());
	_floatValue = atof(_value.AsCStr());
}

Var::~Var() {
}

bool Var::setVal(const String& value) {
	if (_flags & CV_READONLY) {
		Log::Error("%s is write protected", _name.AsCStr());
		return false;
	}
	_dirty = _value != value;
	if (_dirty) {
		_value = value;
		_intValue = atoi(_value.AsCStr());
		_longValue = atol(_value.AsCStr());
		_floatValue = atof(_value.AsCStr());
	}

	return _dirty;
}

}

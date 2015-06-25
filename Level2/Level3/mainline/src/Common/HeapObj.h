#pragma once

#include <memory>


#define HEAPOBJ(classname,varname, constructor)  std::auto_ptr<classname> autoptr_##varname(new constructor); classname & varname = * autoptr_##varname;

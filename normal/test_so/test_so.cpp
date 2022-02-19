#include <dlfcn.h>

#include <iostream>

#include "so/module.h"

typedef Module* CreateModule();

int main() {
    char* errstr;
    void* handle = nullptr;
    handle = dlopen("./so/module.so", RTLD_NOW | RTLD_NODELETE);
    errstr = dlerror();
    if (errstr != nullptr) {
        std::cerr << "open so failed! errstr: " << errstr << std::endl;
        return 1;
    }

    CreateModule* p = (CreateModule*)dlsym(handle, "create");
    errstr = dlerror();
    if (errstr != nullptr) {
        std::cerr << "create module failed! errstr: " << errstr << std::endl;
        dlclose(handle);
        return 1;
    }

    Module* module = (Module*)p();
    module->init();
    delete module;

    // Soclass* p = new Soclass;
    // p->init();
    // p->func();
    dlclose(handle);
    return 0;
}
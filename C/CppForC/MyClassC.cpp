// my_class_c_interface.cpp
#include "MyClass.h"
#include "MyClassC.h"

struct MyClassWrapper {
    MyClass* instance;
};

extern "C" {
    MyClassWrapper* MyClass_create() {
        return new MyClassWrapper{ new MyClass() };
    }

    int MyClass_add(MyClassWrapper* wrapper, int a, int b) {
        return wrapper->instance->Add(a, b);
    }

    void MyClass_destroy(MyClassWrapper* wrapper) {
        delete wrapper->instance;
        delete wrapper;
    }
}



#ifdef __cplusplus
extern "C" {
#endif

typedef struct MyClassWrapper MyClassWrapper;

MyClassWrapper* MyClass_create();
int MyClass_add(MyClassWrapper* wrapper, int a, int b);
void MyClass_destroy(MyClassWrapper* wrapper);

#ifdef __cplusplus
}
#endif


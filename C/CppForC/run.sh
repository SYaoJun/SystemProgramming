g++ -std=c++11 -fPIC -shared MyClassC.cpp  MyClass.cpp -o libclass.so
gcc main.c -o main -lclass -L. -Wl,-rpath,.

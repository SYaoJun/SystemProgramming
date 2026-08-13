gcc -fPIC -shared add.c -o libadd.so
g++  main.cpp -o main -ladd -L. -Wl,-rpath,.
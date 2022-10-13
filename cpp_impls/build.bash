
cd ./build

export BUILDFORTEST=true
cmake -DCMAKE_BUILD_TYPE=Debug .. && make

cd ../release

unset BUILDFORTEST
cmake -DCMAKE_BUILD_TYPE=Release .. && make

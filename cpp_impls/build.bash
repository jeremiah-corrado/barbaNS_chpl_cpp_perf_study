if [[ $# -eq 0 ]]; then
    echo "building cpp source ..."
    cd ./build

    export BUILDFORTEST=true
    cmake -DCMAKE_BUILD_TYPE=Debug .. && make
else
    echo "building cpp source in Release mode ..."
    cd ./release

    unset BUILDFORTEST
    cmake -DCMAKE_BUILD_TYPE=Release .. && make
fi

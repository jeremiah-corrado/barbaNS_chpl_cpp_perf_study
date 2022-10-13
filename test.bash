
echo "Builing..."

cd ./chpl_impls && bash build.bash
cd ../cpp_impls && bash build.bash

echo "Step 7:"

cd ../chpl_impls && ./build/nsStep7
cd ../cpp_impls && ./build/nsStep7

echo "Step 8/9:"

cd ../chpl_impls && ./build/nsStep89
cd ../cpp_impls && ./build/nsStep89

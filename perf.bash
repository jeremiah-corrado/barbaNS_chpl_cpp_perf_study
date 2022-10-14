# echo "Builing..."

# cd ./chpl_impls && bash build.bash "release"
# cd ../cpp_impls && bash build.bash "release"
# cd ../

export OMP_NUM_THREADS=8

args7="--nx=4096 --xLen=256 --ny=4096 --yLen=256"

echo "Step 7 sanity check with args: $args7..."
./cpp_impls/release/nsStep7 $args7
./chpl_impls/release/nsStep7 $args7

echo "Collecting step 7 performance data..."
for i in $(eval echo {1..$1})
do
    /usr/bin/time -f "real %e user %U sys %S" -a -o ./dat/times_7_cpp.txt ./cpp_impls/release/nsStep89 $args89 > /dev/null
    /usr/bin/time -f "real %e user %U sys %S" -a -o ./dat/times_7_chpl.txt ./chpl_impls/release/nsStep89 $args89 > /dev/null
done

args89="--nx=512 --xLen=32 --ny=512 --yLen=32 --max_iters=1000"

echo "Step 8/9 sanity check with args: $args89..."
./cpp_impls/release/nsStep89 $args89
./chpl_impls/release/nsStep89 $args89

echo "Collecting step 8/9 performance data..."
for i in $(eval echo {1..$2})
do
    /usr/bin/time -f "real %e user %U sys %S" -a -o ./dat/times_89_cpp.txt ./cpp_impls/release/nsStep89 $args89 > /dev/null
    /usr/bin/time -f "real %e user %U sys %S" -a -o ./dat/times_89_chpl.txt ./chpl_impls/release/nsStep89 $args89 > /dev/null
done

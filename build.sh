# deploy with picoprobe and SWD
# otherwise just copy the uf2 via USB
pushd ./src/build/x360c64

make -j4

popd

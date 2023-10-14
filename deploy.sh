# deploy with picoprobe and SWD
# otherwise just copy the uf2 via USB
pushd ./src/build/x360c64

make -j4
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 3000" -c " program x360c64.elf verify reset exit"

popd

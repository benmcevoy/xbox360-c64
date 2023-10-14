
## prepare
clone with submodules, e,g,

`git clone --recurse-submodules -j8 git://github.com/foo/bar.git`

or

```
git submodule init 
git submodule update
```

export environment variables - i use .envrc https://direnv.net/
or ~/.bashrc or whatever

export PICO_SDK_PATH=/path/to/xbox360-c64/pico-sdk

## cmake
```
cd src
mkdir build
cd build
cmake ..
```

## make
```
cd src/build/x360c64
make -j4
```

## build and deploy

```
# make executable
chmod +x ./swd-deploy.sh
chmod +x ./monitor.sh
```

```
# get the picoprobe uf2 https://github.com/raspberrypi/picoprobe
# deploy via picoprobe
./swd-deploy.sh
```



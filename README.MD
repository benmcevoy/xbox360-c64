# XBOX 360 Controller to C64

Try and get a USB 360 contorller to work on the c64

TODO:

- read controller via tinyusb
- dpad is joystick
- A is fire
- Analog stick is POTX/POTY
- B is UP
- X and Y are c64gs secondary fire?
- PS4 controller?
- Web interface to configure? see the pico fight stick



## prepare
clone with submodules, e,g,

`git clone --recurse-submodules https://github.com/benmcevoy/xbox360-c64`

or

```
git submodule init 
git submodule update
```

export environment variables 
- i use .envrc https://direnv.net/
- or use  ~/.bashrc 
- or whatever
  

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



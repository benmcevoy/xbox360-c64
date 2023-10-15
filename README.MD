# XBOX 360 Controller to C64

Try and get a USB 360 contorller to work on the c64

Oh dear.  I should really rename this repo, as the xbox controller is a pain...

So far can talk to PS4 dualshock and a generic switch controller.


TODO:

- read controller via tinyusb
- dpad is joystick
- A is fire
- B is UP
- Analog stick is POTX/POTY
- X and Y are c64gs secondary fire?
- PS4 controller?
- Web interface to configure? see the pico fight stick



## prepare
clone with submodules, e,g,

`git clone --recurse-submodules git://github.com/benmcevoy/xbox360-c64`

or you may need to run the below under the pico-sdk folder.

```
cd pico-sdk
git submodule init 
git submodule update
```

export environment variables 
- i use .envrc https://direnv.net/
- or use  ~/.bashrc 
- or whatever
  

`export PICO_SDK_PATH=/path/to/xbox360-c64/pico-sdk`

## cmake
```
cd src\build
cmake ..
```

## make

or use `./build.sh`

```
cd src/build/x360c64
make -j4
```

## build and deploy

```
# make executable
chmod +x ./build.sh
chmod +x ./deploy.sh
chmod +x ./monitor.sh
```

```
# get the picoprobe uf2 https://github.com/raspberrypi/picoprobe
# build, then deploy via picoprobe, then start monitoring UART

./build.sh && ./deploy.sh && ./monitor.sh
```

# Wiring

For USB HID power the PICO via VBUS and not VSYS. VBUS needs power so it can power the controllers.

I am able to power from the other pico acting as the pico probe.

It might be possible to power from the 5V line on the C64 joystick port, but I believe it only supplies 100mA?

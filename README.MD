# XBOX 360 Controller to C64

Try and get a USB 360 controller to work on the c64

xinput support is more or less copied from https://github.com/hathach/tinyusb/discussions/1929



So far I can read:

- xbox 360 wired controller
- generic wired switch controller
- ps4 ds4


TODO:

- DONE: read controller via tinyusb
- dpad is joystick
- A is fire
- B is UP
- Analog stick map to d-pad, with a threshold
- Web interface to configure? see the pico fight stick
- NOPE: Analog stick is POTX/POTY - yeah maybe not - got no DAC
- NOPE: X and Y are c64gs secondary fire? - this I can do
- DONE: PS4 controller? yep
- DONE: read controller via tinyusb

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
# debug build
cmake ../ -DCMAKE_BUILD_TYPE=Debug
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

# Schematic

For USB HID power the Pico via VBUS and not VSYS. VBUS needs power so it can power the controllers.

USB controller is plugged directly into the Pico USB port.  

Thanks to this, https://www.hackster.io/DocSnyderde/connect-usb-joystick-to-commodore-c64-2fb5ba

The power and joystick headers will be right angled or just solder direct.

![Board front](docs/assets/board_front.png?raw=true "Board front")


![Board back](docs/assets/board_back.png?raw=true "Board back")






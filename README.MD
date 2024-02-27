# XBOX 360 Controller to C64

Try and get a USB 360 controller to work on the c64

xinput support is from https://github.com/Ryzee119/tusb_xinput

DPAD and Right analog stick can be used as joystick.

A is fire.

B is up/jump.

X and Y are autofire.


So far I can read (refer: device.h):

- xbox 360 wireless controller :)
- xbox 360 wired controller 
- generic wired switch controller 
- ps4 ds4
- GP2040-CE ps4, xinput, arcade, gp2040, directinput

I cannot read but have on hand to test with:

- GP2040-CE ps3 
- GP2040-CE nintendo 
- real ps3
- 8bitdo ultimate

TODO:

- DONE: read controller via tinyusb
- DONE: dpad is joystick
- DONE: A is fire
- DONE: B is UP
- DONE: Analog stick map to d-pad, with a threshold
- NOPE: Web interface to configure? see the pico fight stick
- NOPE: Analog stick is POTX/POTY - yeah maybe not - got no DAC
- NOPE: X and Y are c64gs secondary fire? - this I can do
- DONE: PS4 controller? yep
- DONE: read controller via tinyusb

Things I wish I had done/done differently...

- connect all C64 pins to GPIO, for the POTX/Y as second buttons 
- use an USB header instead of the pico usb port (as it is a pain and needs an adapter)
- use the waveshare pico or a "stamp" board (it's a lot smaller)
- use the PCB manufacturer to add the transistors and resistors as SMD parts
- and basically get the size down and remove the need for USB A to micro adapter
- analog support, either a digital pot or DAC
- better use of the LED for status
- some way to configure it? not much to configure, but it would be nice.

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
cd ./src/build
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

In the current v02 design power is taken direct from the C64 control port. This mean the USB controller is also being powered from the C64. 

Do not plug in the adapter while the power is on.

![Board front](docs/assets/board_front.png?raw=true "Board front")

![Board back](docs/assets/board_back.png?raw=true "Board back")


# ChocoEF
A simple focuser made for use on multiple different microcontrollers and stepper motors drivers with STP/DIR inputs.
It uses a modified version of [hansastro's moonlite library](https://github.com/Hansastro/Focuser).

## Software
### Setup

1. Drop the ```Moonlite``` folder (inside Software) into your ```.\Documents\Arduino\libraries\``` folder.
2. Drop the ```focuser``` folder into your ```.\Documents\Arduino\``` folder and open it using the Arduino IDE.
3. Setup the stp, dir, MS1, MS2, MS3 in accordance with your own pinout. The led pin is used as a debug led, you can set it to an unused pin on your microcontroller. If you have more or less microstepping pins, add or remove the pins at the top of the document. Also add or remove the pin setup in the setup() function, as well as the microstepping setting in the ML_SF and ML_SH case statement (in my case I set it to 8th and 16th microstepping for the A4988). In some cases you might also have to set up Enable, Sleep and Reset pins. Again, if your situation differs from mine add or remove the pins from the top of the document and add or remove the pin setup in the setup() function.
(optional) you can set a sleep mode near the bottom of the document, this can be helpfull to reduce heat buildup or to save power. The stepper.setEnablePin() function can be used for this. Comment out if not used.
4. Upload the sketch by following the upload procedure required for your board.

### How to use

- Windows (ASCOM)

  Install the latest ASCOM driver from the ASCOM website, then install the Moonlite ASCOM driver from [focuser.com](https://focuser.com/downloads.php) (included in ZIP file that can be downloaded with the ```MoonLite DRO Setup.zip``` button). Then you can use your program of choice to use the focuser. The driver should automatically scan all the available com ports for a moonlite focuser.
- Linux (Ekos/astroberry system)

  Select the Moonlite focuser in the configuration settings.

Of course connect a USB cable to your microcontroller. The motor might run in the wrong direction. I'd recommend switching the motor's phases around to fix this.

### To be implemented

- Temperature compensation.

### Troubleshooting

> The motor isn't turning.

Check your wiring, read through the documentation of your stepper motor driver to see if there are any pins that need to be high, low or can be left floating (personally don't reccommend letting it float because something as little as your finger can make it flip to either side). It is also important to check if you are supplying a high enough voltage and don't have the current setting on the driver turned all the way down.
> The motor is just purring and not/barely moving.

The current setting and/or voltage might be too low or too high. You can also try switching the wires on one of the two motor phases around, this can help in case the motor is trying to step against itself. So like one step clockwise and then one step counterclockwise. What might also help is adding a large electrolytic capacitor in parallel with the driver's motor voltage input. Long cables and/or badly designed power supplies can cause problems because these motors can have very high current spikes. There is a chance it won't propperly step if there isn't a capacitor. Get a big one, between 100 and 1000 uF with a voltage rated for what you are using (take a safety margin, at least 5V or 25%, but more to be safe).
> The motor/driver is getting quite hot.

Try lowering the current setting. A focuser really doesn't need a whole lot of torque. What might also help is setting up a sleep function. Most drivers have some sort of sleep or enable pin. Do some research and find out if it is the case. In step 3 of the setup it is explained how it can be configured in software.

## Known Issues

- ~~NINA will give an error when you try to move larger steps.~~ (fixed) the microcontroller was always reporting that the motor was stopped, which the driver didn't seem to like.

## Hardware
Build at your own risk, I cannot guarantee that it will work for you. This form factor works for me, so it might not be ideal for your case.
In this case a SeeedStudio XIAO RP2040 is used. It is soldered directly onto the PCB like an SMD component, so without pin headers. This is because connecting and disconneting might dislodge the microcontroller from normal pin headers. It does mean that it is basically to impossible to get it off again. It uses a 5.5mm barrel plug for motor power, the pitch between the two pins about 6.3 mm for this example, if you have a connector with 6.5 mm pitch you will have to change that in the PCB design yourself. This design is meant to use terminal blocks with a pitch of 2.54 mm, but JST-XH and maybe JST-PH also fit. There is also a capacitor you need, anything between 100uF and 1000uF should be good. It needs to be rated for the supply voltage you want to use. Either 5V or 25% higher than your supply voltage, whichever voltage is highest. The PCB allows two different pitch capacitors: 2.5 mm or 5 mm, the maximum diameter is 10 mm, the maximum height depends on the pin headers but 10 mm can be assumed.
![image](https://github.com/user-attachments/assets/46584444-f6fd-4f3d-be6b-1ff7a88f74a7)
![image](https://github.com/user-attachments/assets/3aad311a-f688-4fb9-bef1-4d5c4ccda830)


The KiCAD files can be found under Hardware/ChocoEQ (KiCAD 8.0.6 or later required). You don't need to import the libraries if you just want to change some simple things. In case you do need the libraries, you can find them under Hardware/Libraries.

There are also gerber and drill files available in Hardware/gerbers, in case you want to order them without exporting from KiCAD.

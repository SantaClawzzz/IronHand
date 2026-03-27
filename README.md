# Evert keyboard

* Keyboard specs:
  * 1 nice!nano v2
  * 104 switches (MX)
  * 104 DO-35 diodes (1N4148)
  * 104 100nF 0402 capacitors (Any 100nF)
  * 106 LEDs (WS2812-2020) [Two LEDs are under MCU]
  * One 2000mAh 3.7V LiPo Battery [Bridge the Charging pads under nice!nano]
  * 1 battery connector (JST PH 2.0)

## How to flash

Connect with USB cable
 In v0 design no button is present to enter bootloader. This means that RST and GND will manually have to be shorted to enter bootloader. (DO NOT SHORT VCC BY ACCIDENT!)
 
 In v1 probably button will be implemented, just hold it down for 1s and let go.


This will enter bootloader and the nice!nano will look like a storage device to the PC (Like a USB stick)

Go to this github repositories "Actions" tab and choose the latest push. You should find the .uf2 file download under that.

Install this and drag the .uf2 file into the nice!nano storage, it should immediately upload it and flash it.
It might say it can't upload it duer to some error, just skip it, it uploads it and closes so the PC runs into error.


If all works perfectly the RED LED should switch off and only the BLUE should shine.



## Notes to consider when building custom keyboard
There is a FUCKTON of small nuances that can break your keyboard and nothing works, GPT is useless with this as well and will lead you down a rabbit hole. Either search forums, reddit, youtube and best of all the ZMK documentation page (trust me it's so fking good)

* There was an update so some info might be deprecated, like shields being move from the config folder into the boards/shields/"ShieldName" folder, so most recent info is on the ZMK page
* Firstly make sure your directory is correct, this repo should be a good example
  * Make sure your shields files are in the EXACT same name Shield folder so boards/shield/"ShieldName"  <--FOLDER
  * If you made a new keyboard base on the default given ones then you have two options
    * Change the file in config/nice60.keymap (in my case) to be YOUR keymap, since ZMK reads this before the shield (I think I have mine setup weird tho)
    * Change the files name, so nice60.keymap to idontfkingcare.keymap, so ZMK won't find the keymap and will default to the one in shield, if not you have a different error (possibly in the name)
 * When you upload the project to github to compile you can look under the actions and West build and the devicetree for what ZMK has chosen, this is insanely good for debugging
   * If you see your keymap, overlay and shield being chosen (and correct board) then you did the naming good, good job
   * If you don't see it or even get the classic error of "I can't fking find your shield, you named it wrong" and it's not named wrong. For me it wasn't in the named shield folder as I mentioned before, all files were in the Shields folder, not one folder deeper
   * Other errors might be due to Kconfig files incorrectly done, look at what I have done, for shields it usually SHIELD_"NAME" and boards BOARD_"NAME" but this is if you make your own devicetree and so on.
     * There are more problems, but I suggest the ZMK docs.
   * Another classic one is in the build.yaml the shield isn't defined
     * Make sure it is:

include:

\- board: nice_nano

  shield: Evert

And more importantly make sure the spacing is good!! I had one space too few and fked me for hours on end
the s of shield and b of board should be one the same line!

* Other problems are usually from the compile you can debug, I would not recommend GPT for this.
* Use other keyboards as refrence and try to dig through the dts files to find the node or whatever is pricking you up the ass. It's also good to try to learn Zephyr
* To get the final product I re-made this project 4 times, if its too far gone, try to make a new one and copy only the essentials!
* Try to get a good first commit and try changing ever so slightly each step, don't take a big leap.

Good luck!

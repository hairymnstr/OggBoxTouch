# OggBox Touch

This is a new project to make a standalone music player in the iPod style.  Much like the original [OggBox](https://github.com/hairymnstr/oggbox) it's a portable dedicate music playing device with no wireless connectivity.  This version (as the name suggests) is based around a large touch screen interface and plans to implement all the CODEC stuff in software.

It is based on some work I was doing on a "NoodleGame" project to build a portable games console using dev-boards and wires.  That project ran into difficulties as the screen I was using couldn't do anything like a usable frame rate but for a simple menu based music library it should be fine.

For now it's all based around an STM32F429I-Disc1 discovery board although I've taken the screen off and attached a bigger SPI one, added an I2S DAC and SD card socket.  If it all comes together I will look at a proper PCB with battery management etc.
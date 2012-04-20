  ledc
=======

Simple app to toggle the BeagleBone LEDs

Used to generate some LED activity for another project.

The program ledc cycles through the 4 User Leds on the BeagleBone turning
one on at a time.


  Usage
-------

Assuming you have a compiler on the BeagleBone

    git clone git://github.com/Pansenti/BoneDemo.git
    cd BoneDemo/ledc
    make
    sudo ./ledc


Use Ctrl-C to kill the program and it will put the leds back into their
default state.

The program needs to run as root in order to play with the entries under

    /sys/class/leds


#!/usr/bin/env python

import os
from time import sleep
PINS = {
    'upper middle': 24,
    'upper left': 23,
    'upper right': 25,
    'center middle': 22,
    'lower left': 4,
    'lower right': 18,
    'lower middle': 17,
    'decimal point': 21
}

DIGITS = {
    0: ['upper middle', 'upper left', 'upper right', 'lower left', 'lower right', 'lower middle'],
    1: ['upper right', 'lower right'],
    2: ['upper middle', 'upper right', 'center middle', 'lower left', 'lower middle'],
    3: ['upper middle', 'upper right', 'center middle', 'lower right', 'lower middle'],
    4: ['upper left', 'upper right', 'center middle', 'lower right'],
    5: ['upper middle', 'upper left', 'center middle', 'lower right', 'lower middle'],
    6: ['upper middle', 'upper left', 'center middle', 'lower left', 'lower right', 'lower middle'],
    7: ['upper middle', 'upper right', 'lower right'],
    8: ['upper middle', 'upper left', 'upper right', 'center middle', 'lower left', 'lower right', 'lower middle'],
    9: ['upper middle', 'upper left', 'upper right', 'center middle', 'lower right', 'lower middle'],
}

segment_files = {}

def light_segment(segment):
    pin = PINS[segment]
    pin_file = segment_files[pin]
    pin_file.write('0')
    pin_file.flush()

def shadow_segment(segment):
    pin = PINS[segment]
    pin_file = segment_files[pin]
    pin_file.write('1')
    pin_file.flush()

def display_digit(digit):
    shadow_all()
    for segment in DIGITS[digit]:
        light_segment(segment)

def shadow_all():
    for segment in PINS.iterkeys():
        shadow_segment(segment)

if __name__ == "__main__":
    os.chdir("/sys/class/gpio")
    with open('export', 'a') as export:
        for pin in PINS.itervalues():
            export.write(str(pin))
            export.flush()
            with open("gpio%d/direction" % pin, 'w') as direction:
                direction.write('out')
            segment_files[pin] = open("gpio%d/value" % pin, 'w')

    time = 0.50
    while time >= 0:
        for x in xrange(10):
            display_digit(x)
            sleep(time)
        time -= 0.06


    with open('unexport', 'a') as unexport:
        for pin in PINS.itervalues():
            unexport.write(str(pin))
            unexport.flush()
            segment_files[pin].close()

#!/usr/bin/python
# -*- coding: utf-8 -*-

# Adds a "night vision" effect to an image.
# Modifies the image in place, so make a backup or lose your data.

import optparse
import PIL.Image as Image
import PIL.ImageDraw as ImageDraw
import PIL.ImageEnhance as ImageEnhance

import sys


def nightvision(orig, brightness, lines, colour, alpha):

    line_colour = (colour[0] + 30,
                   colour[1] + 30,
                   colour[2] + 30)

    B = ImageEnhance.Brightness

    im = B(orig).enhance(brightness)

    mask = im.copy()
    draw = ImageDraw.Draw(mask)
    draw.rectangle([(0, 0), mask.size], fill=colour)
    for row in range(0, mask.size[1], mask.size[1]/lines):
        draw.line([(0, row), (mask.size[0], row)], line_colour)

    return Image.blend(im, mask, alpha)


def main():
    parser = optparse.OptionParser(usage="%prog [options] FILE")
    parser.add_option("-b", "--brightness",
                      metavar="B", default=1.0, type="float",
                      help="enhance brightness with factor B")
    parser.add_option("-l", "--lines",
                      type="int", metavar="L",default=30,
                      help="insert L scan lines (default 30)")
    parser.add_option("-m", "--mask",
                      metavar="M", default="33cc00",
                      help=("color (RGB) for the night vision mask "
                            "(default 33cc00)"))
    parser.add_option("-a", "--alpha",
                      metavar="A", default=0.5, type="float",
                      help="ratio between night vision mask and image")
    (options, args) = parser.parse_args()

    if len(args) != 1:
        parser.error("FILE argument is required")

    try:
        options.mask = (int(options.mask[0:2], 16),
                        int(options.mask[2:4], 16),
                        int(options.mask[4:6], 16))
    except ValueError:
        parser.error("malformed mask: must be 6 hex chars")

    im = nightvision(Image.open(args[0]),
                     options.brightness,
                     options.lines,
                     options.mask,
                     options.alpha)
    im.save(args[0])

if __name__ == '__main__':
    main()

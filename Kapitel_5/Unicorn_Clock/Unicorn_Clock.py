# Original Code:
# https://gist.github.com/jan-martinek/d1051350be5a8def4df0332b9b7a4ff6
#!/usr/bin/env python

# H      | M*10 | M
# 1 5  9 | 1 5  | 1 5 9
# 2 6 10 | 2    | 2 6
# 3 7 11 | 3    | 3 7
# 4 8 12 | 4    | 4 8

import time
import unicornhat as unicorn

print("Unicorn pHAT clock")

unicorn.set_layout(unicorn.AUTO)
unicorn.rotation(0)
unicorn.brightness(0.5)
width,height=unicorn.get_shape()

def showNum(num, cols, colOffset, r, g, b):
    for col in range(cols):
        for row in range(4):
            if (col * 4) + (row + 1) <= num:
                unicorn.set_pixel(col + colOffset, row, r, g, b)

def showHours():
    h = int(time.strftime('%I'))
    showNum(h, 3, 0, 255, 0, 0)

def showTenMinutes():
    tm = int(time.strftime('%M')) / 10
    showNum(tm, 2, 3, 0, 0, 255)

def showMinutes():
    m = int(time.strftime('%M')) % 10
    showNum(m, 2, 5, 255, 0, 255)

while True:
    unicorn.off()

    showHours()
    showTenMinutes()
    showMinutes()    
    
    unicorn.show()
    time.sleep(1)
    

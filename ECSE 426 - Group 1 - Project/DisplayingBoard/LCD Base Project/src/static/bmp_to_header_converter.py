"""
    Converts a bitmap image (.bmp) to text usable in a header file for deployment on an STM board.

    This script converts our Trottier floor map bmp file into hex that is deployable
"""

import sys

if __name__ == "__main__":
    mem = []
    if len(sys.argv) > 1:
       fname = sys.argv[1]
    else:
       fname = 'trottier16.bmp'
    with open(fname, 'r') as f:
        byte = f.read(1)
        while byte != "":
            mem.append(ord(byte))
            if len(mem) == 4:
                print '0x%02x%02x%02x%02x,' % (mem[0], mem[1], mem[2], mem[3]) 
                mem = []
            byte = f.read(1)
    if len(mem) != 0:
        print "0x%02x%02xffff" % (mem[0], mem[1])

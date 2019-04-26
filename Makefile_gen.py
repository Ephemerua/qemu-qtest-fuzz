# edit the Makefile in a extremely ugly way
# a exsample to fuzz pci network device
import sys
import os

if sys.argc != 3:
    print("Usage :./Makefile_gen [src Makefile] test_module_name")
    return 0

f = open(sys.argv[1])
old = f.read()
f.close()

new = old.replace("rtl8139", sys.argv[2])

bak = open(sys.argv[1]+".bak", "w")
bak.write(old)
bak.close()

f = open(sys.argv[1], "w")
f.write(new)
f.close()

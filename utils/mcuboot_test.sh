# python ../mcuboot/scripts/imgtool.py keygen -k filename.pem -t rsa-2048
# python ../mcuboot/scripts/imgtool.py getpub -k filename.pem
python ../mcuboot/scripts/imgtool.py sign  -k img_prv.pem --public-key-format hash --align 4 --version 1.0.0 --header-size 0x1000 --align 4 --slot-size 0x31000 --pad-header CherryUSB-CH58x-DAPLink.hex sign-CherryUSB-CH58x-DAPLink.hex
python uf2conv.py sign-CherryUSB-CH58x-DAPLink.hex -o sign-CherryUSB-CH58x-DAPLink.uf2 -c -f 0xabcdc582
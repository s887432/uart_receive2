# uart_receive2
UART test tool

Usage
compile
$ git clone https://github.com/s887432/uart_receive2.git
$ cd uart_receive2
$ mkdir build
$ cd build
$ cmake .. -DARCH=arm9
where arm9 is built for sam9x60, sam9x75
cortex is built for SAMA5, SAMA7 series
$ make

The output will be placed at project folder.
If built for ARM9, the output folder will be arm9_bin
If built for CORTEX, the output folder will be cortex_bin

Have fun!!!
Patrick @ Taipei

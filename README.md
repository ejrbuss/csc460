# CSC 460 Project

This is the source code repository for Team 3 of the Universiity of Victoria's CSC 460 Design and Analysis of Real-time Systems Spring 2019. For documentation and dissccussion of the design proceess please visit our [projets site](https://sites.google.com/view/csc-460/home).

## Running

To build this project you need the following dependencies:

 - [Mekpie](https://ejrbuss.net/mekpie/): our chosen build tool
 - [avr-gcc](https://www.nongnu.org/avr-libc/): our chosen compiler
 - [avrdude](https://www.nongnu.org/avrdude/): our downloader
 
If you have all these dependencies navigate to one of the project folders:

```shell
$ cd project-1
```

And run `mekpie test <name-of-test>` to run one of our project files. This will clean, build, and then attempt to download the specified program to a provided serial port.

```
$ mekpie test lcd
```

# Contact

For more information see my [website](https://ejrbuss.net) or contact me by [email](root@ejrbuss.net).

# SmartIO-HostInterfaceLayer-STM32F4
Smart.IO Host Interface Layer for STM32F4 written in GCC and STM LL library

Smart.IO https://imagecraft.com/smartio is a new way of creating UI (User Interface) for embedded designs. Indeed, the word UI is typically not associated with an embedded design unless and until it becomes a product. With Smart.IO, all that changes: with Smart.IO, an embedded designer can create a UI in a few lines of code, and without writing any wireless or app code, the UI will run on any iOS and Android devices. The only investment is to add the Smart.IO hardware module to the design, and to build the firmware with the Smart.IO API library, which is provided in source code form. The system is compatible with nearly all microcontrollers. 

The Host Interface Layer converts the Smart.IO API into calls into the Smart.IO hardware through the SPI interface. Most of the code is written in portable C. The hardware specific code consists of:

1 - SPI read and write
2 - Accept a pin interrupt for the Smart.IO module to alert the host firmware that data is available
3 - Toggling a pin to RESET the Smart.IO module
4 - Hardware initialization for above

In this example, the hardware specific code is written using STM LL library code. The host code calls the function ST_HardwareInit() to initialize the peripherals.

To port the Host Interface Layer to another microcontroller or different compiler, you just need to implement the hardware specific code.

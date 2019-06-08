Commander is a system for handling commands sent over serial ports or other Stream objects.

It allows you to easily define a list of text commands and assign a function to handle each command. All the work of reading the incoming serial data and identifying the appropriate function is handled by the commander object.

As well as reading commands and passing them to the handler functions, Commander can also route or copy data to another port, echo data back to you and redirect responses to a different port.

Because Commander uses Streams, it can be attached to a file on an SD card and read commands from that file.


When using SDFat that supports multiple open files, Commander can be attached to a file and read commands, whilst writing any responses to a different file, and even copying those responses to a third port, for example a serial port.

Written by Bill Bigge.
MIT license, all text above must be included in any redistribution

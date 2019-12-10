# Commands and basics utilities of Contiki-NG

## Introduction

The following document is a basic and informal guide that should help you get
started with Contiki-NG. It will have a focus on the thing that are harder to
get working. You may also refer to Contiki NG online Wiki for better
understanding. <<http://contiki-ng.org/>>

This guide focuses on using Contiki with OpenMote B platform. <<http://www.openmote.com/>>

The OpenMote B board has 2 radio communication chips. The one that will be used
here is the TI CC2538.

From Wikipedia:

Contiki is an operating system for networked, memory-constrained systems with a
focus on low-power wireless Internet of Things devices. Extant uses for Contiki
include systems for street lighting, sound monitoring for smart cities,
radiation monitoring, and alarms. It is open-source software released under a
BSD license. 

See more at <<https://en.wikipedia.org/wiki/Contiki>>.

## Setup and build Contiki-NG

### Windows based system

_Remark: Using a Gnu/Linux system to build Contiki would probably be easier._

This post may help getting started on Windows:
<<https://stackoverflow.com/a/33285483>>. Basically, the idea is to use Cygwin to
install the same toolchain as on a Gnu/Linux system, but on Windows. Mainly GCC
for ARM "gcc-arm-none-eabi". See <<https://cygwin.com/>>.

Another possibility is to use a virtual machine and build contiki inside it.
There is a pre-made one that can be found on the Internet called Instant
Contiki.

### Gnu/Linux Debian based system

#### Install the development tools for Contiki-NG

```
sudo apt-get update
sudo apt-get install build-essential doxygen git curl wireshark python-serial srecord rlwrap
```
	
The `ifconfig` utility is necessary as well. It can be installed with:

```
sudo apt-get install net-tools
```

#### Install GCC for ARM

```
sudo apt-get install gcc-arm-none-eabi
```

If using a non-Debian based Gnu/Linux distribution, search for a package with
a similar name, and install it with your package manager.

#### Others necessary configurations

Give permissions to user to capture packets, open serial ports…

```
sudo usermod -a -G wireshark <user> 
sudo usermod -a -G plugdev <user> 
sudo usermod -a -G dialout <user> 
```

Optionally you can install the coap-client software to make tests.

```
sudo apt-get install libcoap-1-0-bin
```

From here the relative filesystem paths will be given from the root (top) folder
of the project.

To download the additional / third party sources, you run:

```
git submodule update --init --recursive
```

This has to be done at least once before the first build.

## Contiki-NG structure and How to

Contiki NG structure (main directories, from root folder):

* `os`: Operating System main files
* `arch/cpu`: Code files specific to a particular chip.
* `arch/platform`: Code files specific to a particular board.
* `tools`: Tools to flash and communicate with a device. Usually run on the
  host.
* `examples`: Software projects and examples.

___Important Note:___ all the make commands described below must be run from a
specific project folder. It is possible to navigate to such a folder, with
a command like: `cd examples/hello-world`.

### Compiling a program

To compile a program, from the project's folder (for example:
`examples/hello-world`), run:

```
make TARGET=<target> [options]
```
	
Where `<target>` is the name of the targetted hardware platform, for example
openmote-b, z1, native, etc. To see the list of available hardwares you can use
the command:

```
make targets
```

The make command might receive other parameters in `[options]`

You can save a default target with the command:

```
make TARGET=<target> savetarget
```
	
to avoid typing the `TARGET=` part every time. This will create a file named
`Makefile.target`.

The compilation process (if it succeds) will create a folder called `build` and
a file named `<project>.<target>`

If the target was native, it is possible to run the freshly compiled binary
with:

```
./<project>.native
```

`<project>` being the name of the current project.

For an embedded target, the firware has to be uploaded on the device as
described in the next section.

## Flash the hardware

To write the program in the hardware, run the following command:

```
make <project>.upload TARGET=<target> PORT=<usbport>
```

Note that if the `<target>` was saved isn't necessary put `TARGET=<target>`.

In the command above, `<project>` should be replaced with the name of the
current project, and `<usbport>` should be replaced with the file associated
with the appropriate USB port.

The file associated with the USB port depends mainly on the driver used. For
serial (UART) communication with the TI CC2538, the associated file will be:

```
/dev/ttyUSB1
```

Asuming it is the only USB device plugged in at the moment.

### Flashing Example

Suppose that we have the project *hello-world* with its *Makefile* and
*hello-world.c* file and an *openmote-cc2538* board connected to the *USB1*
port. To compile the code and flash de board you may run:

```
make TARGET=openmote-cc2538
make hello-world.upload TARGET=openmote-cc2538 PORT=/dev/ttyUSB1
```

## Other utilities

### Clean

To clean the current project:

```
make clean
```

Clean for every target:

```
make distclean
```

To connect to the serial port and watch what is being printed there (for
instance the printf, logs, etc):

```
make serialview TARGET=<target> PORT=<usbport>
```
or

```
make login TARGET=<target> PORT=<usbport>
```
	
To take a look at Contiki's configuration:

```
make viewconf
```

For more options

```
make help
```

## About project's Makefile 

### Enable the shell on the board

This module allows you to execute shell commands directly on the board. The
availables commands will depend of the added modules.

1) Add the following line to the project's `Makefile`.

```
MODULES += os/services/shell
```
	
It should look similar to this:

```
CONTIKI_PROJECT = hello-world
all: $(CONTIKI_PROJECT)

# Enable shell module
MODULES += os/services/shell

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
```

2) Connect to the serial port as seen in the previous section

3) Press any key to activate the shell

4) The `help` command will show us the available commands list

## Enable logs

You must indicate which kind of logs you want to print in the serial port in the
project's configuration file, which is named `project-conf.h`. The options are:

```
#define LOG_CONF_LEVEL_IPV6                        LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_RPL                         LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_6LOWPAN                     LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_TCPIP                       LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_MAC                         LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_FRAMER                      LOG_LEVEL_DBG
```

For example if you only want to watch RPL's logs, you must add in the
project-conf.h file:

```
#define LOG_CONF_LEVEL_RPL                         LOG_LEVEL_DBG
```

Open the serial port, as explained above, to view the logs messages.

## CoAP

Constrained Application Protocol (CoAP) is the application layer protocol used
in Contiki, and many other embedded applications. For more info, see
<<https://fr.wikipedia.org/wiki/CoAP>>, and
<<https://tools.ietf.org/html/rfc7252>>.

To be able to use CoAP in a project, it is necessary to add the next line in the
`Makefile`:

```
MODULES += os/net/app-layer/coap
```
	
In the `examples/CoAP` folder there is a CoAP server and a CoAP client already
implemented, which can be used as example.

### Communications overview

Often in applications, the requests are performed by a client located
outside of the Wireless Area Network, which will observe a resource located in a
server on a leaf node. 

For instance in the diagram below is shown a CoAP server in a leaf node and a PC that wants to access it.

For make possible that communication, is necessary introduce a node named border router (see border router section below).

This node has connected in it USB interface the PC and in it wireless interface the CoAP server, allowing the communication 
between the PC and the CoAP server.

```
   ----------         -----------               -----------
  |    PC    | <===> | Root node | < - - - - > | Leaf Node |
   ----------         -----------               -----------
    tunslip6    USB     border       Wireless      CoAP
                        router        Link        server
```

### Server

First, navigate to the `examples/coap/coap-example-server` folder.

To use the CoAP server, edit the file `coap-example-server.c`. You need to
activate the resources you want and indicate their URL.

1) To activate a new resource:

```
extern coap_resource_t
  	res_hello,
  	res_push,
	.
	.
	.
 	res_newresource;
```
 		
2) To indicate the URL of the new resource
 
```
coap_activate_resource(&res_hello, "test/hello");
coap_activate_resource(&res_push, "upload/push");
.
.
.
coap_activate_resource(&res_bye, "newresource");
```

When the a request is received on a specific URL, let's say
`coap://[server_ipv6]/myurl/newresource`, the `res_bye` callback registered
above will be called. It should be implemented in `resources/res-newresource.c`,
in the coap-example-server project.

In the resources file I highlight the following code:

```
RESOURCE(res_resourcename,
	"description",
	get_method_function,
	post_method_function,
	put_method_function,
	delete_method_function);
```
	
Here you indicate to which function call depending of the coap method received
(GET, PUSH, PUT or DELETE).

### Client

This section describes how to use a client on the device. Usually the client is
located outside of the Wireless Area Network.

The most important thing is to set the URL that will be request, which can be
done with:

```
#define SERVER_EP "coap://[server_ipv6]"
```

After this you have to set the path to the resource that request will target.

For do it first is necessary indicate how many resources will be requested in

```
#define NUMBER_OF_URLS
```

Immediately it is defined a string array with the resources

```
char *service_urls[NUMBER_OF_URLS]
```

For instance:

```
#define NUMBER_OF_URLS 3

char *service_urls[NUMBER_OF_URLS] = { "test/resource1", "test/resource2","test/resource3"};
```
Now is possible handle the request using the string array `service_urls` (see CoAP client example).

### Border router


The border router is a node that allow to connect the wireless network (between the leaf nodes) with another network (see communication overview above).


1) Navigate to the `contiki-ng/examples/rpl-border-router` folder.

2) Build and upload this project to the hardware target.

3) Create a tunnel interface to connect the border router with the computer. For
this you must go to `contiki-ng/tools/serial-io` and execute:

```
sudo ./tunslip6 -s /dev/<node_usb_port> aaaa::1/64
```

The first time you have to build the `tunslip6` utility by calling `make` in
this folder.

In this example, `aaaa::1/64` will be the IPv6 of the tunnel interface, the
output of the command will show you the IPv6 of the border router.

Now it is possible to make a CoAP request from the computer to a node using the
coap-client command (examples of a POST, then a GET request):

```
echo "test content" | coap-client -m post coap://[server_ipv6]/myurl/myapp -f -

coap-client -m get coap://[server_ipv6]/myurl/myapp
```

See `man coap-client` for more information.

Note: It is possible to configure the border router in other ways, see <<https://github.com/contiki-ng/contiki-ng/wiki/Tutorial:-RPL-border-router>>

## Time Slotted Channel Hopping (TSCH)

TSCH is a method to access a shared medium.

See <<https://en.wikipedia.org/wiki/Time_Slotted_Channel_Hopping>> and
<<https://ieeexplore.ieee.org/document/7460875>>.

What the IEEE 802.15.4 does not say is how to build the schedule. Contiki comes
with two scheduling strategies implemented.

### TSCH Minimal

 TSCH minimal emulates an always-on link on top of TSCH. The schedule consists in a single shared slot for all transmissions and receptions, in a slotframe of fixed length. <<https://github.com/contiki-ng/contiki-ng/wiki/Documentation:-TSCH-and-6TiSCH>>

By default the MAC sublayer is CSMA, to use TSCH is necessary indicate it in
the Makefile with:

```
MAKE_MAC = MAKE_MAC_TSCH
```

If you don't indicate anything else the default implementation of TSCH is
TSCH minimal.

If you look now the MAC's logs you will see the nodes scanning the channels,
looking for the EB.

To establish the communications between the nodes, one of the nodes must be the
coordinator. You have two ways of do this:

1) From the board shell using the command

```
tsch-set-coordinator 1
```

2) Directly from the code including the `tsch.h` library and calling to the
function `tsch_set_coordinator()` at the start of the main thread.

```
#include "tsch.h"
.
.
.
tsch_set_coordinator(1);
```
	
From the shell the following commands could be useful:

```
tsch-status
tsch-schedule
```

## TSCH Orchestra

Orchestra is an autonomous scheduling solution for TSCH, where nodes maintain their own schedule solely based on their local RPL state. There is no centralized scheduler nor negociatoin with neighbors, i.e. no traffic overhead. The default Orchestra rules can be used out-of-box in any RPL network, reducing contention to a low level. <<https://github.com/contiki-ng/contiki-ng/wiki/Documentation:-Orchestra>>


To use TSCH Orchestra implementation add the following in the Makefile:

```
MAKE_MAC = MAKE_MAC_TSCH
MODULES += os/services/orchestra
```

In the project-conf.h

```
#define TSCH_SCHEDULE_CONF_WITH_6TISCH_MINIMAL 0
#define TSCH_CONF_WITH_LINK_SELECTOR 1
#define TSCH_CALLBACK_NEW_TIME_SOURCE orchestra_callback_new_time_source
#define TSCH_CALLBACK_PACKET_READY orchestra_callback_packet_ready
#define NETSTACK_CONF_ROUTING_NEIGHBOR_ADDED_CALLBACK orchestra_callback_child_added
#define NETSTACK_CONF_ROUTING_NEIGHBOR_REMOVED_CALLBACK orchestra_callback_child_removed
```	
```

and in the code

```
#include "orchestra.h"
```

The implementation of Orchestra is in `os/services/orchestra`, by default has
4 rules programed. 

In the file `orchestra-conf.h` you can choose which rules use (from the default
rules or your own rule).

**Note:** The number of TSCH links depend of the implemented rules. If the
maximum number of TSCH links defined in the file `os/net/tsch/tsch-conf.h` in
the line  `#define TSCH_SCHEDULE_MAX_LINKS` is less than the number of TSCH
links needed by Orchestra you will have a compilation error.

## Process and Proto Threads

Define a process:

```
PROCESS(processID, "Process Desc");
```
	
Define a proto thread

```
PT_THREAD(protothreadID(struct pt *pt));
```
	
	
Implementing a process

```
PROCESS_THREAD(processID, ev, data)
{
	PROCESS_BEGIN();
		.
		.
		.
	PROCESS_END();
}
```

Implementing a proto thread

```
PT_THREAD(protothread(struct pt *pt))
{
	PT_BEGIN(pt);
		.
		.
		.
	PT_END(pt);
}
```

You can run automatically a process at the beginning of the program using
`AUTOSTART_PROCESSES(&processID1, &processID2);`. To call a process from a
process is necessary use `process_start(&processID, "data");` (Data is void *
type and is received by the process in `data` parameter).

To run a proto thread fist you must define a handle:

```
static struct pt pt_handle;
```

Then the instruction to run the proto thread is

```
PROCESS_PT_SPAWN(&pt_handle, protothreadID(&pt_handle));
```

**Note:** I added to the examples folder the project "process_test", where you
can find a basic process implementation.

## UART

For UART I modified the UART example of Contiki-NG, in my version I created two
processes, one for sending data and other to receive data, thus it is easy to
adapt the code to your own implementation.

The project is in `examples/uart_test` folder and the file is `test-uart.c`.

### Read and Write from PC using busybox and microcom

To listen to the serial port from a PC (with Linux) we can use the following
command:

```
busybox microcom -s 115200 /dev/<USB_PORT>
```

Where `<USB_PORT>` is the file representation of the serial port you want to
use. Ex: `ttyUSB1`.

To send a message from a PC we can also use the following command:

```
echo "message" | busybox microcom -s 115200 /dev/<USB_PORT>
```

## OpenMote-B

![OpenMote-B](OpenMote-b.jpg)


## References
 
[Contiki-NG Wiki](https://github.com/contiki-ng/contiki-ng/wiki)

[GCC Home page](http://gcc.gnu.org/)

---
IMT - Atlantique

Juan Navarro - juan-antonio.navarro-gutierrez@imt-atlantique.fr

Last revision 7/06/2019
 

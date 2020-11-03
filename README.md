# gpioObserver #

Checks periodically if a GPIO pin on a Raspberry Pi has reached a desired state and calls external program afterwards.


## Build instructions

Open your terminal and enter

```
git clone https://github.com/hardcodes/gpioObserver.git
cd gpioObserver
qmake
make
sudo make install
```

For more details about this tool, look here:

https://hardcodes.github.io/20130924--1.html


## Recap

This remains here mainly for historic reasons. What bugged me from the beginning is the handling of the GPIO pins on the Raspberry Pi. I would have expected an inteface where you can register for an interrupt and would be notified from the kernel when a change occured. All libraries that I found so far do some kind of polling (even the ones that create some kind of interrupt for you).

Writing this with Qt was rather complicated, today I would use Rust for the job.
# Simple HC-SR04 UltraSonic Range Finder demo

This is just some code I wrote to test out some ideas for implementing a sensor
using the Arduino framework without using blocking delay calls or busy waiting.

For this demo, I'm using a WeAct Studio Black Pill STM32F401CCU6 board:

https://stm32-base.org/boards/STM32F401CCU6-WeAct-Black-Pill-V1.2.html

The HC-SR04 has two important pins: trigger and echo.

I connected PA0 to trigger and PA1 to echo. I used a voltage divider on PA1 to
lower the voltage from the HC-SR04 to safely match the 3.3V logic of my dev
board.

The HC-SR04 does nothing until you pulse the trigger pin with a 10us HIGH
signal. It then emits an ultrasonic signal and when it detects the echo, it
pulses the echo pin high.

You can determine the time between the signal being emitted and the echo being
sensed by the duration of the echo pin high signal.

To model this, I use a set of discrete states, microsecond timestamps, and an
interrupt handler on the echo pin.

During execution, the state transitions through the following sequence:

```
ready -> triggered -> echo started -> echo ended -> ready
      ^             ^               ^             ^
      |             |               |             |
    loop    echo_pin_change  echo_pin_change     loop
```

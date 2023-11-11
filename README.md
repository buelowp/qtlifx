# Qt LIFX Library

A comprehensive Qt5 based LIFX accessor library. The goal is to allow for simple API based access
to all LIFX bulbs on a single network. This library uses the broadcast UDP solution published by
LIFX linked below.

This library is a simple asynchronous solution with a defined API. You can get and set bulb
details individually or by group. You can change bulb colors independently or by group. Bulb
discovery is automatic, and stateless. Once a bulb has been discovered and the newBulbAvailable signal
has been published, the bulb can be used by the application.

This library provides a full abstraction. You don't need to know anything about the bulbs or network
to use it. Simply wire up the signals/slots to your code, and call initialize().

The purpose of this library is simply to allow me to run a Christmas display that has both LIFX
and Hue bulbs. There are currently some apps on phones/tablets than can do this, but getting both
at the same time is complicated, expensive, and honestly, doesn't work as well as I would like. I have
both types of bulbs around my house, so being able to use them for displays is a priority for me.

## BUILD

```
git clone github.com:buelow/qtlifx.git
mkdir build
cd build
cmake ..
make
make install
```

To use this in your library, simple add qtlifx to your link step

## HOWTO

Find the Doxygen documentation at https://buelowp.github.io/qtlifx/

See the discover example in the examples directory, but it's pretty simple to use.

The design is to create a LifxProtocol which owns the comms between your app
and the bulbs. On start, fire off a discover and as bulbs reply, the protocol will
send a signal for each new bulb and group it finds.

The calling application can then manage each bulb as needed. Any specific operation
on a bulb or group can be done with LifxBulb and LifxGroup directly. You should not have
to call into LifxProtocol other than to run a discover.

The bulbs have a configurable timer which allows them to ask for status on an interval.
That interval defaults to 10 seconds, but calling setInterval will change it.

The entirety of this library revolves around the bulb. The bulb has enough logic to
handle itself. The protocol is only there to jump start as something needs to request
discovery if nothing exists at all.

* Do not delete a LifxBulb in your code, bad things will happen.

The LAN API is documented at https://lan.developer.lifx.com/docs/introduction

## PRODUCT

LIFX provides a JSON document with all of their products identified by VID/PID. This document
is pretty critical to a fully functioning library as each product has some caveats about
what it can support versus what it cannot. The most important distinction is that some bulbs
have different Kelvin min/max values. Without that decoded, the default is much smaller than what
is possible, and you may lose some capabilities when trying to communicate with your bulbs.

It is possible to set product capabilities prior to running initialize() or after. However, if done
after, you will need to accept that getting a newBulbAvailable() signal does not mean the bulb
is ready to use. It *can* be used, but it will be lacking it's full potential capabilities.

You can get the products.json file from https://github.com/LIFX/products

## COLORS

So, LIFX uses HSBK (Hue, Saturation, Brightness, and Kelvin) as it's color space. If you're familiar
with Hue, they use an XY color space. Qt has QColor, which is RGB based, but does provide an HSV ability.
Overall, mapping doesn't work great now. Setting the colors is pretty good, but making them look good in a
GUI is tricky.

## RELEASE

* BETA: This works pretty well, and has been run though valgrind to prove it doesn't currently leak memory
  * Testing done is still somewhat limited, but that will improve soon I hope

## NOTES

* It does work on the Pi just fine, I'm using it that way now
* The API is still very early in development. It works for me, I don't know that it will work for anyone else ATM. I'll publish a release when I think it's ready.
* This is a LAN library, and only works when local to the bulbs. It cannot be used outside the home network the bulbs are on.
* This library does not support zones or tiles yet. That may come in the future, but for now, I don't have any to test against.
* The get/set API is incomplete still, and does not support a lot of the set functions provided by the LIFX API.
  * e.g. reboot/set label/set group.
* LIFX doesn't do push notifications, the lights won't tell us if some other app changes them.



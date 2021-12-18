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

See the example main.cpp in the examples directory, but it's pretty simple to use.

```
LifxManager *manager = new LifxManager();
connect(manager, &LifxManager::newBulbAvailable, this, &Class::handleNewBulb);
connect(manager, &LifxManager::bulbDiscoveryFinished, this, &Class::handleEndOfDiscovery);
manager->initialize();
```

Once you have received the bulbDiscoveryFinished(bulb) signal, that bulb has completed discovery
and can be queried or set as needed. If you choose to provide the product definitions (see below), then
each bulb includes all of the LIFX capabilities. Otherwise, some functions may not work as the lights
are not assumed to support them by default.

```
QFile file("products.json");
if (!file.open())
    handle_error()
QByteArray json = file.readAll();
QJsonDocument doc = QJsonDocument::fromJson(json);
if (doc.isObject())
    manager->setProductCapabilities(doc);
```

The entirety of this library revolves around two objects, the manager and a bulb. The manager
is created by the application, and is what does the work. The bulb is the complete code
representation of a LIFX product. Each of these should be treated as immutable and owned
completely by the library.

* Do not delete a LifxBulb in your code, bad things will happen.
* Do not try to change the contents of the maps, bad things will happen.

It is possible to query for bulbs in a variety of ways, and you can talk to them
using a pointer to the object, the bulb name, the bulb group name, the bulb group UUID,
and the bulb MAC address.

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

## CODE

I did as much as possible to provide helpers where that made sense to me at least. There is an HSBK
object which allows for easy set and go for a set of colors, or to create a color in the native
LIFX format. Things like Red (360), Yellow (120), etc are based on the color wheel precentages. Orange is approximiation.
The white values included are roughly tuned to the app, as I got them from the app directly. So, neutral
here is the same as neutral in the app.

It is possible to write your own manager, this code does nothing to stop that. But it's structured
to provide a simple clean solution, and avoid having to do the lifting on your own.

## RELEASE

* ALPHA: Currently at alpha quality, and not ready for normal usage

## NOTES

* It does work on the Pi just fine, I'm using it that way now
* The API is still very early in development. It works for me, I don't know that it will work for anyone else ATM. I'll publish a release when I think it's ready.
* This is a LAN library, and only works when local to the bulbs. It cannot be used outside the home network the bulbs are on.
* This library does not support zones or tiles yet. That may come in the future, but for now, I don't have any to test against.
* The get/set API is incomplete still, and does not support a lot of the set functions provided by the LIFX API.
 * e.g. reboot/set label/set group.
* There is no push notifications here, the lights won't tell us if someone else changes them.
 * One solution is to query the lights every now and then to ask what's going on.



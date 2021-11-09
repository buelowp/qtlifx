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

## HOWTO

See the example main.cpp in the examples directory, but it's pretty simple to use.

```
LifxManager *manager = new LifxManager();
connect(manager, &LifxManager::newBulbAvailable, this, &Class::handleNewBulb);
connect(manager, &LifxManager::bulbDiscoveryFinished, this, &Class::handleEndOfDiscovery);
manager->initialize();
```

Once you have received the bulbDiscoveryFinished(bulb) signal, that bulb has completed discovery
and can be queried or set as needed. If you choose to provide the product definitions, then
it includes all of the LIFX capabilities. Otherwise, some functions may not work as the lights
are not assumed to support them.

```
QFile file("products.json");
if (!file.open())
    handle_error()
QByteArray json = file.readAll();
QJsonDocument doc = QJsonDocument::fromJson(json);
if (doc.isObject())
    manager->setProductCapabilities(doc);
```

You can get the products.json file from https://github.com/LIFX/products
The LAN API is documented at https://lan.developer.lifx.com/docs/introduction

## Some early Notes

* The LIFX API is little endian (LE) or network byte order. This library currently assumes a LE CPU. 
* It likely will not work on a Pi right now because of the above assumption. There is a plan to fix, but not right now
* The API is still very early in development. It works for me, I don't know that it will work for anyone else ATM. I'll publish a release when I think it's ready.
* This is a LAN library, and only works when local to the bulbs. It cannot be used outside the home network the bulbs are on.



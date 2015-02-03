# catleya-arduino

This project explores the possibility of using Arduino Uno units as a base station and as field units that receive/transmit instructions from the base unit. This will enable a RaspberryPi base station to send instructions wirelessly through 315MHz wireless RF modules via the Arduino base station.

## The Big Picture

This code is part of a larger Home Automation project (Catleya). The entirety of the project is as follows:

1. A RaspberryPi will be designated as the primary control unit.
    + A [MobileAngularUI](http://mobileangularui.com/ "MobileAngularUI Official Website")-based website will provide all the
      user interface that is exposed to the outside world.
    + [Node-RED](http://nodered.org/ "Node-RED Official Website") will be used as the logic controller, providing
      "if-this-then-that" logic for the entire system. The configuration will be written so as to expose an API-like
      interface, thus acting as a "backend" server.
    + An [nginx](http://nginx.org/ "nginx Official Website") will be set up to provide reverse-proxy capabilites and tie up
      the Angular-based UI with the Node-RED "backend".
2. An Arduino base station will be "coupled" to the RaspberryPi control unit and acts as a wireless communication gateway
   between the primary control unit and all field units.
3. Many Arduino field units will either control physical devices or acquire field information remotely via wireless
   communication.

## Objectives

1. Enable wireless communications between an Arduino base station attached to a wired local area network and Arduino field
   units dispersed in a reasonably wide area:
    + Enable an Arduino base station to communicate wirelessly to Arduino field units to remotely control physical devices
      attached to the field unit (relays, etc.)
    + Enable Arduino field units to communicate wirelessly to an Arduino base station to remotely report sensor states, etc.
2. Use MQTT at the base station to receive instructions from and send messages back to an MQTT broker.
    + The base station will subscribe to a preset MQTT topic, which is expected to be published by an MQTT broker.
    + The base station will publish to preset MQTT topics, which is expected to be intercepted by an MQTT broker.
3. Define an arbitrary communications standard (i.e. a standard message format) to be used throughout the entire Catleya
   project.
4. Achieve all previous objectives with the lowest possible cost.

## Project Status

The project is currently in prototyping stage, both hardware and software.

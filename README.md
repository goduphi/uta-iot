# UTA IoT
This project is an implementation of an IoT which uses a custom low-overhead protocol and a contention free communication scheme to transmit and receive messages between low-powered devices.

### Goals of the project
- Low-overhead protocol
- Contention-free communication scheme
- Must run on an ARM based microcontroller
- Ability to add devices by the push of a specialized join button

### Implementation
- Protocol - The protocol was designed to have a header of only 7 Bytes long leaving a total of 25 Bytes left for data.
- Communication Scheme - The communication scheme was based on a timer where slots were allotted to each device in a round-robin fashion. Data transfer would only take place during their designated slots.

The entire project, beginning from the design of the network, to integrating the network to work with a bridge, to establishing connection with the World Wide Web using MQTT was achieved within 16 days.

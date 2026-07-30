# Uno335

An Arduino Uno R3 derivative with **on-board level shifters**, so you can talk to
3.3 V parts without an adapter board. Plus a serial SRAM on SPI — see the note
below on that one.

## What makes it different from a plain Uno R3

* **I²C level shifter** — BSS138 MOSFET circuit on the two dedicated I²C pins next
  to the reset button. Use 5 V on A4/A5 and 3.3 V on SCL/SDA at the same time.
* **SPI level shifter** — TI **TXB0104**, bidirectional, on pins 10–13.
* **512 kbit serial SRAM** on SPI behind the TXB0104. Chip select sits on **A3**
  rather than D10, so D10 stays free for whatever else you need. A3 is an ordinary
  5 V pin and does not run through the TXB0104 — whose four channels are already
  taken by SCK, MOSI, MISO and SS on D10–13 — so its level is brought down to
  3.3 V by a simple resistive divider. A jumper disconnects that chip select
  again when you want A3 back as a normal pin.
* **Own USB identity** — VendorID `0x2C72`, ProductID `0x0335`. The ATmega16U2
  runs a slightly modified bootloader/DFU; the CDC SubClass was changed to `0x02`
  so Windows 10 loads `USBser.sys` without a custom driver. The ATmega328P
  bootloader is identical to the Uno R3, and the Arduino IDE treats the board as
  a plain Uno R3.

Every one of these features can be switched off by jumpers:

| Jumper configuration | What you get |
|:---|:---|
| **Pin setting 1** | 100 % Arduino R3 compatible — shifters and SRAM disabled |
| **Pin setting 2** | SPI and I²C level shifting plus serial SRAM enabled |

To use the SRAM you need the TXB0104 enabled (the SRAM runs its SPI at 3.3 V)
*and* the A3 chip select connected.

## Heads-up: the SRAM does not work on the boards we shipped

Fair warning before anyone spends an evening debugging: **on our production
batch the serial SRAM is a dud.** We drew the resistive divider for the A3 chip
select the wrong way round, so instead of dividing 5 V down to about 3.3 V it
produces roughly 1.7 V. That lands squarely in the forbidden zone between the
memory's input thresholds — not a clean high, not a clean low — so the chip is
never reliably deselected and does not answer. That is our design mistake, plain
and simple, and it made it all the way through to the finished boards.

While we are at it: the silkscreen on that batch is incomplete — some markings
simply did not make it onto the board. The labels that *are* printed are correct,
so you can trust what you see; there is just less of it than there should be.

We are not reworking several hundred boards over this, so treat that batch as
what it is — a faulty run. **The level shifters work**, though. They sit on a
separate path, they are unaffected by any of the above, and they are the reason
to reach for this board in the first place.

## The library is fine

`BB_SramStack/` and the sketches in `examples/` were written against working
hardware and the code is sound. They are worth keeping as a complete, documented
example of driving a Microchip 23xx-series serial SRAM from an ATmega328:

* SPI mode 0, MSB first, clock f/4
* chip select on **A3**
* commands `0x02` write, `0x03` read, `0x01` write status, `0x05` read status
* 16-bit addressing, 64 KByte (512 kbit), byte mode
* LIFO stack API on top: `push` / `pop` / `peek`, byte or word cells, plus an iterator

If you are wiring a 23LC512 to an Arduino yourself, this should port with little
effort — just point the chip select at whichever pin you used.

**Note:** `BB_SramStack::begin()` forces D10 (SS) to OUTPUT HIGH. That is required
so the AVR SPI hardware does not fall back into slave mode, but it means D10
cannot be used as a general-purpose input while the library is active.

## Contents

| Path | Description |
|:---|:---|
| `BB_SramStack/` | LIFO stack access to the serial SRAM (byte and word mode, iterator) |
| `examples/BB_sramBasicTutorial/` | Raw SPI without the library: computes and stores 2048 digits of *e* |
| `examples/BB_sramStackTutorial_example1/` | Stack basics: push, pop, peek |
| `examples/BB_sramStackTutorial_example2/` | Stack with iterator |
| `examples/BB_ledWithSramStack/` | LED matrix driven from data held in the SRAM |

## History

* **2016-10-11** — Engelbert Mittermeier: example sketch for the LED matrix tutorial
* **2016-09-01** — Engelbert Mittermeier: tutorial code
* **2016-08-10** — Engelbert Mittermeier: `BB_SramStack` library
* **2015-09-23** — initial commit

## License

See [LICENSE](LICENSE).

---

More electronics write-ups (in German) at **[electronobotics.de](https://electronobotics.de)**.

# Uno335

Arduino-compatible board with an on-board serial SRAM (SPI), plus the
`BB_SramStack` library and example sketches that use it.

---

## ⚠️ Important: the serial SRAM does not work on production boards

**On the boards that were actually produced, the on-board serial SRAM is not
usable.** The supply voltage for the SRAM is derived through a resistive
divider, and on the production run the two resistors were fitted the wrong way
round. The resulting supply sits below the minimum operating voltage of the
memory chip, so it does not respond reliably — or at all.

Two things made this slip through:

* A resistive divider is not a proper supply rail to begin with. It sags as
  soon as the SRAM draws current during SPI transfers, so even correctly
  populated it would be marginal.
* The board passed its factory functional test because that test was run at an
  elevated supply voltage, which lifted the divider output far enough for the
  SRAM to answer.

The boards are **not** being reworked. Please treat the SRAM as absent and use
the Uno335 as a plain Arduino-compatible board. Everything else on it is fine.

## What this repository is still good for

The library and the examples are **not** the problem — they were developed
against working hardware and the code is sound. They remain here because they
are a complete, documented example of driving a Microchip 23xx-series serial
SRAM from an ATmega328 over SPI:

* SPI mode 0, MSB first, clock f/4
* chip select on **A3** (not the usual D10)
* commands `0x02` write, `0x03` read, `0x01` write status, `0x05` read status
* 16-bit addressing, 64 KByte (512 kbit) capacity, byte mode

If you are wiring a 23LC512 or similar to an Arduino yourself, the code in
`BB_SramStack/` and the sketches in `examples/` should port with little effort —
just point the chip select at whichever pin you used.

**Note:** `BB_SramStack::begin()` forces D10 (SS) to OUTPUT HIGH. This is
required so the AVR SPI hardware does not fall back into slave mode, but it
means D10 can no longer be used as a general-purpose input while the library is
active.

## Contents

| Path | Description |
|:---|:---|
| `BB_SramStack/` | Library providing LIFO stack access to the serial SRAM (byte and word mode, iterator) |
| `examples/BB_sramBasicTutorial/` | Raw SPI access without the library: computes and stores 2048 digits of *e* |
| `examples/BB_sramStackTutorial_example1/` | Stack basics: push, pop, peek |
| `examples/BB_sramStackTutorial_example2/` | Stack with iterator |
| `examples/BB_ledWithSramStack/` | LED matrix driven from data held in the SRAM |

## History

* **2016-10-11** — Engelbert Mittermeier: example sketch for the LED matrix tutorial
* **2016-09-01** — Engelbert Mittermeier: tutorial code
* **2016-08-10** — Engelbert Mittermeier: `BB_SramStack` library for SRAM usage
* **2015-09-23** — initial commit

## License

See [LICENSE](LICENSE).

---

More electronics write-ups (in German) at **[electronobotics.de](https://electronobotics.de)**.

---
title: "HackRF Tripups with GNURadio"
date: 2019-09-07T16:48:29-05:00
tags:
- programming
- hackrf
- hardware
- radio
description: My HackRF wasn't working with my GNURadio sketches because I set the sample rate wrong. Here's how to do it right.
---

The HackRF One is an amazing tool, but the documentation about how to use it with GNURadio
is... scattered, to say the least. I got mine working today after a few hours of effort,
and I wanted to share what I did so others could benefit, because it's a little arcane.

Specifically, I was using the wrong **output sample rate**.

Symptoms of having the wrong output sample rate (too low) are scratchy audio and/or only
transmitting in "chunks" (because the device is waiting to get a full buffer before
transmitting).

{{< figure "right" >}}
![A screenshot of the Osmocom sink block's options](/images/radio/osmo_sdr_block.png)
The options for the Osmocom sink, used with the HackRF.
{{< /figure >}}

In GNURadio, communicating with the HackRF One is done via the Osmocom sink block.
In this block, there are two important fields for connection to the radio, in addition
to gain, frequency, et cetera.

First, **sample rate** must be between 2 and 20 Msps (million samples per second), so the
"Sample Rate (sps)" field must be between 2000000 and 20000000. However, it's not a great
plan to do *all* your processing at 2 and 20 Msps, since that will massively load down
your CPU.

My solution is to use three sample rates - audio/input rate (in the case of my simple FM
transmitter, 16k), "internal" rate (in my case 128k), to which the default "samp_rate"
variable should be set, and finally transmitter rate "tx_rate", 2M or more.

{{< figure "left" >}}
![A screenshot of the Rational Resampler block's options](/images/radio/rational_resamp.png)
A Rational Resampler block converting from the internal sampling rate to the transmission
rate.
{{< /figure >}}

I store all of these in variables: `tx_rate`, `samp_rate` (where most things operate),
and `input_rate`. For many flowgraphs, the modulator block will convert from `input_rate`
to `samp_rate` and only just before the sink is the rate converted to `tx_rate` with a
Rational Resampler block, with the incoming rate in decimation and the outgoing rate
in interpolation.

Second, you _can_ use the HackRF One without setting any device options in the "Device
Arguments" section, but if you're using more than one HackRF One, you can use the
`hackrf_info` tool to get the serial number and then set `hackrf=<serial>` in that field.


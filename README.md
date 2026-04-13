# ROKER PACK 🎸

**ROKER PACK** — a set of guitar VST3 plugins for both Linux and Windows.

## GUITAR EFFECTS/TOOLS

Guitar pedal effects from Bedroom Studio ARE NOT emulation of famous Boss DS-1, Boss Overdrive or Ibanez Tube Screamer. My effort is to create its own, specific sound, although it can be designed after hardware analogs. As VST3 plugins, you can use them in your preferred DAW: Ardour, Reaper, Muse, Cubase and others.

### Bronza

The plain fuzz pedal with two parameters — **Level** and **Fuzz**. Sounds like in the sixties.

### Grelka Overdrive

The classic overdrive. Has **Drive**, **Level**, **Lows** and **Treble** parameters to define the sound.

### Metalluga

The hard and crisp distortion with five controls to customize the effect for your needs: **Drive**, **Level**, **Weight**, **Resonance** and **Warmth**. The main control here is **Level**, all others build around it. If you don't touch too much the distortion stuff, you can use Metalluga in softer genres such as blues.

### Mistral

Old-fashioned flanger with warm sound.

### Charm

Simple, one-parameter saturator to add some warmth. BTW, it is built into Metalluga at the end of the chain.

### GuitarTuner

Tuner for six-string guitar with note display, frequency readout, and fretboard visualization.

### BassTuner

Tuner for bass guitar with note display, frequency readout, and fretboard visualization.

## Requirements

- **Host:** Any DAW that supports VST3 (Ardour, REAPER, Mixbus, Qtractor, Muse, Carla, Cakewalk, FL Studio, Studio One, etc.)
- **Operating Systems:**
  - **Windows 7** and newer (64-bit)
  - **Linux** (64-bit)

## Installation

### Windows

1. Download the latest installer (`RokerPack_Setup.exe`) from [Releases](https://github.com/psemiletov/rokerpack/releases)
2. Run the installer and follow the instructions
3. Plugins will be installed to `C:\Program Files\Common Files\VST3\`

### Linux

#### Binaries

The fastest way is to download pre-built binaries from the [Releases](https://github.com/psemiletov/rokerpack/releases) section.

#### From source

```bash
git clone https://github.com/psemiletov/rokerpack.git
cd rokerpack
mkdir build && cd build
cmake ..
make
sudo make install

## Support the Project

If you find ROKER PACK useful, you can support further development:

**PayPal:** peter.semiletov@gmail.com

**Bitcoin:** bc1qpdjzcedha54rpaa3nl5q0xh3hmz2sfcx49z99w

Your support is greatly appreciated! 🙌
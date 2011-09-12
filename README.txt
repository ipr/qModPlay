
A (tracker) module player with Qt&C++

Author: Ilkka Prusi
Contact: ilkka.prusi@gmail.com

Purpose/goals:

1. Somewhat portable implementation for playing tracker modules:
some are only playable with emulated m68k code now, 
some players/plugins are even expecting Amiga sound-chip (Paula) emulation as well..

2. Simple to maintain/extend:
sources available, (somewhat) clean&simple code/structure etc.

3. Free and open:
keep open so these modules can be played in future when we no longer have PCs..

Note: large parts _will_ be used from other GPL-licensed apps like xmp and uade
but rewritten/refractored for my tastes anyway.. Maybe fix a bug or two while at it..

This will never be more than a player and my personal interest is in tracker-formats used on Amiga
so progress may be slower than expected on some formats.

Licensing:
GPL? (v2/v3?) (or BSD?/MIT? undecided..)

Point is, non-profit use only: this is supposed to stay open so that modules can be played in future also
when there's no PC anymore and everything is done on handheld-devices (or other such scenario).

Status:

Some fileformats mostly read, others not so much.
Decode/playback missing from all currently (working on it).

List below:
* DBM0 (DigiBooster PRO) 
  - fileformat is missing sample-handling (working on it)
  - no playback
* DIGI (old-style DigiBooster) 
  - maybe 20-30% of fileformat? 
  - no playback
* SyMMod (Symphonie) 
  - fileformat maybe half-way there? 
  - no playback
* TFMX 
  - maybe 5% of fileformat done?
* AHX 
  - nothing yet (placeholder)
* OctaMED 
  - nothing yet (placeholder)
* ProTracker 
  - maybe 5% of fileformat done?
* Oktalyzer 
  - maybe 5% of fileformat done?
* NoiseTracker? 
  - nothing yet (placeholder)
* S3M (ScreamTracker 2/3) 
  - nothing yet (placeholder)
* IT (ImpulseTracker) 
  - nothing yet (placeholder)
* MT2 (Mad Tracker 2) 
  - maybe 5% of fileformat done?
* MTM (MultiTracker)
  - nothing yet (placeholder)
* XM (FastTracker 2)
  - nothing yet (placeholder)
* ... and so on ..

Seems sad? Try locating documentation for some of these..
And come on, I've only used about a week on it so far..

Timetable? Forever..


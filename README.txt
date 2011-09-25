
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

Current status:
* Some fileformats mostly read, others not so much
 - some sample-formats are not yet handled
 - sample-format handling is biggest step to do before starting work on playback of modules..
* Decode/playback of modules missing from all currently (working on it)
* Application-side "framework" for handling playback mostly done
 - may need some work but should make noise when data is given by player-object
* packed/crunched file support started using external libraries for most common cases
 - LhA and LZX archive libraries should be fully functional (GUI-side missing)
 - XPK/XFD and other "cruncher" formats in progress..
* GUI/playlist is still awful.. need to work on that later..

Status by module formats below:
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
  - fileformat close to finishing even..?
  - no playback
* OctaMED 
  - nothing yet (placeholder)
* ProTracker 
  - maybe 5-10% of fileformat done?
* Oktalyzer 
  - maybe 20% of fileformat done?
* NoiseTracker? 
  - nothing yet (placeholder)
* S3M (ScreamTracker 2/3) 
  - maybe 10-20% of fileformat, without samples/instruments
  - no playback
* IT (ImpulseTracker) 
  - maybe 10-20% of fileformat, without samples/instruments
  - no playback
* MT2 (Mad Tracker 2) 
  - maybe 5% of fileformat done?
* MTM (MultiTracker)
  - nothing yet (placeholder)
* XM (FastTracker 2)
  - maybe 20-35% of fileformat done?
  - working on sampleformat support (generic stuff for other formats also)
  - no playback yet
* ... and so on ..

Seems sad? Try locating documentation for some of these..
And come on, I've only used about a week on it so far..

Timetable? Forever..


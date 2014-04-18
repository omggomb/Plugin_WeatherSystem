WeatherSystem Plugin for CryEngine SDK
=====================================
ONLY WORKS with CRYENGINE Free SDK ver 3.4.5 Build 6666

Adds a weather system controllable via FlowGraph to the game.

*Please read Docs/Quickstart.odt*

Installation / Integration
==========================
Needs the plugin manager to be installed.

Clone into  <CERoot>\Code\ and build

CVars / Commands
================
* ```ws_weatherSystemActive```
  Toggles the weather system to be active or not (1 or 0)
  
* ```WSReloadSituations```
  Reloads all situation definitions

* ```WSForceSituation <name>```
 Applies weather situation named <name> if it is found

* ```WSForceSituationChange```
 Forces a situation change now

 * ```WSFadeOutAndStop [seconds]```
  The current situation gets faded out over [seconds = 1] seconds and the weather system is turned off

* ```WSListSituations```
 Lists all loaded situations

Flownodes
=========
See Docs/FlowNodes.odt
# Privacy & Crash Reporting

CoopAndreas may automatically send anonymous crash reports when an error occurs to help us fix bugs and improve stability.

## What Is Sent
Technical debugging data only, including:
* Game and CoopAndreas versions
* Error/exception codes
* Memory state (CPU registers, stack dump, backtrace)
* Loaded modules and active scripts

## What Is NOT Sent
We do **not** collect personal data such as:
* Passwords or usernames
* Personal documents or browser history
* Save files

### Path Anonymization
To protect your privacy, local folder paths are masked before sending:
* `C:\Users\<username>` → `%USER%`
* `C:\Windows` → `%WINDIR%`
* Game installation folder → `%GAME%`

Crash report data is used strictly for debugging purposes.

## How to Disable Crash Reporting
You can turn off crash reports at any time by editing your configuration file:

1. Open `%USER%\Documents\GTA San Andreas User Files\coopandreas.ini`
2. Set `report-crashlogs` to `false`
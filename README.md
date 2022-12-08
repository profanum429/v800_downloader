v800_downloader
===============

Update: I haven't followed this in roughly 4 years; I kinda forgot it existed, sorry guys! I haven't owned a V800 for probably 3 years now so I can't do any testing or changes for stuff. Probably already have but consider this pretty much abandoned / no more work on it (not changing much from what it's been at though, lol). Feel free to fork and modify to heart's content.

Update: Release 5 fixes a bug where some garbage data would result in thinking files and directories existed where they did not and had obviously faulty names. This has been fixed. I only saw this after I did a factory reset, so it might not occur in normal usage but it shouldn't be an issue now.

Update: Release 4 now properly supports multisport sessions. Exported files will have a _(NUMBER) appended to them before the extension. This number is the order of the sport in the multisport session. Single sport sessions now have a _0 appended before the extension to represent that this is sport 1 of the session.

Update: Release 3 still works with the new 1.0.12 firmware but right after the update you won't see any sessions since when the firmware updates all the sessions on the watch have their full set of data removed and leave only the statistics information behind. Any new activities will be downloadable :)

V800 Downloader is a tool that is used to download sessions from the Polar V800 watch. In normal usage there is no first party way
to export data from the V800, but with V800 Downloader + Bipolar (https://www.github.com/pcolby/bipolar) data can be exported from
the V800 without using any Polar software.

If you use Polar FlowSync to sync with the watch already I'd suggest using <a href="https://www.github.com/pcolby/bipolar">Bipolar</a>
by itself as it will install a hook that will collect session data as you use FlowSync. It will be a lot simpler and not as complicated
as using V800 Downloader.

If, however, you sync with your phone, such as with the iPhone Polar Flow app, then this tool is for you. It's
a combination of a front end that lets you select which sessions to export and formats and two backends that handle the work. The first
backend is the USB backend that handles all communication to the V800 itself and is responsible for getting session data. The second
backend is from <a href="https://www.github.com/pcolby/bipolar">Bipolar</a> and is responsible for doing the actual hardwork; the converting
the session data into good formats like TCX, GPX and HRM.

### Steps to use:
1) Connect your V800 to the computer with the USB cable<br>
2) Run V800 Downloader; it should display a message saying that it is retrieving session data<br>
3) Select which sessions you desire to download from the list. You can grab as many or little as you like. <br>
4) Check what output formats you'd like to get<br>
5) Click Download Sessions. Progress will be displayed and when finished a message telling you where the files are located will be displayed. <br>
   Additionally any error messages will appear here also.<br>
6) Upload your files :)<br>

### Building
To build on Linux, first install Qt 5 according to your disto, then:
```sh
cd path/to/v800_downloader
qmake
make
```

On Windows, install Qt 5, then:
```sh
cd path\to\v800_downloader
qmake
nmake
```

### Credits
I couldn't of done this program without the <a href="https://www.github.com/pcolby/bipolar">Bipolar</a> project as it does the hard work of
converting the data into actual files. All of my modifications are available at https://www.github.com/profanum429/v800_downloader, under the
src/bipolar directory.<br>
LibUSB from libusb.info. Version 1.0.19 is incorporated into V800 Downloader. I've made no source modifications. <br>
rawhid from https://www.pjrc.com/teensy/rawhid.html. I use the V1.0 of the OS X version for USB communication in OS X.<br>

### License
All V800 Downloader files are licensed GPL3. The Bipolar files under src/bipolar are licensed GPL3 per the original source.

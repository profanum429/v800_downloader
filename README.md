v800_downloader
===============

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

Steps to use:
1) Connect your V800 to the computer with the USB cable
2) Run V800 Downloader; it should display a message saying that it is retrieving session data
3) Select which sessions you desire to download from the list. You can grab as many or little as you like.
4) Check what output formats you'd like to get
5) Click Download Sessions. Progress will be displayed and when finished a message telling you where the files are located will be displayed.
   Additionally any error messages will appear here also.
6) Upload your files :)

Credits:
I couldn't of done this program without the <a href="https://www.github.com/pcolby/bipolar">Bipolar</a> project as it does the hard work of
converting the data into actual files. All of my modifications are available at https://www.github.com/profanum429/v800_downloader, under the
src/bipolar directory.

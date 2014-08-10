v800_downloader
===============

Tool to download data files from the Polar V800 GPS watch.

The tool looks through the filesystem on the V800 and determines which sessions still have valid data. FlowSync will only sync a session once but it doesn't delete the session; it just creates an ID.BPB file that is checked whenever FlowSync runs. The actual session data (route, samples, laps, etc.) are still on the watch until they are automatically deleted by some process, probably a housekeeping thing that deletes the older data as the watch gets full from newer data. Older sessions still retain the summary and statistics but lose the route and all the detailed data.

This tool lets you download the older session that still exist (in my case my watch has about a month worth of data on it when running ~4-5/hours a week). The data is output to the Bipolar directory (https://www.github.com/pcolby/bipolar) to be converted by the Bipolar program.

There is also a few options, the first is to output raw data files. This option will bring up a directory selection box that allows you to save the raw data files wherever you'd like. The raw data files are downloaded and also the correct files are placed in the Bipolar directory. These raw data files retain the same name as on the watch and are everything for a session. These will probably only be of interest to other developers who are looking to play around with the files.

The second and third option are straightforward; buttons to either select all sessions or deselect all sessions.

The final option is a button that will bring up the V800 Filesystem explorer. This allows you to dig into the filesystem on the V800 and see every file and directory. It also gives the option to download files from anywhere on the watch. This again will probably only be of interest to other developers.

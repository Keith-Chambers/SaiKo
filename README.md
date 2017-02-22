## Saiko ##

Saiko is a music player and organizational tool for managing multiple libraries of music. It is planned to run on both Linux and Windows, however at the moment it has only been tested on Windows.

Saiko is currently in *very* early development but if you wish to test out the program(On Windows) it is quite easy to do so. Just follow the steps outlined below..

1. Download the Saiko_0.3.zip file inside the releases folder.
2. Extract the zip file in your desired location
3. Open configLib.txt from inside the resulting folder and modify the path to the library you wish to include for testing. There are a couple of things to note at this step. Firstly this is just a silly hack for testing purposes. Secondly this method only supports a single path name. Finally you should include a path that contains only folders. Any non folder files at the root library directory will be ignored. Also, for the moment please end your folder path with a '/'.
E.G `C:/path/to/music/folder/`
4. Once you've added your library path in you can run the executable, saiko.exe and test away.

On startup the program will traverse your music library and create a .saik file in each directory. It will also scan around to see if there are any .jpg files. If there are it will add them to the .saik file using the following format
`#ART:C:/currentdir/imageName.jpg`. The program will then use this information to show you album art inside the program. Saiko will use the first valid image path in the file. If you want to change the default album art image of a folder just open up the .saik file with a normal text editor and change the order of the enties(Or delete entries with higher priority). Each entry **must** be on a seperate line.

When the program ends it will delete all the .saik files again. For this reason the program will be rather slow on startup and exit. It is particularly slow on startup, however later releases won't delete these files on every run of the program. This is just to make testing easier for those other than myself.

This program should play all .mp3 and .flac files, furthermore it will filter out any files that are not of those types from being viewed inside the program.

The program is lacking many crucial features at this point in time but I hope to have all that resolved relatively soon.
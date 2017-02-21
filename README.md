## Saiko ##

Saiko is a music player and organizational tool for managing multiple libraries of music. 

Saiko is currently in *very* early development but if you wish to test out the program it is quite easy to do so. Just follow the steps outlined below..

1. Download the Saiko_0.2.zip file inside the releases folder.
2. Extract the zip file in your desired location
3. Open configLib.txt from inside the resulting folder and modify the path to the library you wish to include for testing. There are a couple of things to note at this step. Firstly this is just a silly hack for testing purposes. Secondly this method only supports a single path name. Finally you should include a path that contains only folders. Any non folder files at the root library directory will be ignored.
4. Once you've added your library path in you can run the executable, saiko.exe and test away.

If you have album art that you wish to be displayed within the program instead of the default folder box simply copy the image(As a backup of the original name, optional) and rename it as follows `parentname_art.jpg`. E.g If you have a folder called music and you have an image *inside* that folder that you wish to be rendered, rename the file music_art.jpg and re-run the program. At the moment only .jpg files are supported for this feature.

This program should play all .mp3 and .flac files, furthermore it will filter out any files that are not of those types.

There are a number of bugs atm that I'm not going to even discuss since they should all be resolved within the next few days. Don't worry though, none of them involve crashing your computer, test away ;)
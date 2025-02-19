### Thanks to FontForge for their open source software! Visit them at https://fontforge.org/ ###

https://www.youtube.com/watch?v=0YaARkF4A3E
# ^ A video showcasing how you can use the plugin.

https://forums.unrealengine.com/t/editorfont-discussion-thread/2312529?u=xtheos
# ^ This link goes to the forum post where you can ask questions or discuss with me about the plugin.

	You choose a new folderpath. If the path is valid and no folder of that name exists in the parent directory, 
a new folder of that name will be created.

Some fonts only accept .ttf, and some accept .otf. The dropdown menus ensure only the allowed type can be chosen.

	The font converter property has 3 fields. A path field, a mini-path-picker, and a Folder Conversion button. 
Inputs into the path field and the mini path picker accept a direct filepath including extension, e.g. (C:/Path/Here/font.ttf). If the filepath is valid,
the font will be converted and your logs should have no errors. A file of the other extension will be created in the same filepath, e.g. (C:/Path/Here/font.otf).
The FolderConversion button instead opens a file picker that chooses a directory. All .ttf and .otf will be converted into the other type. Any failed conversions
will be logged in the output.
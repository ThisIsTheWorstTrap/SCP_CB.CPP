### How to change a model from blitz3d to a .m3d model

# 1 : intall blender version 5

- On Windows : go to the official blender web site, and dowload the latest version
- On Linux   : use ```sudo pacman -S blender```

# 2 : download the required addons

- The SCP_TOOLSET addon : go to https://github.com/General-101/SCP-CB-Toolset and download the latest release
- The m3d blender exporter : go to https://gitlab.com/bztsrc/model3d/tree/master/blender and download ```io_scene_m3d.py```

# 3 : install the addons

Go to Edit -> Preferences -> Add-ons -> Install from disk and then select your extensions

# 4 : export the model

First, you need to import the .b3d model : go to File -> Import.
Then, you need to check if textures are found : go to Shading, and if textures are set, you're good to go
Finally, go to File -> Export -> Model 3d (.m3d) and enable option ```Embed Assets```, it can be found on the right of the menu
Then, export to the place you want (preferably into models)
### How to change a model from blitz3d to a .m3d model

# 1 : intall blender version 5

- On Windows : go to the official blender web site, and dowload the latest version
- On Linux   : use ```sudo pacman -S blender```

# 2 : download the required addons

- The SCP_TOOLSET addon : go to https://github.com/General-101/SCP-CB-Toolset and download the latest release
- The m3d blender exporter : go to https://gitlab.com/bztsrc/model3d/tree/master/blender and download ```io_scene_m3d.py```

# 3 : install the addons

Go to Edit -> Preferences -> Add-ons -> Install from disk and then select your extensions

# 4 : transform the model

First, you need to import the .b3d model : go to File -> Import.
Then, you need to check if textures are found : go to Shading, and click on the model : some box should appear on the bottom
Then, find the texture in these box : if the texture is a png, don't change it. Else, use GIMP to change the format to png
Then, go to UV Editing : if the UV are correctly mapped on the texture, then you're good
Else, you need to go to Scripting, then select the model and click new scrpit and write this python script : 
```import bpy

obj = bpy.context.object
mesh = obj.data
attr = mesh.attributes['uvmap_render'] # Here, you might change the name of uvmap_render, based on what you see in UV Editing -> UV Maps

for d in attr.data:
    d.vector[1] = d.vector[1] % 1.0

mesh.update()```



Finally, go to File -> Export -> Model 3d (.m3d) and enable option ```Embed Assets```, it can be found on the right of the menu
Then, export to the place you want (preferably into models)

If there are issues with the textures, try convert them into .png.
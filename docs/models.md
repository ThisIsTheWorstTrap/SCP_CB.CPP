### How to change a model from blitz3d to a .glb model ?

# 1st step : setup

- Download Blender v.4.2.23 from the official source.
- Then, install the blitz3d addon at : https://github.com/GreenXenith/io_scene_b3d/releases/tag/5bef187
- Upload the plug-in into Blender to activate it

# 2nd step : transform the model

- Go to the scpcb official repository, and download the wanted model and texture
- Change the texture to .png in GIMP
- Go to Blender, and import the .b3d model you downloaded
- Go to "Shading" and apply the texture you transformed ealier
- Then, export the whole scene to .glb

Warning! : Don't forget to delete the base cube model
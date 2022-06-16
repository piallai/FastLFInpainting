Fast Inpainting project. Inria License.

Needs CMake minimum 3.0 to be installed.
Needs the following libraries to be installed : OpenCV(+contribs), and boost (system, filesystem, thread).

I - Examples to generate the project :
Go to the directory where FastInpainting is.
a) Linux 
cd project
mkdir linux
cd linux
cmake ../..

b) Windows/Visual
cd project
mkdir visual
cd visual
cmake ../.. -G "Visual Studio 15 Win64"

II - Compilation :
a) Linux
cd project/linux
make

b) Windows/Visual
Open project/visual/FastInpaintingProject.sln
Press "Run"

III - Input :
The executable file uses a master config file. A default config file is located at DATA/cfg/cfg_master.txt
A master config file can be specified as argument : ./bin/Release/FastLFInpainting.exe DATA/cfg/cfg_master.txt
This config file calls other config files in charge of different parameters of the software, such as loading/saving light field functions or inpainting funtion.

Main input parameters :
- Path to light field is defined in DATA/cfg/cfg_loader.txt
- Path to inpainted subaperture and mask are defined in DATA/cfg/cfg_inpainting.txt
- Angular position of inpainted subaperture is also defined in DATA/cfg/cfg_inpainting.txt

IV - Output :
Output path is defined in cfg_master.txt

V - Example :
An example is given under the directory DATA/totoro_waterfall. The configuration files, the initial light field, the mask and the inpainted view are provided under this directory.

To inpaint the light field "totoro_waterfall", enter the following command from the directory where the executable FastLFInpainting.exe is:

./FastLFInpainting.exe ../../DATA/totoro_waterfall/cfg/cfg_master.txt



Tips :
- Subapertures of Lytro light fields are usually not of very good quality at angular edges (vignetting effect).
One can remove these subapertures by setting subaperture_offsets in DATA/cfg/cfg_loader.txt
Using offset of 2 images is usually enough to obtain a consistent light field.
- Also, one can use parameter l_histogram_matching in DATA/cfg/cfg_loader.txt to obtain a more consistent light field regarding luminosity and colorimetry.
- If loading of light field fails because of subapertures coordinates parsing issues, one can open DATA/cfg/cfg_loader.txt and set Nimages_auto to the corresponding number of images in the light field to help guessing of parsing.
- Parsing can also fail because of two images having same angular coordinates in their name. One can use filter_strings parameter in DATA/cfg/cfg_loader.txt to filter out images containing defined strings.

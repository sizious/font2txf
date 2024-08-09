# font2txf

I used Mark's program called 'gentexfont' to convert
X fonts into '.txf' format - which can be read into
the PLIB FNT component.

**font2txf** is an utility used for generating texture mapped font files,
also known as the `TXF` format.




Below you'll find an example of the *ADK/Napalm* MR image shown in the boot
screen (this **MR** logo was extracted from the `dcload` bootstrap):

![Example](img/mrlogo.png "Sample MR image")

## Building

This program is a standard C program which may be compiled with **GNU Make**.
It requires `libpng-dev` installed.
[Learn more about libpng here](http://www.libpng.org/pub/png/libpng.html).

1. Edit the `Makefile` and check if everything is OK for you (e.g. `libpng`
   directories);
2. Enter `make` (`gmake` on BSD systems).

## Usage

To use this tool, several modes are available:

1. Using a IP template file (`ip.txt`);
2. Using command-line arguments.

Available options are (displayed with the `-h` switch):
	
	-f                 Force overwrite output file if already exist
	-h                 Print usage information (you're looking at it)
	-l <infilename>    Load/insert an image into bootstrap (MR; PNG)
	-t <tmplfilename>  Use an external IP.TMPL file (override default)
	-u                 Print field usage information
	-s <outfilename>   Save image from <infilename> to MR format (see '-l')
	-v                 Enable verbose mode

Example:

	./ttf2txf -o revenant.txf -s 22 -w 256 -h 256 -c "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()-_=+[]{}|;:,.<>~ ?/" revenant.ttf

xxx

## Useful links

* [A Simple OpenGL-based API for Texture Mapped Text](http://sgifiles.irixnet.org/sgi/opengl/contrib/mjk/tips/TexFont/TexFont.html)
* [Building .txf font texture files for the Celestia project](https://en.wikibooks.org/wiki/Celestia/Internationalization#Building_.txf_font_texture_files)
* [Online Celestia Font Texture Generator](http://web.archive.org/web/20080413101857/http://celestia.teyssier.org/ttf2txf/index.html)
* [OpenGL GLUT Examples](https://www.opengl.org/archives/resources/code/samples/glut_examples/texfont/texfont.html)

## Acknowledgments

* [Jacob Alberty](https://github.com/jacobalberty) (LiENUS): Copyright-free
  bootstrap replacement (`IP.TMPL`). It is used by default in the **IP creator**
  program.
* [Marcus Comstedt](http://mc.pp.se/dc/sw.html)
  ([zeldin](https://github.com/zeldin)): Initial release of the 
  [IP creator](http://mc.pp.se/dc/sw.html) tool (`makeip`).
* [Andrew Kieschnick](http://napalm-x.thegypsy.com/andrewk/dc/) (ADK/Napalm):
  Code to insert `MR` images into `IP.BIN` and `PNG` to `MR` from
  [logotools](http://napalm-x.thegypsy.com/andrewk/dc/).
* [Hayden Kowalchuk](https://twitter.com/HaydenKowalchuk)
  ([mrneo240](https://github.com/mrneo240)): Fixes on the original `logotools`
  package.
* [Andress Antonio Barajas](https://twitter.com/bbhoodsta)
  ([BBHoodsta](https://gitlab.com/BBHoodsta)): Tons of fixes and improvements.
  Maker of the **GIMP MR Image Plug-in**.
* [SiZiOUS](http://sizious.com/): Complete refactoring of the project and
  provider of the snippet for decoding `MR` images (extracted from
  [Selfboot Inducer](https://sizious.com/download/dreamcast/#selfboot-inducer)).

## License

This project is licensed under the **BSD 2-clause "Simplified" License** - see
the [LICENSE](LICENSE) file for details.

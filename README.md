<h1 align="center">EFLETE (EFL Edge Theme Editor)</h1>

<p align="center">
  <img src="./data/images/eflete.svg" width="300" height="300" border="0"></>
</p>

<br/>
<p align="center">
  <img alt="GitHub repo size" src="https://img.shields.io/github/repo-size/dimmus/eflete?style=for-the-badge">
  <img alt="GitHub commit activity" src="https://img.shields.io/github/commit-activity/w/dimmus/eflete?style=for-the-badge">
  <img alt="Maintenance" src="https://img.shields.io/maintenance/yes/2022?style=for-the-badge">
</p>
<br/>

<p align="center">
  <i>Work in progress! Limited functionality.</i>
</p>

Efl Edje Theme Editor is a rich graphical tool that provides UI for edc markup. It is aimed at simplifying the creation and editing of widget styles. No one, for sure, wants to learn EDC syntax. Eflete was designed with the idea that its target users are not just programmers but also designers. Besides styles, Eflete is also able to create and edit custom layouts. This is a useful feature for developers who create application layouts.

```
For comfortable work with Eflete your monitor should have the HD (1366x768) resolution. If resolution is less not all UI componenets is displayed.
```

## Requirements

   * efl (>= 1.23.3 && < 1.26.99)

Please note that some features may not quite function correctly or
completely!

## Compiling

Once you have met the requirements, compiling and installing is simple:
```sh
./autogen.sh
make
sudo make install
```
If you want a really debuggable piece of code where optimizations mess with little to nothing at all, use:
```sh
./configure CFLAGS="-O -g -ffast-math -march=native -ggdb3"
```
Note that if you wish to compile for older architectures than your current system, please look up the compiler documentation and replace ''-march=native'' with something appropriate.

Use the --prefix option on the configure step to install the program to a specific directory:
```sh
./autogen.sh --prefix=/full/path/to/some/dir
```
You will want to ensure the default prefix `/usr/local` is available to build tools. If you know what you are doing, you can change the prefix, but this assumes you do not and the above prefix is used. These environment variables are used during the build, so you may want to make them more permanent.
```sh
export PATH=/usr/local/bin:"$PATH"
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:"$PKG_CONFIG_PATH"
export LD_LIBRARY_PATH=/usr/local/lib:"$LD_LIBRARY_PATH"
```
## Applying themes

To apply a theme to elementary, you should place the .edj-file in the elementary themes directory (default: `/usr/local/share/elementary/themes)`. Note: do not forget to add read permissions to the copied file. After that, the theme will be available for selection from `elementary_config`.

## Bash autocompletion

To enable bash autocompletion for eflete, copy `eflete_autocompletion.sh` file (you can find it in `scripts` folder) to your
bash completion directory or add the following line to your ~/.bashrc:
`source /YOUR/PATH/eflete_autocompletion.sh`

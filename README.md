# Bitmap-Image-Cutter

## Description
A program that is used to cut a Bitmap file into equal parts with command line. Each part is saved in a new Bitmap file.

## Usage
Command-line syntax:
<program> <file Bmp> [-h <parts in height>] [-w <parts in width>]

Example: program cutbmp.exe
  - Cut 3 parts in height (save in 3 new Bitmap files): cutbmp.exe d:/images/img1.bmp cutbmp.exe d:/images/img1.bmp -h 3
  - Cut 2 parts in height, 4 parts in width (save in 8 new Bitmap files): cutbmp.exe d:/images/img1.bmp -h 2 -w 4

## Note:
The program just takes 24-bit bitmap photo as input. With other types of input, the program doesn't run and prints error.

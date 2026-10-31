# This is a template pre-synthesis.tcl file which will trigger the generation
# of the APx-FS Info ROM data.
#
# You will need to create a 'vivado' folder in your project which contains a
# copy of or symlink to this file as appropriate.
# 
# For example, in your repository's root directory:
#   $ cd apd1
#   $ mkdir vivado
#   $ cd vivado
#   $ ln -s ../../submodules/apx-fs/scripts/pre_synthesis.tcl ./pre_synthesis.tcl

#puts ${::env(TOP_DIR)}
#puts ${::env(PROJ_DIR)}
exec /usr/bin/env -uPYTHON -uPYTHONPATH -uPYTHONHOME /usr/bin/python3 "${::env(TOP_DIR)}/submodules/apx-fs/scripts/apxfs_romgen.pyz" -m "${::env(PROJ_DIR)}/cfg/apx_fs_info.mem" -p "${::env(PROJ_DIR)}/cfg/PrjSpecPkg.vhd" -s "${::env(TOP_DIR)}/submodules/apx-fs" 2>&1

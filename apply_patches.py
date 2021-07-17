from os.path import join, isfile
import sys
import shutil

Import("env")

package_root = join(env["PROJECT_LIBDEPS_DIR"], env["BOARD"], "Adafruit LittlevGL Glue Library")
patchflag_path = join(package_root, ".patching-done")
original_file = join(package_root, "lv_conf.h")
modified_file = join("lv_conf.h")
shutil.copyfile(modified_file, original_file)


from os.path import join, isdir
import sys
import shutil

Import("env")

package_root = join(env["PROJECT_LIBDEPS_DIR"], env["PIOENV"], "Adafruit LittlevGL Glue Library")
if isdir(package_root):
    patchflag_path = join(package_root, ".patching-done")
    original_file = join(package_root, "lv_conf.h")
    modified_file = join("lv_conf.h")
    shutil.copyfile(modified_file, original_file)


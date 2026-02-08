import sys

for fn in sys.argv[1:]:
    fn = fn.replace('.', '_')
    print('    mx_xpm_write_bitmap(dir, "%s", %s);' % (fn, fn))


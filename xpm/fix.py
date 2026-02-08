import sys

for fn in sys.argv[1:]:
    f = open(fn)
    of = open('temp/' + fn, 'w')
    for l in f.read().split('\n'):
        if l.find('static') == -1:
            of.write(l + '\n')
        else:
            fn = fn.replace('.', '_')
            of.write('static const char *%s[] = {\n' % fn)
    f.close()
    of.close()

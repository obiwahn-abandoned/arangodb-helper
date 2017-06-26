#!/usr/bin/python3

import sys

one_mega_byte = 1024 * 1024
def mb(x):
    return (x * one_mega_byte, str(x) + "mb")

def gb(x):
    return (mb(x)[0] * 1024, str(x) + "gb")

def default_generator():
    i = 0;
    yield "_key, blarg\n"
    while True:
        yield "Key{:010d},Val{}\n".format(i, 'abcd'*100)
        i += 1

def create(basename, size_in_gb, gen):
    size_limit, suffix = gb(size_in_gb)
    print("requested file size: " + str(size_limit))
    fn = "{}-{}.csv".format(basename,suffix)
    print("writing to " + fn)
    with open(fn,"w") as fh:
        fh.write(next(gen))
        i = 0; size = 1
        percent=0;
        while size <= size_limit:
            i+=1
            new = int((size/size_limit)*100)
            if new > percent:
                sys.stdout.write(str(percent)+"%\n")
                sys.stdout.flush()
                percent = new
            line=next(gen)
            size += len(line)
            fh.write(line)
    print()
    return 0

if __name__ == "__main__":
    sys.exit(create("outfile",30,default_generator()))

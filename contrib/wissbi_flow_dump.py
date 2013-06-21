#!/usr/bin/env python

import os
import pwd
import grp

base_dir = "/var/lib/wissbi/sub"

clusters = {}
links = []

def add_to_cluster(path):
    stat = os.stat(path)
    owner = pwd.getpwuid(stat.st_uid).pw_name
    group = grp.getgrgid(stat.st_gid).gr_name

    fullname = os.path.relpath(path, base_dir)
    (head, tail) = os.path.split(fullname)
    if len(head) == 0:
        head = '.'
    tup = (tail, owner, group)
    if head in clusters:
        clusters[head].append(tup)
    else:
        clusters[head] = [tup]

def visit(arg, dirname, names):
    is_final = (len(names) == 0)

    for child in names:
        fullpath = os.path.join(dirname, child)
        src = os.path.relpath(fullpath, base_dir)
        if os.path.islink(fullpath):
            add_to_cluster(fullpath)
            dest_tuple = os.path.split(os.path.relpath(os.path.realpath(fullpath), base_dir))
            src_tuple = os.path.split(src)
            links.append((src_tuple, dest_tuple))
        elif os.path.isfile(fullpath):
            is_final = True
    if is_final:
        add_to_cluster(dirname)

os.path.walk(base_dir, visit, "")

print "#!/bin/sh"
for cluster_name in clusters:
    cluster = clusters[cluster_name]

    for d in cluster:
        print "mkdir -p %s/%s" % (cluster_name, d[0])
        if len(d[1]) != 0:
            print "chown %s:%s %s/%s" % (d[1], d[2], cluster_name, d[0])

for link in links:
    print "rmdir %s/%s" % (link[0][0], link[0][1])
    print "ln -f -s `pwd`/%s/%s `pwd`/%s/%s" % (link[1][0], link[1][1], link[0][0], link[0][1])


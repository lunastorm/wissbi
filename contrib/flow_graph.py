#!/usr/bin/env python

import os

base_dir = "/var/lib/wissbi/sub"

clusters = {}
links = []

def add_to_cluster(path):
    fullname = os.path.relpath(path, base_dir)
    (head, tail) = os.path.split(fullname)
    if head in clusters:
        clusters[head].append(tail)
    else:
        clusters[head] = [tail]

def visit(arg, dirname, names):
    is_final = (len(names) == 0)

    for child in names:
        fullpath = os.path.join(dirname, child)
        src = os.path.relpath(fullpath, base_dir)
        if os.path.islink(fullpath):
            add_to_cluster(fullpath)
            dest_tuple = os.path.split(os.path.relpath(os.path.realpath(fullpath), base_dir))
            src_tuple = os.path.split(src)

            links.append(('"%s":"%s"' % src_tuple, '"%s":"%s"' % dest_tuple))
        elif os.path.isfile(fullpath):
            is_final = True
    if is_final:
        add_to_cluster(dirname)

os.path.walk(base_dir, visit, "")

print "digraph G {"

for cluster_name in clusters:
    cluster = clusters[cluster_name]
    cluster = ["<%s> %s" % (x, x) for x in cluster]

    print '"%s" [shape=record, label="{%s | {%s}}"];' % (cluster_name, cluster_name, "|".join(cluster))

for link in links:
    print '%s -> %s [arrowhead = none];' % link

print "}"

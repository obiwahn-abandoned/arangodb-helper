#!/bin/bash

outfile="${1##.}.csv"
echo "writing to $outfile"

echo "percent,seconds,rss(KiB),rss(MB)" > "$outfile"
while read _ per _ secs _ _ _ rss _  ; do
    echo "$per,$secs,$rss,$((rss / 1024))"
done < <(grep ^percent "$1") >> "$outfile"

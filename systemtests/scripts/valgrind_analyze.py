#!/usr/bin/env python


import os
import re
import json

regex = re.compile(b"memcheck__(?P<testname>[^_]*)__(?P<daemon>[^-]*)-.*.log")

deflost = re.compile(
    ".*definitely lost: (?P<bytes>[0-9,.]*) bytes in (?P<blocks>[0-9,.]*) blocks"
)
indlost = re.compile(
    ".*indirectly lost: (?P<bytes>[0-9,.]+) bytes in (?P<blocks>[0-9,.]+) blocks"
)
posslost = re.compile(
    ".*possibly lost: (?P<bytes>[0-9,.]+) bytes in (?P<blocks>[0-9,.]+) blocks"
)
stillreach = re.compile(
    ".*still reachable: (?P<bytes>[0-9,.]+) bytes in (?P<blocks>[0-9,.]+) blocks"
)

"""
==812887==    indirectly lost: 0 bytes in 0 blocks
==812887==      possibly lost: 0 bytes in 0 blocks
==812887==    still reachable: 1,738 bytes in 4 blocks
"""


results = dict()

results["TOTALS"] = dict()
results["TOTALS"]["definitely_lost_bytes"] = 0
results["TOTALS"]["definitely_lost_blocks"] = 0
results["TOTALS"]["possibly_lost_bytes"] = 0
results["TOTALS"]["possibly_lost_blocks"] = 0
results["TOTALS"]["indirectly_lost_bytes"] = 0
results["TOTALS"]["indirectly_lost_blocks"] = 0
results["TOTALS"]["still_reachable_bytes"] = 0
results["TOTALS"]["still_reachable_blocks"] = 0

for root, dirs, files in os.walk(b".", topdown=False):
    for name in files:
        match = regex.match(name)
        if match:
            testname = match.group("testname").decode()
            daemon = match.group("daemon").decode()
            if not testname in results:
                results[testname] = dict()
            if not daemon in results[testname]:
                results[testname][daemon] = dict()
            file = open(os.path.join(root, name))
            for line in file:
                deflostm = deflost.match(line)
                indlostm = indlost.match(line)
                posslostm = posslost.match(line)
                stillreachm = stillreach.match(line)
                if deflostm:
                    results["TOTALS"]["definitely_lost_bytes"] += int(
                        deflostm.group("bytes").replace(",", "")
                    )
                    results["TOTALS"]["definitely_lost_blocks"] += int(
                        deflostm.group("blocks").replace(",", "")
                    )
                    results[testname][daemon]["definitely_lost_bytes"] = int(
                        deflostm.group("bytes").replace(",", "")
                    )
                    results[testname][daemon]["definitely_lost_blocks"] = int(
                        deflostm.group("blocks").replace(",", "")
                    )
                if indlostm:
                    results["TOTALS"]["indirectly_lost_bytes"] += int(
                        indlostm.group("bytes").replace(",", "")
                    )
                    results["TOTALS"]["indirectly_lost_blocks"] += int(
                        indlostm.group("blocks").replace(",", "")
                    )
                    results[testname][daemon]["indirectly_lost_bytes"] = int(
                        indlostm.group("bytes").replace(",", "")
                    )
                    results[testname][daemon]["indirectly_lost_blocks"] = int(
                        indlostm.group("blocks").replace(",", "")
                    )
                if posslostm:
                    results["TOTALS"]["possibly_lost_bytes"] += int(
                        posslostm.group("bytes").replace(",", "")
                    )
                    results["TOTALS"]["possibly_lost_blocks"] += int(
                        posslostm.group("blocks").replace(",", "")
                    )
                    results[testname][daemon]["possibly_lost_bytes"] = int(
                        posslostm.group("bytes").replace(",", "")
                    )
                    results[testname][daemon]["possibly_lost_blocks"] = int(
                        posslostm.group("blocks").replace(",", "")
                    )
                if stillreachm:
                    results["TOTALS"]["still_reachable_bytes"] += int(
                        stillreachm.group("bytes").replace(",", "")
                    )
                    results["TOTALS"]["still_reachable_blocks"] += int(
                        stillreachm.group("blocks").replace(",", "")
                    )
                    results[testname][daemon]["still_reachable_bytes"] = int(
                        stillreachm.group("bytes").replace(",", "")
                    )
                    results[testname][daemon]["still_reachable_blocks"] = int(
                        stillreachm.group("blocks").replace(",", "")
                    )


print(json.dumps(results, sort_keys=True, indent=4))

import sys
import os

stats_list = [
    "in use at exit",
    "total heap usage",
    "definitely lost",
    "possibly lost",
    "indirectly lost",
    "still reachable",
]
headers = ["id", "stat", "bytes", "blocks", "allocs", "frees"]
data = []
id = int(sys.argv[1])
with open(mode="r", file="valgrind.out") as file:
    for line in file:
        for stat in stats_list:
            if stat in line:
                row = ["" for h in headers]
                amount = line.split(":")[1]
                row[0] = str(id)
                row[1] = stat.replace(" ", "_")
                if stat == "total heap usage":
                    amount = amount.split(", ")
                    allocs = amount[0].lstrip().split(" ")
                    row[4] = row[4].join(allocs[0].split(","))
                    row[5] = row[5].join(amount[1].split(" ")[0].split(","))
                    row[2] = row[2].join(amount[2].split(" ")[0].split(","))
                else:
                    amount = amount.split(" ")
                    row[2] = row[2].join(amount[1].lstrip().split(","))
                    row[3] = row[3].join(amount[-2].lstrip().split(","))

                data.append(row)
                break

if not os.path.exists("valgrind.csv"):
    data.insert(0, headers)

with open("valgrind.csv", "a") as file:
    for row in data:
        file.write(",".join(row) + "\n")

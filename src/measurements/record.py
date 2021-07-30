import os
import re
import sys
# all the .log files in this directory
L = filter(lambda x: x[-4:] == ".log",os.listdir("."))
# this gives us all individual measurements as time stamps
dates = set(re.compile(r"[0-9]{3,10}").findall(" ".join(L)))

#define what to extract
keys = [r"(PQC-Public-time) is (.*)s",
        r"(PQC-Private-time) is (.*) s",
        r"(TPM-CreateLoaded-time) is (.*)s",
        r"(read-time) was (.*)s",
        r"(write-time) was (.*)s",
        r"(crypto-sign-keypair-total-kcycles):(.*)",
        r"(crypto-sign-signature-streaming-total-kcycles):(.*)",
        r"(crypto-sign-signature-streaming-write-kcycles):(.*)",
        r"(crypto-sign-verify-streaming-total-kcycles):(.*)",
        r"(crypto-sign-verify-streaming-read-kcycles):(.*)",
        r"(Scheme) is (.*)"]

def extractData(s,key):
    return re.compile(key).findall(s)


results = []
# go through them in pairs that were recorded with the same timestamp
for  date in dates:
    files = re.compile("("+date+"-[^ ]+.log)").findall(" ".join(L))
    # read in both files
    data = reduce(lambda x,y: x+y,map(lambda x: open(x,"r").read(),files))
    d = {}
    for k in keys:
        for t in extractData(data,k):
            d[t[0]] = t[1]
    results = results + [d]
    d["date"] = str(date)

print(" ".join([k.replace(" ", "-").replace("_", "-") for k in sys.argv[1:]]))

for datum in results:
    l = " ".join(["{"+datum[k].replace("_", "\\_")+"}" if k == "Scheme" else datum[k] if k in datum else "{---}" for k in sys.argv[1:]])
#    for k in sys.argv[1:]:
#        if k in datum:
#            l = l + datum[k] + " "
    print(l)

#!/usr/bin/bash
python2 record.py Scheme TPM-CreateLoaded-time crypto-sign-keypair-total-kcycles PQC-Private-time read-time crypto-sign-signature-streaming-total-kcycles crypto-sign-signature-streaming-write-kcycles PQC-Public-time write-time crypto-sign-verify-streaming-total-kcycles crypto-sign-verify-streaming-read-kcycles | (sed -u 1q; sort -t_ -k 1,2 -k 4,4.6 -k 3,3)


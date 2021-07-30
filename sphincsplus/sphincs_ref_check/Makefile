BIN_DIR = spx_bin
SIG_DIR ?= ../sphincs_sha_shake/out_files/

paramsets = sphincs-sha256-128f sphincs-sha256-128s sphincs-sha256-192f sphincs-sha256-192s sphincs-sha256-256f sphincs-sha256-256s sphincs-shake256-128f sphincs-shake256-128s sphincs-shake256-192f sphincs-shake256-192s sphincs-shake256-256f sphincs-shake256-256s

# does not work with -jX
.NOTPARALLEL:

.PHONY: all pre
all: pre $(paramsets)

pre:
	mkdir -p $(BIN_DIR)

$(paramsets):
	echo "build $@" && \
	cd sphincs_ref && \
	$(MAKE) clean && \
	$(MAKE) -j$(nproc) test/spx PARAMS=$@ THASH=robust PARAMSETNAME=$@-robust SIG_DIR=${SIG_DIR} && \
	cp test/spx ../$(BIN_DIR)/$@_robust && \
	$(MAKE) clean && \
	$(MAKE) -j$(nproc) test/spx PARAMS=$@ THASH=simple PARAMSETNAME=$@-simple SIG_DIR=${SIG_DIR} && \
	cp test/spx ../$(BIN_DIR)/$@-simple && \
	$(MAKE) clean



.PHONY: clean
clean:
	rm -rf $(BIN_DIR) && \
	cd sphincs_ref && \
	$(MAKE) clean

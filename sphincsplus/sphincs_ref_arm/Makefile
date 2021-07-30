BIN_DIR = spx_bin

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
	$(MAKE) TARGET=stm32f4 spx.bin PARAMS=$@ THASH=robust && \
	cp build/spx.bin ../$(BIN_DIR)/$@_robust.bin && \
	$(MAKE) clean && \
	$(MAKE) TARGET=stm32f4 spx.bin PARAMS=$@ THASH=simple && \
	cp build/spx.bin ../$(BIN_DIR)/$@_simple.bin && \
	$(MAKE) clean



.PHONY: clean
clean:
	rm -rf $(BIN_DIR) && \
	cd sphincs_ref && \
	$(MAKE) clean

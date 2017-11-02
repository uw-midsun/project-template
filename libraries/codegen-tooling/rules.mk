# Defines $(T)_SRC, $(T)_INC, $(T)_DEPS, and $(T)_CFLAGS for the build makefile.
# Tests can be excluded by defining $(T)_EXCLUDE_TESTS.
# Pre-defined:
# $(T)_SRC_ROOT: $(T)_DIR/src
# $(T)_INC_DIRS: $(T)_DIR/inc{/$(PLATFORM)}
# $(T)_SRC: $(T)_DIR/src{/$(PLATFORM)}/*.{c,s}

$(T)_PREBUILD_CMD := python make/git_fetch.py -folder=libraries/codegen-tooling -user=uw-midsun -repo=codegen-tooling -tag=latest -file=codegen-tooling-out.zip

$(T)_DEPS := ms-common


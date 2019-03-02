.PHONY: clean build iso qemu shapelogs help
.DEFAULT_GOAL := help

define PRINT_HELP_PYSCRIPT
import re, sys

for line in sys.stdin:
	match = re.match(r'^([a-zA-Z_-]+):.*?## (.*)$$', line)
	if match:
		target, help = match.groups()
		print("%-20s %s" % (target, help))
endef
export PRINT_HELP_PYSCRIPT

folders:
	@mkdir -p log sysroot

help: ## print help info
	@python -c "$$PRINT_HELP_PYSCRIPT" < $(MAKEFILE_LIST)

clean: ## clean build objects and dist files
	$(MAKE) -C ./src clean
	$(RM) -rf ./log ./sysroot ./isodir

build: folders ## main build sequence
	$(MAKE) -C ./src install


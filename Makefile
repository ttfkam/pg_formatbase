EXTENSION = formatbase        # the extensions name
DATA = formatbase--2.0.0.sql  # script files to install
REGRESS = formatbase_test     # test script file (without extension)
MODULES = formatbase          # c module file to build

# postgres build stuff
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

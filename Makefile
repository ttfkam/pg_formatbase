EXTENSION = formatbase        # the extensions name
DATA = formatbase--1.0.0.sql  # script files to install
REGRESS = formatbase_test     # our test script file (without extension)

# postgres build stuff
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

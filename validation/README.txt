Validation commands (Linux x86_64)

Official assessment:
  make clean && make
  python3 driver.py -i 25303060076 -A

Independent cache checks:
  python3 validation/extra_checks.py

Matrix correctness, input preservation and sanitizers:
  gcc -g -Wall -Werror -fsanitize=address,undefined -DregisterFunctions=registerTransFunctions -c trans.c -o /tmp/trans-check.o
  gcc -g -Wall -Werror -fsanitize=address,undefined -DregisterFunctions=registerMulFunctions -c mul.c -o /tmp/mul-check.o
  gcc -g -Wall -Werror -fsanitize=address,undefined validation/matrix_check.c /tmp/trans-check.o /tmp/mul-check.o cachelab.c -o /tmp/matrix-check
  /tmp/matrix-check

The course test programs, Makefile and driver.py are unchanged.
Logs were captured on WSL Ubuntu x86_64 on 2026-11-24.

/**
 * @file
 *
 * @date Oct 4, 2012
 * @author: Anton Bondarev
 */
#include <cmd/shell.h>
#include <string.h>


ARRAY_SPREAD_DEF(const struct shell, __shell_registry);

const struct shell *shell_lookup(const char *shell_name) {
	const struct shell *shell;

	array_foreach_ptr(shell, __shell_registry, ARRAY_SPREAD_SIZE(__shell_registry)) {
		if (0 == strcmp(shell->name, shell_name)) {
			return shell;
		}
	}

	return NULL;
}


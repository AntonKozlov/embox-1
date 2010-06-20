/**
 * @file
 * @brief TODO
 *
 * @date 10.06.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_BIND_H_
# error "Do not include this file directly, use <mod/bind.h> instead!"
#endif /* MOD_BIND_H_ */

#ifndef __EMBUILD_MOD__
#  error "Do not include without __EMBUILD_MOD__ defined (e.g. from lib code)!"
#endif /* __EMBUILD_MOD__ */

/*
 * Implementation note:
 * Many macros uses some of their arguments to construct symbol names. This
 * imposes well-known restrictions to the input values of these arguments.
 * To prevent a confusion all such arguments are prefixed by 's_' (symbol).
 */

#include <util/macro.h>

#include "decls.h"
#include "info.h"

#include "array.h"

/* Here goes public macros API implementation. */

#define __MOD_PTR(s_mod) (&__MOD(s_mod))

#define __MOD_INFO_DEF(s_mod, _mod_data, _mod_ops) \
		const struct mod_info __MOD_INFO(s_mod) = { \
				.data = (void *) _mod_data, \
				.ops = (struct mod_ops *) _mod_ops, \
			}

/*
 * Macros with 'SELF' suffix are used to access the self module of the current
 * compilation unit (referenced by __EMBUILD_MOD__), e.g. to bind something
 * like #mod_data.
 */

#define __MOD_SELF(macro) \
		__MOD_SELF_EXPAND(macro, __EMBUILD_MOD__)
#define __MOD_SELF_EXPAND(macro, mod) \
		__MOD_SELF_INVOKE(macro, mod)
#define __MOD_SELF_INVOKE(macro, mod) \
		macro(mod)

#define __MOD_SELF_PTR  __MOD_SELF(MOD_PTR)

#define __MOD_SELF_NAME __MOD_SELF(__MOD_NAME)

#define __MOD_SELF_INFO_DEF(_mod_data, _mod_ops) \
		__MOD_INFO_DEF(__EMBUILD_MOD__, _mod_data, _mod_ops)

